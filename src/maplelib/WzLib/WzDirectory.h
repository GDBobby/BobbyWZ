#pragma once

#include "WzObject.h"
#include "Util/WzBinaryReader.h"
#include "Util/WzTool.h"

#include <string>
#include <list>
#include <stdexcept>

namespace MapleLib {
	namespace WzLib {
		/// <summary>
		/// A directory in the wz file, which may contain sub directories or wz images
		/// </summary>
		class WzDirectory : WzObject {
		public:
			std::list<WzImage> images;
			std::list<WzDirectory> subDirs{};
			Util::WzBinaryReader reader;
			uint32_t offset = 0;
			std::wstring name;
			uint32_t hash;
			int size, checksum, offsetSize;
			uint8_t* WzIv;
			WzObject parent;
			WzFile wzFile;

			WzObjectType getObjectType() {
				return WzObjectType::Directory;
			}
			WzFile getWzFileParent() {
				return wzFile;
			}

			/// <summary>
			/// Disposes the obejct
			/// </summary>
			void Dispose() override {
				name = null;
				reader = null;
				for (WzImage& img : images) {
					img.Dispose();
				}
				for(WzDirectory & dir : subDirs) {
					dir.Dispose();
				}
				images.clear();
				subDirs.clear();
				images = null;
				subDirs = null;
			}

			//public int BlockSize { get { return size; } set { size = value; } }

			//public int Checksum{ get { return checksum; } set { checksum = value; } }

			std::list<WzDirectory> getWzDirectories() { return subDirs; }
			std::list<WzImage> getWzImages() { return images; }


			void searchSet(std::wstring value) {
				if (value != null) {
					value.name = name;
					if (value is WzDirectory) {
						AddDirectory((WzDirectory)value);
					}
					else if (value is WzImage) {
						AddImage((WzImage)value);
					}
					else {
						throw exception("must be directory or image");
						//throw new ArgumentException("Value must be a Directory or Image");
					}
				}
			}
			WzObject searchGet(std::wstring name) { //might need to have this return a object type as well
				for (WzImage& i : images) {
					if (i.Name.ToLower() == name.ToLower()){
						return i;
					}
				}
				for (WzDirectory& d : subDirs) {
					if (d.Name.ToLower() == name.ToLower()) {
						return d;
					}
				}
				//throw new KeyNotFoundException("No wz image or directory was found with the specified name");
				return null;
			}

			public WzDirectory() { }
			public WzDirectory(std::wstring name) : name{ name } {}
					/// <summary>
					/// Creates a WzDirectory
					/// </summary>
					/// <param name="reader">The BinaryReader that is currently reading the wz file</param>
					/// <param name="blockStart">The start of the data block</param>
					/// <param name="parentname">The name of the directory</param>
					/// <param name="wzFile">The parent Wz File</param>
			WzDirectory(Util::WzBinaryReader reader, std::wstring dirName, uint32_t verHash, uint8_t* WzIv, WzFile wzFile) :
				reader{reader},
				name{dirName},
				hash{verHash},
				WzIv{WzIv},
				wzFile{wzFile}
			{}

			void ParseDirectory(){
				int entryCount = reader.ReadCompressedInt();
				for (int i = 0; i < entryCount; i++) {
					uint8_t type = reader.ReadByte();
					std::wstring fname{""};
					int fsize{};
					int checksum{};
					uint32_t offset{};

					long rememberPos = 0;
					if (type == 1) //01 XX 00 00 00 00 00 OFFSET (4 bytes) 
					{
						int unknown = reader.ReadInt32();
						reader.ReadInt16();
						uint32_t offs = reader.ReadOffset();
						continue;
					}
					else if (type == 2){
						int stringOffset = reader.ReadInt32();
						rememberPos = reader.BaseStream.Position;
						reader.BaseStream.Position = reader.Header.FStart + stringOffset;
						type = reader.ReadByte();
						fname = reader.ReadString();
					}
					else if (type == 3 || type == 4) {
						fname = reader.ReadString();
						rememberPos = reader.BaseStream.Position;
					}
					else
					{}

					reader.BaseStream.Position = rememberPos;
					fsize = reader.ReadCompressedInt();
					checksum = reader.ReadCompressedInt();
					offset = reader.ReadOffset();
					if (type == 3) {
						WzDirectory subDir = new WzDirectory(reader, fname, hash, WzIv, wzFile);
						subDir.BlockSize = fsize;
						subDir.Checksum = checksum;
						subDir.Offset = offset;
						subDir.Parent = this;
						subDirs.Add(subDir);
					}
					else {
						WzImage img = new WzImage(fname, reader);
						img.BlockSize = fsize;
						img.Checksum = checksum;
						img.Offset = offset;
						img.Parent = this;
						images.Add(img);
					}
				}

				for(WzDirectory& subdir : subDirs) {
					reader.BaseStream.Position = subdir.offset;
					subdir.ParseDirectory();
				}
			}

			void SaveImages(BinaryWriter wzWriter, FileStream fs) {
				for(WzImage& img : images) {
					if (img.changed) {
						fs.Position = img.tempFileStart;
						byte[] buffer = new byte[img.size];
						fs.Read(buffer, 0, img.size);
						wzWriter.Write(buffer);
					}
					else {
						img.reader.BaseStream.Position = img.tempFileStart;
						wzWriter.Write(img.reader.ReadBytes((int)(img.tempFileEnd - img.tempFileStart)));
					}
				}
				for(WzDirectory& dir : subDirs)
					dir.SaveImages(wzWriter, fs);
			}
			int GenerateDataFile(std::wstring fileName) {
				size = 0;
				int entryCount = subDirs.size() + images.size();
				if (entryCount == 0) {
					offsetSize = 1;
					return (size == 0);
				}
				size = WzTool::GetCompressedIntLength(entryCount);
				offsetSize = WzTool::GetCompressedIntLength(entryCount);

				WzBinaryWriter imgWriter = null;
				MemoryStream memStream = null;
				FileStream fileWrite = new FileStream(fileName, FileMode.Append, FileAccess.Write);
				WzImage img;
				for (int i = 0; i < images.Count; i++) {
					img = images[i];
					if (img.changed) {
						memStream = new MemoryStream();
						imgWriter = new Util::WzBinaryWriter(memStream, this.WzIv);
						img.SaveImage(imgWriter);
						img.checksum = 0;
						for(uint8_t b : memStream.ToArray()){
							img.checksum += b;
						}
						img.tempFileStart = fileWrite.Position;
						fileWrite.Write(memStream.ToArray(), 0, (int)memStream.Length);
						img.tempFileEnd = fileWrite.Position;
						memStream.Dispose();
					}
					else {
						img.tempFileStart = img.offset;
						img.tempFileEnd = img.offset + img.size;
					}
					img.UnparseImage();

					int nameLen = WzTool::GetWzObjectValueLength(img.name, 4);
					size += nameLen;
					int imgLen = img.size;
					size += WzTool::GetCompressedIntLength(imgLen);
					size += imgLen;
					size += WzTool::GetCompressedIntLength(img.Checksum);
					size += 4;
					offsetSize += nameLen;
					offsetSize += WzTool.GetCompressedIntLength(imgLen);
					offsetSize += WzTool.GetCompressedIntLength(img.Checksum);
					offsetSize += 4;
					if (img.changed) {
						imgWriter.Close();
					}
				}
				fileWrite.Close();

				WzDirectory dir;
				for (int i = 0; i < subDirs.Count; i++) {
					dir = subDirs[i];
					int nameLen = WzTool.GetWzObjectValueLength(dir.name, 3);
					size += nameLen;
					size += subDirs[i].GenerateDataFile(fileName);
					size += WzTool.GetCompressedIntLength(dir.size);
					size += WzTool.GetCompressedIntLength(dir.checksum);
					size += 4;
					offsetSize += nameLen;
					offsetSize += WzTool.GetCompressedIntLength(dir.size);
					offsetSize += WzTool.GetCompressedIntLength(dir.checksum);
					offsetSize += 4;
				}
				return size;
			}
			void SaveDirectory(Util::WzBinaryWriter writer) {
				offset = (uint32_t)writer.BaseStream.Position;
				int entryCount = subDirs.Count + images.Count;
				if (entryCount == 0) {
					BlockSize = 0;
					return;
				}
				writer.WriteCompressedInt(entryCount);
				for (WzImage& img : images) {
					writer.WriteWzObjectValue(img.name, 4);
					writer.WriteCompressedInt(img.BlockSize);
					writer.WriteCompressedInt(img.Checksum);
					writer.WriteOffset(img.Offset);
				}
				for (WzDirectory& dir : subDirs) {
					writer.WriteWzObjectValue(dir.name, 3);
					writer.WriteCompressedInt(dir.BlockSize);
					writer.WriteCompressedInt(dir.Checksum);
					writer.WriteOffset(dir.Offset);
				}
				for (WzDirectory& dir : subDirs) {
					if (dir.BlockSize > 0) {
						dir.SaveDirectory(writer);
					}
					else {
						writer.Write((uint8_t)0);
					}
				}
			}
			uint32_t GetOffsets(uint32_t curOffset) {
				offset = curOffset;
				curOffset += (uint)offsetSize;
				for(WzDirectory& dir : subDirs) {
					curOffset = dir.GetOffsets(curOffset);
				}
				return curOffset;
			}
			uint32_t GetImgOffsets(uint curOffset) {
				for (WzImage& img : images) {
					img.Offset = curOffset;
					curOffset += (uint)img.BlockSize;
				}
				for(WzDirectory& dir >: subDirs) {
					curOffset = dir.GetImgOffsets(curOffset);
				}
				return curOffset;
			}
			void ExportXml(StreamWriter writer, bool oneFile, int level, bool isDirectory) {
				if (oneFile) {
					if (isDirectory) {
						writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag("WzDirectory", this.name, true));
					}
					for(WzDirectory& subDir : WzDirectories) {
						subDir.ExportXml(writer, oneFile, level + 1, isDirectory);
					}
					for(WzImage& subImg : WzImages) {
						subImg.ExportXml(writer, oneFile, level + 1);
					}
					if (isDirectory) {
						writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::CloseTag("WzDirectory"));
					}
				}
			}
					/// <summary>
					/// Parses the wz images
					/// </summary>
			void ParseImages() {
				for (WzImage& img : images) {
					if (reader.BaseStream.Position != img.Offset) {
						reader.BaseStream.Position = img.Offset;
					}
					img.ParseImage();
				}
				for(WzDirectory& subdir : subDirs) {
					if (reader.BaseStream.Position != subdir.Offset) {
						reader.BaseStream.Position = subdir.Offset;
					}
					subdir.ParseImages();
				}
			}

			void SetHash(uint newHash) {
				hash = newHash;
				for (WzDirectory& dir : subDirs) {
					dir.SetHash(newHash);
				}
			}

			void AddImage(WzImage img) {
				images.Add(img);
				img.Parent = this;
			}

			void AddDirectory(WzDirectory dir) {
				subDirs.Add(dir);
				dir.wzFile = wzFile;
				dir.Parent = this;
			}

			void ClearImages() {
				for (WzImage& img : images) { 
					img.Parent = null; 
				}
				images.Clear();
			}

			void ClearDirectories() {
				for (WzDirectory& dir : subDirs) {
					dir.Parent = null;
				}
				subDirs.Clear();
			}

			WzImage GetImageByName(std::wstring name) {
				for (WzImage& wzI : images) {
					if (wzI.Name.ToLower() == name.ToLower()){
						return wzI;
					}
				}
				return null;
			}

			WzDirectory GetDirectoryByName(std::wstring name) {
				for(WzDirectory& dir : subDirs)
					if (dir.Name.ToLower() == name.ToLower())
						return dir;
				return null;
			}

			std::list<WzImage> GetChildImages() {
				std::list<WzImage> imgFiles{};
				imgFiles.AddRange(images);
				for(WzDirectory& subDir : subDirs) {
					imgFiles.AddRange(subDir.GetChildImages());
				}
				return imgFiles;
			}

			void RemoveImage(WzImage image) {
				images.Remove(image);
				image.Parent = null;
			}

			void RemoveDirectory(WzDirectory dir) {
				subDirs.Remove(dir);
				dir.Parent = null;
			}

			WzDirectory DeepClone() {
				WzDirectory result = (WzDirectory)MemberwiseClone();
				result.WzDirectories.Clear();
				result.WzImages.Clear();
				for (WzDirectory& dir : WzDirectories) {
					result.WzDirectories.Add(dir.DeepClone());
				}
				for (WzImage& img : WzImages) {
					result.WzImages.Add(img.DeepClone());
				}
				return result;
			}

			int CountImages() {
				int result = images.Count;
				for (WzDirectory& subdir : WzDirectories) {
					result += subdir.CountImages();
				}
				return result;
			}

			void Remove() override {
				((WzDirectory)Parent).RemoveDirectory(this);
			}
		};
	}
}