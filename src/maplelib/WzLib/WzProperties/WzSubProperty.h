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

			class WzSubProperty : public WzImageProperty {
			public:
				WzSubProperty() { }

				WzSubProperty(std::wstring name) : WzImageProperty{ name } {}
				void setName(std::wstring& name) {
					this->name = name;
				}
				std::wstring getName() {
					return name;
				}

				//internal WzImage imgParent;

				/*
				WzImageProperty DeepClone() override {
					WzSubProperty clone{ name };
					for (WzImageProperty& prop : properties) {
						clone.AddProperty(prop.DeepClone());
					}
					return clone;
				}
				*/

				WzPropertyType getPropertyType() override { WzPropertyType::SubProperty; }
				/*
				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.WriteStringValue("Property", 0x73, 0x1B);
					WzImageProperty.WritePropertyList(writer, properties);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag(L"WzSub", name, true));
					WzImageProperty.DumpPropertyList(writer, level, WzProperties);
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::CloseTag(L"WzSub"));
				}
				*/
			};
		}
	}
}