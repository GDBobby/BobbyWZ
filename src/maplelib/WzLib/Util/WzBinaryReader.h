#pragma once


//using System;
//using System.IO;
//using System.Text;
//using MapleLib.MapleCryptoLib;


#include "WzKeyGenerator.h"
#include "../WzHeader.h"
#include "WzTool.h"

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <ios>

namespace MapleLib {
	namespace WzLib {
		namespace Util { 
			class WzBinaryReader {// : BinaryReader { what is binary reader
			public:
				WzBinaryReader(std::vector<char>& fileBuffer, std::vector<uint8_t>& WzIv);
				std::vector<char> fileBuffer;
				std::vector<uint8_t> WzKey{};
				uint32_t Hash{};
				WzHeader header{};
				size_t filePosition{ 0 };

				std::wstring ReadStringAtOffset(int64_t Offset) {
					return ReadStringAtOffset(Offset, false);
				}

				std::wstring ReadStringAtOffset(int64_t Offset, bool readByte);

				std::wstring ReadString();
				std::wstring ReadString(int length);
				std::wstring ReadNullTerminatedString();

				/// <summary>
				/// Reads an ASCII string, without decryption
				/// </summary>
				/// <param name="filePath">Length of bytes to read</param>
				std::vector<uint8_t> ReadBytes(int length);
				int8_t ReadSByte() {
					return static_cast<int8_t>(fileBuffer[filePosition++]);
				}
				uint8_t ReadByte() {
					return fileBuffer[filePosition++];
				}
				template <typename T>
				T ReadTemplateAsCopy();
				template <typename T>
				void ReadTemplate(T* obj);


				int32_t ReadCompressedInt();

				int64_t ReadLong();

				uint32_t ReadOffset();

				std::wstring DecryptString(std::wstring& stringToDecrypt);

				std::wstring DecryptNonUnicodeString(std::vector<wchar_t>& stringToDecrypt);

				std::wstring ReadStringBlock(uint32_t offset);

				//i dont see this in the source either? wtf

								//parsedData returned empty?
				//why is this even here?
				/*
				std::vector<std::wstring> ParseListFile() {
					std::vector<std::wstring> ret{};
					std::wstring strChrs{};
					wchar_t readBuf;
					while (filePosition < fileBuffer.size()) {
						int len;
						//readFile.read(reinterpret_cast<char*>(&len), sizeof(len));
						filePosition += len;
						strChrs.clear();
						strChrs.reserve(len);
						for (int i = 0; i < len; i++) {
							//readFile.read(reinterpret_cast<char*>(&readBuf), sizeof(int16_t));
							memcpy(&readBuf, &fileBuffer[filePosition], sizeof(int16_t));

							strChrs.push_back(readBuf);
							filePosition += 2;
							ret.emplace_back(DecryptString(strChrs));
						}
					}
					return ret;
				}
				*/
				
				void SetHeader() {

					header.ident = ReadString(4);
					ReadTemplate<uint64_t>(&header.fsize);
					ReadTemplate<uint64_t>(&header.fstart);
					header.copyright = ReadNullTerminatedString();
					filePosition = header.fstart; //really want to go to the header??
					//readFile.seekg(header.fstart, std::ios::beg);
					
				}
				
			};
		}
	}
}