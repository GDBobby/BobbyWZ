#pragma once

#include <string>
#include <list>
#include "util/WzTool.h"
#include "Util/WzBinaryReader.h"


namespace MapleLib {
    namespace WzLib {
        /// <summary>
        /// A class that parses and contains the data of a wz list file
        /// </summary>
        class ListFileParser {
        public:
            /// <summary>
            /// Parses a wz list file on the disk
            /// </summary>
            /// <param name="filePath">Path to the wz file</param>
            std::list<std::wstring> ParseListFile(std::wstring filePath, WzMapleVersion version) {
                return ParseListFile(filePath, WzTool::GetIvByMapleVersion(version));
            }

            /// <summary>
            /// Parses a wz list file on the disk
            /// </summary>
            /// <param name="filePath">Path to the wz file</param>
            static std::list<std::wstring> ParseListFile(std::wstring filePath, uint8_t* WzIv) {
                std::list<std::wstring> listEntries{};
                uint8_t* wzFileBytes = File.ReadAllBytes(filePath);
                Util::WzBinaryReader wzParser{ new MemoryStream(wzFileBytes), WzIv };
                while (wzParser.PeekChar() != -1) {
                    int len = wzParser.ReadInt32();
                    std::vector<int16_t> strChrs(len);

                    for (int i = 0; i < len; i++) {
                        strChrs[i] = (char)wzParser.ReadInt16();
                    }
                    wzParser.ReadUInt16(); //encrypted null
                    std::wstring decryptedStr = wzParser.DecryptString(strChrs);
                    listEntries.Add(decryptedStr);
                }
                wzParser.Close();
                std::wstring& lastEntry = listEntries.back();
                lastEntry = lastEntry.substr(0, lastEntry.length() - 1) + L"g";
                return listEntries;
            }

            static void SaveToDisk(std::wstring path, WzMapleVersion version, std::list<std::wstring> listEntries) {
                SaveToDisk(path, Util::WzTool::GetIvByMapleVersion(version), listEntries);
            }

            static void SaveToDisk(std::wstring path, uint8_t* WzIv, std::list<std::wstring> listEntries) {
                int lastIndex = listEntries.size() - 1;
                auto& lastEntry = listEntries.back();
                lastEntry = lastEntry.substr(0, lastEntry.length() - 1) + L"/";
                Util::WzBinaryWriter wzWriter{ File.Create(path), WzIv };
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