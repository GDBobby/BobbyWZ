#pragma once


#include "../Util/WzBinaryWriter.h"
#include "../Util/ParseLine.h"
#include "../Util/XmlUtil.h"
#include "../WzImageProperty.h"

#include <string>
#include <vector>
#include <cstdint>

namespace MapleLib {
	namespace WzLib {
		namespace WzProperties {

			/// <summary>
			/// A property that has the value of a double
			/// </summary>
			class WzDoubleProperty : WzImageProperty {
			public:
				std::wstring name;
				double val;
				WzObject parent;
				//internal WzImage imgParent;
				void SetValue(object value) override {
					val = (double)value;
				}

				public WzImageProperty DeepClone() override {
					return WzDoubleProperty(name, val);
				}

				public object getWzValue() override { return Value; }

				public WzPropertyType getPropertyType() override { return WzPropertyType::Double; }

				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.Write((uint8_t)5);
					writer.Write(val);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair(L"WzDouble", name, std::to_string(val)));
				}
				public WzDoubleProperty() { }

				public WzDoubleProperty(std::wstring name) : name{ name } {}
				/// <summary>
				/// Creates a WzDoubleProperty with the specified name and value
				/// </summary>
				/// <param name="name">The name of the property</param>
				/// <param name="value">The value of the property</param>
				public WzDoubleProperty(std::wstring name, double value) : name{ name }, val{ value } { }

				float GetFloat() override { return static_cast<double>(val); }

				double GetDouble() override { return val; }

				int GetInt() override { return static_cast<int>(val); }
				int16_t GetShort() override { return static_cast<int16_t>(val); }
				int64_t GetLong() override { return static_cast<int64_t>(val); }
				std::wstring ToString() override { return std::to_wstring(val); }
			};
		}
	}
}