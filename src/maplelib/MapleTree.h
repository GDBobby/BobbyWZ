#pragma once
#include <memory>
#include <vector>

#include "WzLib/Util/WzBinaryReader.h"
#include "WzLib/Util/WzBinaryWriter.h"
#include "WzLib/Util/WzTool.h"
#include "WzLib/Util/MapleNode.h"
#include "WzLib/WzParsing.h"


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
				WzParsing::ParseMainWzDirectory(wzFile, reader, &headNode, WzIv);
			}

			std::vector<char> fileContents;
			std::vector<uint8_t> WzIv;
			Util::WzBinaryReader reader;
			WzFile wzFile;

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

