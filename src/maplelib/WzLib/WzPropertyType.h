#pragma once

namespace MapleLib {
	namespace WzLib {
		enum class WzPropertyType {
			Null,
			Short,
			Int,
			Long,
			Float,
			Double,
			String,

			SubProperty,
			Canvas,
			Vector,
			Convex,
			Sound,
			UOL,

			PNG
		};
	}
}