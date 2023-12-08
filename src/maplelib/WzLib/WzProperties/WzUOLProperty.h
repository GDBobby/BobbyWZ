#pragma once

#define UOLRES

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
			/// A property that's value is a string
			/// </summary>
			class WzUOLProperty : WzExtended {
			public:
				std::wstring name, val;
				WzObject parent;
				//internal WzImage imgParent;
				WzObject linkVal;

				void SetValue(object value) override {
					val = (std::wstring)value;
				}

				WzImageProperty DeepClone() override {
					WzUOLProperty clone{ name, val };
					clone.linkVal = null;
					return clone;
				}

				object getWzValue() override {
#if UOLRES
					return LinkValue;
#else
					return this;
#endif
				}

#if UOLRES
				public override List<WzImageProperty> WzProperties
				{
					get
					{
						return LinkValue is WzImageProperty ? ((WzImageProperty)LinkValue).WzProperties : null;
					}
				}


					public override WzImageProperty this[string name]
						{
							get
							{

								return LinkValue is WzImageProperty ? ((WzImageProperty)LinkValue)[name] : LinkValue is WzImage ? ((WzImage)LinkValue)[name] : null;
							}
						}

						public override WzImageProperty GetFromPath(string path)
							{
								return LinkValue is WzImageProperty ? ((WzImageProperty)LinkValue).GetFromPath(path) : LinkValue is WzImage ? ((WzImage)LinkValue).GetFromPath(path) : null;
							}
#endif

							/// <summary>
							/// The WzPropertyType of the property
							/// </summary>
				WzPropertyType getPropertyType() override { return WzPropertyType::UOL; }

				void WriteValue(Util::WzBinaryWriter writer) override {
					writer.WriteStringValue("UOL", 0x73, 0x1B);
					writer.Write((uint8_t)0);
					writer.WriteStringValue(val, 0, 1);
				}

				void ExportXml(StreamWriter writer, int level) override {
					writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedValuePair("WzUOL", name, val));
				}

				void Dispose() override {
					name = null;
					val = null;
				}

#if UOLRES
					public WzObject LinkValue
				{
					get
					{
						if (linkVal == null)
						{
							string[] paths = val.Split('/');
							linkVal = (WzObject)this.parent;
							string asdf = parent.FullPath;
							foreach(string path in paths)
							{
								if (path == "..")
								{
									linkVal = (WzObject)linkVal.Parent;
								}
								else
								{
									if (linkVal is WzImageProperty)
										linkVal = ((WzImageProperty)linkVal)[path];
									else if (linkVal is WzImage)
										linkVal = ((WzImage)linkVal)[path];
									else if (linkVal is WzDirectory)
										linkVal = ((WzDirectory)linkVal)[path];
									else
									{
										MapleLib.Helpers.ErrorLogger.Log(MapleLib.Helpers.ErrorLevel.Critical, "UOL got nexon'd at property: " + this.FullPath);
										return null;
									}
								}
							}
						}
						return linkVal;
					}
				}
#endif

				WzUOLProperty() { }

				WzUOLProperty(std::wstring name) : name{name} {}

					/// <summary>
					/// Creates a WzUOLProperty with the specified name and value
					/// </summary>
					/// <param name="name">The name of the property</param>
					/// <param name="value">The value of the property</param>
				WzUOLProperty(std::wstring name, std::wstring value) : name{ name }, val{value} {}

#if UOLRES
						public override int GetInt()
					{
						return LinkValue.GetInt();
					}

					public override short GetShort()
					{
						return LinkValue.GetShort();
					}

					public override long GetLong()
					{
						return LinkValue.GetLong();
					}

					public override float GetFloat()
					{
						return LinkValue.GetFloat();
					}

					public override double GetDouble()
					{
						return LinkValue.GetDouble();
					}

					public override string GetString()
					{
						return LinkValue.GetString();
					}

					public override Point GetPoint()
					{
						return LinkValue.GetPoint();
					}

					public override Bitmap GetBitmap()
					{
						return LinkValue.GetBitmap();
					}

					public override byte[] GetBytes()
					{
						return LinkValue.GetBytes();
					}
#else
				std::wstring GetString() override {
					return val;
				}
#endif
				std::wstring ToString() override {
					return val;
				}
			};
		}
	}
}