#pragma once

#include <cryptopp890/aes.h>
#include <cryptopp890/modes.h>
#include <cryptopp890/filters.h>

#include <fstream>
#include <cstdint>
#include <string>
#include <stdexcept>

#include "../../crypto/CryptoConstants.h"
#include "../../crypto/MapleCrypto.h"
#include "../../helpers/ErrorLog.h"

namespace MapleLib {
	namespace WzLib {
		namespace Util {
			class WzKeyGenerator {
					/// <summary>
					/// Generates the wz key used in the encryption from ZLZ.dll
					/// </summary>
					/// <param name="pathToZlz">Path to ZLZ.dll</param>
					/// <returns>The wz key</returns>
			public:
				static std::vector<uint8_t> GenerateKeyFromZlz(std::string pathToZlz) {
					std::ifstream zlzStream{ pathToZlz };

					//FileStream zlzStream = File.OpenRead(pathToZlz);
					auto ivZLZ = GetIvFromZlz(zlzStream);
					auto aesZLZ = GetAesKeyFromZlz(zlzStream);

					std::vector<uint8_t> wzKey = GenerateWzKey(ivZLZ, aesZLZ);
					zlzStream.close();
					return wzKey;
				}

				static std::vector<uint8_t> GetIvFromZlz(std::ifstream& zlzStream) {
					std::vector<uint8_t> iv(4);// = new byte[4];
					if (!zlzStream.is_open()) {
						printf("failed to open zlz stream \n");
						throw std::exception("failed to open file");
					}

					zlzStream.seekg(0x10040, std::ios::beg);
					//zlzStream.Seek(0x10040, SeekOrigin.Begin);
					zlzStream.read((char*)(&iv[0]), 4);
					//zlzStream.Read(iv, 0, 4);
					return iv;
				}
			private:
				static std::vector<uint8_t> GetAesKeyFromZlz(std::ifstream& zlzStream) {
					std::vector<uint8_t> aes(32);// = new byte[32];

					zlzStream.seekg(0x10060, std::ios::beg);
					for (int i = 0; i < 8; i++) {
						zlzStream.read(reinterpret_cast<char*>(&aes[i * 4]), 4);
						zlzStream.seekg(12, std::ios::cur);
					}
					return aes;
				}
			public:
				static std::vector<uint8_t> GenerateWzKey(std::vector<uint8_t>& WzIv) {
					return GenerateWzKey(WzIv, MapleLib::CryptoLib::CryptoConstants::getTrimmedUserKey());
				}

				static std::vector<uint8_t> GenerateWzKey(std::vector<uint8_t>& WzIv, std::vector<uint8_t> AesKey) {
					if (*reinterpret_cast<const int*>(&WzIv[0]) == 0) {
						return std::vector<uint8_t>(UINT16_MAX, 0); // Assuming ushort.MaxValue is equivalent to C#'s ushort.MaxValue
					}

					CryptoPP::AES::Encryption aesEncryption(&AesKey[0], CryptoPP::AES::DEFAULT_KEYLENGTH);
					CryptoPP::ECB_Mode_ExternalCipher::Encryption ecbEncryption(aesEncryption);

					std::vector<uint8_t> input = CryptoLib::MapleCrypto::multiplyBytes(WzIv, 4, 4);
					std::vector<uint8_t> wzKey(UINT16_MAX);

					for (size_t i = 0; i < wzKey.size() / 16; i++) {
						ecbEncryption.ProcessData(&input[0], &input[0], 16);
						std::copy(input.begin(), input.begin() + 16, wzKey.begin() + (i * 16));
					}

					return wzKey;
				}
			};
		}
	}
}