#pragma once

#include <vector>

namespace MapleLib{
	namespace CryptoLib {
		
		/// <summary>
		/// Class to handle the MapleStory Custom Encryption routines
		/// </summary>
		class MapleCustomEncryption {
		public:
			/// Encrypt data using MapleStory's Custom Encryption
			static void Encrypt(std::vector<uint8_t>& data);

			/// Decrypt data using MapleStory's Custom Encryption
			static void Decrypt(std::vector<uint8_t>& data);

			/// Rolls a byte left
			static uint8_t rol(uint8_t val, int num);

			/// Rolls a byte right
			static uint8_t ror(uint8_t val, int num);
		};
	}
}
	