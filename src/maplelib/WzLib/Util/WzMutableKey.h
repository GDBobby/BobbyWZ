#pragma once

#include <vector>
#include <cstdint>
#include <cmath>

namespace MapleLib{namespace WzLib{ namespace Util {
    class WzMutableKey {
    public:
        WzMutableKey(std::vector<uint8_t> WzIv, std::vector<uint8_t>& AesKey) :
            iv{ WzIv },
                aesKey{ AesKey }
        {
        }
    private:

        static const int BatchSize = 4096;
        std::vector<uint8_t> iv;
        std::vector<uint8_t> aesKey;

        std::vector<uint8_t> keys;
    public:
        uint8_t at(int index) {
            if ((keys.size() == 0) || (keys.size() <= index)) {
                EnsureKeySize(index + 1);
            }
            return keys.at(index);
        }

        void EnsureKeySize(int size){ 
            if ((keys.size() == 0) && (keys.size() >= size)) {
                //?
                return;
            }

            size = static_cast<int>(std::ceil(static_cast<double>(size) / static_cast<double>(BatchSize)) * BatchSize);
            std::vector<uint8_t> newKeys(size);// = new byte[size];

            if ((iv[0] | iv[1] | iv[2] | iv[3]) == 0) {
                keys = newKeys;
                return;
            }

            int startIndex = 0;

            if (keys.size() != 0) {
                std::copy(keys.begin(), keys.end(), newKeys.begin());
                startIndex = keys.size();
            }

            Rijndael aes = Rijndael.Create();
            aes.KeySize = 256;
            aes.BlockSize = 128;
            aes.Key = aesKey;
            aes.Mode = CipherMode.ECB;
            MemoryStream ms = new MemoryStream(newKeys, startIndex, newKeys.Length - startIndex, true);
            CryptoStream s = new CryptoStream(ms, aes.CreateEncryptor(), CryptoStreamMode.Write);

            std::vector<uint8_t> block(16);
            for (int i = startIndex; i < size; i += 16) {
                if (i == 0) {
                    for (int j = 0; j < 16; j++) {
                        block[j] = iv[j % 4];
                    }
                    s.Write(block, 0, block.Length);
                }
                else
                {
                    s.Write(newKeys, i - 16, 16);
                }
            }

            s.Flush();
            ms.Close();
            this.keys = newKeys;
        }
    }
}