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
#include "Util/WzTool.h"
#include "WzMapleVersion.h"

#include <string>
#include <stdexcept>
#include <filesystem>
#include <fstream>


namespace MapleLib {
	namespace WzLib {

		//maybe put the tree in here?
		/// A class that contains all the information of a wz file
		class WzFile : WzObject {
		public:
			WzFile(int16_t gameVersion, std::vector<uint8_t>& wzIV);

			WzFile(std::string& filePath, std::vector<uint8_t>& wzIV);

			WzFile(std::string& filePath, int16_t gameVersion, std::vector<uint8_t>& wzIV);

			std::wstring path;
			WzHeader header{};
			int16_t version = 0;
			uint32_t versionHash = 0;
			int16_t fileVersion = 0;
			WzMapleVersion mapleVersion;
			std::vector<uint8_t> WzIv;

			std::wstring getName() override {
				return name;
			}
			void setName(std::wstring& nextName) override {
				name = nextName;
			}

			WzObjectType getObjectType() override { return WzObjectType::File; }


			//public WzHeader Header { get { return header; } set { header = value; } }

			//public short Version { get { return fileVersion; } set { fileVersion = value; } }

			//public string FilePath { get { return path; } }

			//public WzMapleVersion MapleVersion { get { return mapleVersion; } set { mapleVersion = value; } }

			//public override WzObject Parent { get { return null; } set { } }

			//public override WzFile WzFileParent { get { return this; } }

			/// <summary>
			/// Parses the wz file, if the wz file is a list.wz file, WzDirectory will be a WzListDirectory, if not, it'll simply be a WzDirectory
			/// </summary>
			void ParseWzFile() {
				if (mapleVersion == WzMapleVersion::GENERATE) {
					throw std::exception("Cannot call ParseWzFile() if WZ file type is GENERATE");
				}
				ParseMainWzDirectory();
			}

			void ParseWzFile(std::vector<uint8_t>& WzIv) {
				if (mapleVersion != WzMapleVersion::GENERATE) {
					throw std::exception("Cannot call ParseWzFile(byte[] generateKey) if WZ file type is not GENERATE");
				}
				this->WzIv = WzIv;
				ParseMainWzDirectory();
			}
			void GetVersionHash();

		private:

			void CreateVersionHash() {
				versionHash = 0;
				std::wstring fileVersionString = std::to_string(fileVersion);
				for (wchar_t& ch : fileVersionString) {
					versionHash = (versionHash * 32) + (uint8_t)ch + 1;
				}
				uint32_t a = (versionHash >> 24) & 0xFF;
				uint32_t b = (versionHash >> 16) & 0xFF;
				uint32_t c = (versionHash >> 8) & 0xFF;
				uint32_t d = versionHash & 0xFF;
				version = static_cast<uint8_t>(~(a ^ b ^ c ^ d));
			}
		public:
			void SaveToDisk(std::wstring& path) {
				CreateVersionHash();
				wzDir.SetHash(versionHash);
				string tempFile = Path.GetFileNameWithoutExtension(path) + ".TEMP";
				File.Create(tempFile).Close();
				wzDir.GenerateDataFile(tempFile);
				Util::WzTool::StringCache.Clear();
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
					writer.WriteLine(Util::XMLUtil::Indentation(level) + Util::XMLUtil::OpenNamedTag("WzFile", this.name, true));
					this.wzDir.ExportXml(writer, oneFile, level, false);
					writer.WriteLine(Util::XMLUtil::Indentation(level) + Util::XMLUtil::CloseTag("WzFile"));

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
		};
	}
}