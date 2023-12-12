#include "WzFile.h"


namespace MapleLib {
	namespace WzLib {
		WzFile::WzFile(int16_t gameVersion, std::vector<uint8_t>& wzIV) : fileVersion{ gameVersion }, mapleVersion{ version } {
			header = WzHeader::GetDefault();
			WzIv = wzIV;
		}

		WzFile::WzFile(std::string& filePath, std::vector<uint8_t>& wzIV) {
			//name = Path.GetFileName(filePath);
			// 
			//convert this from string to wstring
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			path = converter.from_bytes(filePath);

			name = path.substr(path.find_last_of(L'\\'));
			name = name.substr(0, name.find_last_of(L'.'));
			printf("wzfile name : %s \n", name.c_str());

			fileVersion = -1;
		}

		WzFile::WzFile(std::string& filePath, int16_t gameVersion, std::vector<uint8_t>& wzIV) :
			path{ filePath },
			fileVersion{ gameVersion },
			mapleVersion{ version }
		{
			name = filePath.substr(name.find_last_of('\\'));
			name = name.substr(0, name.find_last_of('.'));
			WzIv = wzIV;
		}

		void WzFile::GetVersionHash() {
			int EncryptedVersionNumber = version;
			int VersionNumber = fileVersion;
			int VersionHash = 0;
			int DecryptedVersionNumber = 0;
			std::wstring VersionNumberStr;
			int a = 0, b = 0, c = 0, d = 0, l = 0;

			VersionNumberStr = std::to_wstring(VersionNumber);

			for (int i = 0; i < VersionNumberStr.length(); i++) {
				VersionHash = (32 * VersionHash) + (int)VersionNumberStr[i] + 1;
			}
			a = (VersionHash >> 24) & 0xFF;
			b = (VersionHash >> 16) & 0xFF;
			c = (VersionHash >> 8) & 0xFF;
			d = VersionHash & 0xFF;
			DecryptedVersionNumber = (0xff ^ a ^ b ^ c ^ d);

			if (EncryptedVersionNumber == DecryptedVersionNumber) {
				versionHash = static_cast<uint32_t>(VersionHash);
			}
			else {
				//should i throw an error here?
				versionHash = 0;
			}
		}

	}
}