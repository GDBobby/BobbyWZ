#pragma once

#include "../../helpers/ErrorLog.h"
#include "../WzImage.h"
#include "../WzImageProperty.h"

#include <string>
#include <vector>


namespace MapleLib {
    namespace WzLib {
        namespace WzStructure {
            //Credits to Bui for some of the info
            class MapInfo {
            public:
                //default constructor??
                //MapInfo Default = new MapInfo();
            private:
                WzImage image = null;
            public:
                MapInfo(){}

                MapInfo(WzImage image, std::wstring strMapName, std::wstring strStreetName, std::wstring strCategoryName) :
                    image{ image },
                    strMapName{ strMapName },
                    strStreetName{ strStreetName },
                    strCategoryName{strCategoryName},
                    
                {
                    //idk what this means, might need booleans to track usage of C#'s conditional variables
                    int ? startHour;
                    int ? endHour;
                    WzFile file = (WzFile)image.WzFileParent;
                    std::wstring loggerSuffix = ", map " + image.Name + 
                        ((file != null) ? (" of version " + Enum.GetName(typeof(WzMapleVersion), file.MapleVersion) + ", v" + file.Version.ToString()) : "");
                    for (WzImageProperty& prop : image["info"].WzProperties) {
                        if (prop.name == "bgm") {
                            bgm = InfoTool.GetString(prop);
                        }
                        else if (pro.name == "cloud") {
                            cloud = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "swim") {
                            swim = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "forcedReturn") {
                            forcedReturn = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "hideMinimap") {
                            hideMinimap = InfoTool::GetBool(prop);
                        }
                        else if (prop.name == "mapDesc") {
                            mapDesc = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "mapName") {
                            mapName = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "mapMark") {
                            mapMark = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "mobRate") {
                            mobRate = InfoTool.GetFloat(prop);
                        }
                        else if (prop.name == "moveLimit") {
                            moveLimit = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "returnMap") {
                            returnMap = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "town") {
                            town = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "version") {
                            version = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "fieldLimit") {
                            int fl = InfoTool.GetInt(prop);
                            if (fl >= (int)Math.Pow(2, 23)) {
                                ErrorLogger.Log(ErrorLevel.IncorrectStructure, "Invalid fieldlimit " + fl.ToString() + loggerSuffix);
                                fl = fl & ((int)Math.Pow(2, 23) - 1);
                            }
                            fieldLimit = (FieldLimit)fl;
                        }
                        else if (prop.name == "timeLimit") {
                            timeLimit = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "lvLimit") {
                            lvLimit = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "onFirstUserEnter") {
                            onFirstUserEnter = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "onUserEnter") {
                            onUserEnter = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "fly") {
                            fly = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "noMapCmd") {
                            noMapCmd = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "partyOnly") {
                            partyOnly = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "fieldType") {
                            int ft = InfoTool.GetInt(prop);
                            if (!Enum.IsDefined(typeof(FieldType), ft)) {
                                ErrorLogger.Log(ErrorLevel.IncorrectStructure, "Invalid fieldType " + ft.ToString() + loggerSuffix);
                                ft = 0;
                            }
                            fieldType = (FieldType)ft;
                        }
                        else if (prop.name == "miniMapOnOff") {
                            miniMapOnOff = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "reactorShuffle") {
                            reactorShuffle = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "reactorShuffleName") {
                            reactorShuffleName = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "personalShop") {
                            personalShop = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "entrustedShop") {
                            entrustedShop = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "effect") {
                            effect = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "lvForceMove") {
                            lvForceMove = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "timeMob") {
                            startHour = InfoTool.GetOptionalInt(prop["startHour"]);
                            endHour = InfoTool.GetOptionalInt(prop["endHour"]);
                            int ? id = InfoTool.GetOptionalInt(prop["id"]);
                            string message = InfoTool.GetOptionalString(prop["message"]);
                            if (id == null || message == null || (startHour == null ^ endHour == null)) {
                                Helper::ErrorLogger::Log(Helper::ErrorLevel::IncorrectStructure, "timeMob" + loggerSuffix);
                            }
                            else {
                                timeMob = new TimeMob((int ? )startHour, (int ? )endHour, (int)id, message);
                            }
                        }
                        else if (prop.name == "help") {
                            help = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "snow") {
                            snow = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "rain") {
                            rain = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "dropExpire") {
                            dropExpire = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "decHP") {
                            decHP = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "decInterval") {
                            decInterval = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "autoLieDetector") {
                            startHour = InfoTool.GetOptionalInt(prop["startHour"]);
                            endHour = InfoTool.GetOptionalInt(prop["endHour"]);
                            int ? interval = InfoTool.GetOptionalInt(prop["interval"]);
                            int ? propInt = InfoTool.GetOptionalInt(prop["prop"]);
                            if (startHour == null || endHour == null || interval == null || propInt == null) {
                                ErrorLogger.Log(ErrorLevel.IncorrectStructure, "autoLieDetector" + loggerSuffix);
                            }
                            else {
                                autoLieDetector = new AutoLieDetector((int)startHour, (int)endHour, (int)interval, (int)propInt);
                            }
                        }
                        else if (prop.name == "expeditionOnly") {
                            expeditionOnly = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "fs") {
                            fs = InfoTool.GetFloat(prop);
                        }
                        else if (prop.name == "protectItem") {
                            protectItem = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "createMobInterval") {
                            createMobInterval = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "fixedMobCapacity") {
                            fixedMobCapacity = InfoTool.GetInt(prop);
                        }
                        else if (prop.name == "streetName") {
                            streetName = InfoTool.GetString(prop);
                        }
                        else if (prop.name == "noRegenMap") {
                            noRegenMap = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "allowedItem") {
                            allowedItem = new List<int>();
                            if (prop.WzProperties != null && prop.WzProperties.Count > 0)
                                foreach(WzImageProperty item in prop.WzProperties)
                                allowedItem.Add(item.GetInt());
                        }
                        else if (prop.name == "recovery") {
                            recovery = InfoTool.GetFloat(prop);
                        }
                        else if (prop.name == "blockPBossChange") {
                            blockPBossChange = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "everlast") {
                            everlast = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "damageCheckFree") {
                            damageCheckFree = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "dropRate") {
                            dropRate = InfoTool.GetFloat(prop);
                        }
                        else if (prop.name == "scrollDisable") {
                            scrollDisable = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "needSkillForFly") {
                            needSkillForFly = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "zakum2Hack") {
                            zakum2Hack = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "allMoveCheck") {
                            allMoveCheck = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "VRLimit") {
                            VRLimit = InfoTool.GetBool(prop);
                        }
                        else if (prop.name == "consumeItemCoolTime") {
                            consumeItemCoolTime = InfoTool.GetBool(prop);
                        }
                        else{
                            ErrorLogger.Log(ErrorLevel::MissingFeature, "Unknown Prop: " + prop.Name + loggerSuffix);
                            additionalProps.Add(prop.DeepClone());
                        }


                    }
                        /*
                        case "VRTop":
                        case "VRBottom":
                        case "VRLeft":
                        case "VRRight":
                            break;
                        case "link":
                            //link = InfoTool.GetInt(prop);
                            break;
                        */
                    
                }

                static Rectangle ? GetVR(WzImage image) {
                    Rectangle ? result = null;
                    if (image["info"]["VRLeft"] != null) {
                        WzImageProperty info = image["info"];
                        int left = InfoTool.GetInt(info["VRLeft"]);
                        int right = InfoTool.GetInt(info["VRRight"]);
                        int top = InfoTool.GetInt(info["VRTop"]);
                        int bottom = InfoTool.GetInt(info["VRBottom"]);
                        result = new Rectangle(left, top, right - left, bottom - top);
                    }
                    return result;
                }

                void Save(WzImage dest, Rectangle ? VR) {
                    WzSubProperty info = new WzSubProperty();
                    info["bgm"] = InfoTool.SetString(bgm);
                    info["cloud"] = InfoTool.SetBool(cloud);
                    info["swim"] = InfoTool.SetBool(swim);
                    info["forcedReturn"] = InfoTool.SetInt(forcedReturn);
                    info["hideMinimap"] = InfoTool.SetBool(hideMinimap);
                    info["mapDesc"] = InfoTool.SetOptionalString(mapDesc);
                    info["mapName"] = InfoTool.SetOptionalString(mapDesc);
                    info["mapMark"] = InfoTool.SetString(mapMark);
                    info["mobRate"] = InfoTool.SetFloat(mobRate);
                    info["moveLimit"] = InfoTool.SetOptionalInt(moveLimit);
                    info["returnMap"] = InfoTool.SetInt(returnMap);
                    info["town"] = InfoTool.SetBool(town);
                    info["version"] = InfoTool.SetInt(version);
                    info["fieldLimit"] = InfoTool.SetInt((int)fieldLimit);
                    info["timeLimit"] = InfoTool.SetOptionalInt(timeLimit);
                    info["lvLimit"] = InfoTool.SetOptionalInt(lvLimit);
                    info["onFirstUserEnter"] = InfoTool.SetOptionalString(onFirstUserEnter);
                    info["onUserEnter"] = InfoTool.SetOptionalString(onUserEnter);
                    info["fly"] = InfoTool.SetOptionalBool(fly);
                    info["noMapCmd"] = InfoTool.SetOptionalBool(noMapCmd);
                    info["partyOnly"] = InfoTool.SetOptionalBool(partyOnly);
                    info["fieldType"] = InfoTool.SetOptionalInt((int ? )fieldType);
                    info["miniMapOnOff"] = InfoTool.SetOptionalBool(miniMapOnOff);
                    info["reactorShuffle"] = InfoTool.SetOptionalBool(reactorShuffle);
                    info["reactorShuffleName"] = InfoTool.SetOptionalString(reactorShuffleName);
                    info["personalShop"] = InfoTool.SetOptionalBool(personalShop);
                    info["entrustedShop"] = InfoTool.SetOptionalBool(entrustedShop);
                    info["effect"] = InfoTool.SetOptionalString(effect);
                    info["lvForceMove"] = InfoTool.SetOptionalInt(lvForceMove);
                    if (timeMob != null) {
                        WzSubProperty prop = new WzSubProperty();
                        prop["startHour"] = InfoTool.SetOptionalInt(timeMob.Value.startHour);
                        prop["endHour"] = InfoTool.SetOptionalInt(timeMob.Value.endHour);
                        prop["id"] = InfoTool.SetOptionalInt(timeMob.Value.id);
                        prop["message"] = InfoTool.SetOptionalString(timeMob.Value.message);
                        info["timeMob"] = prop;
                    }
                    info["help"] = InfoTool.SetOptionalString(help);
                    info["snow"] = InfoTool.SetOptionalBool(snow);
                    info["rain"] = InfoTool.SetOptionalBool(rain);
                    info["dropExpire"] = InfoTool.SetOptionalInt(dropExpire);
                    info["decHP"] = InfoTool.SetOptionalInt(decHP);
                    info["decInterval"] = InfoTool.SetOptionalInt(decInterval);
                    if (autoLieDetector != null) {
                        WzSubProperty prop = new WzSubProperty();
                        prop["startHour"] = InfoTool.SetOptionalInt(autoLieDetector.Value.startHour);
                        prop["endHour"] = InfoTool.SetOptionalInt(autoLieDetector.Value.endHour);
                        prop["interval"] = InfoTool.SetOptionalInt(autoLieDetector.Value.interval);
                        prop["prop"] = InfoTool.SetOptionalInt(autoLieDetector.Value.prop);
                        info["autoLieDetector"] = prop;
                    }
                    info["expeditionOnly"] = InfoTool.SetOptionalBool(expeditionOnly);
                    info["fs"] = InfoTool.SetOptionalFloat(fs);
                    info["protectItem"] = InfoTool.SetOptionalInt(protectItem);
                    info["createMobInterval"] = InfoTool.SetOptionalInt(createMobInterval);
                    info["fixedMobCapacity"] = InfoTool.SetOptionalInt(fixedMobCapacity);
                    info["streetName"] = InfoTool.SetOptionalString(streetName);
                    info["noRegenMap"] = InfoTool.SetOptionalBool(noRegenMap);
                    if (allowedItem != null) {
                        WzSubProperty prop = new WzSubProperty();
                        for (int i = 0; i < allowedItem.Count; i++)
                        {
                            prop[i.ToString()] = InfoTool.SetInt(allowedItem[i]);
                        }
                        info["allowedItem"] = prop;
                    }
                    info["recovery"] = InfoTool.SetOptionalFloat(recovery);
                    info["blockPBossChange"] = InfoTool.SetOptionalBool(blockPBossChange);
                    info["everlast"] = InfoTool.SetOptionalBool(everlast);
                    info["damageCheckFree"] = InfoTool.SetOptionalBool(damageCheckFree);
                    info["dropRate"] = InfoTool.SetOptionalFloat(dropRate);
                    info["scrollDisable"] = InfoTool.SetOptionalBool(scrollDisable);
                    info["needSkillForFly"] = InfoTool.SetOptionalBool(needSkillForFly);
                    info["zakum2Hack"] = InfoTool.SetOptionalBool(zakum2Hack);
                    info["allMoveCheck"] = InfoTool.SetOptionalBool(allMoveCheck);
                    info["VRLimit"] = InfoTool.SetOptionalBool(VRLimit);
                    info["consumeItemCoolTime"] = InfoTool.SetOptionalBool(consumeItemCoolTime);
                    for(WzImageProperty& prop : additionalProps) {
                        info.AddProperty(prop);
                    }
                    if (VR.HasValue) {
                        info["VRLeft"] = InfoTool.SetInt(VR.Value.Left);
                        info["VRRight"] = InfoTool.SetInt(VR.Value.Right);
                        info["VRTop"] = InfoTool.SetInt(VR.Value.Top);
                        info["VRBottom"] = InfoTool.SetInt(VR.Value.Bottom);
                    }
                    dest["info"] = info;
                }

                //Cannot change
                int version = 10;

                //Must have
                std::wstring bgm = L"Bgm00/GoPicnic";
                std::wstring mapMark = L"None";
                FieldLimit fieldLimit = FieldLimit::FIELDOPT_NONE;
                int returnMap = 999999999;
                int forcedReturn = 999999999;
                bool cloud = false;
                bool swim = false;
                bool hideMinimap = false;
                bool town = false;
                float mobRate = 1.5f;

                //Optional
                //public int link = -1;
                int ? timeLimit = null;
                int ? lvLimit = null;
                FieldType ? fieldType = null;
                string onFirstUserEnter = null;
                string onUserEnter = null;
                MapleBool fly = null;
                MapleBool noMapCmd = null;
                MapleBool partyOnly = null;
                MapleBool reactorShuffle = null;
                string reactorShuffleName = null;
                MapleBool personalShop = null;
                MapleBool entrustedShop = null;
                string effect = null; //Bubbling; 610030550 and many others
                int ? lvForceMove = null; //limit FROM value
                TimeMob ? timeMob = null;
                string help = null; //help string
                MapleBool snow = null;
                MapleBool rain = null;
                int ? dropExpire = null; //in seconds
                int ? decHP = null;
                int ? decInterval = null;
                AutoLieDetector ? autoLieDetector = null;
                MapleBool expeditionOnly = null;
                float ? fs = null; //slip on ice speed, default 0.2
                int ? protectItem = null; //ID, item protecting from cold
                int ? createMobInterval = null; //used for massacre pqs
                int ? fixedMobCapacity = null; //mob capacity to target (used for massacre pqs)

                //Unknown optional
                int ? moveLimit = null;
                string mapDesc = null;
                string mapName = null;
                string streetName = null;
                MapleBool miniMapOnOff = null;
                MapleBool noRegenMap = null; //610030400
                List<int> allowedItem = null;
                float ? recovery = null; //recovery rate, like in sauna (3)
                MapleBool blockPBossChange = null; //something with monster carnival
                MapleBool everlast = null; //something with bonus stages of PQs
                MapleBool damageCheckFree = null; //something with fishing event
                float ? dropRate = null;
                MapleBool scrollDisable = null;
                MapleBool needSkillForFly = null;
                MapleBool zakum2Hack = null; //JQ hack protection
                MapleBool allMoveCheck = null; //another JQ hack protection
                MapleBool VRLimit = null; //use vr's as limits?
                MapleBool consumeItemCoolTime = null; //cool time of consume item

                //Special
                List<WzImageProperty> additionalProps = new List<WzImageProperty>();
                List<WzImageProperty> additionalNonInfoProps = new List<WzImageProperty>();
                string strMapName = "<Untitled>";
                string strStreetName = "<Untitled>";
                string strCategoryName = "HaCreator";
                int id = 0;

                //Editor related, not actual properties
                MapType mapType = MapType.RegularMap;

                WzImage getImage() {
                    return image;
                }
                void setImage(WzImage value) { image = value; }

                bool ShouldSerializeImage() {
                    // To keep JSON.NET from serializing this
                    return false;
                }

                struct TimeMob {
                    int ? startHour, endHour;
                    int id;
                    string message;

                    TimeMob(int ? startHour, int ? endHour, int id, string message) {
                        this.startHour = startHour;
                        this.endHour = endHour;
                        this.id = id;
                        this.message = message;
                    }
                };

                struct AutoLieDetector {
                    int startHour, endHour, interval, prop; //interval in mins, prop default = 80

                    AutoLieDetector(int startHour, int endHour, int interval, int prop)
                        :startHour{startHour}, endHour{endHour}, interval{interval}, prop{prop}
                    {}
                };
            };
        }
    }
}