#pragma once
namespace MapleLib {
    namespace WzLib {
        class WzImageResource : IDisposable {
        public:
            bool parsed;
            WzImage img;
            WzImageResource(WzImage img) :img{ img }, parsed{ img.parsed } {
                if (!parsed) {
                    //this image or parameter image? bruh
                    img.ParseImage();
                }
            }

            public void Dispose() {
                if (!parsed) {
                    img.UnparseImage();
                }
            }
        };
    }
}