#pragma once

#include "WzObject.h"
#include <list>
#include <string>
#include <locale>
#include <codecvt>

namespace MapleLib { namespace WzLib {

    /// An interface for wz img properties
    class WzImageProperty : public WzObject {
    public:
        std::list<WzImageProperty> WzProperties{};

        //i dont understand this, removes copy and move?
        //virtual new WzImageProperty this[string name] { get{ return null; } set{ throw new NotImplementedException(); } }

        virtual WzImageProperty GetFromPath(std::wstring path) {
            return null;
        }

        virtual WzPropertyType getPropertyType() = 0;

            /// <summary>
            /// The image that this property is contained in
            /// </summary>
        WzImage getParentImage() {
            WzObject ret = parent;
            while (parent != null) {
                if (ret is WzImage) return (WzImage)ret; //i gotta figure out how to fix this
                else ret = parent.parent;
            }
            return null;
        }

        WzObjectType getObjectType() override { return WzObjectType::Property; }

        void WriteValue(Util::WzBinaryWriter writer);

        virtual WzImageProperty DeepClone() = 0;

        virtual void SetValue(object value) = 0;

        void Remove() override {
            ((IPropertyContainer)Parent).RemoveProperty(this);
        }

        public virtual void ExportXml(StreamWriter writer, int level) {
            writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag(this.PropertyType.ToString(), name, true));
            writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::CloseTag(this.PropertyType.ToString()));
        }

        WzFile getWzFileParent() override {
            return getParentImage().getWzFileParent();
        }

        static void WritePropertyList(Util::WzBinaryWriter writer, std::list<WzImageProperty>& properties) {
            writer.Write((uint16_t)0);
            writer.WriteCompressedInt(properties.size());
            for (int i = 0; i < properties.size(); i++) {
                writer.WriteStringValue(properties[i].Name, 0x00, 0x01);
                if (properties[i] is WzExtended) { //is identifier
                    WriteExtendedValue(writer, (WzExtended)properties[i]);
                }
                else {
                    properties[i].WriteValue(writer);
                }
            }
        }

        static void DumpPropertyList(StreamWriter writer, int level, List<WzImageProperty> properties) {
            for(WzImageProperty& prop : properties) {
                prop.ExportXml(writer, level + 1);
            }
        }

        static std::list<WzImageProperty> ParsePropertyList(uint32_t offset, Util::WzBinaryReader reader, WzObject parent, WzImage parentImg)
        {
            int entryCount = reader.ReadCompressedInt();
            std::list<WzImageProperty> properties{};// = new List<WzImageProperty>(entryCount);
            for (int i = 0; i < entryCount; i++) {
                std::wstring name = reader.ReadStringBlock(offset);
                uint8_t ptype = reader.ReadByte();
                switch (ptype) {
                case 0:
                    properties.Add(new WzNullProperty(name){ Parent = parent });
                    break;
                case 11:
                case 2:
                    properties.Add(new WzShortProperty(name, reader.ReadInt16()){ Parent = parent });
                    break;
                case 3:
                case 19:
                    properties.Add(new WzIntProperty(name, reader.ReadCompressedInt()){ Parent = parent });
                    break;
                case 20:
                    properties.Add(new WzLongProperty(name, reader.ReadLong()){ Parent = parent });
                    break;
                case 4:
                    uint8_t type = reader.ReadByte();
                    if (type == 0x80) {
                        properties.Add(new WzFloatProperty(name, reader.ReadSingle()){ Parent = parent });
                    }
                    else if (type == 0) {
                        properties.Add(new WzFloatProperty(name, 0f){ Parent = parent });
                    }
                    break;
                case 5:
                    properties.Add(new WzDoubleProperty(name, reader.ReadDouble()){ Parent = parent });
                    break;
                case 8:
                    properties.Add(new WzStringProperty(name, reader.ReadStringBlock(offset)){ Parent = parent });
                    break;
                case 9:
                    int eob = (int)(reader.ReadUInt32() + reader.BaseStream.Position);
                    WzImageProperty exProp = ParseExtendedProp(reader, offset, eob, name, parent, parentImg);
                    properties.Add(exProp);
                    if (reader.BaseStream.Position != eob) reader.BaseStream.Position = eob;
                    break;
                default:
                    throw new Exception("Unknown property type at ParsePropertyList");
                }
            }
            return properties;
        }

        static WzExtended ParseExtendedProp(Util::WzBinaryReader reader, uint32_t offset, int endOfBlock, std::wstring name, WzObject parent, WzImage imgParent) {
            switch (reader.ReadByte()) {
                case 0x01:
                case 0x1B:
                    return ExtractMore(reader, offset, endOfBlock, name, reader.ReadStringAtOffset(offset + reader.ReadInt32()), parent, imgParent);
                case 0x00:
                case 0x73:
                    return ExtractMore(reader, offset, endOfBlock, name, "", parent, imgParent);
                default:
                    throw std::exception("Invlid byte read at ParseExtendedProp");
            }
        }

        static WzExtended ExtractMore(Util::WzBinaryReader reader, uint32_t offset, int eob, std::wstring name, std::wstring iname, WzObject parent, WzImage imgParent){
            if (iname == "") {
                iname = reader.ReadString();
            }
            if (iname == L"Property") {
                WzSubProperty subProp = new WzSubProperty(name){ Parent = parent };
                reader.BaseStream.Position += 2; // Reserved?
                subProp.AddProperties(WzImageProperty.ParsePropertyList(offset, reader, subProp, imgParent));
                return subProp;
            }
            else if (iname == L"Canvas") {
                //WzCanvasProperty canvasProp = new WzCanvasProperty(name){ Parent = parent }; //i need to decode this, inline object construction?
                WzCanvasProperty canvasProp{ name };
                reader.BaseStream.Position++;
                if (reader.ReadByte() == 1) {
                    reader.BaseStream.Position += 2;
                    canvasProp.AddProperties(WzImageProperty.ParsePropertyList(offset, reader, canvasProp, imgParent));
                }
                canvasProp.PngProperty = new WzPngProperty(reader, imgParent.parseEverything){ parent = canvasProp };
                return canvasProp;
            }
            else if (iname == L"Shape2D#Vector2D") {
                WzVectorProperty vecProp = new WzVectorProperty(name){ Parent = parent };
                vecProp.X = new WzIntProperty("X", reader.ReadCompressedInt()){ Parent = vecProp };
                vecProp.Y = new WzIntProperty("Y", reader.ReadCompressedInt()){ Parent = vecProp };
                return vecProp;
            }
            else if (iname == L"Shape2D#Convex2D") {
                WzConvexProperty convexProp = new WzConvexProperty(name){ Parent = parent };
                int convexEntryCount = reader.ReadCompressedInt();
                convexProp.WzProperties.Capacity = convexEntryCount;
                for (int i = 0; i < convexEntryCount; i++) {
                    convexProp.AddProperty(ParseExtendedProp(reader, offset, 0, name, convexProp, imgParent));
                }
            }
            else if (iname == L"Sound_DX8") {
                WzSoundProperty soundProp = new WzSoundProperty(name, reader, imgParent.parseEverything){ Parent = parent };
                return soundProp;
            }
            else if (iname == "UOL") {
                reader.BaseStream.Position++;
                switch (reader.ReadByte()) {
                case 0:
                    return new WzUOLProperty(name, reader.ReadString()){ Parent = parent };
                case 1:
                    return new WzUOLProperty(name, reader.ReadStringAtOffset(offset + reader.ReadInt32())){ Parent = parent };
                }
                throw std::exception("Unsupported UOL type");
            }
            else {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                throw std::exception("Unknown iname: " + converter.to_bytes(iname);
            }
        }

        static void WriteExtendedValue(Util::WzBinaryWriter writer, WzExtended property) {
            writer.Write((uint8_t)9);
            int64_t beforePos = writer.BaseStream.Position;
            writer.Write((Int32)0); // Placeholder
            property.WriteValue(writer);
            int len = (int)(writer.BaseStream.Position - beforePos);
            int64_t newPos = writer.BaseStream.Position;
            writer.BaseStream.Position = beforePos;
            writer.Write(len - 4);
            writer.BaseStream.Position = newPos;
        }

    };
}
}