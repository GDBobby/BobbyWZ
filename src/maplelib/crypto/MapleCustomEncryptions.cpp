#include "MapleCustomEncryptions.h"

namespace MapleLib {
	namespace CryptoLib {
		void MapleCustomEncryption::Encrypt(std::vector<uint8_t>& data) {
			int size = data.size();
			int j;
			char a, c;
			for (int i = 0; i < 3; i++)
			{
				a = 0;
				for (j = size; j > 0; j--)
				{
					c = data[size - j];
					c = rol(c, 3);
					c = (char)(c + j);
					c ^= a;
					a = c;
					c = ror(a, j);
					c ^= 0xFF;
					c += 0x48;
					data[size - j] = c;
				}
				a = 0;
				for (j = data.size(); j > 0; j--)
				{
					c = data[j - 1];
					c = rol(c, 4);
					c = (char)(c + j);
					c ^= a;
					a = c;
					c ^= 0x13;
					c = ror(c, 3);
					data[j - 1] = c;
				}
			}
		}
	

		void MapleCustomEncryption::Decrypt(std::vector<uint8_t>& data) {
			int size = data.size();
			int j;
			char a, b, c;
			for (int i = 0; i < 3; i++) {
				a = 0;
				b = 0;
				for (j = size; j > 0; j--) {
					c = data[j - 1];
					c = rol(c, 3);
					c ^= 0x13;
					a = c;
					c ^= b;
					c = (char)(c - j); // Guess this is supposed to be right?
					c = ror(c, 4);
					b = a;
					data[j - 1] = c;
				}
				a = 0;
				b = 0;
				for (j = size; j > 0; j--)
				{
					c = data[size - j];
					c -= 0x48;
					c ^= 0xFF;
					c = rol(c, j);
					a = c;
					c ^= b;
					c = (char)(c - j); // Guess this is supposed to be right?
					c = ror(c, 3);
					b = a;
					data[size - j] = c;
				}
			}
		}

		uint8_t MapleCustomEncryption::rol(uint8_t val, int num) {
			int highbit;
			for (int i = 0; i < num; i++) {
				highbit = ((val & 0x80) != 0 ? 1 : 0);
				val <<= 1;
				val |= (uint8_t)highbit;
			}
			return val;
		}
		uint8_t MapleCustomEncryption::ror(uint8_t val, int num) {
			int lowbit;
			for (int i = 0; i < num; i++) {
				lowbit = ((val & 1) != 0 ? 1 : 0);
				val >>= 1;
				val |= (uint8_t)(lowbit << 7);
			}
			return val;
		}
	}
}