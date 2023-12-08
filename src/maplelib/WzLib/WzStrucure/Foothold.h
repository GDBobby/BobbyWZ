#pragma once

namespace MapleLib {
    namespace WzLib {
        namespace WzStructure {
            struct Foothold {
                int x1, x2, y1, y2;
                int num, layer;
                int prev{ 0 };
                int next{ 0 };

                Foothold(int x1, int x2, int y1, int y2, int num, int layer)
                    :
                    x1{x1}, x2{x2}, y1{y1}, y2{y2}, num{num}, layer{layer}
                {
                    next = 0;
                    prev = 0;
                }

                bool IsWall() {
                    return x1 == x2;
                }
            };
        }
    }
}