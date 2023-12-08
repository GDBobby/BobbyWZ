#pragma once

#include <string>
#include <format>

#include "../Util/XmlUtil.h"
#include "../Util/WzBinaryReader.h"
#include "../../helpers/ErrorLog.h"

#include "../WzImageProperty.h"

namespace MapleLib {
    namespace WzLib {
        namespace WzProperties {
            /// <summary>
            /// A property that contains data for an MP3 file
            /// </summary>
            class WzSoundProperty : WzExtended {
            public:
                std::wstring name;
                std::vector<uint8_t> mp3bytes{};
                WzObject parent;
                int len_ms;
                uint8_t* header;
                // WzImage imgParent;
                Util::WzBinaryReader wzReader;
                bool headerEncrypted = false;
                int64_t offs;
                int soundDataLen;
                constexpr static std::vector<uint8_t> soundHeader = {
                    0x02,
                    0x83, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70,
                    0x8B, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70,
                    0x00,
                    0x01,
                    0x81, 0x9F, 0x58, 0x05, 0x56, 0xC3, 0xCE, 0x11, 0xBF, 0x01, 0x00, 0xAA, 0x00, 0x55, 0x59, 0x5A };

                WaveFormat wavFormat;

                WzImageProperty DeepClone() override {
                    return WzSoundProperty{ name, len_ms, header, mp3bytes };
                }

                object getWzValue() override { return GetBytes(false); }

                WzPropertyType getPropertyType()  override { return WzPropertyType::Sound; }
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

                int getFrequency(){
                    if (wavFormat == nullptr) {
                        return 0;
                    }
                    return wavFormat.sampleRate;
                }
                
                    /// <summary>
                    /// BPS of the mp3 file
                    /// </summary>
                    //public byte BPS { get { return bps; } set { bps = value; } }
                    /// <summary>
                    /// Creates a WzSoundProperty with the specified name
                    /// </summary>
                    /// <param name="name">The name of the property</param>
                    /// <param name="reader">The wz reader</param>
                    /// <param name="parseNow">Indicating whether to parse the property now</param>
                WzSoundProperty(std::wstring name, Util::WzBinaryReader reader, bool parseNow) :name{ name }, wzReader{ reader } {
                    reader.BaseStream.Position++;

                    //note - soundDataLen does NOT include the length of the header.
                    soundDataLen = reader.ReadCompressedInt();
                    len_ms = reader.ReadCompressedInt();

                    int64_t headerOff = reader.BaseStream.Position;
                    reader.BaseStream.Position += soundHeader.Length; //skip GUIDs
                    int wavFormatLen = reader.ReadByte();
                    reader.BaseStream.Position = headerOff;

                    header = reader.ReadBytes(soundHeader.Length + 1 + wavFormatLen);
                    ParseHeader();

                    //sound file offs
                    offs = reader.BaseStream.Position;
                    if (parseNow) {
                        mp3bytes = reader.ReadBytes(soundDataLen);
                    }
                    else {
                        reader.BaseStream.Position += soundDataLen;
                    }
                }

                WzSoundProperty(std::wstring name, int len_ms, uint8_t* header, uint8_t* data) :
                    name{ name },
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
                WzSoundProperty(std::wstring name, std::wstring file) : name{ name } {
                    Mp3FileReader reader = new Mp3FileReader(file);
                    wavFormat = reader.Mp3WaveFormat;
                    len_ms = (int)((double)reader.Length * 1000d / (double)reader.WaveFormat.AverageBytesPerSecond);
                    RebuildHeader();
                    reader.Dispose();
                    mp3bytes = File.ReadAllBytes(file);
                }

                //??
                static bool memcmp(uint8_t* a, uint8_t* b, int n) {
                    for (int i = 0; i < n; i++) {
                        if (a[i] != b[i]) {
                            return false;
                        }
                    }
                    return true;
                }

                static std::wstring ByteArrayToString(std::vector<uint8_t>& ba) {
                    std::wstringstream hex{};
                    for (uint8_t& b : ba) {
                        hex << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
                    }
                    return hex.str();
                }

                void RebuildHeader()  {

                    std::ostringstream stream{};

                    // Write soundHeader
                    stream.write(reinterpret_cast<const char*>(soundHeader.data()), soundHeader.size());

                    // Convert wavFormat to bytes using StructToBytes
                    std::vector<uint8_t> wavHeader = StructToBytes(wavFormat);

                    // If header is encrypted, XOR with WzKey
                    if (headerEncrypted) {
                        for (size_t i = 0; i < wavHeader.size(); ++i) {
                            wavHeader[i] ^= wzReader.WzKey[i];
                        }
                    }

                    // Write the length of wavHeader as a byte
                    stream.put(static_cast<char>(wavHeader.size()));

                    // Write wavHeader
                    stream.write(reinterpret_cast<const char*>(wavHeader.data()), wavHeader.size());

                    // Get the resulting data from the stringstream
                    std::string data = stream.str();

                    // Copy the data to the header vector
                    header.assign(data.begin(), data.end());
                }

            private:
                template<typename T>
                static std::vector<uint8_t> StructToBytes(T obj) {
                    std::vector<uint8_t> result(Marshal.SizeOf(obj));
                    GCHandle handle = GCHandle.Alloc(result, GCHandleType.Pinned);
                    try {
                        Marshal.StructureToPtr(obj, handle.AddrOfPinnedObject(), false);
                        return result;
                    }
                    finally {
                        handle.Free();
                    }
                }

                template<typename T>
                static T BytesToStruct(std::vector<uint8_t>& data) {
                    GCHandle handle = GCHandle.Alloc(data, GCHandleType.Pinned);
                    try {
                        return Marshal.PtrToStructure<T>(handle.AddrOfPinnedObject());
                    }
                    finally {
                        handle.Free();
                    }
                }
                template<typename T>
                static T BytesToStructConstructorless(std::vector<uint8_t>& data) {
                    GCHandle handle = GCHandle.Alloc(data, GCHandleType.Pinned);
                    try {
                        T obj = (T)FormatterServices.GetUninitializedObject(typeof(T));
                        Marshal.PtrToStructure<T>(handle.AddrOfPinnedObject(), obj);
                        return obj;
                    }
                    finally {
                        handle.Free();
                    }
                }

                void ParseHeader() {
                    std::vector<uint8_t> wavHeader(header.size() - soundHeader.size() - 1);
                    Buffer.BlockCopy(header, soundHeader.size() + 1, wavHeader, 0, wavHeader.size());

                    if (wavHeader.size() < Marshal.SizeOf<WaveFormat>()) {
                        return;
                    }

                    WaveFormat wavFmt = BytesToStruct<WaveFormat>(wavHeader);

                    if (Marshal.SizeOf<WaveFormat>() + wavFmt.ExtraSize != wavHeader.size()) {
                        //try decrypt
                        for (int i = 0; i < wavHeader.size(); i++) {
                            wavHeader[i] ^= wzReader.WzKey[i];
                        }
                        wavFmt = BytesToStruct<WaveFormat>(wavHeader);

                        if (Marshal.SizeOf<WaveFormat>() + wavFmt.ExtraSize != wavHeader.size()) {
                            Helpers::ErrorLogger::Log(Helpers::ErrorLevel::Critical, "parse sound header failed");
                            return;
                        }
                        headerEncrypted = true;
                    }

                    // parse to mp3 header
                    if (wavFmt.Encoding == WaveFormatEncoding.MpegLayer3 && wavHeader.size() >= Marshal.SizeOf<Mp3WaveFormat>()) {
                        wavFormat = BytesToStructConstructorless<Mp3WaveFormat>(wavHeader);
                    }
                    else if (wavFmt.Encoding == WaveFormatEncoding.Pcm) {
                        wavFormat = wavFmt;
                    }
                    else {
                        Helpers::ErrorLogger::Log(Helpers::ErrorLevel::MissingFeature, std::format("Unknown wave encoding {0}", wavFmt.Encoding.ToString()));
                    }
                }
            public:
                std::vector<uint8_t> GetBytes(bool saveInMemory) {
                    if (mp3bytes.size() > 0) {
                        return mp3bytes;
                    }
                    else {
                        if (wzReader == null) {
                            return null;
                        }
                        long currentPos = wzReader.BaseStream.Position;
                        wzReader.BaseStream.Position = offs;
                        mp3bytes = wzReader.ReadBytes(soundDataLen);
                        wzReader.BaseStream.Position = currentPos;
                        if (saveInMemory) {
                            return mp3bytes;
                        }
                        else {
                            std::vector<uint8_t> result = mp3bytes;
                            mp3bytes.clear();
                            return result;
                        }
                    }
                }

                void SaveToFile(std::string path) {
                    std::ofstream outFile{ path };
                    auto theBytes = GetBytes(false);
                    outFile.write(static_cast<char*>(theBytes.data()), theBytes.size());
                }

                std::vector<uint8_t> GetBytes() override {
                    return GetBytes(false);
                }
            };
        }
    }
}