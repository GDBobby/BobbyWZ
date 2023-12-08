#pragma once

#include <array>
#include <string>
#include <utility>
#include <unordered_map>

namespace MapleLib {
    namespace WzLib {
        namespace WzStructure {
            namespace Data {

                class Tables {
                public:
                    //static constexpr std::array<std::pair<wchar_t, std::wstring>, 5> characterMap{

                    static constexpr std::unordered_map<std::wstring, std::string> PortalTypeNames{ //put the def in the cpp
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

                    static std::vector<std::string> BackgroundTypeNames = { //def in cpp
                        "Regular",
                        "Horizontal Copies",
                        "Vertical Copies",
                        "H+V Copies",
                        "Horizontal Moving+Copies",
                        "Vertical Moving+Copies",
                        "H+V Copies, Horizontal Moving",
                        "H+V Copies, Vertical Moving"
                    };
                };

                class WzConstants {
                public:
                    static constexpr int MinMap = 0;
                    static constexpr int MaxMap = 999999999;
                };

                enum class QuestState {
                    Available = 0,
                    InProgress = 1,
                    Completed = 2
                };

            }
        }
    }
}