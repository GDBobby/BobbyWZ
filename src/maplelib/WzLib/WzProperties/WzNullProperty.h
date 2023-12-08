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
				std::wstring name;
				WzObject parent;
				//internal WzImage imgParent;

				void SetValue(object value) {
					//replace this
					throw std::exception("not implemented");
				}

				WzImageProperty DeepClone() override {
					return WzNullProperty(name);
				}

				WzPropertyType getPropertyType override { return WzPropertyType::Null; }

				WzObjectType getObjectType override{ return WzObjectType.Property; }

				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.Write((byte)0);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedTag("WzNull", this.Name));
				}

				public WzNullProperty() { }
				public WzNullProperty(std::wstring propName) : name{ propName } {}

			};
		}
	}
}