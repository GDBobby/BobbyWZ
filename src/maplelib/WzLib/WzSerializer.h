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

#include <string>
#include <vector>
#include <list>
#include <filesystem>
#include <locale>

#include "WzImageProperty.h"
#include "Util/XmlUtil.h"
#include "Util/WzBinaryWriter.h"
#include "Util/WzBinaryReader.h"

namespace MapleLib {
    namespace WzLib {
        namespace Serialization {
            class ProgressingWzSerializer {
            protected:
                int total = 0;
                int curr = 0;

                static void createDirSafe(std::wstring& path) {
                    if (path.back() == L'\\') {
                        path.pop_back();
                        //path = path.substr(0, path.length() - 1);
                    }
                    std::wstring basePath = path;
                    int curridx = 0;

                    while (std::filesystem::exists(path)) {
                        curridx++;
                        path = basePath + std::to_wstring(curridx);
                    }
                    if (!std::filesystem::is_directory(path)) {
                        std::filesystem::create_directory(path);
                    }
                }
            public:
                int getTotal() {
                    return total;
                }
                int getCurrent() { return curr; }
            };

            struct MyNumPunct : std::numpunct<char> {
                char do_decimal_point() const override { return '.'; }
                char do_thousands_sep() const override { return ','; }
            };
            enum class LineBreak {
                None,
                Windows,
                Unix
            };

            class WzXmlSerializer : public ProgressingWzSerializer {
            protected:
                std::wstring indent;
                std::wstring lineBreak;
                bool ExportBase64Data = false;

                //init in the cpp file
                static wchar_t amp[] = L"&amp;";
                static wchar_t lt[] = L"&lt;";
                static wchar_t gt[] = L"&gt;";
                static wchar_t apos[] = L"&apos;";
                static wchar_t quot[] = L"&quot;";

            public:
                static std::locale formattingInfo;

                WzXmlSerializer() {
                    /*
                    formattingInfo = new NumberFormatInfo();
                    formattingInfo.NumberDecimalSeparator = ".";
                    formattingInfo.NumberGroupSeparator = ",";
                    */


                    // Create a locale with the customized numpunct facet
                    formattingInfo = std::locale{ std::locale(), new MyNumPunct };

                }

                WzXmlSerializer(int indentation, LineBreak lineBreakType) {
                    switch (lineBreakType) {
                    case LineBreak::None:
                        lineBreak = L"";
                        break;
                    case LineBreak::Windows:
                        lineBreak = L"\r\n";
                        break;
                    case LineBreak::Unix:
                        lineBreak = L"\n";
                        break;
                    }
                    indent.resize(indentation, (wchar_t)0x20);
                }
            protected:
                void WritePropertyToXML(TextWriter tw, std::wstring depth, WzImageProperty prop) {
                    if (prop is WzCanvasProperty) {
                        WzCanvasProperty property3 = (WzCanvasProperty)prop;
                        if (ExportBase64Data) {
                            MemoryStream stream = new MemoryStream();
                            property3.PngProperty.GetPNG(false).Save(stream, System.Drawing.Imaging.ImageFormat.Png);
                            uint8_t* pngbytes = stream.ToArray();
                            stream.Close();
                            tw.Write(
                                depth + L"<canvas name=\"" + Util::XmlUtil::SanitizeText(property3.Name) + L"\" width=\"" +
                                property3.PngProperty.Width + L"\" height=\"" + property3.PngProperty.Height + L"\" basedata=\"" +
                                Convert.ToBase64String(pngbytes) + L"\">" + lineBreak
                            );
                        }
                        else {
                            tw.Write(string.Concat(new object[]{ depth, L"<canvas name=\"", Util::XmlUtil::SanitizeText(property3.Name), L"\" width=\"", property3.PngProperty.Width, L"\" height=\"", property3.PngProperty.Height, L"\">" }) + lineBreak);
                        }
                        std::wstring newDepth = depth + indent;
                        for (WzImageProperty& prop : property3.WzProperties) {
                            WritePropertyToXML(tw, newDepth, prop);
                        }
                        tw.Write(depth + L"</canvas>" + lineBreak);
                    }
                    else if (prop is WzIntProperty) {
                        WzIntProperty property4 = (WzIntProperty)prop;
                        tw.Write(string.Concat(new object[]{ depth, L"<int name=\"", Util::XmlUtil::SanitizeText(property4.Name), L"\" value=\"", property4.Value, L"\"/>" }) + lineBreak);
                    }
                    else if (prop is WzDoubleProperty) {
                        WzDoubleProperty property5 = (WzDoubleProperty)prop;
                        tw.Write(string.Concat(new object[]{ depth, L"<double name=\"", Util::XmlUtil::SanitizeText(property5.Name), L"\" value=\"", property5.Value, L"\"/>" }) + lineBreak);
                    }
                    else if (prop is WzNullProperty) {
                        WzNullProperty property6 = (WzNullProperty)prop;
                        tw.Write(depth + "<null name=\"" + Util::XmlUtil::SanitizeText(property6.Name) + "\"/>" + lineBreak);
                    }
                    else if (prop is WzSoundProperty) {
                        WzSoundProperty property7 = (WzSoundProperty)prop;
                        if (ExportBase64Data) {
                            tw.Write(string.Concat(new object[]{ depth, "<sound name=\"", Util::XmlUtil::SanitizeText(property7.Name), "\" length=\"", property7.Length.ToString(), "\" basehead=\"", Convert.ToBase64String(property7.Header), "\" basedata=\"", Convert.ToBase64String(property7.GetBytes(false)), "\"/>" }) + lineBreak);
                        }
                        else {
                            tw.Write(depth + "<sound name=\"" + Util::XmlUtil::SanitizeText(property7.Name) + "\"/>" + lineBreak);
                        }
                    }
                    else if (prop is WzStringProperty) {
                        WzStringProperty property8 = (WzStringProperty)prop;
                        string str = Util::XmlUtil::SanitizeText(property8.Value);
                        tw.Write(depth + "<string name=\"" + Util::XmlUtil::SanitizeText(property8.Name) + "\" value=\"" + str + "\"/>" + lineBreak);
                    }
                    else if (prop is WzSubProperty)
                    {
                        WzSubProperty property9 = (WzSubProperty)prop;
                        tw.Write(depth + "<imgdir name=\"" + Util::XmlUtil::SanitizeText(property9.Name) + "\">" + lineBreak);
                        string newDepth = depth + indent;
                        for (WzImageProperty prop : property9.WzProperties)
                            WritePropertyToXML(tw, newDepth, prop);
                        tw.Write(depth + "</imgdir>" + lineBreak);
                    }
                    else if (prop is WzShortProperty)
                    {
                        WzShortProperty property10 = (WzShortProperty)prop;
                        tw.Write(string.Concat(new object[]{ depth, "<short name=\"", Util::XmlUtil::SanitizeText(property10.Name), "\" value=\"", property10.Value, "\"/>" }) + lineBreak);
                    }
                    else if (prop is WzLongProperty)
                    {
                        WzLongProperty long_prop = (WzLongProperty)prop;
                        tw.Write(string.Concat(new object[]{ depth, "<long name=\"", Util::XmlUtil::SanitizeText(long_prop.Name), "\" value=\"", long_prop.Value, "\"/>" }) + lineBreak);
                    }
                    else if (prop is WzUOLProperty)
                    {
                        WzUOLProperty property11 = (WzUOLProperty)prop;
                        tw.Write(depth + "<uol name=\"" + property11.Name + "\" value=\"" + Util::XmlUtil::SanitizeText(property11.Value) + "\"/>" + lineBreak);
                    }
                    else if (prop is WzVectorProperty)
                    {
                        WzVectorProperty property12 = (WzVectorProperty)prop;
                        tw.Write(string.Concat(new object[]{ depth, "<vector name=\"", Util::XmlUtil::SanitizeText(property12.Name), "\" x=\"", property12.X.Value, "\" y=\"", property12.Y.Value, "\"/>" }) + lineBreak);
                    }
                    else if (prop is WzFloatProperty)
                    {
                        WzFloatProperty property13 = (WzFloatProperty)prop;
                        string str2 = Convert.ToString(property13.Value, formattingInfo);
                        if (!str2.Contains("."))
                            str2 = str2 + ".0";
                        tw.Write(depth + "<float name=\"" + Util::XmlUtil::SanitizeText(property13.Name) + "\" value=\"" + str2 + "\"/>" + lineBreak);
                    }
                    else if (prop is WzConvexProperty)
                    {
                        tw.Write(depth + "<extended name=\"" + Util::XmlUtil::SanitizeText(prop.Name) + "\">" + lineBreak);
                        WzConvexProperty property14 = (WzConvexProperty)prop;
                        string newDepth = depth + indent;
                        for (WzImageProperty prop : property14.WzProperties)
                            WritePropertyToXML(tw, newDepth, prop);
                        tw.Write(depth + "</extended>" + lineBreak);
                    }
                }
            };

            public interface IWzFileSerializer
            {
                void SerializeFile(WzFile file, string path);
            }

            public interface IWzDirectorySerializer : IWzFileSerializer
            {
                void SerializeDirectory(WzDirectory dir, string path);
            }

            public interface IWzImageSerializer : IWzDirectorySerializer
            {
                void SerializeImage(WzImage img, string path);
            }

            public interface IWzObjectSerializer
            {
                void SerializeObject(WzObject file, string path);
            }

            /*
            public class NoBase64DataException : System.Exception {
                public NoBase64DataException() : base() { }
                public NoBase64DataException(string message) : base(message) { }
                public NoBase64DataException(string message, System.Exception inner) : base(message, inner) { }
                protected NoBase64DataException(System.Runtime.Serialization.SerializationInfo info,
                    System.Runtime.Serialization.StreamingContext context) { }
            }
            */

            class WzImgSerializer : public ProgressingWzSerializer, IWzImageSerializer {

            private:
                uint8_t* serializeImageInternal(WzImage img) {
                    MemoryStream stream = new MemoryStream();
                    Util::WzBinaryWriter wzWriter{ stream, ((WzDirectory)img.parent).WzIv };
                    img.SaveImage(wzWriter);
                    uint8_t* result = stream.ToArray();
                    wzWriter.Close();
                    return result;
                }

                void serializeImageInternal(WzImage img, std::wstring outPath) {
                    //FileStream stream = File.Create(outPath);
                    std::fstream stream{ outPath };
                    WzBinaryWriter wzWriter = new WzBinaryWriter(stream, ((WzDirectory)img.parent).WzIv);
                    img.SaveImage(wzWriter);
                    wzWriter.Close();
                }
            public:
                uint8_t* SerializeImage(WzImage img) {
                    total = 1; curr = 0;
                    return serializeImageInternal(img);
                }

                void SerializeImage(WzImage img, std::wstring outPath) {
                    total = 1; curr = 0;
                    std::wstring extension = outPath.substr(outPath.find_last_of('.'));

                    if (extension != ".img") {
                        outPath += ".img";
                    }
                    serializeImageInternal(img, outPath);
                }

                void SerializeDirectory(WzDirectory dir, string outPath) {
                    total = dir.CountImages();
                    curr = 0;
                    if (!std::filesystem::exists(outPath)) {
                        WzXmlSerializer::createDirSafe(outPath);
                    }
                    if (outPath.substr(outPath.Length - 1, 1) != "\\") {
                        outPath += "\\";
                    }
                    for (WzDirectory& subdir : dir.WzDirectories) {
                        SerializeDirectory(subdir, outPath + subdir.Name + "\\");
                    }
                    for (WzImage& img : dir.WzImages) {
                        SerializeImage(img, outPath + img.Name);
                    }
                }

                void SerializeFile(WzFile f, string outPath) {
                    SerializeDirectory(f.WzDirectory, outPath);
                }
            }


            class WzImgDeserializer : public ProgressingWzSerializer {
            private:
                bool freeResources;
            public:
                WzImgDeserializer(bool freeResources) : freeResources{ freeResources } {}

                WzImage WzImageFromIMGBytes(std::vector<uint8_t>& bytes, WzMapleVersion version, std::wstring name, bool freeResources) {
                    std::vector<uint8_t>& iv = Util::WzTool::GetIvByMapleVersion(version);
                    MemoryStream stream = new MemoryStream(bytes);
                    Util::WzBinaryReader wzReader{ stream, iv };
                    WzImage img{ name, wzReader };
                    img.BlockSize = bytes.size();
                    img.Checksum = 0;
                    for (uint8_t b : bytes) {
                        img.Checksum += b;
                    }
                    img.Offset = 0;
                    if (freeResources) {
                        img.ParseImage(true);
                        img.Changed = true;
                        wzReader.Close();
                    }
                    return img;
                }

                WzImage WzImageFromIMGFile(std::wstring inPath, std::vector<uint8_t>& iv, std::wstring name) {
                    std::fstream stream{ inPath, std::ios::binary };
                    std::vector<uint8_t> bytes(std::istreambuf_iterator<char>(stream), {});
                    stream.seekg(0);
                    Util::WzBinaryReader wzReader{ bytes, iv };
                    WzImage img{ name, wzReader };
                    img.BlockSize = static_cast<int>(stream.tellg());
                    img.Checksum = 0;

                    for (uint8_t b : bytes) {
                        img.Checksum += b;
                    }
                    img.Offset = 0;
                    if (freeResources) {
                        img.ParseImage(true);
                        img.Changed = true;
                        wzReader.Close();
                    }
                    return img;
                }
            }


            class WzPngMp3Serializer : ProgressingWzSerializer, IWzImageSerializer, IWzObjectSerializer {
                //List<WzImage> imagesToUnparse = new List<WzImage>();
            private:
                std::wstring outPath;

            public:
                void SerializeObject(WzObject obj, std::wstring outPath) {
                    //imagesToUnparse.Clear();
                    this->outPath = outPath;
                    total = 0; curr = 0;
                    if (!std::filesystem::exists(outPath)) {
                        WzXmlSerializer::createDirSafe(outPath);
                    }
                    if (outPath.back() != "\\") {
                        outPath += "\\";
                    }
                    total = CalculateTotal(obj);
                    ExportRecursion(obj, outPath);
                    /*foreach (WzImage img in imagesToUnparse)
                        img.UnparseImage();
                    imagesToUnparse.Clear();*/
                }

                void SerializeFile(WzFile file, std::wstring path) {
                    SerializeObject(file, path);
                }

                void SerializeDirectory(WzDirectory file, std::wstring path) {
                    SerializeObject(file, path);
                }

                void SerializeImage(WzImage file, std::wstring path) {
                    SerializeObject(file, path);
                }
            private:
                int CalculateTotal(WzObject currObj) {
                    int result = 0;
                    if (currObj is WzFile) {
                        result += ((WzFile)currObj).WzDirectory.CountImages();
                    }
                    else if (currObj is WzDirectory) {
                        result += ((WzDirectory)currObj).CountImages();
                    }
                    return result;
                }

                void ExportRecursion(WzObject currObj, std::wstring outPath) {
                    if (currObj is WzFile) {
                        ExportRecursion(((WzFile)currObj).WzDirectory, outPath);
                    }
                    else if (currObj is WzDirectory) {
                        outPath += currObj.Name + "\\";
                        if (!std::filesystem::exists(outPath)) {
                            Directory.CreateDirectory(outPath);
                        }
                        for (WzDirectory& subdir : ((WzDirectory)currObj).WzDirectories) {
                            ExportRecursion(subdir, outPath + subdir.Name + "\\");
                        }
                        for (WzImage& subimg : ((WzDirectory)currObj).WzImages) {
                            ExportRecursion(subimg, outPath + subimg.Name + "\\");
                        }
                    }
                    else if (currObj is WzCanvasProperty) {
                        Bitmap bmp = ((WzCanvasProperty)currObj).PngProperty.GetPNG(false);
                        string path = outPath + currObj.Name + ".png";
                        bmp.Save(path, ImageFormat.Png);
                        //curr++;
                    }
                    else if (currObj is WzSoundProperty) {
                        string path = outPath + currObj.Name + ".mp3";
                        ((WzSoundProperty)currObj).SaveToFile(path);
                    }
                    else if (currObj is WzImage) {
                        outPath += currObj.Name + "\\";
                        if (!std::filesystem::directory(outPath)) {
                            std::filesystem::create_directory(outPath);
                        }
                        bool parse = ((WzImage)currObj).Parsed || ((WzImage)currObj).Changed;
                        if (!parse) {
                            ((WzImage)currObj).ParseImage();
                        }
                        for (WzImageProperty& subprop : ((IPropertyContainer)currObj).WzProperties) {
                            ExportRecursion(subprop, outPath);
                        }
                        if (!parse) {
                            ((WzImage)currObj).UnparseImage();
                        }
                        curr++;
                    }
                    else if (currObj is IPropertyContainer) {
                        outPath += currObj.Name + L".";
                        for (WzImageProperty& subprop : ((IPropertyContainer)currObj).WzProperties) {
                            ExportRecursion(subprop, outPath);
                        }
                    }
                    else if (currObj is WzUOLProperty) {
                        ExportRecursion(((WzUOLProperty)currObj).LinkValue, outPath);
                    }
                }
            };

            class WzClassicXmlSerializer : public WzXmlSerializer, IWzImageSerializer {
            public:
                WzClassicXmlSerializer(int indentation, LineBreak lineBreakType, bool exportbase64)
                    : WzXmlSerializer{ indentation, lineBreakType } {
                    ExportBase64Data = exportbase64;
                }

            private:
                void exportXmlInternal(WzImage img, std::wstring path) {
                    bool parsed = img.Parsed || img.Changed;
                    if (!parsed) {
                        img.ParseImage();
                    }
                    curr++;
                    TextWriter tw = new StreamWriter(path);
                    tw.Write(L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" + lineBreak);
                    tw.Write(L"<imgdir name=\"" + Util::XmlUtil::SanitizeText(img.Name) + L"\">" + lineBreak);
                    for (WzImageProperty& prop : img.WzProperties) {
                        WritePropertyToXML(tw, indent, prop);
                    }
                    tw.Write(L"</imgdir>" + lineBreak);
                    tw.Close();
                    if (!parsed) img.UnparseImage();
                }

                void exportDirXmlInternal(WzDirectory dir, std::wstring path) {
                    if (!std::filesystem::is_directory(path)) {
                        ProgressingWzSerializer::createDirSafe(path);
                    }
                    if (path.back() != L"\\") path += L"\\";
                    for (WzDirectory& subdir : dir.WzDirectories) {
                        exportDirXmlInternal(subdir, path + subdir.name + L"\\");
                    }
                    foreach(WzImage & subimg : dir.WzImages) {
                        exportXmlInternal(subimg, path + subimg.Name + L".xml");
                    }
                }

            public:
                void SerializeImage(WzImage img, std::wstring path) {
                    total = 1; curr = 0;
                    std::wstring extension = path.substr(path.find_last_of(L'.'));
                    if (extension != L".xml") path += L".xml";
                    exportXmlInternal(img, path);
                }

                void SerializeDirectory(WzDirectory dir, std::wstring path) {
                    total = dir.CountImages();
                    curr = 0;
                    exportDirXmlInternal(dir, path);
                }

                void SerializeFile(WzFile file, std::wstring path) {
                    SerializeDirectory(file.WzDirectory, path);
                }
            };

            class WzNewXmlSerializer : WzXmlSerializer {
                WzNewXmlSerializer(int indentation, LineBreak lineBreakType) : WzXmlSerializer(indentation, lineBreakType)
                { }
            private:
                void DumpImageToXML(TextWriter tw, std::wstring depth, WzImage img)
                {
                    bool parsed = img.Parsed || img.Changed;
                    if (!parsed) {
                        img.ParseImage();
                    }
                    curr++;
                    tw.Write(depth + L"<wzimg name=\"" + Util::XmlUtil::SanitizeText(img.Name) + L"\">" + lineBreak);
                    std::wstring newDepth = depth + indent;
                    for (WzImageProperty& prop : img.WzProperties) {
                        WritePropertyToXML(tw, newDepth, prop);
                    }
                    tw.Write(depth + L"</wzimg>");
                    if (!parsed) img.UnparseImage();
                }

                void DumpDirectoryToXML(TextWriter tw, std::wstring depth, WzDirectory dir) {
                    tw.Write(depth + L"<wzdir name=\"" + Util::XmlUtil::SanitizeText(dir.Name) + L"\">" + lineBreak);
                    forach(WzDirectory & subdir : dir.WzDirectories) {
                        DumpDirectoryToXML(tw, depth + indent, subdir);
                    }
                    for (WzImage& img : dir.WzImages) {
                        DumpImageToXML(tw, depth + indent, img);
                    }
                    tw.Write(depth + L"</wzdir>" + lineBreak);
                }

            public:
                void ExportCombinedXml(std::list<WzObject>& objects, std::wstring path) {
                    total = 1; curr = 0;
                    std::wstring extension = path.substr(path.find_last_of(L'.'));
                    if (extension != L".xml") {
                        path += L".xml";
                    }
                    for (WzObject& obj : objects) {
                        //total += obj is WzImage;

                        if (obj is WzImage) {
                            total++;
                        }
                        else if (obj is WzDirectory) {
                            total += ((WzDirectory)obj).CountImages();
                        }
                    }

                    ExportBase64Data = true;
                    TextWriter tw = new StreamWriter(path);
                    tw.Write(L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" + lineBreak);
                    tw.Write(L"<xmldump>" + lineBreak);
                    for (WzObject obj : objects) {
                        if (obj is WzDirectory) DumpDirectoryToXML(tw, indent, (WzDirectory)obj);
                        else if (obj is WzImage) DumpImageToXML(tw, indent, (WzImage)obj);
                        else if (obj is WzImageProperty) WritePropertyToXML(tw, indent, (WzImageProperty)obj);
                    }
                    tw.Write(L"</xmldump>" + lineBreak);
                    tw.Close();
                }
            };

            /* ill come back to this later, my sanity is drained
            class WzXmlDeserializer : public ProgressingWzSerializer {
            public:
                static NumberFormatInfo formattingInfo;

            private:
                bool useMemorySaving;
                uint8_t* iv;
                WzImgDeserializer imgDeserializer = new WzImgDeserializer(false);
            public:
                WzXmlDeserializer(bool useMemorySaving, uint8_t* iv) : useMemorySaving{useMemorySaving}, iv{iv} {}
                
                std::list<WzObject> ParseXML(std::wstring path) {
                    std::list<WzObject> result{};
                    XmlDocument doc = new XmlDocument();
                    doc.Load(path);
                    XmlElement mainElement = (XmlElement)doc.ChildNodes[1];
                    curr = 0;
                    if (mainElement.Name == "xmldump") {
                        total = CountImgs(mainElement);
                        for(XmlElement subelement : mainElement) {
                            if (subelement.Name == "wzdir") {
                                result.Add(ParseXMLWzDir(subelement));
                            }
                            else if (subelement.Name == "wzimg") {
                                result.Add(ParseXMLWzImg(subelement));
                            }
                            else { 
                                throw std::exception("unknown XML prop " + subelement.Name); 
                            }
                        }
                    }
                    else if (mainElement.Name == "imgdir")
                    {
                        total = 1;
                        result.Add(ParseXMLWzImg(mainElement));
                        curr++;
                    }
                    else throw new InvalidDataException("unknown main XML prop " + mainElement.Name);
                    return result;
                }
                

                 int CountImgs(XmlElement element)
                {
                    int result = 0;
                    foreach(XmlElement subelement in element)
                    {
                        if (subelement.Name == "wzimg") result++;
                        else if (subelement.Name == "wzdir") result += CountImgs(subelement);
                    }
                    return result;
                }

                internal WzDirectory ParseXMLWzDir(XmlElement dirElement)
                {
                    WzDirectory result = new WzDirectory(dirElement.GetAttribute("name"));
                    foreach(XmlElement subelement in dirElement)
                    {
                        if (subelement.Name == "wzdir")
                            result.AddDirectory(ParseXMLWzDir(subelement));
                        else if (subelement.Name == "wzimg")
                            result.AddImage(ParseXMLWzImg(subelement));
                        else throw new InvalidDataException("unknown XML prop " + subelement.Name);
                    }
                    return result;
                }

                internal WzImage ParseXMLWzImg(XmlElement imgElement)
                {
                    string name = imgElement.GetAttribute("name");
                    WzImage result = new WzImage(name);
                    foreach(XmlElement subelement in imgElement)
                        result.WzProperties.Add(ParsePropertyFromXMLElement(subelement));
                    result.Changed = true;
                    if (this.useMemorySaving)
                    {
                        string path = Path.GetTempFileName();
                        WzBinaryWriter wzWriter = new WzBinaryWriter(File.Create(path), iv);
                        result.SaveImage(wzWriter);
                        wzWriter.Close();
                        result.Dispose();
                        result = imgDeserializer.WzImageFromIMGFile(path, iv, name);
                    }
                    return result;
                }

                internal WzImageProperty ParsePropertyFromXMLElement(XmlElement element)
                {
                    switch (element.Name)
                    {
                    case "imgdir":
                        WzSubProperty sub = new WzSubProperty(element.GetAttribute("name"));
                        foreach(XmlElement subelement in element)
                            sub.AddProperty(ParsePropertyFromXMLElement(subelement));
                        return sub;

                    case "canvas":
                        WzCanvasProperty canvas = new WzCanvasProperty(element.GetAttribute("name"));
                        if (!element.HasAttribute("basedata")) throw new NoBase64DataException("no base64 data in canvas element with name " + canvas.Name);
                        canvas.PngProperty = new WzPngProperty();
                        MemoryStream pngstream = new MemoryStream(Convert.FromBase64String(element.GetAttribute("basedata")));
                        canvas.PngProperty.SetPNG((Bitmap)Image.FromStream(pngstream, true, true));
                        foreach(XmlElement subelement in element)
                            canvas.AddProperty(ParsePropertyFromXMLElement(subelement));
                        return canvas;

                    case "int":
                        WzIntProperty compressedInt = new WzIntProperty(element.GetAttribute("name"), int.Parse(element.GetAttribute("value"), formattingInfo));
                        return compressedInt;

                    case "double":
                        WzDoubleProperty doubleProp = new WzDoubleProperty(element.GetAttribute("name"), double.Parse(element.GetAttribute("value"), formattingInfo));
                        return doubleProp;

                    case "null":
                        WzNullProperty nullProp = new WzNullProperty(element.GetAttribute("name"));
                        return nullProp;

                    case "sound":
                        if (!element.HasAttribute("basedata") || !element.HasAttribute("basehead") || !element.HasAttribute("length")) throw new NoBase64DataException("no base64 data in sound element with name " + element.GetAttribute("name"));
                        WzSoundProperty sound = new WzSoundProperty(element.GetAttribute("name"),
                            int.Parse(element.GetAttribute("length")),
                            Convert.FromBase64String(element.GetAttribute("basehead")),
                            Convert.FromBase64String(element.GetAttribute("basedata")));
                        return sound;

                    case "string":
                        WzStringProperty stringProp = new WzStringProperty(element.GetAttribute("name"), element.GetAttribute("value"));
                        return stringProp;

                    case "short":
                        WzShortProperty shortProp = new WzShortProperty(element.GetAttribute("name"), short.Parse(element.GetAttribute("value"), formattingInfo));
                        return shortProp;

                    case "long":
                        WzLongProperty longProp = new WzLongProperty(element.GetAttribute("name"), long.Parse(element.GetAttribute("value"), formattingInfo));
                        return longProp;

                    case "uol":
                        WzUOLProperty uol = new WzUOLProperty(element.GetAttribute("name"), element.GetAttribute("value"));
                        return uol;

                    case "vector":
                        WzVectorProperty vector = new WzVectorProperty(element.GetAttribute("name"), new WzIntProperty("x", Convert.ToInt32(element.GetAttribute("x"))), new WzIntProperty("y", Convert.ToInt32(element.GetAttribute("y"))));
                        return vector;

                    case "float":
                        WzFloatProperty floatProp = new WzFloatProperty(element.GetAttribute("name"), float.Parse(element.GetAttribute("value"), formattingInfo));
                        return floatProp;

                    case "extended":
                        WzConvexProperty convex = new WzConvexProperty(element.GetAttribute("name"));
                        foreach(XmlElement subelement in element)
                            convex.AddProperty(ParsePropertyFromXMLElement(subelement));
                        return convex;
                    }
                    throw new InvalidDataException("unknown XML prop " + element.Name);
                }
                #endregion
            };
            */
        }
    }
}