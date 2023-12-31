#pragma once

#include <string>
#include <stdexcept>
//#include "WzFile.h"
#include "WzObjectType.h"

namespace MapleLib {
    namespace WzLib {
        /// <summary>
        /// An abstract class for wz objects
        /// </summary>
        class WzObject {
        public:
            WzObject() {}
            WzObject(std::wstring& name) : name{ name } {}

        protected:
            void* tag{ nullptr };// Used in HaCreator to save already parsed images
            void* tag2{ nullptr };// Used in HaCreator's MapSimulator to save already parsed textures
            void* tag3{ nullptr };// Used in HaRepacker to save WzNodes

            std::wstring name;
            WzObjectType objectType;
        public:

            //public abstract void Dispose(); explicit deconstruction while maintaining the object?

            virtual std::wstring getName() {
                return name;
            }
            virtual void setName(std::wstring& name) {
                this->name = name;
            }
            virtual WzObjectType getObjectType() = 0;
            /*
            virtual std::wstring getFullPath() {
                * this is a WzDirectory overload
                if (objectType == WzObjectType::Directory) {
                    return ((WzFile*)this)->WzDirectory.Name; 
                }
                *
                std::wstring ret = getName();
                WzObject* currObj = this;
                while (currObj->getParent() != nullptr) {
                    currObj = currObj->getParent();
                    ret = currObj->name + L'\\' + ret;
                }
                return ret;
            }
        */
            virtual void* getWzValue() { return nullptr; }

            //Credits to BluePoop for the idea of using cast overriding
            //2015 - That is the worst idea ever, removed and replaced with Get* methods
            //-bobby - lmao whos writing this shit
            virtual int GetInt() { throw std::exception("not implemented"); return 0; }
            virtual int16_t GetShort() { throw std::exception("not implemented"); return 0; }
            virtual double GetDouble() { throw std::exception("not implemented"); return 0; }
            virtual float GetFloat() { throw std::exception("not implemented"); return 0; }
            virtual int64_t GetLong() { throw std::exception("not implemented"); return 0; }
            virtual std::wstring GetString() { throw std::exception("not implemented"); return 0; }

            virtual Point GetPoint() { throw std::exception("not implemented"); return {}; }
            virtual Bitmap GetBitmap() { throw std::exception("not implemented"); return {}; }
            virtual std::vector<uint8_t> GetBytes() { throw std::exception("not implemented"); return {}; }

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
        };
    }
}