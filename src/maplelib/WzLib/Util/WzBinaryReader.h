#pragma once


//using System;
//using System.IO;
//using System.Text;
//using MapleLib.MapleCryptoLib;

#include <vector>
#include <cstdint>
#include <string>

#include "WzKeyGenerator.h"

namespace MapleLib {
	namespace WzLib {
		namespace Util { 
			class WzBinaryReader {// : BinaryReader { what is binary reader
			public:
				std::vector<uint8_t> WzKey{};
				uint32_t Hash{};
				WzHeader Header{};

				WzBinaryReader(Stream input, std::vector<uint8_t>& WzIv) : base(input) {
					WzKey = WzKeyGenerator.GenerateWzKey(WzIv);
				}

				std::string ReadStringAtOffset(int64_t Offset) {
					return ReadStringAtOffset(Offset, false);
				}

				 std::string ReadStringAtOffset(int64_t Offset, bool readByte) {
					int64_t CurrentOffset = BaseStream.Position;
					BaseStream.Position = Offset;
					if (readByte) {
						ReadByte();
					}
					std::string ReturnString = ReadString();
					BaseStream.Position = CurrentOffset;
					return ReturnString;
				}

				std::string ReadString() override {
					int8_t smallLength = base.ReadSByte();

					if (smallLength == 0) {
						return "";
					}

					int length{};
					std::string retString{};
					if (smallLength > 0) // Unicode
					{
						uint16_t mask = 0xAAAA;
						if (smallLength == sbyte.MaxValue) {
							length = ReadInt32();
						}
						else {
							length = (int32_t)smallLength;
						}
						if (length <= 0) {
							return "";
						}

						for (int i = 0; i < length; i++) {
							uint16_t encryptedChar = ReadUInt16();
							encryptedChar ^= mask;
							encryptedChar ^= (uint16_t)((WzKey[i * 2 + 1] << 8) + WzKey[i * 2]);
							retString += ((char)encryptedChar);
							mask++;
						}
					}
					else { // ASCII
						uint8_t mask = 0xAA;
						if (smallLength == INT8_MIN) {
							length = ReadInt32();
						}
						else {
							length = (int32_t)(-smallLength);
						}
						if (length <= 0) {
							return "";
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

				/// <summary>
				/// Reads an ASCII string, without decryption
				/// </summary>
				/// <param name="filePath">Length of bytes to read</param>
				std::string ReadString(int length) {
					//return Encoding.ASCII.GetString(ReadBytes(length));
					return ReadString(ReadBytes(length));
				}

				std::string ReadNullTerminatedString() {
					std::string retString{};
					uint8_t b = ReadByte();
					while (b != 0) {
						retString += (char)b;
						b = ReadByte();
					}
					return retString;
				}

				int32_t ReadCompressedInt()
				{
					int8_t sb = base.ReadSByte();
					if (sb == INT8_MIN) {
						return ReadInt32();
					}
					return sb;
				}

				int64_t ReadLong() {
					int8_t sb = base.ReadSByte();
					if (sb == INT8_MIN) {
						return ReadInt64();
					}
					return sb;
				}

				uint32_t ReadOffset() {
					uint32_t offset = (uint32_t)BaseStream.Position;
					offset = (offset - Header.FStart) ^ UINT_MAX;
					offset *= Hash;
					offset -= CryptoConstants.WZ_OffsetConstant;
					offset = WzTool.RotateLeft(offset, (uint8_t)(offset & 0x1F));
					uint32_t encryptedOffset = ReadUInt32();
					offset ^= encryptedOffset;
					offset += Header.FStart * 2;
					return offset;
				}

				std::string DecryptString(std::vector<char>& stringToDecrypt) {
					std::string outputString = "";
					for (int i = 0; i < stringToDecrypt.size(); i++)
						outputString += (char)(stringToDecrypt[i] ^ ((char)((WzKey[i * 2 + 1] << 8) + WzKey[i * 2])));
					return outputString;
				}

				std::string DecryptNonUnicodeString(std::vector<char>& stringToDecrypt) {
					std::string outputString = "";
					for (int i = 0; i < stringToDecrypt.size(); i++) {
						outputString += (char)(stringToDecrypt[i] ^ WzKey[i]);
					}
					return outputString;
				}

				std::string ReadStringBlock(uint32_t offset)
				{
					switch (ReadByte())
					{
					case 0:
					case 0x73:
						return ReadString();
					case 1:
					case 0x1B:
						return ReadStringAtOffset(offset + ReadInt32());
					default:
						return "";
					}
				}

			};
		}
	}
}