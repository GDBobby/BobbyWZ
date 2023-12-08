#pragma once

#include <cstdint>
#include <vector>
#include "AESEncryption.h"
#include "CryptoConstants.h"

namespace MapleLib { 
	namespace CryptoLib {
		/// <summary>
		/// Class to manage Encryption and IV generation
		/// </summary>
		class MapleCrypto {
		private:

			uint8_t* _IV;//IV used in the packet encryption
			uint16_t _mapleVersion;

		public:

			/// <summary>
			/// (public) IV used in the packet encryption
			/// </summary>
			uint8_t* IV;
			/*
		{
			get { return _IV; }
			set { _IV = value; }
		}
		*/

			MapleCrypto(uint8_t* IV, int16_t mapleVersion) : _IV{ IV }, _mapleVersion{ mapleVersion } {}

			/// <summary>
			/// Updates the current IV
			/// </summary>
			void updateIV() {
				_IV = getNewIV(_IV);
			}

			/// Encrypts data with AES and updates the IV
			void crypt(std::vector<uint8_t>& data) {
				AESEncryption::aesCrypt(_IV, data.data(), data.size());
				updateIV();
			}

			// The Old IV used to generate the new IV
			static uint8_t* getNewIV(uint8_t* oldIv) {
				//uint8_t* start = CryptoConstants.bDefaultAESKeyValue;
				uint8_t start[4] = { 0xf2, 0x53, 0x50, 0xc6 };//TODO: ADD GLOBAL VAR BACK
				for (int i = 0; i < 4; i++) {
					shuffle(oldIv[i], start);
				}
				return start;
			}

			/// <summary>
			/// Shuffle the bytes in the IV
			/// </summary>
			/// <param name="inputByte">Byte of the old IV</param>
			/// <param name="start">The Default AES Key</param>
			/// <returns>The shuffled bytes</returns>
			static uint8_t* shuffle(uint8_t inputByte, uint8_t* start)
			{
				uint8_t a = start[1];
				uint8_t b = a;
				uint32_t c, d;
				b = CryptoConstants::bShuffle[b];
				b -= inputByte;
				start[0] += b;
				b = start[2];
				b ^= CryptoConstants::bShuffle[inputByte];
				a -= b;
				start[1] = a;
				a = start[3];
				b = a;
				a -= start[0];
				b = CryptoConstants::bShuffle[b];
				b += inputByte;
				b ^= start[2];
				start[2] = b;
				a += CryptoConstants::bShuffle[inputByte];
				start[3] = a;

				c = (uint32_t)(start[0] + start[1] * 0x100 + start[2] * 0x10000 + start[3] * 0x1000000);
				d = c;
				c >>= 0x1D;
				d <<= 0x03;
				c |= d;
				start[0] = (uint8_t)(c % 0x100);
				c /= 0x100;
				start[1] = (uint8_t)(c % 0x100);
				c /= 0x100;
				start[2] = (uint8_t)(c % 0x100);
				start[3] = (uint8_t)(c / 0x100);

				return start;
			}

			/// <summary>
			/// Get a packet header for a packet being sent to the server
			/// </summary>
			/// <param name="size">Size of the packet</param>
			/// <returns>The packet header</returns>
			uint8_t* getHeaderToClient(int size) {
				uint8_t header[4];
				int a = _IV[3] * 0x100 + _IV[2];
				a ^= -(_mapleVersion + 1);
				int b = a ^ size;
				header[0] = (uint8_t)(a % 0x100);
				header[1] = (uint8_t)((a - header[0]) / 0x100);
				header[2] = (uint8_t)(b ^ 0x100);
				header[3] = (uint8_t)((b - header[2]) / 0x100);
				return header;
			}

			/// Get a packet header for a packet being sent to the client

			uint8_t* getHeaderToServer(int size) {
				uint8_t header[4];
				int a = _IV[3] * 0x100 + _IV[2];
				a = a ^ (_mapleVersion);
				int b = a ^ size;
				header[0] = static_cast<uint8_t>(a % 0x100);
				header[1] = static_cast<uint8_t>(a / 0x100);
				header[2] = static_cast<uint8_t>(b % 0x100);
				header[3] = static_cast<uint8_t>(b / 0x100);
				return header;
			}

			/// <summary>
			/// Gets the length of a packet from the header
			/// </summary>
			/// <param name="packetHeader">Header of the packet</param>
			/// <returns>The length of the packet</returns>
			static int getPacketLength(int packetHeader) {
				return getPacketLength(BitConverter.GetBytes(packetHeader));
			}

			/// <summary>
			/// Gets the length of a packet from the header
			/// </summary>
			/// <param name="packetHeader">Header of the packet</param>
			/// <returns>The length of the packet</returns>
			static int getPacketLength(uint8_t* packetHeader) {
				if (packetHeader.Length < 4) {
					return -1;
				}
				return (packetHeader[0] + (packetHeader[1] << 8)) ^ (packetHeader[2] + (packetHeader[3] << 8));

			}

			/// <summary>
			/// Checks to make sure the packet is a valid MapleStory packet
			/// </summary>
			/// <param name="packetHeader">The header of the packet received</param>
			/// <returns>The packet is valid</returns>
			bool checkPacketToServer(uint8_t* packet) {
				int a = packet[0] ^ _IV[2];
				int b = _mapleVersion;
				int c = packet[1] ^ _IV[3];
				int d = _mapleVersion >> 8;
				return (a == b && c == d);
			}

			/// repeats bytes
			static uint8_t* multiplyBytes(uint8_t* input, int count, int mult) {
				std::vector<uint8_t> ret(count * mult);

				for (int x = 0; x < ret.size(); x++) {
					ret[x] = input[x % count];
				}
				return ret.data();
			}

		};
	}
}