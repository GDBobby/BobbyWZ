#pragma once
/*  MapleLib - A general-purpose MapleStory library
 * Copyright (C) 2009, 2010, 2015 Snow and haha01haha01

 * This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

 * This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "WzDirectory.h"
#include "WzObject.h"
#include "../helpers/ErrorLog.h"
#include "Util/ParseLine.h"

#include <string>
#include <stdexcept>


namespace MapleLib {
	namespace WzLib {
		/// <summary>
		/// A class that contains all the information of a wz file
		/// </summary>
		class WzFile : WzObject {
		public:
			std::wstring path;
			WzDirectory wzDir;
			WzHeader header;
			std::wstring name = L"";
			int16_t version = 0;
			uint32_t versionHash = 0;
			int16_t fileVersion = 0;
			WzMapleVersion mapleVersion;
			uint8_t* WzIv;

			WzDirectory getWzDirectory(){ return wzDir; }


			WzObjectType getObjectType() override { return WzObjectType.File; }


			//search
			WzObject this[string name] {
				get{ return WzDirectory[name]; }
				}

				//public WzHeader Header { get { return header; } set { header = value; } }

				//public short Version { get { return fileVersion; } set { fileVersion = value; } }

				//public string FilePath { get { return path; } }

				//public WzMapleVersion MapleVersion { get { return mapleVersion; } set { mapleVersion = value; } }

				//public override WzObject Parent { get { return null; } set { } }

				//public override WzFile WzFileParent { get { return this; } }

					void Dispose() override {
					if (wzDir.reader == null) return;
					wzDir.reader.Close();
					Header = null;
					path = null;
					name = null;
					WzDirectory.Dispose();
					GC.Collect();
					GC.WaitForPendingFinalizers();
				}

				WzFile(int16_t gameVersion, WzMapleVersion version) : fileVersion{ gameVersion }, mapleVersion{ version } {
					wzDir = new WzDirectory();
					this.Header = WzHeader.GetDefault();
					WzIv = WzTool::GetIvByMapleVersion(mapleVersion);
					wzDir.WzIv = WzIv;
				}

				WzFile(string filePath, WzMapleVersion version) {
					name = Path.GetFileName(filePath);
					path = filePath;
					fileVersion = -1;
					mapleVersion = version;
					if (version == WzMapleVersion::GETFROMZLZ) {
						FileStream zlzStream = File.OpenRead(Path.Combine(Path.GetDirectoryName(filePath), "ZLZ.dll"));
						WzIv = Util.WzKeyGenerator.GetIvFromZlz(zlzStream);
						zlzStream.Close();
					}
					else { WzIv = WzTool.GetIvByMapleVersion(version); }
				}

				WzFile(std::wstring filePath, int16_t gameVersion, WzMapleVersion version) :
					name{ Path.GetFileName(filePath) },
					path{ filePath },
					fileVersion{ gameVersion },
					mapleVersion{ version }
				{
					if (version == WzMapleVersion.GETFROMZLZ) {
						FileStream zlzStream = File.OpenRead(Path.Combine(Path.GetDirectoryName(filePath), "ZLZ.dll"));
						WzIv = Util.WzKeyGenerator.GetIvFromZlz(zlzStream);
						zlzStream.Close();
					}
					else {
						WzIv = WzTool.GetIvByMapleVersion(version);
					}
				}

				/// <summary>
				/// Parses the wz file, if the wz file is a list.wz file, WzDirectory will be a WzListDirectory, if not, it'll simply be a WzDirectory
				/// </summary>
				void ParseWzFile() {
					if (mapleVersion == WzMapleVersion::GENERATE) {
						throw std::exception("Cannot call ParseWzFile() if WZ file type is GENERATE");
					}
					ParseMainWzDirectory();
					GC.Collect();
					GC.WaitForPendingFinalizers();
				}

				void ParseWzFile(uint8_t* WzIv) {
					if (mapleVersion != WzMapleVersion::GENERATE) {
						throw std::exception("Cannot call ParseWzFile(byte[] generateKey) if WZ file type is not GENERATE");
					}
					this->WzIv = WzIv;
					ParseMainWzDirectory();
					GC.Collect();
					GC.WaitForPendingFinalizers();
				}

				void ParseMainWzDirectory() {
					if (this->path == null) {
						Helpers::ErrorLogger::Log(Helpers::ErrorLevel::Critical, "[Error] Path is null");
						return;
					}

					Util::WzBinaryReader reader = new Util::WzBinaryReader(File.Open(this.path, FileMode.Open, FileAccess.Read, FileShare.Read), WzIv);

					this.Header = new WzHeader();
					this.Header.Ident = reader.ReadString(4);
					this.Header.FSize = reader.ReadUInt64();
					this.Header.FStart = reader.ReadUInt32();
					this.Header.Copyright = reader.ReadNullTerminatedString();
					reader.ReadBytes((int)(Header.FStart - reader.BaseStream.Position));
					reader.Header = this.Header;
					this->version = reader.ReadInt16();
					if (fileVersion == -1) {
						for (int16_t j = 0; j < INT16_MAX; j++) {
							this.fileVersion = j
								this.versionHash = GetVersionHash(version, fileVersion);
							if (this.versionHash != 0) {
								reader.Hash = this.versionHash;
								int64_t position = reader.BaseStream.Position;
								WzDirectory testDirectory = null;
								try {
									testDirectory = new WzDirectory(reader, this.name, this.versionHash, this.WzIv, this);
									testDirectory.ParseDirectory();
								}
								catch {
									reader.BaseStream.Position = position;
									continue;
								}
								WzImage testImage = testDirectory.GetChildImages()[0];

								try {
									reader.BaseStream.Position = testImage.Offset;
									byte checkByte = reader.ReadByte();
									reader.BaseStream.Position = position;
									testDirectory.Dispose();
									switch (checkByte) {
									case 0x73:
									case 0x1b: {
										WzDirectory directory = new WzDirectory(reader, this.name, this.versionHash, this.WzIv, this);
										directory.ParseDirectory();
										this.wzDir = directory;
										return;
									}
									}
									reader.BaseStream.Position = position;
								}
								catch {
									reader.BaseStream.Position = position;
								}
							}
						}
						throw std::exception("Error with game version hash : The specified game version is incorrect and WzLib was unable to determine the version itself");
					}
					else
					{
						this.versionHash = GetVersionHash(version, fileVersion);
						reader.Hash = this.versionHash;
						WzDirectory directory = new WzDirectory(reader, this.name, this.versionHash, this.WzIv, this);
						directory.ParseDirectory();
						this.wzDir = directory;
					}
				}
		private:
			uint32_t GetVersionHash(int encver, int realver) {
				int EncryptedVersionNumber = encver;
				int VersionNumber = realver;
				int VersionHash = 0;
				int DecryptedVersionNumber = 0;
				std::wstring VersionNumberStr;
				int a = 0, b = 0, c = 0, d = 0, l = 0;

				VersionNumberStr = std::to_string(VersionNumber);

				for (int i = 0; i < VersionNumberStr.length(); i++) {
					VersionHash = (32 * VersionHash) + (int)VersionNumberStr[i] + 1;
				}
				a = (VersionHash >> 24) & 0xFF;
				b = (VersionHash >> 16) & 0xFF;
				c = (VersionHash >> 8) & 0xFF;
				d = VersionHash & 0xFF;
				DecryptedVersionNumber = (0xff ^ a ^ b ^ c ^ d);

				if (EncryptedVersionNumber == DecryptedVersionNumber) {
					return Convert.ToUInt32(VersionHash);
				}
				else
				{
					return 0;
				}
			}

			void CreateVersionHash() {
				versionHash = 0;
				std::wstring fileVersionString = std::to_string(fileVersion);
				for (wchar_t& ch : fileVersionString) {
					versionHash = (versionHash * 32) + (byte)ch + 1;
				}
				uint32_t a = (versionHash >> 24) & 0xFF;
				uint32_t b = (versionHash >> 16) & 0xFF;
				uint32_t c = (versionHash >> 8) & 0xFF;
				uint32_t d = versionHash & 0xFF;
				version = (uint8_t)~(a ^ b ^ c ^ d);
			}
		public:
			void SaveToDisk(string path) {
				WzIv = WzTool.GetIvByMapleVersion(mapleVersion);
				CreateVersionHash();
				wzDir.SetHash(versionHash);
				string tempFile = Path.GetFileNameWithoutExtension(path) + ".TEMP";
				File.Create(tempFile).Close();
				wzDir.GenerateDataFile(tempFile);
				WzTool.StringCache.Clear();
				uint totalLen = wzDir.GetImgOffsets(wzDir.GetOffsets(Header.FStart + 2));
				WzBinaryWriter wzWriter = new WzBinaryWriter(File.Create(path), WzIv);
				wzWriter.Hash = (uint32_t)versionHash;
				Header.FSize = totalLen - Header.FStart;
				for (int i = 0; i < 4; i++) {
					wzWriter.Write((uint8_t)Header.Ident[i]);
				}
				wzWriter.Write((long)Header.FSize);
				wzWriter.Write(Header.FStart);
				wzWriter.WriteNullTerminatedString(Header.Copyright);
				long extraHeaderLength = Header.FStart - wzWriter.BaseStream.Position;
				if (extraHeaderLength > 0) {
					wzWriter.Write(new uint8_t[extraHeaderLength]); //i dont understand? writing undefined data?
				}
				wzWriter.Write(version);
				wzWriter.Header = Header;
				wzDir.SaveDirectory(wzWriter);
				wzWriter.StringCache.Clear();
				FileStream fs = File.OpenRead(tempFile);
				wzDir.SaveImages(wzWriter, fs);
				fs.Close();
				File.Delete(tempFile);
				wzWriter.StringCache.Clear();
				wzWriter.Close();
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			void ExportXml(string path, bool oneFile) {
				if (oneFile) {
					FileStream fs = File.Create(path + "/" + this.name + ".xml");
					StreamWriter writer = new StreamWriter(fs);

					int level = 0;
					writer.WriteLine(XmlUtil.Indentation(level) + XmlUtil.OpenNamedTag("WzFile", this.name, true));
					this.wzDir.ExportXml(writer, oneFile, level, false);
					writer.WriteLine(XmlUtil.Indentation(level) + XmlUtil.CloseTag("WzFile"));

					writer.Close();
				}
				else {
					//replace this ASAP
					throw std::exception("Under Construction");
				}
			}

			std::list<WzObject> GetObjectsFromWildcardPath(std::wstring path) {
				if (path.ToLower() == name.ToLower()) {
					return new List<WzObject>{ WzDirectory };
				}
				else if (path == "*") {
					List<WzObject> fullList = new List<WzObject>();
					fullList.Add(WzDirectory);
					fullList.AddRange(GetObjectsFromDirectory(WzDirectory));
					return fullList;
				}
				else if (!path.Contains("*")) {
					return new List<WzObject>{ GetObjectFromPath(path) };
				}
				std::vector<std::wstring> seperatedNames = Util::ParseLine(path);
				if (seperatedNames.Length == 2 && seperatedNames[1] == L"*") {
					return GetObjectsFromDirectory(WzDirectory);
				}
				std::list<WzObject> objList = std::list<WzObject>();
				for (WzImage& img : WzDirectory.WzImages) {
					for (std::wstring& spath : GetPathsFromImage(img, name + "/" + img.Name)) {
						if (path == spath) { //this was using strMatch(path, spath), idk what that did
							objList.Add(GetObjectFromPath(spath));
						}
					}
				}
				for (WzDirectory& dir : wzDir.WzDirectories) {
					for (std::wstring spath : GetPathsFromDirectory(dir, name + "/" + dir.Name)) {
						if (path == spath) {
							objList.Add(GetObjectFromPath(spath));
						}
					}
				}
				GC.Collect();
				GC.WaitForPendingFinalizers();
				return objList;
			}

			std::list<WzObject> GetObjectsFromRegexPath(std::wstring path) {
				if (path.ToLower() == name.ToLower()) {
					return std::list<WzObject>{ WzDirectory };
				}
				std::list<WzObject> objList{};
				for (WzImage& img : WzDirectory.WzImages) {
					auto tempPathsFromImage = GetPathsFromImage(img, name + "/" + img.Name);
					for (std::wstring& spath : tempPathsFromImage) {
						if (Regex.Match(spath, path).Success) {
							objList.Add(GetObjectFromPath(spath));
						}
					}
				}
				for (WzDirectory& dir : wzDir.WzDirectories) {
					for (std::wstring& spath in GetPathsFromDirectory(dir, name + "/" + dir.Name)) {
						if (Regex.Match(spath, path).Success) {
							objList.Add(GetObjectFromPath(spath));
						}
					}
				}
				GC.Collect();
				GC.WaitForPendingFinalizers();
				return objList;
			}

			std::list<WzObject> GetObjectsFromDirectory(WzDirectory dir) {
				std::list<WzObject> objList{};
				for (WzImage& img : dir.WzImages) {
					objList.Add(img);
					objList.AddRange(GetObjectsFromImage(img));
				}
				for (WzDirectory& subdir : dir.WzDirectories) {
					objList.Add(subdir);
					objList.AddRange(GetObjectsFromDirectory(subdir));
				}
				return objList;
			}

			std::list<WzObject> GetObjectsFromImage(WzImage img) {
				std::list<WzObject> objList{};
				for (WzImageProperty& prop : img.WzProperties) {
					objList.Add(prop);
					objList.AddRange(GetObjectsFromProperty(prop));
				}
				return objList;
			}

			std::list<WzObject> GetObjectsFromProperty(WzImageProperty prop) {
				std::list<WzObject> objList{};
				switch (prop.PropertyType) {
				case WzPropertyType::Canvas:
					for (WzImageProperty& canvasProp : ((WzCanvasProperty)prop).WzProperties) {
						objList.AddRange(GetObjectsFromProperty(canvasProp));
					}
					objList.Add(((WzCanvasProperty)prop).PngProperty);
					break;
				case WzPropertyType::Convex:
					for (WzImageProperty& exProp : ((WzConvexProperty)prop).WzProperties) {
						objList.AddRange(GetObjectsFromProperty(exProp));
					}
					break;
				case WzPropertyType::SubProperty:
					for (WzImageProperty subProp : ((WzSubProperty)prop).WzProperties) {
						objList.AddRange(GetObjectsFromProperty(subProp));
					}
					break;
				case WzPropertyType::Vector:
					objList.Add(((WzVectorProperty)prop).X);
					objList.Add(((WzVectorProperty)prop).Y);
					break;
				default:
					break;
				}
				return objList;
			}

			std::list<std::wstring> GetPathsFromDirectory(WzDirectory dir, std::wstring curPath) {
				std::list<std:wstring> objList{};
				for (WzImage& img : dir.WzImages) {
					objList.Add(curPath + "/" + img.Name);

					objList.AddRange(GetPathsFromImage(img, curPath + "/" + img.Name));
				}
				for (WzDirectory& subdir : dir.WzDirectories) {
					objList.Add(curPath + "/" + subdir.Name);
					objList.AddRange(GetPathsFromDirectory(subdir, curPath + "/" + subdir.Name));
				}
				return objList;
			}

			std::list<std::wstring> GetPathsFromImage(WzImage img, std::wstring curPath) {
				std::list<std::wstring> objList{};
				for (WzImageProperty& prop : img.WzProperties) {
					objList.Add(curPath + "/" + prop.Name);
					objList.AddRange(GetPathsFromProperty(prop, curPath + "/" + prop.Name));
				}
				return objList;
			}

			std::list<std::wstring> GetPathsFromProperty(WzImageProperty prop, std::wstring curPath) {
				std::list<std::wstring> objList{};
				switch (prop.PropertyType) {
				case WzPropertyType::Canvas:
					for (WzImageProperty& canvasProp : ((WzCanvasProperty)prop).WzProperties) {
						objList.Add(curPath + "/" + canvasProp.Name);
						objList.AddRange(GetPathsFromProperty(canvasProp, curPath + "/" + canvasProp.Name));
					}
					objList.Add(curPath + "/PNG");
					break;
				case WzPropertyType::Convex:
					for (WzImageProperty& exProp : ((WzConvexProperty)prop).WzProperties) {
						objList.Add(curPath + "/" + exProp.Name);
						objList.AddRange(GetPathsFromProperty(exProp, curPath + "/" + exProp.Name));
					}
					break;
				case WzPropertyType::SubProperty:
					for (WzImageProperty& subProp : ((WzSubProperty)prop).WzProperties) {
						objList.Add(curPath + "/" + subProp.Name);
						objList.AddRange(GetPathsFromProperty(subProp, curPath + "/" + subProp.Name));
					}
					break;
				case WzPropertyType::Vector:
					objList.Add(curPath + "/X");
					objList.Add(curPath + "/Y");
					break;
				}
				return objList;
			}

			WzObject GetObjectFromPath(std::wstring path) {
				std::vector<std::wstring> seperatedPath = Util::ParseLine(path);
				if (seperatedPath[0].ToLower() != wzDir.name.ToLower() && seperatedPath[0].ToLower() != wzDir.name.Substring(0, wzDir.name.Length - 3).ToLower()) {
					return null;
				}
				if (seperatedPath.size() == 1) {
					return WzDirectory;
				}
				WzObject curObj = WzDirectory;
				for (int i = 1; i < seperatedPath.size(); i++) {
					if (curObj == null) {
						return null;
					}
					switch (curObj.ObjectType) {
					case WzObjectType::Directory:
						curObj = ((WzDirectory)curObj)[seperatedPath[i]];
						continue;
					case WzObjectType::Image:
						curObj = ((WzImage)curObj)[seperatedPath[i]];
						continue;
					case WzObjectType::Property:
						switch (((WzImageProperty)curObj).PropertyType) {
						case WzPropertyType::Canvas:
							curObj = ((WzCanvasProperty)curObj)[seperatedPath[i]];
							continue;
						case WzPropertyType::Convex:
							curObj = ((WzConvexProperty)curObj)[seperatedPath[i]];
							continue;
						case WzPropertyType::SubProperty:
							curObj = ((WzSubProperty)curObj)[seperatedPath[i]];
							continue;
						case WzPropertyType::Vector:
							if (seperatedPath[i] == "X")
								return ((WzVectorProperty)curObj).X;
							else if (seperatedPath[i] == "Y")
								return ((WzVectorProperty)curObj).Y;
							else
								return null;
						default: // Wut?
							return null;
						}
					}
				}
				if (curObj == null) {
					return null;
				}
				return curObj;
			}

			bool strMatch(std::wstring strWildCard, std::wstring strCompare) {
				if (strWildCard.length() == 0) return strCompare.length() == 0;
				if (strCompare.length() == 0) return false;
				if (strWildCard[0] == '*' && strWildCard.length() > 1) {
					for (int index = 0; index < strCompare.length(); index++) {
						if (strMatch(strWildCard.substr(1), strCompare.substr(index))) {
							return true;
						}
					}
				}
				else if (strWildCard[0] == '*') {
					return true;
				}
				else if (strWildCard[0] == strCompare[0]) {
					return strMatch(strWildCard.substr(1), strCompare.substr(1));
				}
				return false;
			}

			void Remove() override {
				Dispose();
			}
		};
	}
}