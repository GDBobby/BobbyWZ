#pragma once

#include <cstdint>
#include <array>
#include <vector>

namespace MapleLib {
	namespace CryptoLib{
		/// <summary>
		/// Contains all the constant values used for various functions
		/// </summary>
		class CryptoConstants {
		public:
			/// AES UserKey used by MapleStory
			const static std::array<uint8_t, 128> UserKey;

			/// <summary>
			/// ShuffleBytes used by MapleStory to generate a new IV
			/// </summary>
			const static std::array<uint8_t, 256> bShuffle;

			/// <summary>
			/// Default AES Key used to generate a new IV
			/// </summary>
			const static uint8_t bDefaultAESKeyValue[];

			/// IV used to create the WzKey for GMS
			static std::vector<uint8_t> WZ_GMSIV() {
				return { 0x4D, 0x23, 0xC7, 0x2B };
			}
			/// IV used to create the WzKey for MSEA
			static std::vector<uint8_t>WZ_MSEAIV() {
				return {0xB9, 0x7D, 0x63, 0xE9};
			}

			/// Constant used in WZ offset encryption
			constexpr static uint32_t WZ_OffsetConstant = 0x581C3F6D;

			/// Trims the AES UserKey for use an AES cryptor
			static std::vector<uint8_t> getTrimmedUserKey();
		};
	}
}