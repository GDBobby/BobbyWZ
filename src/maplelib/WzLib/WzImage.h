#pragma once


#include <string>
#include <list>

#include "Util/WzBinaryReader.h"
#include "Util/WzTool.h"

namespace MapleLib {
    namespace WzLib {
        /// <summary>
        /// A .img contained in a wz directory
        /// </summary>
        class WzImage : public WzObject, IPropertyContainer {
        public:
            //TODO: nest wzproperties in a wzsubproperty inside of WzImage

            bool parsed = false;
            std::wstring name;
            int size, checksum;
            uint32_t offset = 0;
            Util::WzBinaryReader reader;
            std:list<WzImageProperty> properties = new List<WzImageProperty>();
            WzObject parent;
            int blockStart = 0;
            long tempFileStart = 0;
            long tempFileEnd = 0;
            bool changed = false;
            bool parseEverything = false;

            WzImage() {}
            WzImage(std::wstring name) : name{ name } {}
            WzImage(std::wstring name, Stream dataStream, WzMapleVersion mapleVersion) : 
                name{ name }, 
                reader{dataStream, Util::WzTool::GetIvByMapleVersion(mapleVersion) } 
            {}
            WzImage(std::wstring name, Util::WzBinaryReader reader) : 
                name{ name }, 
                reader{ reader }, 
                blockStart{(int)reader.BaseStream.Position}
            {}

            std::list<WzImageProperty> getProperties() {
                if (reader != null && !parsed) {
                    ParseImage();
                }
                return properties;
            }

            WzImage DeepClone() {
                if (reader != null && !parsed) { ParseImage(); }
                WzImage clone{ name };
                clone.changed = true;
                for (WzImageProperty& prop : properties) {
                    clone.AddProperty(prop.DeepClone());
                }
                return clone;
            }

                /// <summary>
                /// Gets a wz property by it's name
                /// </summary>
                /// <param name="name">The name of the property</param>
                /// <returns>The wz property with the specified name</returns>
                /// 
                
            WzImageProperty getWzPropertyByName(std::wstring name) {
                if (reader != null && !parsed) { ParseImage(); }
                for (WzImageProperty& iwp : properties)
                    if (iwp.Name.ToLower() == name.ToLower()) {
                        return iwp;
                    }
                return null;
            }
            void setWzPropertyByName(std::wstring name) {
                if (value != null) {
                    value.Name = name;
                    AddProperty(value);
                }
            }
                /// <summary>
                /// Gets a WzImageProperty from a path
                /// </summary>
                /// <param name="path">path to object</param>
                /// <returns>the selected WzImageProperty</returns>
            WzImageProperty GetFromPath(std::wstring path) {
                if (reader != null && !parsed) { ParseImage(); }

                std::vector<std::wstring> segments{};
                {
                    std::wstring segment{};

                    uint32_t begOffset = 0;
                    uint32_t nextOffset = 0;
                    while (begOffset < path.length()) {
                        //might need to do a little bit mroe here, but moving on
                        nextOffset = path.find_first_of(L'/');
                        segment = path.substr(begOffset, nextOffset - begOffset);
                        begOffset = nextOffset + 1;
                        if (!segment.empty()) {
                            segments.push_back(segment);
                        }
                    }
                }
                if (segments[0] == L"..") {
                    return null;
                }

                WzImageProperty ret = null;
                for (int x = 0; x < segments.size(); x++) {
                    bool foundChild = false;
                    if (ret == null) {
                        for (WzImageProperty& iwp : properties) {
                            if (iwp.Name == segments[x])
                            {
                                ret = iwp;
                                foundChild = true;
                                break;
                            }
                        }
                    }
                    else {
                        for (WzImageProperty& iwp : ret.WzProperties) {
                            if (iwp.Name == segments[x])
                            {
                                ret = iwp;
                                foundChild = true;
                                break;
                            }
                        }
                    }

                    if (!foundChild) {
                        return null;
                    }
                }
                return ret;
            }

            /// <summary>
            /// Adds a property to the image
            /// </summary>
            /// <param name="prop">Property to add</param>
            void AddProperty(WzImageProperty prop){
                prop.Parent = this;
                if (reader != null && !parsed) { ParseImage() };
                //properties.Add(prop);
                properties.push(prop);
            }
            void AddProperties(std::list<WzImageProperty> props)
            {
                for(WzImageProperty prop : props) {
                    AddProperty(prop);
                }
            }
            /// <summary>
            /// Removes a property by name
            /// </summary>
            /// <param name="name">The name of the property to remove</param>
            void RemoveProperty(WzImageProperty prop)
            {
                if (reader != null && !parsed) {
                    ParseImage();
                }
                prop.Parent = null;
                //properties.Remove(prop);
                properties.erase(prop);
            }
            void ClearProperties() {
                for (WzImageProperty prop : properties) { prop.Parent = null; }
                properties.clear();
            }

            void Remove() override {
                ((WzDirectory)Parent).RemoveImage(this);
            }
                /// <summary>
                /// Parses the image from the wz filetod
                /// </summary>
                /// <param name="wzReader">The BinaryReader that is currently reading the wz file</param>
            void ParseImage(bool parseEverything) {
                if (parsed) return;
                else if (changed) { parsed = true; return; }
                this->parseEverything = parseEverything;
                long originalPos = reader.BaseStream.Position;
                reader.BaseStream.Position = offset;
                uint8_t b = reader.ReadByte();
                if (b != 0x73 || reader.ReadString() != "Property" || reader.ReadUInt16() != 0) {
                    return;
                }
                properties.AddRange(WzImageProperty.ParsePropertyList(offset, reader, this, this));
                parsed = true;
            }

            /// <summary>
            /// Parses the image from the wz filetod
            /// </summary>
            /// <param name="wzReader">The BinaryReader that is currently reading the wz file</param>
            void ParseImage() {
                if (parsed) return;
                else if (changed) { parsed = true; return; }
                parseEverything = false;
                long originalPos = reader.BaseStream.Position;
                reader.BaseStream.Position = offset;
                uint8_t b = reader.ReadByte();
                if ((b != 0x73) || (reader.ReadString() != "Property") || (reader.ReadUInt16() != 0)) {
                    return;
                }
                properties.AddRange(WzImageProperty.ParsePropertyList(offset, reader, this, this));
                parsed = true;
            }

            uint8_t* getDataBlock() {
                if (reader != null && size > 0) {
                    uint8_t* blockData = reader.ReadBytes(size);
                    reader.BaseStream.Position = blockStart;
                    return blockData;
                }
                return nullptr;
            }

                public void UnparseImage() {
                    parsed = false;
                    properties.clear();
                }

                void SaveImage(WzBinaryWriter writer) {
                    if (changed) {
                        if (reader != null && !parsed) { ParseImage(); }
                        WzSubProperty imgProp = new WzSubProperty();
                        long startPos = writer.BaseStream.Position;
                        imgProp.AddProperties(WzProperties);
                        imgProp.WriteValue(writer);
                        writer.StringCache.Clear();
                        size = (int)(writer.BaseStream.Position - startPos);
                    }
                    else
                    {
                        long pos = reader.BaseStream.Position;
                        reader.BaseStream.Position = offset;
                        writer.Write(reader.ReadBytes(size));
                        reader.BaseStream.Position = pos;
                    }
                }

                void ExportXml(StreamWriter writer, bool oneFile, int level) {
                    if (oneFile) {
                        writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag(L"WzImage", name, true));
                        WzImageProperty.DumpPropertyList(writer, level, WzProperties);
                        writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::CloseTag("WzImage"));
                    }
                    else {
                        //replace this error asap
                        throw new std::exception("Under Construction"); //bruh
                    }
                }
        };
    }
}