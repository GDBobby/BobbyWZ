#pragma once

#include <string>
#include <list>
#include <stdexcept>

#include "../Util/ParseLine.h"
#include "../Util/WzBinaryWriter.h"
#include "../Util/XmlUtil.h"
#include "../WzImageProperty.h"
#include "WzValueProperty.h"

namespace MapleLib {
	namespace WzLib {
		namespace WzProperties {
			/// <summary>
			/// A property that contains an x and a y value
			/// </summary>
			class WzVectorProperty : WzImageProperty {
			public:
				WzVectorProperty() { }

				WzVectorProperty(std::wstring name) : WzImageProperty{ name } {}

				WzVectorProperty(std::wstring name, WzIntProperty x, WzIntProperty y)
					: WzImageProperty{ name },
					x{ x },
					y{ y }
				{}
				void setName(std::wstring& name) {
					this->name = name;
				}
				std::wstring getName() {
					return name;
				}

				WzIntProperty x, y;
				//internal WzImage imgParent;
				/*
				void SetValue(object value) override {
					if (value is System.Drawing.Point) 	{
						//what is this
						x.val = ((System.Drawing.Point)value).X;
						y.val = ((System.Drawing.Point)value).Y;
					}
					else {
						x.val = ((System.Drawing.Size)value).Width;
						y.val = ((System.Drawing.Size)value).Height;
					}
				}

				WzImageProperty DeepClone() override {
					WzVectorProperty clone{ name, x, y };
					return clone;
				}
				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.WriteStringValue(L"Shape2D#Vector2D", 0x73, 0x1B);
					writer.WriteCompressedInt(X.Value);
					writer.WriteCompressedInt(Y.Value);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag(L"WzVector", name, false, false) +
						Util::XmlUtil::Attrib(L"X", x.val.ToString()) + Util::XmlUtil::Attrib(L"Y", y.val.ToString(), true, true));
				}
				*/

				//object getWzValue() override { return new System.Drawing.Point(x.Value, y.Value); }


				WzPropertyType getPropertyType() override { return WzPropertyType::Vector; }

				/// <summary>
				/// Disposes the object
				/// </summary>
				void Dispose() override {
					name = null;
					x.Dispose();
					x = null;
					y.Dispose();
					y = null;
				}

				//System.Drawing.Point Pos{ get { return new System.Drawing.Point(X.Value, Y.Value); } }

				System.Drawing.Point GetPoint() override {
					return new System.Drawing.Point(x.val, y.val);
				}

				std::wstring ToString() override {
					return L"X: " + x.val.ToString() + L", Y: " + y.val.ToString();
				}
			};
		}
	}
}