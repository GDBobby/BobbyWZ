#pragma once

#include "WzImage.h"

namespace MapleLib {
    namespace WzLib {
        class WzImageResource {
        public:
            bool parsed;
            WzImage img;
            WzImageResource(WzImage& img) :img{ img }, parsed{ img.parsed } {
                if (!parsed) {
                    //this image or parameter image? bruh
                    img.ParseImage();
                }
            }

            void Dispose() {
                if (!parsed) {
                    img.UnparseImage();
                }
            }
        };
    }
}