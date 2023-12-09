#pragma once

#include <string>
#include <list>
#include "util/WzTool.h"
#include "Util/WzBinaryReader.h"
#include "Util/WzBinaryWriter.h"
#include "WzMapleVersion.h"


namespace MapleLib {
    namespace WzLib {
        /// <summary>
        /// A class that parses and contains the data of a wz list file
        /// </summary>
        class ListFileParser {
        public:
            /// Parses a wz list file on the disk
            std::vector<std::wstring> ParseListFile(std::wstring filePath, WzMapleVersion version) {
                return ParseListFile(filePath, Util::WzTool::GetIvByMapleVersion(version));
            }

            /// Parses a wz list file on the disk
            static std::vector<std::wstring> ParseListFile(std::wstring& filePath, std::vector<uint8_t>& WzIv) {
                //uint8_t* wzFileBytes = File.ReadAllBytes(filePath);
                //Util::WzBinaryReader wzParser{ new MemoryStream(wzFileBytes), WzIv };
                Util::WzBinaryReader wzParser{ filePath, WzIv };
                std::vector<std::wstring> entries{ wzParser.ParseListFile() };

                std::wstring& lastEntry = entries.back();
                lastEntry = lastEntry.substr(0, lastEntry.length() - 1) + L"g";
                return entries;
            }

            static void SaveToDisk(std::wstring& path, WzMapleVersion version, std::list<std::wstring> listEntries) {
                SaveToDisk(path, Util::WzTool::GetIvByMapleVersion(version), listEntries);
            }

            static void SaveToDisk(std::wstring& path, std::vector<uint8_t>& WzIv, std::list<std::wstring> listEntries) {
                int lastIndex = listEntries.size() - 1;
                auto& lastEntry = listEntries.back();
                lastEntry = lastEntry.substr(0, lastEntry.length() - 1) + L"/";
                Util::WzBinaryWriter wzWriter{ path, WzIv };
                std::wstring s;
                for (auto iter = listEntries.begin(); iter != listEntries.end(); iter++) {
                    s = *iter;
                    wzWriter.Write((int)iter->length());
                    std::vector<wchar_t> encryptedChars = wzWriter.EncryptString(*iter + (wchar_t)0);
                    for (int j = 0; j < encryptedChars.size(); j++)
                        wzWriter.Write((int16_t)encryptedChars[j]);
                }
                //is this a mistake or is it supposed to be like this? does string lastEntry = listEntries[lastIndex] from C# make a reference or a copy?
                //if its a copy, this line is irrelevant. seems like itm ight be either way
                lastEntry = lastEntry.substr(0, lastEntry.length() - 1) + L"/";
            }
        };
    }
}