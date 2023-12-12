#pragma once


#include "Util/WzBinaryReader.h"
#include "Util/WzTool.h"
#include "Util/ParseLine.h"
#include "WzProperties/WzSubProperty.h"


#include <string>
#include <list>
#include <cstdint>

namespace MapleLib {
    namespace WzLib {
        /// A .img contained in a wz directory
        class WzImage : public WzObject {
        public:
            //TODO: nest wzproperties in a wzsubproperty inside of WzImage
            WzImage() {}
            WzImage(std::wstring name) : WzObject{name} {}

            bool parsed = false;
            int size, checksum;
            uint32_t offset = 0;
            //std:list<WzImageProperty> properties{};
            int blockStart = 0;
            long tempFileStart = 0;
            long tempFileEnd = 0;
            bool changed = false;
            bool parseEverything = false;

            std::wstring getName() {
                return name;
            }
            void setName(std::wstring& name) {
                this->name = name;
            }
            WzObjectType getObjectType()  override {  
                //if (reader != nullptr) {
                //    if (!parsed) {
                //        ParseImage();
                //    }
                //}
                return WzObjectType::Image; 
            } 
            /*
            uint8_t* getDataBlock() {
                if (reader != null && size > 0) {
                    uint8_t* blockData = reader.ReadBytes(size);
                    reader.BaseStream.Position = blockStart;
                    return blockData;
                }
                return nullptr;
            }
            */

            void UnparseImage() {
                parsed = false;
            }

            void SaveImage(Util::WzBinaryWriter writer) {
                if (changed) {
                    //if (reader != null && !parsed) { ParseImage(); }
                    WzSubProperty imgProp{};
                    long startPos = writer.filePosition;
                    //imgProp.AddProperties(WzProperties);
                    //imgProp.WriteValue(writer);
                    writer.StringCache.Clear();
                    size = static_cast<int>(writer.filePosition - startPos);
                }
                else {
                    /* wtf is this?
                    long pos = reader.filePosition;
                    reader.filePosition = offset;
                    writer.Write(reader.ReadBytes(size));
                    reader.filePosition = pos;
                    */
                }
            }
        };
    }
}