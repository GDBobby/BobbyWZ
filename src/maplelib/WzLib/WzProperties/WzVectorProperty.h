#pragma once

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
			/// <summary>
			/// A property that contains an x and a y value
			/// </summary>
			class WzVectorProperty : WzExtended {
			public:
				std::wstring name;
				WzIntProperty x, y;
				WzObject parent;
				//internal WzImage imgParent;
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

				object getWzValue() override { return new System.Drawing.Point(x.Value, y.Value); }


				WzPropertyType getPropertyType() override { return WzPropertyType::Vector; }

				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.WriteStringValue(L"Shape2D#Vector2D", 0x73, 0x1B);
					writer.WriteCompressedInt(X.Value);
					writer.WriteCompressedInt(Y.Value);
				}
				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::OpenNamedTag(L"WzVector", name, false, false) +
						Util::XmlUtil::Attrib(L"X", x.val.ToString()) + Util::XmlUtil::Attrib(L"Y", y.val.ToString(), true, true));
				}
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

				WzVectorProperty() { }

				WzVectorProperty(std::wstring name) : name{ name } {}

				WzVectorProperty(std::wstring name, WzIntProperty x, WzIntProperty y)
					: name{name},
					x{x},
					y{y}
				{}

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