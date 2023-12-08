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
				std::wstring name;
				std::wstring val;
				WzObject parent;
				//internal WzImage imgParent;
				void SetValue(object value) override {
					val = (std::wstring)value;
				}

				WzImageProperty DeepClone() override {
					return WzStringProperty{ name, val };
				}

				object getWzValue() override { return value; }

				WzPropertyType getPropertyType() override { return WzPropertyType::String; }

				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.Write((uint8_t)8);
					writer.WriteStringValue(val, 0, 1);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair(L"WzString", name, val));
				}
				/// <summary>
				/// Disposes the object
				/// </summary>
				void Dispose() override {
					name = null;
					val = null;
				}

				WzStringProperty() { }

				WzStringProperty(std::wstring name) : name{ name } {}
				WzStringProperty(std::wstring name, std::wstring value) : name{ name }, val{ value } {}

				std::wstring GetString() override {return val;}

				std::wstring ToString() override{ return val; }
			};
		}
	}
}