#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace MapleLib {
	namespace WzLib {
		namespace Util {
			std::vector<std::wstring> ParseLines(std::wstring const& theString){
				std::wstring ret{};
				std::wstring segment{};

				uint32_t begOffset = 0;
				uint32_t nextOffset = 0;
				while (begOffset < theString.length()) {
					//might need to do a little bit mroe here, but moving on
					nextOffset = theString.find_first_of(L'/');
					segment = theString.substr(begOffset, nextOffset - begOffset);
					begOffset = nextOffset + 1;
					if (!segment.empty()) {
						ret.append(segment);
					}
				}
			}
		}
	}
}