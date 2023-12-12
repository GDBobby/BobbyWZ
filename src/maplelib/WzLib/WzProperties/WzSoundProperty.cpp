#include "WzSoundProperty.h"

namespace MapleLib {
    namespace WzLib {
        namespace WzProperties {
            const std::vector<uint8_t> WzSoundProperty::soundHeader = {
                                0x02,
                                0x83, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70,
                                0x8B, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70,
                                0x00,
                                0x01,
                                0x81, 0x9F, 0x58, 0x05, 0x56, 0xC3, 0xCE, 0x11, 0xBF, 0x01, 0x00, 0xAA, 0x00, 0x55, 0x59, 0x5A
            };



            WzSoundProperty::WzSoundProperty(std::wstring name, Util::WzBinaryReader& reader, bool parseNow) : WzImageProperty{ name }, wzReader{ reader } {
                reader.filePosition++;

                //note - soundDataLen does NOT include the length of the header.
                soundDataLen = reader.ReadCompressedInt();
                len_ms = reader.ReadCompressedInt();

                int64_t headerOff = reader.filePosition;
                reader.filePosition += soundHeader.size(); //skip GUIDs
                int wavFormatLen = reader.ReadByte();
                reader.filePosition = headerOff;

                header = reader.ReadBytes(soundHeader.size() + 1 + wavFormatLen);
                ParseHeader();

                //sound file offs
                offs = reader.filePosition;
                if (parseNow) {
                    mp3bytes = reader.ReadBytes(soundDataLen);
                }
                else {
                    reader.filePosition += soundDataLen;
                }
            }

            bool WzSoundProperty::memcmp(uint8_t* a, uint8_t* b, int n) {
                for (int i = 0; i < n; i++) {
                    if (a[i] != b[i]) {
                        return false;
                    }
                }
                return true;
            }

            std::wstring WzSoundProperty::ByteArrayToString(std::vector<uint8_t>& ba) {
                std::wstringstream hex{};
                for (uint8_t& b : ba) {
                    hex << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
                }
                return hex.str();
            }

            void WzSoundProperty::RebuildHeader() {

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
        
            template<typename T>
            std::vector<uint8_t> WzSoundProperty::StructToBytes(T obj) {
                std::vector<uint8_t> result(sizeof(T));
                memcpy(&result, &obj, sizeof(T));
                return result;
            }
            template<typename T>
            T WzSoundProperty::BytesToStruct(std::vector<uint8_t>& data) {
                T result;
                memcpy(&result, &data, sizeof(T));
            }
        
            void WzSoundProperty::ParseHeader() {
                std::vector<uint8_t> wavHeader(header.begin() + soundHeader.size() + 1, header.end());

                if (wavHeader.size() < sizeof(WaveFormat)) {
                    printf("parsing undersized header\n");
                    return;
                }

                WaveFormat wavFmt = BytesToStruct<WaveFormat>(wavHeader);

                if ((sizeof(WaveFormat) + wavFmt.getExtraSize()) != wavHeader.size()) {
                    //try decrypt
                    for (int i = 0; i < wavHeader.size(); i++) {
                        wavHeader[i] ^= wzReader.WzKey[i];
                    }
                    wavFmt = BytesToStruct<WaveFormat>(wavHeader);

                    if ((sizeof(WaveFormat) + wavFmt.getExtraSize()) != wavHeader.size()) {
                        Helpers::ErrorLogger::Log(Helpers::ErrorLevel::Critical, "parse sound header failed");
                        return;
                    }
                    headerEncrypted = true;
                }

                // parse to mp3 header
                if (wavFmt.getEncoding() == WaveFormatEncoding::MpegLayer3 && wavHeader.size() >= sizeof(Mp3WaveFormat)) {
                    wavFormat = BytesToStruct<Mp3WaveFormat>(wavHeader);
                }
                else if (wavFmt.getEncoding() == WaveFormatEncoding::Pcm) {
                    wavFormat = wavFmt;
                }
                else {
                    Helpers::ErrorLogger::Log(Helpers::ErrorLevel::MissingFeature, std::format("Unknown wave encoding {}", wavFmt.getEncoding()));
                }
            }

            std::vector<uint8_t> WzSoundProperty::GetBytes(bool saveInMemory) {
                if (mp3bytes.size() > 0) {
                    return mp3bytes;
                }
                else {
                    size_t currentPos = wzReader.filePosition;
                    wzReader.filePosition = offs;
                    mp3bytes = wzReader.ReadBytes(soundDataLen);
                    wzReader.filePosition = currentPos;
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
        }
    }
}