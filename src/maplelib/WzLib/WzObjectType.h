#pragma once

namespace MapleLib {
	namespace WzLib {
		struct Point {
			int x;
			int y;
		};
		struct Bitmap {
			//need some header shit
			//channels, colors, etc
			int width;
			int height;
			void* data;
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