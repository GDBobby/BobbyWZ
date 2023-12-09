#pragma once

#include "cryptopp890/modes.h"
#include "cryptopp890/aes.h"
#include "cryptopp890/filters.h"

#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace MapleLib {
	namespace CryptoLib {

		/// <summary>
		/// Class to handle the AES Encryption routines
		/// </summary>


			/// <summary>
			/// Encrypt data using MapleStory's AES algorithm
			/// </summary>
			/// <param name="IV">IV to use for encryption</param>
			/// <param name="data">Data to encrypt</param>
			/// <param name="length">Length of data</param>
			/// <returns>Crypted data</returns>
		static std::vector<uint8_t> aesCrypt(const std::vector<uint8_t>& IV, std::vector<uint8_t>& data, int length, const std::vector<uint8_t>& key = CryptoConstants::getTrimmedUserKey());
	}
}