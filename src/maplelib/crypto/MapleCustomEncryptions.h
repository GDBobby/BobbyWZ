#pragma once

#include <vector>

namespace MapleLib{
	namespace CryptoLib {
		
		/// <summary>
		/// Class to handle the MapleStory Custom Encryption routines
		/// </summary>
		class MapleCustomEncryption {
		public:
			/// <summary>
			/// Encrypt data using MapleStory's Custom Encryption
			/// </summary>
			/// <param name="data">data to encrypt</param>
			/// <returns>Encrypted data</returns>
			static void Encrypt(std::vector<uint8_t> data) {
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

			/// <summary>
			/// Decrypt data using MapleStory's Custom Encryption
			/// </summary>
			/// <param name="data">data to decrypt</param>
			/// <returns>Decrypted data</returns>
			static void Decrypt(std::vector<uint8_t> data) {
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

			/// <summary>
			/// Rolls a byte left
			/// </summary>
			/// <param name="val">input byte to roll</param>
			/// <param name="num">amount of bits to roll</param>
			/// <returns>The left rolled byte</returns>
			static char rol(uint8_t val, int num) {
				int highbit;
				for (int i = 0; i < num; i++)
				{
					highbit = ((val & 0x80) != 0 ? 1 : 0);
					val <<= 1;
					val |= (char)highbit;
				}
				return val;
			}

			/// <summary>
			/// Rolls a byte right
			/// </summary>
			/// <param name="val">input byte to roll</param>
			/// <param name="num">amount of bits to roll</param>
			/// <returns>The right rolled byte</returns>
			static char ror(uint8_t val, int num){
				int lowbit;
				for (int i = 0; i < num; i++)
				{
					lowbit = ((val & 1) != 0 ? 1 : 0);
					val >>= 1;
					val |= (char)(lowbit << 7);
				}
				return val;
			}
		};
	}
}
	