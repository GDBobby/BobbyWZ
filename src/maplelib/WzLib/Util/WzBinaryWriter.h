#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace MapleLib {
	namespace WzLib {
		namespace Util {
			/*
			   TODO : Maybe WzBinaryReader/Writer should read and contain the hash (this is probably what's going to happen)
			*/
			class WzBinaryWriter {
			public:
				uint8_t[] WzKey{};
				uint32_t Hash{};
				Hashtable StringCache{};
				WzHeader Header{};
				bool LeaveOpen{};
				std::fstream output;

					
				WzBinaryWriter(std::fstream& output, std::vector<uint8_t>& WzIv) : WzBinaryWriter(output, WzIv, false) {}

				WzBinaryWriter(std::fstream& output, std::vector<uint8_t>& WzIv, bool leaveOpen) : output{output}, LeaveOpen{leaveOpen} {
					WzKey = WzKeyGenerator::GenerateWzKey(WzIv);
					StringCache = new Hashtable();
				}

				void WriteStringValue(std::wstring s, int withoutOffset, int withOffset) {
					if (s.length() > 4 && StringCache.ContainsKey(s)) {
						Write((uint8_t)withOffset);
						Write((int32_t)StringCache[s]);
					}
					else {
						Write((uint8_t)withoutOffset);
						int32_t sOffset = (int32_t)BaseStream.Position;
						Write(s);
						if (!StringCache.ContainsKey(s))
						{
							StringCache[s] = sOffset;
						}
					}
				}

				void WriteWzObjectValue(std::wstring s, uint8_t type) {
					std::string storeName = type + "_" + s;
					if (s.length() > 4 && StringCache.ContainsKey(storeName)) {
						Write((uint8_t)2);
						Write((int)StringCache[storeName]);
					}
					else {
						int sOffset = (int)(BaseStream.Position - Header.FStart);
						Write(type);
						Write(s);
						if (!StringCache.ContainsKey(storeName))
						{
							StringCache[storeName] = sOffset;
						}
					}
				}

				void Write(std::string value) override {
					if (value.length() == 0) {
						Write((uint8_t)0);
					}
					else {
						bool unicode = false;
						for (int i = 0; i < value.length(); i++) {
							if (value[i] > INT8_MAX)
							{
								unicode = true;
							}
						}

						if (unicode) {
							uint16_t mask = 0xAAAA;

							if (value.length() >= INT8_MAX) {
								// Bugfix - >= because if value.Length = MaxValue, MaxValue will be written and then treated as a long-length marker
								Write(INT8_MAX);
								Write(value.length());
							}
							else {
								Write((int8_t)value.length());
							}

							for (int i = 0; i < value.length(); i++)
							{
								uint16_t encryptedChar = (uint16_t)value[i];
								encryptedChar ^= (uint16_t)((WzKey[i * 2 + 1] << 8) + WzKey[i * 2]);
								encryptedChar ^= mask;
								mask++;
								Write(encryptedChar);
							}
						}
						else {// ASCII
							uint8_t mask = 0xAA;

							if (value.length() > INT8_MAX) {
								// Note - no need for >= here because of 2's complement (MinValue == -(MaxValue + 1))
								Write(INT8_MIN);
								Write(value.length());
							}
							else {
								Write((int8_t)(-value.length()));
							}

							for (int i = 0; i < value.length(); i++) {
								uint8_t encryptedChar = (uint8_t)value[i];
								encryptedChar ^= WzKey[i];
								encryptedChar ^= mask;
								mask++;
								Write(encryptedChar);
							}
						}
					}
				}

				void Write(std::string value, int length) {
					for (int i = 0; i < length; i++)
					{
						if (i < value.length())
						{
							Write(value[i]);
						}
						else
						{
							Write((uint8_t)0);
						}
					}
				}

				std::vector<wchar_t> EncryptString(std::wstring stringToDecrypt) {
					std::vector<wchar_t> outputChars;
					outputChars.resize(stringToDecrypt.length());
					for (int i = 0; i < stringToDecrypt.length(); i++) {
						outputChars[i] = (wchar_t)(stringToDecrypt[i] ^ ((wchar_t)((WzKey[i * 2 + 1] << 8) + WzKey[i * 2])));
					}
					return outputChars;
				}

				std::vector<wchar_t> EncryptNonUnicodeString(std::wstring stringToDecrypt) {
					std::vector<wchar_t> outputChars;
					outputChars.resize(stringToDecrypt.length());
					for (int i = 0; i < stringToDecrypt.length(); i++) {
						outputChars[i] = (wchar_t)(stringToDecrypt[i] ^ WzKey[i]);
					}
					return outputChars;
				}

				void WriteNullTerminatedString(std::string value) {
					for (int i = 0; i < value.length(); i++) {
						Write((uint8_t)value[i]);
					}
					Write((uint8_t)0);
				}

				void WriteCompressedInt(int value) {
					if (value > INT8_MAX || value <= INT8_MIN) {
						Write(INT8_MIN);
						Write(value);
					}
					else
					{
						Write((int8_t)value);
					}
				}

				void WriteCompressedLong(long value) {
					if (value > INT8_MAX || value <= INT8_MIN) {
						Write(INT8_MIN);
						Write(value);
					}
					else {
						Write((int8_t)value);
					}
				}

				void WriteOffset(uint32_t value) {
					uint32_t encOffset = (uint32_t)BaseStream.Position;
					encOffset = (encOffset - Header.FStart) ^ 0xFFFFFFFF;
					encOffset *= Hash;
					encOffset -= CryptoConstants.WZ_OffsetConstant;
					encOffset = RotateLeft(encOffset, (uint8_t)(encOffset & 0x1F));
					uint32_t writeOffset = encOffset ^ (value - (Header.FStart * 2));
					Write(writeOffset);
				}
				void Close() override {
					if (!LeaveOpen) {
						base.Close();
					}
				}

			private:
				uint32_t RotateLeft(uint32_t x, uint8_t n) {
					return (uint32_t)(((x) << (n)) | ((x) >> (32 - (n))));
				}
				uint32_t RotateRight(uint32_t x, uint8_t n) {
					return (uint32_t)(((x) >> (n)) | ((x) << (32 - (n))));
				}

				
			};
		}
	}
}