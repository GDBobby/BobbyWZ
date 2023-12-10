#pragma once

namespace MapleLib {
	namespace WzLib {
		struct Point {
			int x;
			int y;
		};
		struct Color {
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};
		struct Bitmap {
			//need some header shit
			//channels, colors, etc
			int width{ 0 };
			int height{ 0 };
			int bitDepth{ 32 }; //always RGBA?
			//i can probably change this uint8_t to a vector of Colors, uint8_t RGBA
			std::vector<uint8_t> data{};
		};

		enum class WzObjectType {
			File,
			Image,
			Directory,
			Property,
			List
		};
	}
}