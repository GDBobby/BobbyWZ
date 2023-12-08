#pragma once

#include <typeinfo>
#include <string>

#include "WzImage.h"
#include "WzImageProperty.h"
#include "WzMapleVersion.h"

namespace MapleLib{ namespace WzLib {
    class WzSettingsManager {
        std::wstring wzPath;
        const std::type_info& userSettingsType;
        const std::type_info& appSettingsType;
        const std::type_info* xnaColorType = nullptr;


    public:
        WzSettingsManager(std::wstring wzPath, const std::type_info& userSettingsType, const std::type_info& appSettingsType) :
            wzPath{ wzPath },
            userSettingsType{ userSettingsType },
            appSettingsType{ appSettingsType } 
        {}

        WzSettingsManager(std::wstring wzPath, const std::type_info& userSettingsType, const std::type_info& appSettingsType, const std::type_info* xnaColorType) :
            wzPath{ wzPath },
            userSettingsType{ userSettingsType },
            appSettingsType{ appSettingsType },
            xnaColorType{ xnaColorType }
        {}

        struct SettingsInfo {
            WzPropertyType propType;
            WzImageProperty settingProp;

        };

    private:
        void ExtractSettingsImage(WzImage settingsImage, std::type_info settingsHolderType) {
            if (!settingsImage.parsed) settingsImage.ParseImage();
            //settingsImage->extractSettings();

            /*
            for(FieldInfo& fieldInfo : settingsHolderType.GetFields(BindingFlags.Public | BindingFlags.Static)) {
                std::wstring settingName = fieldInfo.Name;
                WzImageProperty settingProp = settingsImage[settingName];
                byte[] argb;
                if (settingProp == null)
                    SaveField(settingsImage, fieldInfo);
                else if (fieldInfo.FieldType.BaseType != null && fieldInfo.FieldType.BaseType.FullName == "System.Enum")
                    fieldInfo.SetValue(null, InfoTool.GetInt(settingProp));
                else switch (fieldInfo.FieldType.FullName)
                    {
                    //case "Microsoft.Xna.Framework.Graphics.Color":
                    case "Microsoft.Xna.Framework.Color":
                        if (xnaColorType == null) throw new InvalidDataException("XNA color detected, but XNA type activator is null");
                        argb = BitConverter.GetBytes((uint)((WzDoubleProperty)settingProp).Value);
                        fieldInfo.SetValue(null, Activator.CreateInstance(xnaColorType, argb[0], argb[1], argb[2], argb[3]));
                        break;
                    case "System.Drawing.Color":
                        argb = BitConverter.GetBytes((uint)((WzDoubleProperty)settingProp).Value);
                        fieldInfo.SetValue(null, System.Drawing.Color.FromArgb(argb[3], argb[2], argb[1], argb[0]));
                        break;
                    case "System.Int32":
                        fieldInfo.SetValue(null, InfoTool.GetInt(settingProp));
                        break;
                    case "System.Double":
                        fieldInfo.SetValue(null, ((WzDoubleProperty)settingProp).Value);
                        break;
                    case "System.Boolean":
                        fieldInfo.SetValue(null, InfoTool.GetBool(settingProp));
                        //bool a = (bool)fieldInfo.GetValue(null);
                        break;
                    case "System.Single":
                        fieldInfo.SetValue(null, ((WzFloatProperty)settingProp).Value);
                        break;
                        *
                        case "WzMapleVersion":
                            fieldInfo.SetValue(null, (WzMapleVersion)InfoTool.GetInt(settingProp));
                            break;
                        case "ItemTypes":
                            fieldInfo.SetValue(null, (ItemTypes)InfoTool.GetInt(settingProp));
                            break;
                        *
                        case "System.Drawing.Size":
                            fieldInfo.SetValue(null, new System.Drawing.Size(((WzVectorProperty)settingProp).X.Value, ((WzVectorProperty)settingProp).Y.Value));
                            break;
                        case "System.String":
                            fieldInfo.SetValue(null, InfoTool.GetString(settingProp));
                            break;
                        case "System.Drawing.Bitmap":
                            fieldInfo.SetValue(null, ((WzCanvasProperty)settingProp).PngProperty.GetPNG(false));
                            break;
                        default:
                            throw new Exception("unrecognized setting type");
                    }
            }
            */
        }

        void CreateWzProp(IPropertyContainer parent, WzPropertyType propType, std::wstring propName, void* value) {
            WzImageProperty addedProp;
            switch (propType) {
                case WzPropertyType::Float:
                    addedProp = new WzFloatProperty(propName);
                    break;
                case WzPropertyType::Canvas:
                    addedProp = new WzCanvasProperty(propName);
                    ((WzCanvasProperty)addedProp).PngProperty = new WzPngProperty();
                    break;
                case WzPropertyType::Int:
                    addedProp = new WzIntProperty(propName);
                    break;
                case WzPropertyType::Double:
                    addedProp = new WzDoubleProperty(propName);
                    break;
                    /*case WzPropertyType.Sound:
                        addedProp = new WzSoundProperty(propName);
                        break;*/
                case WzPropertyType::String:
                    addedProp = new WzStringProperty(propName);
                    break;
                case WzPropertyType::Short:
                    addedProp = new WzShortProperty(propName);
                    break;
                case WzPropertyType::Vector:
                    addedProp = new WzVectorProperty(propName);
                    ((WzVectorProperty)addedProp).X = new WzIntProperty("X");
                    ((WzVectorProperty)addedProp).Y = new WzIntProperty("Y");
                    break;
                default:
                    throw std::exception("not supported type");
            }
            addedProp.SetValue(value);
            parent.AddProperty(addedProp);
        }

        void SetWzProperty(WzImage parentImage, std::wstring propName, WzPropertyType propType, void* value) {
            WzImageProperty property = parentImage.at(propName);
            if (property != null) {
                if (property.getPropertyType() == propType)
                    property.SetValue(value);
                else {
                    property.Remove();
                    CreateWzProp(parentImage, propType, propName, value);
                }
            }
            else {
                CreateWzProp(parentImage, propType, propName, value);
            }
        }

        void SaveSettingsImage(WzImage settingsImage, std::type_info settingsHolderType) {
            if (!settingsImage.Parsed) { 
                settingsImage.ParseImage(); 
            }
            for(FieldInfo& fieldInfo : settingsHolderType.GetFields(BindingFlags.Public | BindingFlags.Static)) {
                SaveField(settingsImage, fieldInfo);
            }
            settingsImage.Changed = true;
        }

        void SaveField(WzImage settingsImage, std::type_info fieldInfo) {
            /* idk yet, heavy metaprogramming here

                std::wstring settingName = fieldInfo.Name;
                if (fieldInfo.FieldType.BaseType != null && fieldInfo.FieldType.BaseType.FullName == "System.Enum")
                    SetWzProperty(settingsImage, settingName, WzPropertyType.Int, fieldInfo.GetValue(null));
                else switch (fieldInfo.FieldType.FullName)
                    {
                    //case "Microsoft.Xna.Framework.Graphics.Color":
                    case "Microsoft.Xna.Framework.Color":
                        object xnaColor = fieldInfo.GetValue(null);
                        //for some odd reason .NET requires casting the result to uint before it can be
                        //casted to double
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Double, (double)(uint)xnaColor.GetType().GetProperty("PackedValue").GetValue(xnaColor, null));
                        break;
                    case "System.Drawing.Color":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Double, (double)((System.Drawing.Color)fieldInfo.GetValue(null)).ToArgb());
                        break;
                    case "System.Int32":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Int, fieldInfo.GetValue(null));
                        break;
                    case "System.Double":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Double, fieldInfo.GetValue(null));
                        break;
                    case "Single":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Float, fieldInfo.GetValue(null));
                        break;
                    case "System.Drawing.Size":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Vector, fieldInfo.GetValue(null));
                        break;
                    case "System.String":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.String, fieldInfo.GetValue(null));
                        break;
                    case "System.Drawing.Bitmap":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Canvas, fieldInfo.GetValue(null));
                        break;
                    case "System.Boolean":
                        SetWzProperty(settingsImage, settingName, WzPropertyType.Int, (bool)fieldInfo.GetValue(null) ? 1 : 0);
                        break;
                }
            */
        }

    public:
        void Load() {
            if (std::filesystem::exists(wzPath)) {
                WzFile wzFile{ wzPath, 1337, WzMapleVersion::CLASSIC };
                try {
                    wzFile.ParseWzFile();
                    ExtractSettingsImage((WzImage)wzFile["UserSettings.img"], userSettingsType);
                    ExtractSettingsImage((WzImage)wzFile["ApplicationSettings.img"], appSettingsType);
                    wzFile.Dispose();
                }
                catch(std::exception e) {
                    wzFile.Dispose();
                    printf("exception thrown on WzSettings::Load() : %s\n", e.what());
                    throw; //wat this
                }
            }
        }

        void Save() {
            bool settingsExist = std::filesystem::exists(wzPath);
            WzFile wzFile;
            if (settingsExist) {
                wzFile = WzFile(wzPath, 1337, WzMapleVersion::CLASSIC);
                wzFile.ParseWzFile();
            }
            else {
                wzFile = WzFile(1337, WzMapleVersion::CLASSIC);
                wzFile.Header.Copyright = "Wz settings file generated by MapleLib's WzSettings module created by haha01haha01";
                wzFile.Header.RecalculateFileStart();
                WzImage US{ "UserSettings.img" };
                US.changed = true;
                US.parsed = true;
                WzImage AS{ "ApplicationSettings.img" };
                AS.changed = true;
                AS.parsed = true;

                wzFile.WzDirectory.WzImages.Add(US);
                wzFile.WzDirectory.WzImages.Add(AS);
            }
            SaveSettingsImage((WzImage)wzFile["UserSettings.img"], userSettingsType);
            SaveSettingsImage((WzImage)wzFile["ApplicationSettings.img"], appSettingsType);
            if (settingsExist) {
                std::wstring tempFile = Path.GetTempFileName();
                std::wstring settingsPath = wzFile.FilePath;
                wzFile.SaveToDisk(tempFile);
                wzFile.Dispose();
                File.Delete(settingsPath);
                File.Move(tempFile, settingsPath);
            }
            else {
                wzFile.SaveToDisk(wzPath);
            }
        }
    };
}