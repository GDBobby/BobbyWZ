#include "MapleTree.h"


namespace MapleLib {
	namespace WzLib {
		void MapleTree::ParseMainWzDirectory(WzFile& wzFile) {
			if (wzFile.path == L"") {
				Helpers::ErrorLogger::Log(Helpers::ErrorLevel::Critical, "[Error] Path is null");
				return;
			}

			//Util::WzBinaryReader reader{ path, WzIv };
			reader.SetHeader();
			wzFile.header = reader.header;
			/* this was moved in WzReader::reader
			header.ident = reader.ReadString(4);
			header.fsize = reader.ReadUInt64();
			header.fstart = reader.ReadUInt32();
			header.copyright = reader.ReadNullTerminatedString();
			reader.ReadBytes((int)(Header.FStart - reader.BaseStream.Position));
			reader.Header = this.Header;
			*/
			reader.ReadTemplate<int16_t>(&wzFile.version);
			if (wzFile.fileVersion == -1) {
				for (int16_t j = 0; j < INT16_MAX; j++) {
					wzFile.fileVersion = j;
					wzFile.GetVersionHash();
					if (wzFile.versionHash != 0) {
						reader.Hash = wzFile.versionHash;
						int64_t basePos = reader.filePosition;
						MapleNode directoryNode{ reinterpret_cast<WzObject*>(new WzDirectory(wzFile.getName(), wzFile.versionHash, WzIv)) };
						WzDirectory testDirectory;
						try {
							ParseDirectory(&directoryNode);
							//headNode.addChild(&directoryNode);

							//testDirectory = WzDirectory(wzFile.getName(), wzFile.versionHash, WzIv);
							//ParseDirectory(&testDirectory);
						}
						catch (std::exception e) {
							reader.filePosition = basePos;
							directoryNode.dispose();
							continue;
						}
						//for some reason, it creates this image then deconstructs the testDirectory, then reconstructs it?
						MapleNode* testImageNode;
						for (auto& child : directoryNode.children) {
							//the original maplelib uses a separate container for directories and images
							//using that, i wouldnt have to iterate through this to find subdirs or images
							//im a little committed to the tree tho
							if (dynamic_cast<WzImage*>(child->object) != nullptr) {
								testImageNode = child;
								break;
							}
						}
						WzImage* castedImage = dynamic_cast<WzImage*>(testImageNode->object);

						//WzImage testImage = testDirectory.GetChildImages()[0];

						try {
							//i think it just wants this offset from the testImage?
							reader.filePosition = castedImage->offset;
							//reader.readFile.seekg(testImage.Offset, std::ios::beg);
							uint8_t checkByte = reader.ReadSByte();
							//reader.readFile.read((char*)&checkByte, 1);
							reader.filePosition = basePos;
							//testDirectory.Dispose(); //i dont understand why its reconstructed
							switch (checkByte) {
							case 0x73:
							case 0x1b: {
								//i think that mf reparses for no reason
								//MapleNode directoryNode{ reinterpret_cast<WzObject*>(new WzDirectory(wzFile.getName(), wzFile.versionHash, WzIv )) };
								//ParseDirectory(&directoryNode);
								headNode.placeChild(&directoryNode);
								//wzDir = directory;
								return;
							}
							}
							//reader.readFile.seekg(wzFile.position, std::ios::beg);
							reader.filePosition = basePos;
						}
						catch (std::exception e) {
							reader.filePosition = basePos;
						}
					}
				}
				throw std::exception("Error with game version hash : The specified game version is incorrect and WzLib was unable to determine the version itself");
			}
			else {
				wzFile.GetVersionHash();
				reader.Hash = wzFile.versionHash;
				MapleNode directoryNode{ reinterpret_cast<WzObject*>(new WzDirectory(wzFile.getName(), wzFile.versionHash, WzIv)) };
				//WzDirectory directory{ wzFile.getName(), wzFile.versionHash, WzIv};
				ParseDirectory(&directoryNode);
				headNode.placeChild(&directoryNode);
				//wzDir = directory;
			}
		}

		void MapleTree::ParseDirectory(MapleNode* wzDirNode) {
			WzDirectory* wzDir = reinterpret_cast<WzDirectory*>(wzDirNode->object);

			int entryCount = reader.ReadCompressedInt();
			for (int i = 0; i < entryCount; i++) {
				uint8_t type = reader.ReadByte();
				std::wstring fname{ L"" };
				int fsize{};
				int checksum{};
				uint32_t offset{};

				if (type == 1) {
					//01 XX 00 00 00 00 00 OFFSET (4 bytes) 
					int unknown = reader.ReadInt32();
					reader.filePosition += 2;
					//reader.ReadInt16();
					uint32_t offs = reader.ReadOffset();
					continue;
				}
				else if (type == 2) {
					int stringOffset = reader.ReadInt32();
					long rememberPos = reader.filePosition;
					reader.filePosition = reader.header.fstart + stringOffset;
					type = reader.ReadByte();
					fname = reader.ReadString();
					reader.filePosition = rememberPos;
				}
				else if (type == 3 || type == 4) {
					fname = reader.ReadString();
				}
				else {} //?

				fsize = reader.ReadCompressedInt();
				checksum = reader.ReadCompressedInt();
				offset = reader.ReadOffset();
				if (type == 3) { //what is this 3?
					WzDirectory* subDir = new WzDirectory(fname, wzDir->hash, WzIv);
					subDir->size = fsize;
					subDir->checksum = checksum;
					subDir->offset = offset;
					//subDir.parent = this;
					wzDirNode->constructChild(reinterpret_cast<WzObject*>(subDir));
					
					
					//subDirs.Add(subDir);
				}
				else {
					WzImage* img = new WzImage(fname);
					img->size = fsize;
					img->checksum = checksum;
					img->offset = offset;
					//img.Parent = this;
					//images.Add(img);
					wzDirNode->constructChild(reinterpret_cast<WzObject*>(img));
				}
			}
			for (auto& child : wzDirNode->children) {
				if (dynamic_cast<WzDirectory*>(child->object) != nullptr) { //i think the != nullptr is implicit but im gonna add it anyways
					//the original guy uses a separate container for directories and images
					//so this dynamic cast is only necessary because i removed the distinction
					reader.filePosition = reinterpret_cast<WzDirectory*>(child->object)->offset;
					ParseDirectory(child);
				}
			}
			//for (WzDirectory& subdir : subDirs) {
			//	reader.BaseStream.Position = subdir.offset;
			//	subdir.ParseDirectory();
			//}
		}

		void MapleTree::ParseImage(MapleNode* node, bool parseEverything = false) {
#ifdef _DEBUG
			if (dynamic_cast<WzImage*>(node->object) == nullptr) {
				throw std::exception("invalid input");
			}
#endif
			WzImage* image = reinterpret_cast<WzImage*>(node->object);

			if (image->parsed || image->changed) {
				image->parsed = true;
				return;
			}
			image->parseEverything = parseEverything;
			size_t originalPos = reader.filePosition;
			reader.filePosition = offset;
			uint8_t b = reader.ReadByte();
			if (b != 0x73 || reader.ReadString() != L"Property" || reader.ReadTemplateAsCopy<uint16_t>() != 0) {
				return;
			}
			ParsePropertyList(offset, node, node);
			//properties.AddRange(WzImageProperty.ParsePropertyList(offset, reader, this, this));
			image->parsed = true;
		}

		void MapleTree::ParsePropertyList(uint32_t offset, MapleNode* parent, MapleNode* parentImg) {
			int entryCount = reader.ReadCompressedInt();
			//List<WzImageProperty> properties = new List<WzImageProperty>(entryCount);
			for (int i = 0; i < entryCount; i++) {
				std::wstring name = reader.ReadStringBlock(offset);
				uint8_t ptype = reader.ReadByte();
				switch (ptype) {
				case 0:
					parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzNullProperty(name)));
					//properties.Add(new WzNullProperty(name){ Parent = parent });
					break;
				case 11:
				case 2:
					parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzShortProperty(name, reader.ReadTemplateAsCopy<int16_t>())));
					//properties.Add(new WzShortProperty(name, reader.ReadInt16()){ Parent = parent });
					break;
				case 3:
				case 19:
					parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzIntProperty(name, reader.ReadCompressedInt())));
					//properties.Add(new WzIntProperty(name, reader.ReadCompressedInt()){ Parent = parent });
					break;
				case 20:
					parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzLongProperty(name, reader.ReadLong())));
					//properties.Add(new WzLongProperty(name, reader.ReadLong()){ Parent = parent });
					break;
				case 4:
					uint8_t type = reader.ReadByte();
					if (type == 0x80) {
						parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzFloatProperty(name, reader.ReadSingle())));
					}
					else if (type == 0) {
						parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzFloatProperty(name, 0.f)));
					}
					break;
				case 5:
					parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzDoubleProperty(name, reader.ReadDouble())));
					break;
				case 8:
					parent->addChild(reinterpret_cast<WzObject*>(new WzProperties::WzStringProperty(name, reader.ReadStringBlock(offset))));
					//properties.Add(new WzStringProperty(name, reader.ReadStringBlock(offset)){ Parent = parent });
					break;
				case 9:
					int eob = static_cast<int>(reader.ReadUInt32() + reader.filePosition);
					WzImageProperty exProp = ParseExtendedProp(reader, offset, eob, name, parent, parentImg);
					parent->constructChild(reinterpret_cast<WzObject*>(&exProp));
					properties.Add(exProp);
					//parent->addChild(new WzImagePro)
					reader.filePosition = eob;
					
					break;
				default:
					throw std::exception("Unknown property type at ParsePropertyList");
				}
			}
			//return properties;
		}

		WzImageProperty MapleTree::ParseExtendedProp(uint32_t offset, int endOfBlock, std::wstring name, MapleNode* parent, MapleNode* imgParent) {
			switch (reader.ReadByte()) {
			case 0x01:
			case 0x1B:
				return ExtractMore(reader, offset, endOfBlock, name, reader.ReadStringAtOffset(offset + reader.ReadInt32()), parent, imgParent);
			case 0x00:
			case 0x73:
				return ExtractMore(reader, offset, endOfBlock, name, L"", parent, imgParent);
			default:
				throw std::exception("Invalid byte read at ParseExtendedProp");
			}
		}
		WzImageProperty MapleTree::ExtractMore(uint32_t offset, int eob, std::wstring name, std::wstring iname, MapleNode* parent, MapleNode* imgParent) {
			if (iname == L"")
				iname = reader.ReadString();
			if (iname == L"Property") {
				WzSubProperty subProp = new WzSubProperty(name);
				reader.BaseStream.Position += 2; // Reserved?
				subProp.AddProperties(WzImageProperty.ParsePropertyList(offset, reader, subProp, imgParent));
				return subProp;
			}
			else if (iname == L"Canvas") {
				WzCanvasProperty canvasProp = new WzCanvasProperty(name){ Parent = parent };
				reader.BaseStream.Position++;
				if (reader.ReadByte() == 1) {
					reader.BaseStream.Position += 2;
					canvasProp.AddProperties(WzImageProperty.ParsePropertyList(offset, reader, canvasProp, imgParent));
				}
				canvasProp.PngProperty = new WzPngProperty(reader, imgParent.parseEverything){ Parent = canvasProp };
				return canvasProp;
			}
			else if (iname == "Shape2D#Vector2D") {
				WzVectorProperty vecProp = new WzVectorProperty(name);
				vecProp.x = new WzIntProperty(L"X", reader.ReadCompressedInt());
				vecProp.y = new WzIntProperty(L"Y", reader.ReadCompressedInt());
				return vecProp;
			}

			switch (iname) {
			case :
				
			case "Shape2D#Convex2D":
				WzConvexProperty convexProp = new WzConvexProperty(name){ Parent = parent };
				int convexEntryCount = reader.ReadCompressedInt();
				convexProp.WzProperties.Capacity = convexEntryCount;
				for (int i = 0; i < convexEntryCount; i++)
				{
					convexProp.AddProperty(ParseExtendedProp(reader, offset, 0, name, convexProp, imgParent));
				}
				return convexProp;
			case "Sound_DX8":
				WzSoundProperty soundProp = new WzSoundProperty(name, reader, imgParent.parseEverything){ Parent = parent };
				return soundProp;
			case "UOL":
				reader.BaseStream.Position++;
				switch (reader.ReadByte())
				{
				case 0:
					return new WzUOLProperty(name, reader.ReadString()){ Parent = parent };
				case 1:
					return new WzUOLProperty(name, reader.ReadStringAtOffset(offset + reader.ReadInt32())){ Parent = parent };
				}
				throw new Exception("Unsupported UOL type");
			default:
				throw new Exception("Unknown iname: " + iname);
			}
		}
	}
}