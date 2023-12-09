#pragma once

#include "WzObject.h"
#include "WzPropertyType.h"
#include "WzImage.h"
#include "Util/XmlUtil.h"

#include <list>
#include <string>
#include <locale>
#include <codecvt>

namespace MapleLib { namespace WzLib {

    /// An interface for wz img properties
    class WzImageProperty : public WzObject {
    protected:
        bool isExtended{ false };
    public:
        bool getExtended() { return isExtended; }

        //i dont understand this, removes copy and move?
        //virtual new WzImageProperty this[string name] { get{ return null; } set{ throw new NotImplementedException(); } }

        WzObject* at(const std::wstring& name) {
            return nullptr;
        }

        virtual WzPropertyType getPropertyType() = 0;

            /// <summary>
            /// The image that this property is contained in
            /// </summary>

        WzObjectType getObjectType() override { 
            return WzObjectType::Property; 
        }

        virtual void WriteValue(Util::WzBinaryWriter writer) = 0;

        virtual WzImageProperty DeepClone() = 0;

        virtual void SetValue(void* value) = 0;

    };
    }
}