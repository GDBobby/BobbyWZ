#include "AESEncryption.h"
namespace MapleLib {
	namespace CryptoLib {
		std::vector<uint8_t> aesCrypt(const std::vector<uint8_t>& IV, std::vector<uint8_t>& data, int length, const std::vector<uint8_t>& key) {
			// Create AES object with 256-bit key
			CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption encryptor;
			encryptor.SetKey(key.data(), 32); //32 is bytes, 256 bits

			int remaining = length;
			int llength = 0x5B0;
			int start = 0;

			while (remaining > 0) {
				uint8_t myIV[16]; // IV size for AES is 16 bytes
				for (int x = 0; x < 16; x++) {
					myIV[x] = IV[x % 4];
				}

				if (remaining < llength) {
					llength = remaining;
				}

				for (int x = start; x < (start + llength); x++) {
					if ((x - start) % 16 == 0) {
						encryptor.ProcessData(&data[x], myIV, 16);
					}
					data[x] ^= myIV[(x - start) % 16];
				}

				start += llength;
				remaining -= llength;
				llength = 0x5B4;
			}

			return data;
		}
	}
}