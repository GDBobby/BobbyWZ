#pragma once

#include <array>
#include <string>
#include <utility>
#include <unordered_map>

namespace MapleLib {
    namespace WzLib {
        namespace WzStructure {
            namespace Data {

                namespace Tables {

                    static const std::unordered_map<std::wstring, std::string> PortalTypeNames;

                    static const std::vector<std::string> BackgroundTypeNames;
                };

                namespace WzConstants {
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