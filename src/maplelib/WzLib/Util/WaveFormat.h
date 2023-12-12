#pragma once

#include "WavFormatEncoding.h"

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <vector>

namespace MapleLib {

    struct WaveFormat {
    protected:
        WaveFormatEncoding waveFormatTag;
        int16_t channels;
        int sampleRate;
        int averageBytesPerSecond;
        int16_t blockAlign;
        int16_t bitsPerSample;
        int16_t extraSize;

    public:
        //extra size might nto be it, placeholder
        WaveFormat() : WaveFormat{ 44100, 16, 2 } {}

        WaveFormat(int sampleRate, int channels) : WaveFormat{sampleRate, 16, channels} {}

        int ConvertLatencyToByteSize(int ms) {
            int bytes = static_cast<int>(averageBytesPerSecond / 1000.0) * ms;
            if ((bytes % blockAlign) != 0) {
                bytes += blockAlign - (bytes % blockAlign);
            }
            return bytes;
        }
        WaveFormat(int rate, int bits, int channels) {
            if (channels < 1) {
                printf("channels must be greater than one\n");
                assert(channels >= 1);
            }

            waveFormatTag = WaveFormatEncoding::Pcm;
            this->channels = static_cast<int16_t>(channels);
            sampleRate = rate;
            bitsPerSample = static_cast<int16_t>(bits);
            extraSize = 0;

            blockAlign = static_cast<int16_t>(channels * (bits / 8));
            averageBytesPerSecond = rate * blockAlign;
        }
        WaveFormat(std::vector<uint8_t>& binary) {
            int formatChunkLength;
            memcpy(&formatChunkLength, &binary[0], sizeof(int));

            ReadWaveFormat(binary, formatChunkLength, 4);
        }

        //skip this until its necessary?
        static WaveFormat CreateCustomFormat() {}
        static WaveFormat CreateALawFormat() {}
        static WaveFormat CreateAMuLawFormat() {}

        WaveFormatEncoding getEncoding() {
            return waveFormatTag;
        }
        int getSampleRate() {
            return sampleRate;
        }
        int getExtraSize() {
            return extraSize;
        }

    private:
        void ReadWaveFormat(std::vector<uint8_t>& binary, int formatChunkLength, int startingOffset) {
            assert(formatChunkLength >= 16 && "format chunk length too small");
                
            uint16_t shortEncoding = (binary[startingOffset] << 8);
            shortEncoding += binary[startingOffset + 1];
            waveFormatTag = (WaveFormatEncoding)(shortEncoding);

            memcpy(&channels, &binary[startingOffset + 2], sizeof(int16_t));
            memcpy(&sampleRate, &binary[startingOffset + 4], sizeof(int32_t));
            
            memcpy(&averageBytesPerSecond, &binary[startingOffset + 8], sizeof(int32_t));
            memcpy(&blockAlign, &binary[startingOffset + 12], sizeof(int16_t));
            memcpy(&bitsPerSample, &binary[startingOffset + 14], sizeof(int16_t));

            if (formatChunkLength > 16) {
                memcpy(&extraSize, &binary[startingOffset + 16], sizeof(int16_t));
                if (extraSize != (formatChunkLength - 18)) {
                    printf("format chunk mismatch\n");
                    extraSize = static_cast<int16_t>(formatChunkLength - 18);
                }

            }


        }

    };




    struct Mp3WaveFormat : WaveFormat {
        enum Mp3WaveFormatId : uint16_t {
            /// <summary>MPEGLAYER3_ID_UNKNOWN</summary>
            Unknown = 0,
            /// <summary>MPEGLAYER3_ID_MPEG</summary>
            Mpeg = 1,
            /// <summary>MPEGLAYER3_ID_CONSTANTFRAMESIZE</summary>
            ConstantFrameSize = 2
        };
        enum Mp3WaveFormatFlags {
            /// <summary>
            /// MPEGLAYER3_FLAG_PADDING_ISO
            /// </summary>
            PaddingIso = 0,
            /// <summary>
            /// MPEGLAYER3_FLAG_PADDING_ON
            /// </summary>
            PaddingOn = 1,
            /// <summary>
            /// MPEGLAYER3_FLAG_PADDING_OFF
            /// </summary>
            PaddingOff = 2,
        };

        Mp3WaveFormatId id;
        Mp3WaveFormatFlags flags;
        uint16_t blockSize;
        uint16_t framesPerBlock;
        uint16_t codecDelay;

        Mp3WaveFormat(int sampleRate, int channels, int blockSize, int bitRate) {
            waveFormatTag = WaveFormatEncoding::MpegLayer3;
            channels = (short)channels;
            averageBytesPerSecond = bitRate / 8;
            bitsPerSample = 0; // must be zero
            blockAlign = 1; // must be 1
            sampleRate = sampleRate;

            extraSize = Mp3WaveFormatExtraBytes;
            id = Mp3WaveFormatId::Mpeg;
            flags = Mp3WaveFormatFlags::PaddingIso;
            blockSize = static_cast<uint16_t>(blockSize);
            framesPerBlock = 1;
            codecDelay = 0;
        }
    private:
        const int16_t Mp3WaveFormatExtraBytes = 12;
    };
}