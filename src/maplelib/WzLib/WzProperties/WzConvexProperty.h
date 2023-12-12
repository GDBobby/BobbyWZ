#pragma once

#include "../WzImageProperty.h"


#include <list>
#include <string>


namespace MapleLib {
	namespace WzLib {
		namespace WzProperties {
			/// <summary>
			/// A property that contains several WzExtendedPropertys
			/// </summary>
			class WzConvexProperty : WzImageProperty {
			public:

				WzConvexProperty() { }
				/// <summary>
				/// Creates a WzConvexProperty with the specified name
				/// </summary>
				/// <param name="name">The name of the property</param>
				WzConvexProperty(std::wstring name) : WzImageProperty{ name } {}

				std::wstring getName() { return name; }
				void setName(std::wstring& name) { this->name = name; }

				WzPropertyType getPropertyType() { return WzPropertyType::Convex; }

				/*
				WzImageProperty DeepClone() override {
					WzConvexProperty clone{ name };
					for (WzImageProperty prop : properties) {
						clone.AddProperty(prop.DeepClone());
					}
					return clone;
				}

				void WriteValue(Util::WzBinaryWriter writer) override {
					std::list<WzExtended> extendedProps{};
					for (WzImageProperty prop : properties) {
						if (prop is WzExtended) {
							extendedProps.Add((WzExtended)prop);
						}
					}
					writer.WriteStringValue("Shape2D#Convex2D", 0x73, 0x1B);
					writer.WriteCompressedInt(extendedProps.Count);
					for (int i = 0; i < extendedProps.Count; i++) {
						properties[i].WriteValue(writer);
					}
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XMLUtil::Indentation(level) + Util::XMLUtil::OpenNamedTag("WzConvex", this.Name, true));
					WzImageProperty.DumpPropertyList(writer, level, WzProperties);
					writer.WriteLine(Util::XMLUtil::Indentation(level) + Util::XMLUtil::CloseTag("WzConvex"));
				}
				*/
			};
		}
	}
}