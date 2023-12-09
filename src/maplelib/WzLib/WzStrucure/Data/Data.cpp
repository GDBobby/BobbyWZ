#include "Data.h"

namespace MapleLib {
    namespace WzLib {
        namespace WzStructure {
            namespace Data {
                const std::unordered_map<std::wstring, std::string> Tables::PortalTypeNames{ //put the def in the cpp
                    { L"sp", "Start Point"},
                    { L"pi", "Invisible" },
                    { L"pv", "Visible" },
                    { L"pc", "Collision" },
                    { L"pg", "Changable" },
                    { L"pgi", "Changable Invisible" },
                    { L"tp", "Town Portal" },
                    { L"ps", "Script" },
                    { L"psi", "Script Invisible" },
                    { L"pcs", "Script Collision" },
                    { L"ph", "Hidden" },
                    { L"psh", "Script Hidden" },
                    { L"pcj", "Vertical Spring" },
                    { L"pci", "Custom Impact Spring" },
                    { L"pcig", "Unknown (PCIG)" }
                };
                const std::vector<std::string> Tables::BackgroundTypeNames = { //def in cpp
                    "Regular",
                    "Horizontal Copies",
                    "Vertical Copies",
                    "H+V Copies",
                    "Horizontal Moving+Copies",
                    "Vertical Moving+Copies",
                    "H+V Copies, Horizontal Moving",
                    "H+V Copies, Vertical Moving"
                };
            }
        }
    }
}