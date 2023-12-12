#include "WzPngProperty.h"

namespace MapleLib {
    namespace WzLib {
        namespace WzProperties {

            bool WzPngProperty::wasFPNGinit{ false };


            WzPngProperty::WzPngProperty(Util::WzBinaryReader& reader, bool parseNow) : reader{ reader } {
                // Read compressed bytes
                width = reader.ReadCompressedInt();
                height = reader.ReadCompressedInt();
                format = reader.ReadCompressedInt();
                format2 = reader.ReadByte();
                reader.filePosition += 4;
                offs = reader.filePosition;
                int32_t len;
                //reader.readFile.read((char*)(&len), sizeof(int32_t));
                reader.ReadTemplate<int32_t>(&len);
                len -= 1;

                reader.filePosition += 1;

                if (len > 0) {
                    if (parseNow) {
                        compressedBytes.resize(len);
                        //wzReader.readFile.read((char*)(&compressedBytes[0]), len);
                        reader.ReadBytesInto(&compressedBytes[0], len);

                        //ParsePng(); //need a parent image wzkey
                    }
                    else {
                        reader.filePosition += len;
                    }
                }
            }

            std::vector<uint8_t> WzPngProperty::GetCompressedBytes(bool saveInMemory) {
                if (compressedBytes.size() != 0) {
                    //i dont really understand this
                    return compressedBytes;
                }

                int64_t pos = reader.filePosition;
                reader.filePosition = offs;
                int len;
                reader.ReadTemplate<int32_t>(&len);
                len -= 1;
                reader.filePosition += 1;

                if (len > 0) {
                    compressedBytes.resize(len);
                    //wzReader.readFile.read((char*)compressedBytes.data(), len);
                    reader.ReadBytesInto(&compressedBytes[0], len);
                }
                reader.filePosition = pos;
                if (!saveInMemory) {
                    std::vector<uint8_t> copyVec{ compressedBytes };
                    compressedBytes.clear();
                    return copyVec;
                }

                return compressedBytes;
            }

            Bitmap WzPngProperty::GetPNG(bool saveInMemory) {
                if (png.data.size() == 0) { //idk how to fix this, maybe a flag?
                    int64_t pos = reader.filePosition;
                    reader.filePosition = offs;
                    int len;
                    reader.ReadTemplate<int32_t>(&len);
                    len -= 1;
                    reader.filePosition += 1;
                    if (len > 0) {
                        compressedBytes.resize(len);
                        reader.ReadBytesInto(&compressedBytes[0], len);
                    }
                    ParsePng(); //need parent image wzkey
                    reader.filePosition = pos;
                    if (!saveInMemory) {
                        Bitmap copyImage{ png };
                        png.data.clear();
                        compressedBytes.clear();
                        return copyImage;
                    }
                }
                return png;
            }

            std::vector<uint8_t> WzPngProperty::Decompress(std::vector<uint8_t>& compressedBuffer, int decompressedSize) {
                //probably shouldnt use this

                std::vector<uint8_t> buffer(decompressedSize);
                z_stream stream;
                stream.zalloc = Z_NULL;
                stream.zfree = Z_NULL;
                stream.opaque = Z_NULL;
                stream.avail_in = compressedBuffer.size() - 2;
                stream.next_in = &compressedBuffer[2];
                stream.avail_out = buffer.size();
                stream.next_out = &buffer[0];

                if (inflateInit(&stream) != Z_OK) {
                    std::cerr << "Error initializing zlib" << std::endl;
                    return {};
                }

                if (inflate(&stream, Z_FINISH) != Z_STREAM_END) {
                    std::cerr << "Error decompressing data" << std::endl;
                    inflateEnd(&stream);
                    return {};
                }

                inflateEnd(&stream);

                return buffer;
            }

            std::vector<uint8_t> WzPngProperty::Compress(const std::vector<uint8_t>& decompressedBuffer) {
                std::vector<uint8_t> compressedBuffer;

                // Write the zlib header (0x78 0x9C) to the output buffer
                compressedBuffer.push_back(0x78);
                compressedBuffer.push_back(0x9C);

                // Compress the data
                z_stream stream;
                stream.zalloc = Z_NULL;
                stream.zfree = Z_NULL;
                stream.opaque = Z_NULL;

                if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
                    throw std::exception("failed to compress");
                    std::cerr << "Error initializing zlib" << std::endl;
                    return {};
                }

                stream.next_in = const_cast<uint8_t*>(decompressedBuffer.data());
                stream.avail_in = decompressedBuffer.size();

                std::vector<uint8_t> tempBuffer(1024);

                do {
                    stream.next_out = tempBuffer.data();
                    stream.avail_out = tempBuffer.size();

                    if (deflate(&stream, Z_FINISH) == Z_STREAM_ERROR) {
                        std::cerr << "Error compressing data" << std::endl;
                        deflateEnd(&stream);
                        return {};
                    }

                    size_t compressedSize = tempBuffer.size() - stream.avail_out;
                    compressedBuffer.insert(compressedBuffer.end(), tempBuffer.begin(), tempBuffer.begin() + compressedSize);

                } while (stream.avail_out == 0);

                deflateEnd(&stream);

                return compressedBuffer;
            }

            void WzPngProperty::ParsePng() {

                z_stream zstream;
                int uncompressedSize = 0;
                int x = 0, y = 0, b = 0, g = 0;
                Bitmap bmp{};
                std::vector<uint8_t> decBuf{}; //decompressed buffer
                std::vector<uint8_t> outData{};

                uint16_t header = compressedBytes[0] << 8 + compressedBytes[1];
                listWzUsed = (header != 0x9C78) && (header != 0xDA78) && (header != 0x0178) && (header != 0x5E78);
                if (!listWzUsed) {
                    zstream.zalloc = Z_NULL;
                    zstream.zfree = Z_NULL;
                    zstream.opaque = Z_NULL;
                    zstream.avail_in = compressedBytes.size();
                    zstream.next_in = &compressedBytes[2];
                    zstream.avail_out = decBuf.size();
                    zstream.next_out = &decBuf[0];

                    if (deflateInit(&zstream) != Z_OK) {
                        std::cerr << "Error initializing zlib" << std::endl;
                        throw std::exception("error deflating");
                    }

                    //zlib = new DeflateStream(reader.BaseStream, CompressionMode.Decompress);
                }
                else {
                    int blockSize = 0;
                    int endOfPng = compressedBytes.size();
                    size_t readerPos = 0;
                    //idk the reserve, maybe just reserve endofpng? then a shrink to fit afterwards?
                    while (readerPos < endOfPng) {
                        blockSize = compressedBytes[readerPos] << 24 + compressedBytes[readerPos + 1] << 16 + compressedBytes[readerPos + 2] << 8 + compressedBytes[readerPos + 3];
                        readerPos += 4;
                        for (int i = 0; i < blockSize; i++) {
                            outData.push_back(compressedBytes[readerPos + i] ^ reader.WzKey[i]);
                        }
                        readerPos += blockSize;
                    }
                    zstream.zalloc = Z_NULL;
                    zstream.zfree = Z_NULL;
                    zstream.opaque = Z_NULL;
                    zstream.avail_in = outData.size();
                    zstream.next_in = &outData[2];
                    zstream.avail_out = decBuf.size();
                    zstream.next_out = &decBuf[0];

                    if (deflateInit(&zstream) != Z_OK) {
                        std::cerr << "Error initializing zlib" << std::endl;
                        throw std::exception("error deflating");
                    }

                    //zlib = new DeflateStream(dataStream, CompressionMode.Decompress);
                }

                bmp.width = width;
                bmp.height = height;
                switch (format + format2) {
                case 1: {
                    //bmp.bitDepth = Format32bppArgb;

                        //wtf is this? BMP is not that hard lmao
                    //bmpData = bmp.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
                    uncompressedSize = width * height * 2;

                    decBuf.resize(uncompressedSize);

                    if (deflate(&zstream, Z_FINISH) != Z_STREAM_END) {
                        std::cerr << "Error decompressing data" << std::endl;
                        deflateEnd(&zstream);
                        throw std::exception("failed to decompress");
                    }

                    deflateEnd(&zstream);
                    int b = 0, g = 0;

                    std::vector<uint8_t> argb(uncompressedSize * 2);
                    for (int i = 0; i < uncompressedSize; i++) {
                        b = decBuf[i] & 0x0F;
                        b |= (b << 4);
                        argb[i * 2] = static_cast<uint8_t>(b);

                        g = decBuf[i] & 0xF0;
                        g |= (g >> 4);
                        argb[i * 2 + 1] = static_cast<uint8_t>(g);
                    }

                    bmp.data.resize(argb.size());
                    std::memcpy(&bmp.data[0], argb.data(), argb.size());
                    break;
                }
                case 2: {
                    //bmp.bitDepth = Format32bppArgb;
                    uncompressedSize = width * height * 4;
                    decBuf.resize(uncompressedSize);
                    if (deflate(&zstream, Z_FINISH) != Z_STREAM_END) {
                        std::cerr << "Error decompressing data" << std::endl;
                        deflateEnd(&zstream);
                        throw std::exception("failed to decompress");
                    }

                    deflateEnd(&zstream);
                    bmp.data.resize(decBuf.size());
                    memcpy(&bmp.data[0], decBuf.data(), decBuf.size());
                    break;
                }
                case 3: {// thanks to Elem8100 
                    //bmp.bitDepth = Format32bppArgb;
                    int w = static_cast<int>(std::ceil(static_cast<double>(width) / 4.0));
                    int h = static_cast<int>(std::ceil(static_cast<double>(height) / 4.0));
                    uncompressedSize = w * 4 * h * 4 / 8;
                    decBuf.resize(uncompressedSize);

                    if (deflate(&zstream, Z_FINISH) != Z_STREAM_END) {
                        std::cerr << "Error decompressing data" << std::endl;
                        deflateEnd(&zstream);
                        throw std::exception("failed to decompress");
                    }

                    deflateEnd(&zstream);

                    std::vector<int> argb2(width * height);
                    { //random scope
                        int index;
                        int index2;
                        int p;
                        for (int i = 0; i < h; i++) {
                            for (int j = 0; j < w; j++) {
                                index = (j + i * w) * 2;
                                index2 = j * 4 + i * width * 4;
                                p = (decBuf[index] & 0x0F) | ((decBuf[index] & 0x0F) << 4);
                                p |= ((decBuf[index] & 0xF0) | ((decBuf[index] & 0xF0) >> 4)) << 8;
                                p |= ((decBuf[index + 1] & 0x0F) | ((decBuf[index + 1] & 0x0F) << 4)) << 16;
                                p |= ((decBuf[index + 1] & 0xF0) | ((decBuf[index] & 0xF0) >> 4)) << 24;

                                for (int k = 0; k < 4; k++) {
                                    if (x * 4 + k < width) {
                                        argb2[index2 + k] = p;
                                    }
                                    else {
                                        break;
                                    }
                                }
                            }
                            index2 = y * width * 4;
                            for (int m = 1; m < 4; m++) {
                                if (y * 4 + m < height) {
                                    std::copy(
                                        argb2.begin() + index2,
                                        argb2.begin() + index2 + width,
                                        argb2.begin() + (index2 + m * width)
                                    );
                                }
                                else {
                                    break;
                                }
                            }
                        }
                    }
                    bmp.data.resize(argb2.size());
                    memcpy(&bmp.data[0], &argb2[0], argb2.size());
                    break;
                }
                case 513: {
                    bmp.bitDepth = 16;
                    //bmp.bitDepth = Format16bppRgb565

                    uncompressedSize = width * height * 2;
                    decBuf.resize(uncompressedSize);

                    if (deflate(&zstream, Z_FINISH) != Z_STREAM_END) {
                        std::cerr << "Error decompressing data" << std::endl;
                        deflateEnd(&zstream);
                        throw std::exception("failed to decompress");
                    }

                    deflateEnd(&zstream);

                    bmp.data.resize(decBuf.size());
                    memcpy(&bmp.data[0], decBuf.data(), decBuf.size());
                    break;
                }
                case 517: {
                    //no format here??


                    uncompressedSize = width * height / 128;
                    decBuf.resize(uncompressedSize * 32);
                    bmp.data.resize(uncompressedSize);

                    if (deflate(&zstream, Z_FINISH) != Z_STREAM_END) {
                        std::cerr << "Error decompressing data" << std::endl;
                        deflateEnd(&zstream);
                        throw std::exception("failed to decompress");
                    }
                    deflateEnd(&zstream);

                    uint8_t iB = 0;
                    size_t index = 0;
                    for (int i = 0; i < uncompressedSize; i++) {
                        for (uint8_t j = 0; j < 8; j++) {
                            iB = (((decBuf[i] & (0x01 << (7 - j))) >> (7 - j)) * 0xFF);
                            for (int k = 0; k < 16; k++) {
                                if (index >= bmp.data.size()) {
                                    std::cerr << "writing to bmp error \n";
                                    throw std::exception("data error");
                                }

                                bmp.data[index] = 0xFF;
                                bmp.data[index + 1] = iB;
                                bmp.data[index + 2] = iB;
                                bmp.data[index + 3] = iB;
                                index++;
                            }
                        }
                    }
                    break;
                }
                case 1026: {
                    //bmp.format = Format32bppArgb;

                    uncompressedSize = width * height;
                    decBuf.resize(uncompressedSize);

                    if (deflate(&zstream, Z_FINISH) != Z_STREAM_END) {
                        std::cerr << "Error decompressing data" << std::endl;
                        deflateEnd(&zstream);
                        throw std::exception("failed to decompress");
                    }
                    deflateEnd(&zstream);

                    decBuf = GetPixelDataDXT3(decBuf, width, height);

                    bmp.data.resize(decBuf.size());
                    memcpy(&bmp.data[0], decBuf.data(), decBuf.size());
                    
                    break;
                }
                case 2050: { // thanks to Elem8100
                    //bmp.format = Format32bppArgb;
                    
                    uncompressedSize = width * height;
                    decBuf.resize(uncompressedSize);

                    if (deflate(&zstream, Z_FINISH) != Z_STREAM_END) {
                        std::cerr << "Error decompressing data" << std::endl;
                        deflateEnd(&zstream);
                        throw std::exception("failed to decompress");
                    }
                    deflateEnd(&zstream);

                    decBuf = GetPixelDataDXT5(decBuf, width, height);

                    bmp.data.resize(decBuf.size());
                    memcpy(&bmp.data[0], decBuf.data(), decBuf.size());
                    
                    break;
                }
                default:
                    Helpers::ErrorLogger::Log(Helpers::ErrorLevel::MissingFeature, std::format("Unknown PNG format {0} {1}", format, format2));
                    break;
                }
                png = bmp;
            }

            void WzPngProperty::CompressPng(Bitmap& bmp) {


                format = 2;
                format2 = 0;
                width = bmp.width;
                height = bmp.height;
                compressedBytes = Compress(bmp.data);
                if (listWzUsed)
                {
                    std::vector<uint8_t> dataBuffer(compressedBytes.size() + 8);
                    auto wzKey = Util::WzTool::GetIvByMapleVersion(WzMapleVersion::GMS);
                    int tempVal = 2;
                    memcpy(&dataBuffer[0], &tempVal, sizeof(int)); //writes 0-3

                    for (int i = 0; i < 2; i++) {
                        dataBuffer[i + 4] = compressedBytes[i] ^ wzKey[i]; //writes 4-5
                    }
                    int sizeIn4Bytes = compressedBytes.size();
                    memcpy(&dataBuffer[6], &sizeIn4Bytes, sizeof(int)); //writes 6-9
                    for (int i = 2; i < compressedBytes.size(); i++) {
                        dataBuffer[8 + i] = compressedBytes[i] ^ wzKey[i - 2]; //writes to 10 and 11
                    }
                    compressedBytes.clear();
                    compressedBytes = dataBuffer;
                }
            }

            std::vector<uint8_t> WzPngProperty::GetPixelDataDXT3(std::vector<uint8_t>& rawData, int width, int height) {
                std::vector<uint8_t> pixel(width * height * 4);


                Color colorTable[4];// = new Color[4];
                int colorIdxTable[16];// = new int[16];
                uint8_t alphaTable[16];
                for (int y = 0; y < height; y += 4) {
                    for (int x = 0; x < width; x += 4) {
                        int off = x * 4 + y * width;
                        ExpandAlphaTable(alphaTable, rawData, off);
                        uint16_t u0;
                        //= BitConverter.ToUInt16(rawData, off + 8);
                        memcpy(&u0, &rawData[8 + off], sizeof(uint16_t));
                        uint16_t u1;
                        memcpy(&u1, &rawData[10 + off], sizeof(uint16_t));
                        ExpandColorTable(colorTable, u0, u1);
                        ExpandColorIndexTable(colorIdxTable, rawData, off + 12);

                        for (int j = 0; j < 4; j++) {
                            for (int i = 0; i < 4; i++) {
                                SetPixel(pixel,
                                    x + i,
                                    y + j,
                                    width,
                                    colorTable[colorIdxTable[j * 4 + i]],
                                    alphaTable[j * 4 + i]);
                            }
                        }
                    }
                }

                return pixel;
            }

            std::vector<uint8_t> WzPngProperty::GetPixelDataDXT5(std::vector<uint8_t>& rawData, int width, int height) {
                std::vector<uint8_t> pixel(width * height * 4);// = new byte[width * height * 4];

                Color colorTable[4];// = new Color[4];
                int colorIdxTable[16];// = new int[16];
                uint8_t alphaTable[8];// = new byte[8];
                int alphaIdxTable[16];// = new int[16];
                for (int y = 0; y < height; y += 4) {
                    for (int x = 0; x < width; x += 4) {
                        int off = x * 4 + y * width;
                        ExpandAlphaTableDXT5(alphaTable, rawData[off + 0], rawData[off + 1]);
                        ExpandAlphaIndexTableDXT5(alphaIdxTable, rawData, off + 2);
                        uint16_t u0;
                        memcpy(&u0, &rawData[off + 8], sizeof(uint16_t));

                        uint16_t u1;
                        memcpy(&u1, &rawData[off + 10], sizeof(uint16_t));
                        ExpandColorTable(colorTable, u0, u1);
                        ExpandColorIndexTable(colorIdxTable, rawData, off + 12);

                        for (int j = 0; j < 4; j++) {
                            for (int i = 0; i < 4; i++) {
                                SetPixel(pixel,
                                    x + i,
                                    y + j,
                                    width,
                                    colorTable[colorIdxTable[j * 4 + i]],
                                    alphaTable[alphaIdxTable[j * 4 + i]]);
                            }
                        }
                    }
                }

                return pixel;
            }

            void WzPngProperty::ExpandAlphaTableDXT5(uint8_t* alpha, uint8_t a0, uint8_t a1) {
                alpha[0] = a0;
                alpha[1] = a1;
                if (a0 > a1) {
                    for (int i = 2; i < 8; i++) {
                        alpha[i] = (uint8_t)(((8 - i) * a0 + (i - 1) * a1 + 3) / 7);
                    }
                }
                else {
                    for (int i = 2; i < 6; i++) {
                        alpha[i] = (uint8_t)(((6 - i) * a0 + (i - 1) * a1 + 2) / 5);
                    }
                    alpha[6] = 0;
                    alpha[7] = 255;
                }
            }

            void WzPngProperty::ExpandAlphaIndexTableDXT5(int* alphaIndex, std::vector<uint8_t> const& rawData, int offset) {
                for (int i = 0; i < 16; i += 8, offset += 3) {
                    int flags = rawData[offset]
                        | (rawData[offset + 1] << 8)
                        | (rawData[offset + 2] << 16);
                    for (int j = 0; j < 8; j++) {
                        int mask = 0x07 << (3 * j);
                        alphaIndex[i + j] = (flags & mask) >> (3 * j);
                    }
                }
            }

            void WzPngProperty::SetPixel(std::vector<uint8_t>& pixelData, int x, int y, int width, Color color, uint8_t alpha) {
                int offset = (y * width + x) * 4;
                pixelData[offset + 0] = color.b;
                pixelData[offset + 1] = color.g;
                pixelData[offset + 2] = color.r;
                pixelData[offset + 3] = alpha;
            }
           
            void WzPngProperty::ExpandColorTable(Color* color, uint16_t u0, uint16_t u1) {
                color[0] = RGB565ToColor(u0);
                color[1] = RGB565ToColor(u1);

                color[2].r = static_cast<unsigned char>((color[0].r * 2 + color[1].r + 1) / 3);
                color[2].g = static_cast<unsigned char>((color[0].g * 2 + color[1].g + 1) / 3);
                color[2].b = static_cast<unsigned char>((color[0].b * 2 + color[1].b + 1) / 3);

                color[3].r = static_cast<unsigned char>((color[0].r + color[1].r * 2 + 1) / 3);
                color[3].g = static_cast<unsigned char>((color[0].g + color[1].g * 2 + 1) / 3);
                color[3].b = static_cast<unsigned char>((color[0].b + color[1].b * 2 + 1) / 3);
            }

            void WzPngProperty::ExpandColorIndexTable(int* colorIndex, std::vector<uint8_t> const& rawData, int offset) {
                for (int i = 0; i < 16; i += 4) {
                    colorIndex[i + 0] = (rawData[offset] & 0x03);
                    colorIndex[i + 1] = (rawData[offset] & 0x0c) >> 2;
                    colorIndex[i + 2] = (rawData[offset] & 0x30) >> 4;
                    colorIndex[i + 3] = (rawData[offset] & 0xc0) >> 6;
                    offset++;
                }
            }
           
            void WzPngProperty::ExpandAlphaTable(uint8_t* alpha, const std::vector<uint8_t>& rawData, int offset) {
                for (int i = 0; i < 16; i += 2) {
                    alpha[i + 0] = (uint8_t)(rawData[offset] & 0x0f);
                    alpha[i + 1] = (uint8_t)((rawData[offset] & 0xf0) >> 4);
                    offset++;
                }
                for (int i = 0; i < 16; i++) {
                    alpha[i] = (uint8_t)(alpha[i] | (alpha[i] << 4));
                }
            }
            
            Color WzPngProperty::RGB565ToColor(uint16_t val) {
                const int rgb565_mask_r = 0xf800;
                const int rgb565_mask_g = 0x07e0;
                const int rgb565_mask_b = 0x001f;
                int r = (val & rgb565_mask_r) >> 11;
                int g = (val & rgb565_mask_g) >> 5;
                int b = (val & rgb565_mask_b);
                Color c;
                c.r = static_cast<unsigned char>((r << 3) | (r >> 2));
                c.g = static_cast<unsigned char>((g << 2) | (g >> 4));
                c.b = static_cast<unsigned char>((b << 3) | (b >> 2));
                return c;
            }

        }
    }
}