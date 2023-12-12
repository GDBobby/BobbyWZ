#pragma once

#include <string>
#include <stdexcept>

#include "../WzImageProperty.h"


namespace MapleLib {
	namespace WzLib {
		namespace WzProperties {
			/// <summary>
			/// A property that's value is null
			/// </summary>
			class WzNullProperty : public WzImageProperty {
			public:

				WzNullProperty() { }
				WzNullProperty(std::wstring propName) : WzImageProperty{ name } {}

				std::wstring getName() {
					return name;
				}
				void setName(std::wstring& name) {
					this->name = name;
				}

				//internal WzImage imgParent;

				//void SetValue(object value) {
					//replace this
				//	throw std::exception("not implemented");
				//}

				//WzImageProperty DeepClone() override {
				//	return WzNullProperty(name);
				//}

				WzPropertyType getPropertyType() override { return WzPropertyType::Null; }

				WzObjectType getObjectType() override{ return WzObjectType::Property; }
				/*
				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.Write((uint8_t)0);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedTag("WzNull", this.Name));
				}
				*/

			};
		}
	}
}