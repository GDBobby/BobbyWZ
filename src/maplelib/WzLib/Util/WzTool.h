#pragma once

#include "../WzMapleVersion.h"
#include "../../crypto/CryptoConstants.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace MapleLib { 
	namespace WzLib {
		namespace Util {
			class WzTool {
			public:
				static std::unordered_map<std::wstring, int32_t> StringCache;

				static uint32_t RotateLeft(uint32_t x, uint8_t n) {
					return (uint32_t)(((x) << (n)) | ((x) >> (32 - (n))));
				}

				static uint32_t RotateRight(uint32_t x, uint8_t n) {
					return (uint32_t)(((x) >> (n)) | ((x) << (32 - (n))));
				}

				static int GetCompressedIntLength(int i) {
					if (i > 127 || i < -127) {
						return 5;
					}
					return 1;
				}

				static int GetEncodedStringLength(std::wstring& s) {
					int len = 0;
					if (s.size() == 0) {
						return 1;
					}
					bool unicode = false;
					for (auto& c : s) {
						if (c > 255) {
							unicode = true;
							break;
						}
					}

					if (s.length() > (127 - unicode)) {
						len += 5;
					}
					else {
						len += 1;
					}
					len += s.length() * (1 + unicode);
					
					return len;
				}

				static int GetWzObjectValueLength(std::wstring s, uint8_t type) {
					std::wstring storeName{};
					storeName.push_back(type);
					storeName.push_back('_');
					storeName.append(s);

					if (s.length() > 4 && (StringCache.find(storeName) != StringCache.end())) {
						return 5;
					}
					else {
						StringCache[storeName] = 1;
						return 1 + GetEncodedStringLength(s);
					}
				}

				template<typename T>
				static T StringToEnum(const std::string& name) {
					try {
						// Convert the string to an integer
						int enumValue = std::stoi(name);

						// Convert the integer to the enum type
						return static_cast<T>(enumValue);
					}
					catch (...) {
						// Handle the exception (e.g., invalid conversion)
						return T();
					}
				}

				static std::vector<uint8_t> GetIvByMapleVersion(WzMapleVersion ver) {
					switch (ver) {
					case WzMapleVersion::EMS: {
						return CryptoLib::CryptoConstants::WZ_MSEAIV();
					}
					case WzMapleVersion::GMS: {
						return CryptoLib::CryptoConstants::WZ_GMSIV();
					}
					case WzMapleVersion::BMS:
					case WzMapleVersion::CLASSIC:
					default:
						throw std::exception("these are not supported \n");
						return {};
					}
				}

			private:
				static int GetRecognizedCharacters(std::wstring source) {
					int result = 0;
					for (auto& c : source) {
						result += ((0x20 <= c) && (c <= 0x7E));
					}
					return result;
				}

				static double GetDecryptionSuccessRate(std::wstring wzPath, WzMapleVersion encVersion, short& version, bool hasVersion) {
					if (hasVersion) {
						WzFile wzf = new WzFile(wzPath, version, encVersion);
						wzf.ParseWzFile();
					}
					else {
						WzFile wzf = new WzFile(wzPath, encVersion);
						wzf.ParseWzFile();
						version = wzf.Version;//wtf
					}
						
					int recognizedChars = 0;
					int totalChars = 0;
					for (WzDirectory& wzdir : wzf.WzDirectory.WzDirectories) {
						recognizedChars += GetRecognizedCharacters(wzdir.Name);
						totalChars += wzdir.Name.Length;
					}
					foreach(WzImage & wzimg : wzf.WzDirectory.WzImages) {
						recognizedChars += GetRecognizedCharacters(wzimg.Name);
						totalChars += wzimg.Name.Length;
					}
					delete wzf;
					return (double)recognizedChars / (double)totalChars;
				}

			public:
				static WzMapleVersion DetectMapleVersion(std::wstring wzFilePath, short& fileVersion) {
					std::unordered_map<WzMapleVersion, double> mapleVersionSuccessRates{};
					short version = 0;
					mapleVersionSuccessRates.Add(WzMapleVersion::GMS, GetDecryptionSuccessRate(wzFilePath, WzMapleVersion::GMS, version));
					mapleVersionSuccessRates.Add(WzMapleVersion::EMS, GetDecryptionSuccessRate(wzFilePath, WzMapleVersion::EMS, version));
					mapleVersionSuccessRates.Add(WzMapleVersion::BMS, GetDecryptionSuccessRate(wzFilePath, WzMapleVersion::BMS, version));
					fileVersion = version;
					WzMapleVersion mostSuitableVersion = WzMapleVersion::GMS;
					double maxSuccessRate = 0;
					for (auto& mapleVersionEntry : mapleVersionSuccessRates) {
						if (mapleVersionEntry.second > maxSuccessRate)
						{
							mostSuitableVersion = mapleVersionEntry.first;
							maxSuccessRate = mapleVersionEntry.second;
						}
					}
					if (maxSuccessRate < 0.7 && std::filesystem::exists((std::filesystem::path(wzFilePath).parent_path() / "ZLZ.dll").string())) {

						return WzMapleVersion::GETFROMZLZ;
					}
					return mostSuitableVersion;
				}

				const int WzHeader = 0x31474B50; //PKG1 //is this static?

				public static bool IsListFile(std::wstring path) {
					BinaryReader reader = new BinaryReader(std::ifstream{ path });
					bool result = reader.ReadInt32() != WzHeader;
					reader.close();
					return result;
				}

			private:
				static uint8_t* Combine(std::vector<uint8_t>& a, std::vector<uint8_t>& b) {
					std::vector<uint8_t> ret{};
					ret.reserve(a.size() + b.size());
					ret.insert(ret.end(), a.begin(), a.end());
					ret.insert(ret.end(), b.begin(), b.end());
					return ret.data();
				}
			};
		}
	}
}