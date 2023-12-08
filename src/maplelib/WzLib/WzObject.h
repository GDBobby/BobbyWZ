#pragma once

#include <string>
#include <stdexcept>
#include "WzFile.h"
#include "WzObjectType.h"

namespace MapleLib {
    namespace WzLib {
        /// <summary>
        /// An abstract class for wz objects
        /// </summary>
        class WzObject : IDisposable {
        private:
            object tag = null;// Used in HaCreator to save already parsed images
            object tag2 = null;// Used in HaCreator's MapSimulator to save already parsed textures
            object tag3 = null;// Used in HaRepacker to save WzNodes

        public:

            //public abstract void Dispose(); explicit deconstruction while maintaining the object?
            std::wstring name;
            WzObjectType objectType;
            WzFile fileParent;
            WzObject parent;


            virtual std::wstring getName() = 0;
            virtual void setName() = 0;
            virtual WzObjectType getObjectType() = 0;
            virtual void getParent() = 0;
            virtual void setParent() = 0;
            virtual WzFile getWzFileParent() = 0; //bruh

            WzObject& at(const std::wstring& name) { //just overload this mfer? what is this is bullshit
                if (this is WzFile) {
                    return ((WzFile)this)[name];
                }
                else if (this is WzDirectory) {
                    return ((WzDirectory)this)[name];
                }
                else if (this is WzImage) {
                    return ((WzImage)this)[name];
                }
                else if (this is WzImageProperty) {
                    return ((WzImageProperty)this)[name];
                }
                else {
                    throw std::exception("not implemented)";
                }
            }

            std::wstring getFullPath() {
                if (this is WzFile) return ((WzFile)this).WzDirectory.Name;
                std::wstring ret = this.Name;
                WzObject currObj = *this;
                while (currObj.getParent() != null) {
                    currObj = currObj.getParent();
                    ret = currObj.name + L'\\' + ret;
                }
                return result;
            }
            void* getHCTag() {
                return tag;
            }
            void setHCTag(void* val) {
                tag = val;
            }
            void* getMSTag() {
                return tag2;
            }
            void setMSTag(void* val) {
                tag2 = val;
            }
            void* getHRTag() {
                return tag3;
            }
            void setHRTag(void* val) {
                tag3 = val;
            }
            virtual void* getWzValue() { return nullptr; }

            virtual void Remove() = 0;

            //Credits to BluePoop for the idea of using cast overriding
            //2015 - That is the worst idea ever, removed and replaced with Get* methods
            //-bobby - lmao whos writing this shit
            virtual int GetInt() { throw std::exception("not implemented"); return 0; }
            virtual int16_t GetShort() { throw std::exception("not implemented"); return 0; }
            virtual double GetDouble() { throw std::exception("not implemented"); return 0; }
            virtual float GetFloat() { throw std::exception("not implemented"); return 0; }
            virtual int64_t GetLong() { throw std::exception("not implemented"); return 0; }
            virtual std::wstring GetString() { throw std::exception("not implemented"); return 0; }

            virtual Point GetPoint() { throw std::exception("not implemented"); return 0; }
            virtual Bitmap GetBitmap() { throw std::exception("not implemented"); return 0; }
            virtual uint8_t* GetBytes() { throw std::exception("not implemented"); return 0; }

        };
    }
}