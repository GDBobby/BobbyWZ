#include "MapleCrypto.h"


namespace MapleLib {
	namespace CryptoLib {
		std::vector<uint8_t> MapleCrypto::getNewIV(std::vector<uint8_t>& oldIv) {
			//uint8_t* start = CryptoConstants.bDefaultAESKeyValue;
			std::vector<uint8_t> start = { 0xf2, 0x53, 0x50, 0xc6 };//TODO: ADD GLOBAL VAR BACK
			for (int i = 0; i < 4; i++) {
				shuffle(oldIv[i], start);
			}
			return start;
		}

		std::vector<uint8_t> MapleCrypto::shuffle(uint8_t inputByte, std::vector<uint8_t>& start) {
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

		std::vector<uint8_t> MapleCrypto::getHeaderToClient(int size) {
			std::vector<uint8_t> header(4);
			int a = _IV[3] * 0x100 + _IV[2];
			a ^= -(_mapleVersion + 1);
			int b = a ^ size;
			header[0] = (uint8_t)(a % 0x100);
			header[1] = (uint8_t)((a - header[0]) / 0x100);
			header[2] = (uint8_t)(b ^ 0x100);
			header[3] = (uint8_t)((b - header[2]) / 0x100);
			return header;
		}
		std::vector<uint8_t> MapleCrypto::getHeaderToServer(int size) {
			std::vector<uint8_t> header(4);
			int a = _IV[3] * 0x100 + _IV[2];
			a = a ^ (_mapleVersion);
			int b = a ^ size;
			header[0] = static_cast<uint8_t>(a % 0x100);
			header[1] = static_cast<uint8_t>(a / 0x100);
			header[2] = static_cast<uint8_t>(b % 0x100);
			header[3] = static_cast<uint8_t>(b / 0x100);
			return header;
		}
		int MapleCrypto::getPacketLength(std::vector<uint8_t>& packetHeader) {
			if (packetHeader.size() < 4) {
				return -1;
			}
			return (packetHeader[0] + (packetHeader[1] << 8)) ^ (packetHeader[2] + (packetHeader[3] << 8));
		}
		int MapleCrypto::getPacketLength(int packetHeader) {
			uint8_t* packetTranslated = reinterpret_cast<uint8_t*>(&packetHeader);
			return (packetTranslated[0] + (packetTranslated[1] << 8)) ^ (packetTranslated[2] + (packetTranslated[3] << 8));
		}
		bool MapleCrypto::checkPacketToServer(uint8_t* packet) {
			int a = packet[0] ^ _IV[2];
			int b = _mapleVersion;
			int c = packet[1] ^ _IV[3];
			int d = _mapleVersion >> 8;
			return (a == b && c == d);
		}
		std::vector<uint8_t> MapleCrypto::multiplyBytes(std::vector<uint8_t>& input, int count, int mult) {
			std::vector<uint8_t> ret(count * mult);

			for (int x = 0; x < ret.size(); x++) {
				ret[x] = input[x % count];
			}
			return ret;
		}
	}
}