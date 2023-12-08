#pragma once

#include <string>

namespace MapleLib {
    namespace WzLib {
        namespace WzProperties {
            class WzListEntry : public IWzObject {
            public:
                WzListEntry(std::wstring value) : value{ value } {}

            private:
                WzListFile parentFile;
            public:
                std::wstring value;
                IWzObject getParent() {
                    return parentFile;
                }
                void setParent(int value) { //idk
                   parentFile = (WzListFile)value;
                }

                std::wstring getWzValue() {
                    return value;
                }

                void Remove() {
                    parentFile.WzListEntries.Remove(this);
                }

                WzObjectType getObjectType() override {
                    return WzObjectType::List;
                }

                IWzFile getWzFileParent() override {return parentFile; }
                            
            };
        }
    }
}