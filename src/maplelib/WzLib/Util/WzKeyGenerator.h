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
				static uint8_t* GenerateKeyFromZlz(std::string pathToZlz) {
					std::ifstream zlzStream{ pathToZlz };

					//FileStream zlzStream = File.OpenRead(pathToZlz);
					uint8_t* wzKey = GenerateWzKey(GetIvFromZlz(zlzStream), GetAesKeyFromZlz(zlzStream));
					zlzStream.close();
					return wzKey;
				}

				static uint8_t* GetIvFromZlz(std::ifstream& zlzStream) {
					uint8_t iv[4];// = new byte[4];

					zlzStream.seekg(0x10040, std::ios::beg);
					//zlzStream.Seek(0x10040, SeekOrigin.Begin);
					zlzStream.read((char*)(&iv[0]), 4);
					//zlzStream.Read(iv, 0, 4);
					return iv;
				}
			private:
				static uint8_t* GetAesKeyFromZlz(std::ifstream& zlzStream) {
					uint8_t aes[32];// = new byte[32];

					zlzStream.seekg(0x10060, std::ios::beg);
					for (int i = 0; i < 8; i++) {
						zlzStream.read(reinterpret_cast<char*>(&aes[i * 4]), 4);
						zlzStream.seekg(12, std::ios::cur);
					}
					return aes;
				}
			public:
				static uint8_t* GenerateWzKey(uint8_t* WzIv) {
					return GenerateWzKey(WzIv, MapleLib::CryptoLib::CryptoConstants::getTrimmedUserKey());
				}

				static uint8_t* GenerateWzKey(uint8_t* WzIv, uint8_t* AesKey) {
					if (((int32_t*)WzIv)[0] == 0) {
						return new uint8_t[UINT16_MAX]; //memory leak here if i dont fix this. need to replace it iwth a vector or something
					}

					// Set up AES cipher with a 256-bit key and ECB mode
					CryptoPP::SecByteBlock key(AesKey, 32); // 256 bits = 32 bytes
					CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption encryptor;
					encryptor.SetKey(key, key.size());

					uint8_t* input = CryptoLib::MapleCrypto::multiplyBytes(WzIv, 4, 4);
					uint8_t wzKey[UINT16_MAX];

					//for (int i = 0; i < (wzKey.Length / 16); i++) ??
					for (int i = 0; i < (UINT16_MAX / 16); i++) {
						encryptor.ProcessData(wzKey + (i * 16), input, 16);
						input = wzKey + (i * 16);
					}
					encryptor.ProcessData(wzKey + (UINT16_MAX - 15), input, 16);

					delete[] input;

					return wzKey;
				}
			};
		}
	}
}