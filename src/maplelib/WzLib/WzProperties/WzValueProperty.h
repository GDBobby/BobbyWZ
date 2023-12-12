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
			/// A property that has the value of a float
			/// </summary>
			template<typename T>
			class WzValueProperty : WzImageProperty {

			protected:
				WzValueProperty() { }
				WzValueProperty(std::wstring name) : WzImageProperty{ name } {}
				WzValueProperty(std::wstring name, T value) : WzImageProperty{ name }, val{ value } { }
			public:
				T val;
				//internal WzImage imgParent;
				std::wstring getName() override {
					return name;
				}
				void setName(std::wstring& name) override {
					this->name = name;
				}

				template<typename Ti>
				void SetValue(Ti value) {
					val = static_cast<T>(value);
				}
				virtual WzPropertyType getPropertyType() = 0;
				//virtual WzImageProperty DeepClone() = 0;
				
				void* getWzValue() { return &val; }

				//virtual void WriteValue(Util::WzBinaryWriter writer) = 0;

				//virtual void ExportXml(StreamWriter writer, int level) = 0;

			public:
				float GetFloat() override { return static_cast<float>(val)); }
				double GetDouble() override { return static_cast<double>(val); }
				int GetInt() override { return static_cast<int>(val); }
				int16_t GetShort() override { return static_cast<int16_t>(val); }
				int64_t GetLong() override { return static_cast<int64_t>(val); }
				std::wstring ToString() override { return std::to_wstring(val); }
			};

			class WzFloatProperty : public WzValueProperty<float> {
			public:
				WzFloatProperty() : WzValueProperty<float>() {}
				WzFloatProperty(std::wstring name) : WzValueProperty<float>(name) {}
				WzFloatProperty(std::wstring name, float val) : WzValueProperty<float>(name, val) {}

				WzPropertyType getPropertyType() override { return WzPropertyType::Float; }
				/*
				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.Write((uint8_t)4);
					if (val == 0.f) {
						writer.Write((uint8_t)0);
					}
					else {
						writer.Write((uint8_t)0x80);
						writer.Write(val);
					}
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair(L"WzByteFloat", name, std::to_string(val)));
				}
				WzImageProperty DeepClone() override {
					return WzFloatProperty(name, val);
				}
				*/
			};
			class WzDoubleProperty : public WzValueProperty<double> {
			public:
				WzDoubleProperty() : WzValueProperty<double>() {}
				WzDoubleProperty(std::wstring name) : WzValueProperty<double>(name) {}
				WzDoubleProperty(std::wstring name, double val) : WzValueProperty<double>(name, val) {}

				WzPropertyType getPropertyType() override { return WzPropertyType::Double; }
				/*
				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.Write((uint8_t)5);
					writer.Write(val);
				}
				void ExportXml(std::fstream& writer, int level) override {
					writer << (Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair("WzDouble", name), std::to_string(val));
				}
				WzImageProperty DeepClone() override {
					return WzDoubleProperty(name, val);
				}
				*/
			};
			class WzIntProperty : public WzValueProperty<int> {
			public:
				WzIntProperty() : WzValueProperty<int>() {}
				WzIntProperty(std::wstring name) : WzValueProperty<int>(name) {}
				WzIntProperty(std::wstring name, int val) : WzValueProperty<int>(name, val) {}

				WzPropertyType getPropertyType() override { return WzPropertyType::Int; }
				//void WriteValue(Util::WzBinaryWriter writer) override {
				//	writer.Write((uint8_t)3);
				//	writer.WriteCompressedInt(val);
				//}
				//void ExportXml(StreamWriter writer, int level) override {
				//	writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair("WzCompressedInt", name, std::to_string(val)));
				//}
				//WzImageProperty DeepClone() override {
				//	return WzIntProperty(name, val);
				//}
			};
			class WzLongProperty : public WzValueProperty<int64_t> {
			public:
				WzLongProperty() : WzValueProperty<int64_t>() {}
				WzLongProperty(std::wstring name) : WzValueProperty<int64_t>(name) {}
				WzLongProperty(std::wstring name, int64_t val) : WzValueProperty<int64_t>(name, val) {}

				WzPropertyType getPropertyType() override { return WzPropertyType::Long; }
				//void WriteValue(Util::WzBinaryWriter writer) override {
				//	writer.Write((uint8_t)20);
				//	writer.WriteCompressedLong(val);
				//}
				//void ExportXml(StreamWriter writer, int level) override {
				//	writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair("WzLong", name, std::to_string(val)));
				//}
				//WzImageProperty DeepClone() override {
				//	return WzLongProperty(name, val);
				//}
			};
			class WzShortProperty : public WzValueProperty<int16_t> {
			public:
				WzShortProperty() : WzValueProperty<int16_t>() {}
				WzShortProperty(std::wstring name) : WzValueProperty<int16_t>(name) {}
				WzShortProperty(std::wstring name, int16_t val) : WzValueProperty<int16_t>(name, val) {}

				WzPropertyType getPropertyType() override { return WzPropertyType::Short; }
				//void WriteValue(Util::WzBinaryWriter writer) override {
				//	writer.Write((uint8_t)2);
				//	writer.Write(val);
				//}
				//void ExportXml(StreamWriter writer, int level) override {
				//	writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair("WzUnsignedShort", name, std::to_string(val)));
				//}
				//WzImageProperty DeepClone() override {
				//	return WzShortProperty(name, val);
				//}
			};

			
		}
	}
}