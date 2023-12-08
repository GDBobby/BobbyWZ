#pragma once

#include <string>
#include "../WzProperties/WzValueProperty.h"
#include "../WzProperties/WzStringProperty.h"

namespace MapleLib {
    namespace WzLib {
        namespace WzStructure {
            class InfoTool {
            public:
                static std::wstring GetString(WzImageProperty source) {
                    return source.GetString();
                }

                static WzStringProperty SetString(std::wstring value) {
                    return new WzStringProperty{ L"", value };
                }

                static std::wstring GetOptionalString(WzImageProperty source) {
                    if (source == nullptr) {
                        //do this check outside of this function
                        return L"";
                    }
                    return source.getString();
                    
                }

                static WzStringProperty SetOptionalString(std::wstring value) {
                    return SetString(value);
                }

                static double GetDouble(WzImageProperty source) {
                    return source.GetDouble();
                }

                static WzDoubleProperty SetDouble(double value) {
                    return new WzDoubleProperty{ "", value };
                }

                static int GetInt(WzImageProperty source) {
                    return source.GetInt();
                }

                static WzIntProperty SetInt(int value) {
                    return new WzIntProperty{ "", value };
                }
                /*
                static int ? GetOptionalInt(WzImageProperty source) {
                    return source == null ? (int ? )null : source.GetInt();
                }

                static WzIntProperty SetOptionalInt(int ? value) {
                    return value.HasValue ? SetInt(value.Value) : null;
                }


                static MapleBool GetOptionalBool(WzImageProperty source) {
                    if (source == null) return MapleBool.NotExist;
                    else return source.GetInt() == 1;
                }
                static WzIntProperty SetOptionalBool(MapleBool value) {
                    return value.HasValue ? SetBool(value.Value) : null;
                }

                static float ? GetOptionalFloat(WzImageProperty source) {
                    return source == null ? (float ? )null : source.GetFloat();
                }

                static WzFloatProperty SetOptionalFloat(float ? value) {
                    return value.HasValue ? SetFloat(value.Value) : null;
                }
                static int ? GetOptionalTranslatedInt(WzImageProperty source) {
                    string str = InfoTool.GetOptionalString(source);
                    if (str == null) return null;
                    return int.Parse(str);
                }

                static WzStringProperty SetOptionalTranslatedInt(int ? value) {
                    if (value.HasValue) {
                        return SetString(value.Value.ToString());
                    }
                    else {
                        return null;
                    }
                }
                */
                static bool GetBool(WzImageProperty source) {
                    return source.GetInt() == 1;
                }
                static WzIntProperty SetBool(bool value) {
                    return new WzIntProperty{ "", value};
                }

                static float GetFloat(WzImageProperty source) {
                    return source.GetFloat();
                }

                static WzFloatProperty SetFloat(float value) {
                    return new WzFloatProperty("", value);
                }
            };
        }
    }
}