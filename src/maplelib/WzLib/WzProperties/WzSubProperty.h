#pragma once
/*  MapleLib - A general-purpose MapleStory library
 * Copyright (C) 2009, 2010, 2015 Snow and haha01haha01

 * This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

 * This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <string>
#include <list>
#include <stdexcept>

#include "../Util/ParseLine.h"
#include "../Util/WzBinaryWriter.h"
#include "../Util/XmlUtil.h"
#include "../WzImageProperty.h"

namespace MapleLib {
	namespace WzLib {
		namespace WzProperties {

			class WzSubProperty : WzExtended, IPropertyContainer {
			public:
				std::list<WzImageProperty> properties{};
				std::wstring name;
				WzObject parent;
				//internal WzImage imgParent;

				void SetValue(object value) override {
					throw std::exception("not implemented");
					//throw new System.NotImplementedException();
				}

				WzImageProperty DeepClone() override {
					WzSubProperty clone{ name };
					for (WzImageProperty& prop : properties) {
						clone.AddProperty(prop.DeepClone());
					}
					return clone;
				}


				WzPropertyType getPropertyType() override { WzPropertyType::SubProperty; }

				std::list<WzImageProperty> getWzProperties() override {return properties;}


				WzImageProperty getPropertyByName(std::wstring& name) override {
					for (WzImageProperty iwp : properties) {
						if (iwp.Name.ToLower() == name.ToLower()) {
							return iwp;
						}
					}
					//throw new KeyNotFoundException("A wz property with the specified name was not found");
					return null;
				}
				void setPropertyByName(std::wstring& name) {
					//value.Name = name;
					//AddProperty(value);
					//idk
				}

				WzImageProperty GetFromPath(std::wstring& path) override {
					std::vector<std::wstring> segments = Util::ParseLines(path);
					if (segments[0] == L"..") {
						return ((WzImageProperty)Parent)[path.Substring(name.IndexOf('/') + 1)];
					}
					WzImageProperty ret = this;
					for (int x = 0; x < segments.size(); x++) {
						bool foundChild = false;
						for(WzImageProperty iwp : ret.WzProperties) {
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
					writer.WriteStringValue("Property", 0x73, 0x1B);
					WzImageProperty.WritePropertyList(writer, properties);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag(L"WzSub", name, true));
					WzImageProperty.DumpPropertyList(writer, level, WzProperties);
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::CloseTag(L"WzSub"));
				}
			
				void Dispose() override {
					name = null;
					foreach(WzImageProperty prop in properties)
						prop.Dispose();
					properties.Clear();
					properties = null;
				}

				WzSubProperty() { }

				WzSubProperty(std::wstring name) : name{ name } {}

				void AddProperty(WzImageProperty prop) {
					prop.Parent = this;
					properties.Add(prop);
				}
				void AddProperties(std::list<WzImageProperty> props)
				{
					for(WzImageProperty& prop : props) {
						AddProperty(prop);
					}
				}
				void RemoveProperty(WzImageProperty prop) {
					prop.Parent = null;
					properties.Remove(prop);
				}
						/// <summary>
						/// Clears the list of properties
						/// </summary>
				void ClearProperties() {
					for (WzImageProperty& prop : properties) {
						prop.Parent = null; 
					}
					properties.clear();
				}
			};
		}
	}
}