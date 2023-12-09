#pragma once

#include <cstdint>
#include <vector>
#include "AESEncryption.h"
#include "CryptoConstants.h"

namespace MapleLib { 
	namespace CryptoLib {
		/// Class to manage Encryption and IV generation
		class MapleCrypto {
		private:
			//IV used in the packet encryption
			std::vector<uint8_t> _IV;
			uint16_t _mapleVersion;

		public:

			MapleCrypto(std::vector<uint8_t>& IV, int16_t mapleVersion) : _IV{ IV }, _mapleVersion{ mapleVersion } {}

			/// Updates the current IV
			void updateIV() {
				_IV = getNewIV(_IV);
			}

			/// Encrypts data with AES and updates the IV
			void crypt(std::vector<uint8_t>& data) {
				aesCrypt(_IV, data, data.size());
				updateIV();
			}

			// The Old IV used to generate the new IV
			static std::vector<uint8_t> getNewIV(std::vector<uint8_t>& oldIv);

			/// Shuffle the bytes in the IV
			static std::vector<uint8_t> shuffle(uint8_t inputByte, std::vector<uint8_t>& start);

			/// Get a packet header for a packet being sent to the server
			std::vector<uint8_t> getHeaderToClient(int size);

			/// Get a packet header for a packet being sent to the client
			std::vector<uint8_t> getHeaderToServer(int size);

			/// Gets the length of a packet from the header
			static int getPacketLength(std::vector<uint8_t>& packetHeader);
			/// Gets the length of a packet from the header
			static int getPacketLength(int packetHeader);

			/// Checks to make sure the packet is a valid MapleStory packet
			bool checkPacketToServer(uint8_t* packet);

			/// repeats bytes
			static std::vector<uint8_t> multiplyBytes(std::vector<uint8_t>& input, int count, int mult);

		};
	}
}