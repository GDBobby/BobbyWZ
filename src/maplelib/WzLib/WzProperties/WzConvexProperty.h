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
				std::list<WzImageProperty> properties{};
				std::wstring name;
				WzObject parent;

				void SetValue(object value) override {
					//? replace this with something else
					throw new NotImplementedException();
				}

				WzImageProperty DeepClone() override {
					WzConvexProperty clone{ name };
					for (WzImageProperty prop : properties) {
						clone.AddProperty(prop.DeepClone());
					}
					return clone;
				}

				WzPropertyType getPropertyType() { return WzPropertyType::Convex; }
				/// <summary>
				/// The properties contained in the property
				/// </summary>
				std::list<WzImageProperty> getWzProperties override {
					return properties;
				}
					/// <summary>
					/// Gets a wz property by it's name
					/// </summary>
					/// <param name="name">The name of the property</param>
					/// <returns>The wz property with the specified name</returns>
				WzImageProperty getWzImageProperty(std::wstring name) override {
					for (WzImageProperty& iwp : properties) {
						if (iwp.Name.ToLower() == name.ToLower()) {
							return iwp;
						}
					}
					//throw new KeyNotFoundException("A wz property with the specified name was not found");
					return null;
				}


				WzImageProperty GetProperty(std::wstring name) {
					for (WzImageProperty iwp : properties) {
						if (iwp.Name.ToLower() == name.ToLower()) {
							return iwp;
						}
					}
					return null;
				}

				/// <summary>
				/// Gets a wz property by a path name
				/// </summary>
				/// <param name="path">path to property</param>
				/// <returns>the wz property with the specified name</returns>
				WzImageProperty GetFromPath(std::wstring path) override {
					std::vector<std::wstring> segments{ Util::ParseLines(path) };
					if (segments[0] == L"..") {
						size_t index = name.find('/');
						return ((WzImageProperty)Parent)[name.substr(index + 1); ];
					}
					WzImageProperty ret = this;
					for (int x = 0; x < segments.Length; x++) {
						bool foundChild = false;
						for (WzImageProperty iwp : ret.WzProperties) {
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
				void WriteValue(MapleLib.WzLib.Util.WzBinaryWriter writer) override {
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
				void Dispose() override {
					name = null;
					foreach(WzImageProperty exProp in properties)
						exProp.Dispose();
					properties.Clear();
					properties = null;
				}

				WzConvexProperty() { }
				/// <summary>
				/// Creates a WzConvexProperty with the specified name
				/// </summary>
				/// <param name="name">The name of the property</param>
				WzConvexProperty(std::wstring name) :name{ name } {}
				/// <summary>
				/// Adds a WzExtendedProperty to the list of properties
				/// </summary>
				/// <param name="prop">The property to add</param>
				void AddProperty(WzImageProperty prop) {
					if (!(prop is WzExtended)) {
						throw new Exception("Property is not IExtended");
					}
					prop.Parent = this;
					properties.Add((WzExtended)prop);
				}

				void AddProperties(List<WzImageProperty> properties) {
					for (WzImageProperty& prop : properties) {
						AddProperty(prop);
					}
				}

				void RemoveProperty(WzImageProperty prop) {
					prop.Parent = null;
					properties.Remove(prop);
				}

				void ClearProperties() {
					for (WzImageProperty& prop : properties) {
						prop.Parent = null;
					}
					properties.Clear();
				}
			};
		}
	}
}