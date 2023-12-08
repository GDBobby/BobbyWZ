#pragma once

#include "cryptopp890/modes.h"
#include "cryptopp890/aes.h"
#include "cryptopp890/filters.h"

#include <string>
#include <iostream>
#include <stdexcept>

namespace MapleLib {
	namespace CryptoLib {

		/// <summary>
		/// Class to handle the AES Encryption routines
		/// </summary>
		class AESEncryption {
		public:

			/// <summary>
			/// Encrypt data using MapleStory's AES algorithm
			/// </summary>
			/// <param name="IV">IV to use for encryption</param>
			/// <param name="data">Data to encrypt</param>
			/// <param name="length">Length of data</param>
			/// <returns>Crypted data</returns>
			static char* aesCrypt(std::string& ciphertext, std::string& plaintext, int length) {
				CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
				CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
				memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
				memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);

				//
				// String and Sink setup
				//
				std::string decryptedtext;

				//
				// Dump Plain Text
				//
				std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
				std::cout << plaintext;
				std::cout << std::endl << std::endl;

				CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
				CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

				//
				// Create Cipher Text
				//
				CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
				CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

				CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
				stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
				stfEncryptor.MessageEnd();

				//
				// Dump Cipher Text
				//
				std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

				for (int i = 0; i < ciphertext.size(); i++) {

					std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
				}

				std::cout << std::endl << std::endl;

				//
				// Decrypt
				//
				CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
				CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

				CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
				stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
				stfDecryptor.MessageEnd();

				//
				// Dump Decrypted Text
				//
				std::cout << "Decrypted Text: " << std::endl;
				std::cout << decryptedtext;
				std::cout << std::endl << std::endl;


				//MapleLib shit
				return aesCrypt(IV, data, length, CryptoConstants.getTrimmedUserKey());
			}

			/// <summary>
			/// Encrypt data using MapleStory's AES method
			/// </summary>
			/// <param name="IV">IV to use for encryption</param>
			/// <param name="data">data to encrypt</param>
			/// <param name="length">length of data</param>
			/// <param name="key">the AES key to use</param>
			/// <returns>Crypted data</returns>
			static char* aesCrypt(char* IV, char* data, int length, char* key) {
				AesManaged crypto = new AesManaged();
				crypto.KeySize = 256; //in bits
				crypto.Key = key;
				crypto.Mode = CipherMode.ECB; // Should be OFB, but this works too

				MemoryStream memStream = new MemoryStream();
				CryptoStream cryptoStream = new CryptoStream(memStream, crypto.CreateEncryptor(), CryptoStreamMode.Write);

				int remaining = length;
				int llength = 0x5B0;
				int start = 0;
				while (remaining > 0)
				{
					byte[] myIV = MapleCrypto.multiplyBytes(IV, 4, 4);
					if (remaining < llength)
					{
						llength = remaining;
					}
					for (int x = start; x < (start + llength); x++)
					{
						if ((x - start) % myIV.Length == 0)
						{
							cryptoStream.Write(myIV, 0, myIV.Length);
							byte[] newIV = memStream.ToArray();
							Array.Copy(newIV, myIV, myIV.Length);
							memStream.Position = 0;
						}
						data[x] ^= myIV[(x - start) % myIV.Length];
					}
					start += llength;
					remaining -= llength;
					llength = 0x5B4;
				}

				try
				{
					cryptoStream.Dispose();
					memStream.Dispose();
				}
				catch (std::exception e)
				{
					Helpers.ErrorLogger.Log(Helpers.ErrorLevel.Critical, std::string("Error disposing AES streams") + std::string(e.what()));
					//Console.WriteLine("Error disposing AES streams" + e);
				}

				return data;
			}
		};
	}
}