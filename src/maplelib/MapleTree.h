#pragma once
#include <memory>
#include <vector>

#include "WzLib/Util/WzBinaryReader.h"
#include "WzLib/Util/WzBinaryWriter.h"
#include "WzLib/Util/WzTool.h"

#include "WzLib/WzFile.h"
#include "WzLib/WzProperties/WzNullProperty.h"
#include "WzLib/WzProperties/WzValueProperty.h"
#include "WzLib/WzProperties/WzStringProperty.h"
#include "WzLib/WzProperties/WzCanvasProperty.h"
#include "WzLib/WzProperties/WzVectorProperty.h"
#include "WzLib/WzProperties/WzConvexProperty.h"
#include "WzLib/WzProperties/WzSoundProperty.h"
#include "WzLib/WzProperties/WzUOLProperty.h"


namespace MapleLib {
	namespace WzLib {
		class MapleTree {
			//the hub for everything here
			//may end up merging WzFile and this class, but good to work with a clean slate
		public:
			//WzFile //exactly one of these per tree, im pretty sure the WzFile IS a WzDirectory
			
			//the question is IF the WzDirectory is ALSO the WzFile
			//i dont THINK it is

			//WzDirectory - allegedly its possible for a Directory to contain a directory, but i havent seen it
			// not every wzfile contains a directory, some go straight to image

			//WzImage
			//multiple subproperties, havent SEEN any contain an image

			//WzSubProperty - can contain WzSubProperty
			//WzCanvasProperty - can contain WzSubProperty
			//WzValueProperty
			MapleTree(std::wstring& filePath, WzMapleVersion version) :
				fileContents{ loadFileContents(filePath) },
				WzIv{ Util::WzTool::GetIvByMapleVersion(version) },
				reader{ fileContents, WzIv },
				wzFile{ filePath, WzIv},
				headNode{reinterpret_cast<WzObject*>(&wzFile)}
				//wzFile{filePath, version}
			{
				headNode.fileParent = &wzFile;
				ParseMainWzDirectory(wzFile);
			}

			std::vector<char> fileContents;
			std::vector<uint8_t> WzIv;
			Util::WzBinaryReader reader;
			WzFile wzFile;

			struct MapleNode {
				WzFile* fileParent{ nullptr };
				WzObject* object{ nullptr };
				MapleNode* parent{ nullptr };
				std::vector<MapleNode*> children{};

				MapleNode(WzObject* object) : object{ object } {}
				MapleNode(WzObject* object, MapleNode* parent) : fileParent{parent->fileParent} {}

				void dispose() {
					for (auto& child : children) {
						child->dispose();
					}
					delete object;
				}
				MapleNode* constructChild(WzObject* data) { //void* just 
					//auto& child = children.emplace_back(data, fileParent)->parent = this;
					//child->parent = this;
					auto& child = children.emplace_back(data, this);
					return child;
				}
				void placeChild(MapleNode* node) {
					node->parent = this;
					node->fileParent = fileParent;
					children.push_back(node);
				}
			};
			MapleNode headNode;

		protected:
			std::vector<char> loadFileContents(std::wstring& filePath) {
				// Open the file
				std::ifstream file(filePath, std::ios::binary | std::ios::ate);
				std::vector<char> ret{};
				if (!file.is_open()) {
					std::cerr << "Error opening file." << std::endl;
					throw std::exception("failed to open file, need to a way to catch this");
				}

				// Get the file size
				std::streampos fileSize = file.tellg();
				file.seekg(0, std::ios::beg);

				// Create a vector to store the file contents
				ret.resize(fileSize);

				// Read the file contents into the vector
				file.read(&ret[0], fileSize);
				file.close();
			}
			//directory contains wzfile, or wzfile contains directory?
			void ParseWzFile() {
				if (wzFile.mapleVersion == WzMapleVersion::GENERATE) {
					throw std::exception("Cannot call ParseWzFile() if WZ file type is GENERATE");
				}
				ParseMainWzDirectory(wzFile);
			}

			void ParseWzFile(std::vector<uint8_t>& WzIv) {
				if (wzFile.mapleVersion != WzMapleVersion::GENERATE) {
					throw std::exception("Cannot call ParseWzFile(byte[] generateKey) if WZ file type is not GENERATE");
				}
				this->WzIv = WzIv;
				ParseMainWzDirectory(wzFile);
			}
			void ParseMainWzDirectory(WzFile& wzFile);
			void ParseDirectory(MapleNode* wzDir);

			/// Parses the image from the wz filetod
			void ParseImage(MapleNode* node, bool parseEverything = false);

			//wzimageproperty
			void ParsePropertyList(uint32_t offset, MapleNode* parent, MapleNode* parentImg);

			void ParseExtendedProp(uint32_t offset, int endOfBlock, std::wstring& name, MapleNode* parent, MapleNode* imgParent);

			//wzimageproperty
			void ExtractMore(uint32_t offset, int eob, std::wstring& name, std::wstring iname, MapleNode* parent, MapleNode* imgParent);
		};
	}
}

/*
			if (version == WzMapleVersion::GETFROMZLZ) {
				//FileStream zlzStream = File.OpenRead(Path.Combine(Path.GetDirectoryName(filePath), "ZLZ.dll"));
				//^idk what that is
				std::ifstream zlzStream{ filePath + L"ZLZ.dll", std::ios::binary };
				WzIv = Util::WzKeyGenerator::GetIvFromZlz(zlzStream);
				zlzStream.close();
			}
			else { WzIv = Util::WzTool::GetIvByMapleVersion(version); }
			if (version == WzMapleVersion::GETFROMZLZ) {
				std::ifstream zlzStream{ filePath + L"ZLZ.dll", std::ios::binary };
				WzIv = Util::WzKeyGenerator::GetIvFromZlz(zlzStream);
				zlzStream.close();
			}
			else {
				WzIv = wzIV;
			}
*/

