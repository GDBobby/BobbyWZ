#include "WzbinaryReader.h"

namespace MapleLib {
	namespace WzLib {
		namespace Util {

			WzBinaryReader::WzBinaryReader(std::vector<char>& fileBuffer, std::vector<uint8_t>& WzIv)
				: fileBuffer{ fileBuffer }, 
				WzKey{ WzKeyGenerator::GenerateWzKey(WzIv) } 
			{}

			std::wstring WzBinaryReader::ReadStringAtOffset(int64_t Offset, bool readByte) {
				int64_t CurrentOffset = filePosition;
				filePosition = Offset;
				if (readByte) {
					ReadByte();
				}
				std::wstring ReturnString = ReadString();
				filePosition = CurrentOffset;
				return ReturnString;
			}

			std::wstring WzBinaryReader::ReadString() {
				int8_t smallLength = ReadSByte();

				if (smallLength == 0) {
					return L"";
				}

				int length{};
				std::wstring retString{};
				if (smallLength > 0) // Unicode
				{
					uint16_t mask = 0xAAAA;
					if (smallLength == INT8_MAX) {
						ReadTemplate<int32_t>(&length);
					}
					else {
						length = static_cast<int32_t>(smallLength);
					}
					if (length <= 0) {
						return L"";
					}

					for (int i = 0; i < length; i++) {
						uint16_t encryptedChar = ReadTemplateAsCopy<uint16_t>();
						encryptedChar ^= mask;
						encryptedChar ^= static_cast<uint16_t>((WzKey[i * 2 + 1] << 8) + WzKey[i * 2]);
						retString += static_cast<wchar_t>(encryptedChar);
						mask++;
					}
				}
				else { // ASCII
					uint8_t mask = 0xAA;
					if (smallLength == INT8_MIN) {
						ReadTemplate<int32_t>(&length);
					}
					else {
						length = static_cast<int32_t>(-smallLength);
					}
					if (length <= 0) {
						return L"";
					}

					for (int i = 0; i < length; i++) {
						uint8_t encryptedChar = ReadByte();
						encryptedChar ^= mask;
						encryptedChar ^= (uint8_t)WzKey[i];
						retString += (char)encryptedChar;
						mask++;
					}
				}
				return retString;
			}

			std::vector<uint8_t> WzBinaryReader::ReadBytes(int length) {
				std::vector<uint8_t> ret(length);
				for (int i = 0; i < length; i++) {
					ret.push_back(fileBuffer[filePosition++]);
				}
				return ret;
			}

			template <typename T>
			T WzBinaryReader::ReadTemplateAsCopy() {
				T ret;
				memcpy(&ret, &fileBuffer[filePosition], sizeof(T));
				filePosition += sizeof(T);
				return ret;
			}
			template <typename T>
			void WzBinaryReader::ReadTemplate(T* obj) {
				memcpy(&obj, &fileBuffer[filePosition], sizeof(T));
				filePosition += sizeof(T);
			}

			std::wstring WzBinaryReader::ReadString(int length) {
				//return Encoding.ASCII.GetString(ReadBytes(length));
				auto secondBuffer = ReadBytes(length);
				//might be able to skip the middleman and directly initialize the wstring with the vector<uint8_t>?

				std::string buffer{ secondBuffer.begin(), secondBuffer.end() };

				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes(buffer);
			}

			std::wstring WzBinaryReader::ReadNullTerminatedString() {
				std::string retString{};
				uint8_t b = ReadByte();
				while (b != 0) {
					retString += (char)b;
					b = ReadByte();
				}
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				return converter.from_bytes(retString);
			}

			int32_t WzBinaryReader::ReadCompressedInt() {
				int8_t sb = ReadSByte();
				if (sb == INT8_MIN) {
					return ReadTemplateAsCopy<int32_t>();
				}
				return sb;
			}

			int64_t WzBinaryReader::ReadLong() {
				int8_t sb = ReadSByte();
				if (sb == INT8_MIN) {
					return ReadTemplateAsCopy<int64_t>();
				}
				return sb;
			}

			uint32_t WzBinaryReader::ReadOffset() {
				uint32_t offset = static_cast<uint32_t>(filePosition);
				offset = (offset - header.fstart) ^ UINT_MAX;
				offset *= Hash;
				offset -= CryptoLib::CryptoConstants::WZ_OffsetConstant;
				offset = Util::WzTool::RotateLeft(offset, static_cast<uint8_t>(offset & 0x1F));
				offset ^= ReadTemplateAsCopy<uint32_t>();
				offset += header.fstart * 2;
				return offset;
			}

			std::wstring WzBinaryReader::DecryptString(std::wstring& stringToDecrypt) {
				std::wstring outputString{ stringToDecrypt.begin(), stringToDecrypt.end() };
				for (int i = 0; i < stringToDecrypt.size(); i++) {
					outputString[i] ^= static_cast<wchar_t>((WzKey[i * 2 + 1] << 8) + WzKey[i * 2]);
				}
				return outputString;
			}

			std::wstring WzBinaryReader::DecryptNonUnicodeString(std::vector<wchar_t>& stringToDecrypt) {
				std::wstring outputString{ stringToDecrypt.begin(), stringToDecrypt.end() };
				for (int i = 0; i < stringToDecrypt.size(); i++) {
					outputString[i] ^= static_cast<wchar_t>(WzKey[i]);
					//outputString += (char)(stringToDecrypt[i] ^ WzKey[i]);
				}
				return outputString;
			}
		
			std::wstring WzBinaryReader::ReadStringBlock(uint32_t offset) {
				switch (ReadByte()) {
				case 0:
				case 0x73:
					return ReadString();
				case 1:
				case 0x1B:
					return ReadStringAtOffset(offset + ReadTemplateAsCopy<int32_t>());
				default:
					return L"";
				}
			}


		}
	}
}