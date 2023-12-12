#pragma once

#include <list>

#include "../Util/ParseLine.h"
#include "../Util/WzBinaryWriter.h"
#include "../WzImageProperty.h"
#include "WzPngProperty.h"

namespace MapleLib {
	namespace WzLib {
		namespace WzProperties {
			/// <summary>
			/// A property that can contain sub properties and has one png image
			/// </summary>
			class WzCanvasProperty : WzImageProperty {
			public:	
				WzCanvasProperty() { }
				WzCanvasProperty(std::wstring name) : name{ name } { }

			private:
				WzPngProperty imageProp;
				std::wstring name;
				WzObject parent;
				//internal WzImage imgParent;
			public:
				void SetValue(object value) override {
					imageProp.SetValue(value);
				}

				WzImageProperty DeepClone() override {
					WzCanvasProperty clone{ name };
					for (WzImageProperty prop : properties) {
						clone.AddProperty(prop.DeepClone());
					}
					clone.imageProp = (WzPngProperty)imageProp.DeepClone();
					return clone;
				}

				somekindofenum getWzValue() override {
					return somekindofenum::PngProperty;
				}

				WzPropertyType getPropertyType() override { return WzPropertyType::Canvas; }

				const std::list<WzImageProperty> & override getWzProperties() { return properties; }


				/// <summary>
				/// Gets a wz property by it's name
				/// </summary>
				/// <param name="name">The name of the property</param>
				/// <returns>The wz property with the specified name</returns>
				WzImageProperty getWzPropertyByName(std::wstring name) override {

					if (name == L"PNG")
						return imageProp;
					foreach(WzImageProperty iwp in properties)
						if (iwp.Name.ToLower() == name.ToLower())
							return iwp;
					return null;
				}
				void setWzPropertyByName(std::wstring name) override {
					if (value != null) {
						if (name == "PNG")
						{
							imageProp = (WzPngProperty)value;
							return;
						}
						value.Name = name;
						AddProperty(value);
					}

				}

				WzImageProperty GetProperty(std::wstring name) {
					for (WzImageProperty iwp : properties)
						if (iwp.Name.ToLower() == name.ToLower()) {
							return iwp;
						}
					return null;
				}

				/// Gets a wz property by a path name
				/// </summary>
				/// <param name="path">path to property</param>
				/// <returns>the wz property with the specified name</returns>
				WzImageProperty GetFromPath(std::wstring path) override {
					std::vector<std::wstring> segments{ Util::ParseLines(path) };
					if (segments[0] == L"..") {
						return ((WzImageProperty)Parent)[path.Substring(name.IndexOf('/') + 1)];
					}
					WzImageProperty ret = this;
					for (int x = 0; x < segments.Length; x++) {
						bool foundChild = false;
						if (segments[x] == "PNG") {
							return imageProp;
						}
						for (WzImageProperty& iwp : ret.WzProperties) {
							if (iwp.Name == segments[x]) {
								ret = iwp;
								foundChild = true;
								break;
							}
						}
						if (!foundChild) {
							return null;
						}
					}
					return ret;
				}
				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.WriteStringValue("Canvas", 0x73, 0x1B);
					writer.Write((uint8_t)0);
					if (properties.size() > 0)
					{
						writer.Write((uint8_t)1);
						WzImageProperty.WritePropertyList(writer, properties);
					}
					else
					{
						writer.Write((uint8_t)0);
					}
					writer.WriteCompressedInt(PngProperty.Width);
					writer.WriteCompressedInt(PngProperty.Height);
					writer.WriteCompressedInt(PngProperty.format);
					writer.Write((uint8_t)PngProperty.format2);
					writer.Write((int32_t)0);
					uint8_t* bytes = PngProperty::GetCompressedBytes(false);
					writer.Write(bytes.Length + 1);
					writer.Write((uint8_t)0);
					writer.Write(bytes);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag("WzCanvas", name, false, false) +
						Util::XmlUtil::Attrib("width", PngProperty.Width.ToString()) +
						Util::XmlUtil::Attrib("height", PngProperty.Height.ToString(), true, false));

					WzImageProperty::DumpPropertyList(writer, level, this->WzProperties);
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::CloseTag("WzCanvas"));
				}

				void AddProperty(WzImageProperty prop) {
					prop.Parent = this;
					properties.Add(prop);
				}
				void AddProperties(List<WzImageProperty> props) {
					for (WzImageProperty prop : props) {
						AddProperty(prop);
					}
				}


				Bitmap GetBitmap() override {
					return imageProp::GetPNG(false);
				}
			};
		}
	}
}