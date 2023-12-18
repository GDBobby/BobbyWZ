#pragma once

#include "Util/MapleNode.h"
#include "Util/WzBinaryReader.h"

#include "WzFile.h"
#include "WzProperties/WzNullProperty.h"
#include "WzProperties/WzValueProperty.h"
#include "WzProperties/WzStringProperty.h"
#include "WzProperties/WzCanvasProperty.h"
#include "WzProperties/WzVectorProperty.h"
#include "WzProperties/WzConvexProperty.h"
#include "WzProperties/WzSoundProperty.h"
#include "WzProperties/WzUOLProperty.h"

namespace MapleLib {
	namespace WzLib {
		class WzParsing { 
			//full static, these are just functions with no data members. 
			//maybe make a struct or remove just put the functions directly in the namespace idk
		public:

			/*
			static void ParseWzFile(WzFile& wzFile, Util::WzBinaryReader& reader, MapleNode* headNode) {
				if (wzFile.mapleVersion == WzMapleVersion::GENERATE) {
					throw std::exception("Cannot call ParseWzFile() if WZ file type is GENERATE");
				}
				ParseMainWzDirectory(wzFile, reader, headNode);
			}
			*/
			static void ParseWzFile(WzFile& wzFile, std::vector<uint8_t>& WzIv, Util::WzBinaryReader& reader, MapleNode* headNode) {
				if (wzFile.mapleVersion != WzMapleVersion::GENERATE) {
					throw std::exception("Cannot call ParseWzFile(byte[] generateKey) if WZ file type is not GENERATE");
				}
				ParseMainWzDirectory(wzFile, reader, headNode, WzIv);
			}
			static void ParseMainWzDirectory(WzFile& wzFile, Util::WzBinaryReader& reader, MapleNode* headNode, std::vector<uint8_t>& WzIv);
			static void ParseDirectory(MapleNode* wzDir, Util::WzBinaryReader& reader, std::vector<uint8_t>& WzIv);

			/// Parses the image from the wz filetod
			static void ParseImage(MapleNode* node, Util::WzBinaryReader& reader, bool parseEverything = false);

			//wzimageproperty
			static void ParsePropertyList(uint32_t offset, Util::WzBinaryReader& reader, MapleNode* parent, MapleNode* parentImg);

			static void ParseExtendedProp(uint32_t offset, Util::WzBinaryReader& reader, int endOfBlock, std::wstring& name, MapleNode* parent, MapleNode* imgParent);

			//wzimageproperty
			static void ExtractMore(uint32_t offset, Util::WzBinaryReader& reader, int eob, std::wstring& name, std::wstring iname, MapleNode* parent, MapleNode* imgParent);
		};
		
	}
}

