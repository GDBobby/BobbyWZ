#pragma once


#include "../Util/XmlUtil.h"
#include "../Util/WzBinaryReader.h"
#include "../Util/WaveFormat.h"
#include "../../helpers/ErrorLog.h"

#include "../WzImageProperty.h"

#include <string>
#include <format>

namespace MapleLib {
    namespace WzLib {
        namespace WzProperties {
            /// A property that contains data for an MP3 file

            class WzSoundProperty : WzImageProperty {
            public:
                WzSoundProperty(std::wstring name, Util::WzBinaryReader& reader, bool parseNow);
                /*
                //this is just a copy?
                WzSoundProperty(std::wstring name, int len_ms, std::vector<uint8_t>& header, std::vector<uint8_t>& data) :
                    WzImageProperty{ name },
                    len_ms{ len_ms },
                    header{ header },
                    mp3bytes{ data }
                {
                    ParseHeader();
                }
                /// <summary>
                /// Creates a WzSoundProperty with the specified name from a file
                /// </summary>
                /// <param name="name">The name of the property</param>
                /// <param name="file">The path to the sound file</param>
                WzSoundProperty(std::wstring name, std::wstring file) : WzImageProperty{ name } {
                    Mp3FileReader reader = new Mp3FileReader(file);
                    wavFormat = reader.Mp3WaveFormat;
                    len_ms = (int)(static_cast<double>(reader.size()) * 1000.0 / static_cast<double>(reader.WaveFormat.AverageBytesPerSecond));
                    RebuildHeader();
                    reader.Dispose();
                    //what is File?
                    mp3bytes = File.ReadAllBytes(file);
                }
                */
                std::vector<uint8_t> mp3bytes{};
                int len_ms;
                std::vector<uint8_t> header;
                // WzImage imgParent;
                Util::WzBinaryReader wzReader;
                bool headerEncrypted = false;
                int64_t offs;
                int soundDataLen;
                static const std::vector<uint8_t> soundHeader;

                WaveFormat wavFormat; //idk what this is
                WzPropertyType getPropertyType()  override { return WzPropertyType::Sound; }

                int getFrequency(){
                    return wavFormat.getSampleRate();
                }
                
                static bool memcmp(uint8_t* a, uint8_t* b, int n);

                static std::wstring ByteArrayToString(std::vector<uint8_t>& ba);

                void RebuildHeader();

            private:
                template<typename T>
                static std::vector<uint8_t> StructToBytes(T obj);

                template<typename T>
                static T BytesToStruct(std::vector<uint8_t>& data);

                void ParseHeader();
            public:
                std::vector<uint8_t> GetBytes(bool saveInMemory);

                std::vector<uint8_t> GetBytes() override {
                    return GetBytes(false);
                }


                /*
                    WzImageProperty DeepClone() override {
                        return WzSoundProperty{ name, len_ms, header, mp3bytes };
                    }

                    object getWzValue() override { return GetBytes(false); }
                    void WriteValue(Util::WzBinaryWriter writer) override {
                        uint8_t* data = GetBytes(false);
                        writer.WriteStringValue("Sound_DX8", 0x73, 0x1B);
                        writer.Write((uint8_t)0);
                        writer.WriteCompressedInt(data.Length);
                        writer.WriteCompressedInt(len_ms);
                        writer.Write(header);
                        writer.Write(data);
                    }
                    void ExportXml(StreamWriter writer, int level) override {
                        writer.WriteLine(Util::XmlUtil::Indentation(level) + Util::XmlUtil::EmptyNamedTag(L"WzSound", name));
                    }
                    /// <summary>
                    /// Disposes the object
                    /// </summary>
                    void Dispose() override {
                        name = null;
                        mp3bytes = null;
                    }

                    */
            };
        }
    }
}