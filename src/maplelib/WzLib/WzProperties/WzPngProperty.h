#pragma once

#include "../Util/WzBinaryReader.h"
#include "../WzImageProperty.h"

#include <../external/zlib-1.3/zlib.h>
#include <../external/fpng.h>

#include <cstdint>

namespace MapleLib {
    namespace WzLib {
        namespace WzProperties {
            class WzPngProperty : public WzImageProperty {
            private:
                static bool wasFPNGinit;
            public:
                int width, height, format, format2;
                std::vector<uint8_t> compressedBytes{};
                Bitmap png;
                //internal WzImage imgParent;
                bool listWzUsed = false;

                Util::WzBinaryReader wzReader; //keep this as a member or call it then deconstruct it??
                int64_t offs;

                void Dispose() {
                    
                }
                std::wstring getName() {
                    return L"PNG";
                }
                void setName(std::wstring) {}
                void* getWzValue() override {
                    return &GetPNG(false); //need to fix this, its returning a temporary variable
                }

                template<typename T>
                void SetValue(T value) {
                    if constexpr (std::is_same_v<T, Bitmap>) {
                        SetPNG(value);
                    }
                    else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                        compressedBytes = value;
                    }
                }

                WzImageProperty* DeepClone() override {
                    WzPngProperty clone{};
                    clone.SetPNG(GetPNG(false));
                    return &clone; //need to fix this, its returning a temporary variable
                }

                void* getWzValue() { return &GetPNG(false); }//need to fix this, its returning a temporary variable
                void WriteValue(Util::WzBinaryWriter writer) override {
                    throw std::exception("Cannot write a PngProperty");
                }
                /// <summary>
                /// The WzPropertyType of the property
                /// </summary>
                WzPropertyType getPropertyType() override { return WzPropertyType::PNG; }

                int getFormat() { return format + format2; }
                void setFormat(int tempVal) { format = tempVal; format2 = 0; }

                bool getListWzUsed(){ return listWzUsed; } 

                template<typename T>
                void setListWzUsed(T value) { 
                    if (value != listWzUsed) { 
                        listWzUsed = value; 
                        CompressPng(GetPNG(false)); 
                    } 
                }
                void SetPNG(Bitmap png) {
                    this->png = png;
                    CompressPng(png);
                }

                    //[Obsolete("To enable more control over memory usage, this property was superseded by the GetCompressedBytes method and will be removed in the future")]
                std::vector<uint8_t> getCompressedBytes() {
                    return GetCompressedBytes(false);
                }

                WzPngProperty() { }
                WzPngProperty(Util::WzBinaryReader reader, bool parseNow);


                std::vector<uint8_t> GetCompressedBytes(bool saveInMemory);

                Bitmap GetPNG(bool saveInMemory);

                std::vector<uint8_t> Decompress(std::vector<uint8_t>& compressedBuffer, int decompressedSize);
                std::vector<uint8_t> Compress(const std::vector<uint8_t>& decompressedBuffer);

                //the wzkey will come from the parent image
                void ParsePng(const std::vector<uint8_t>& wzKey);

                void CompressPng(Bitmap& bmp);

                Bitmap GetBitmap() override { return GetPNG(false); }

            private:

                static std::vector<uint8_t> GetPixelDataDXT3(std::vector<uint8_t>& rawData, int width, int height);

                static std::vector<uint8_t> GetPixelDataDXT5(std::vector<uint8_t>& rawData, int width, int height);

                static void ExpandAlphaTableDXT5(uint8_t* alpha, uint8_t a0, uint8_t a1);

                static void ExpandAlphaIndexTableDXT5(int* alphaIndex, std::vector<uint8_t> const& rawData, int offset);

                static void SetPixel(std::vector<uint8_t>& pixelData, int x, int y, int width, Color color, uint8_t alpha);

                static void ExpandColorTable(Color* color, uint16_t u0, uint16_t u1);

                static void ExpandColorIndexTable(int* colorIndex, std::vector<uint8_t> const& rawData, int offset);

                static void ExpandAlphaTable(uint8_t* alpha, const std::vector<uint8_t>& rawData, int offset);

                static Color RGB565ToColor(uint16_t val);
            };
        }
    }
}