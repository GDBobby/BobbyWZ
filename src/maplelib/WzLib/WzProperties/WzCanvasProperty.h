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
				WzCanvasProperty(std::wstring name) : WzImageProperty{ name } { }

			private:
				std::unique_ptr<WzPngProperty> imageProp;
				//internal WzImage imgParent;
			public:
				void setName(std::wstring& name) { this->name = name; }
				std::wstring getName() { return name; }
				/*
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
				*/

				WzPropertyType getPropertyType() override { return WzPropertyType::Canvas; }

				/*
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
				*/


				Bitmap GetBitmap() override {
					return imageProp->GetPNG(false);
				}
			};
		}
	}
}