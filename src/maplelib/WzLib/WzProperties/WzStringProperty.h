#pragma once


#include <string>

#include "../Util/WzBinaryWriter.h"
#include "../Util/XmlUtil.h"
#include "../WzImageProperty.h"

namespace MapleLib {
	namespace WzLib {
		namespace WzProperties {
			/// <summary>
			/// A property with a string as a value
			/// </summary>
			class WzStringProperty : WzImageProperty {
			public:
				WzStringProperty() { }
				WzStringProperty(std::wstring name) : WzImageProperty{ name } {}
				WzStringProperty(std::wstring name, std::wstring value) : WzImageProperty{ name }, val{ value } {}
				std::wstring val;
				//internal WzImage imgParent;
				std::wstring getName() override { return name; }
				void setName(std::wstring& name) override { this->name = name; }
				WzPropertyType getPropertyType() override { return WzPropertyType::String; }

				//object getWzValue() override { return value; }

				/*
				WzImageProperty DeepClone() override {
					return WzStringProperty{ name, val };
				}
				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.Write((uint8_t)8);
					writer.WriteStringValue(val, 0, 1);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair(L"WzString", name, val));
				}
				*/
				/// <summary>
				/// Disposes the object
				/// </summary>



				std::wstring GetString() override {return val;}

				//std::wstring ToString() override{ return val; }
			};
		}
	}
}