#pragma once

#include <string>
#include <array>
#include <vector>

namespace MapleLib {
	namespace WzLib {
		namespace Util {
			class XmlUtil {
			private:
				static constexpr std::array<std::pair<char, std::string>, 5> characterMap{
					std::pair<char, std::string>{'"', "&quot;"},
					std::pair<char, std::string>{'\'', "&apos;"},
					std::pair<char, std::string>{'&', "&amp;"},
					std::pair<char, std::string>{'<', "&lt;"},
					std::pair<char, std::string>{'>', "&gt;"}
				};

			public:
				static std::string SanitizeText(std::string text) {
					std::string fixedText{};
					bool charFixed;
					for (int i = 0; i < text.length(); i++) {
						charFixed = false;
						for (auto& sample : characterMap) {

							if (text[i] == sample.first) {
								fixedText += sample.second;
								charFixed = true;
								break;
							}
						}
						if (!charFixed) {
							fixedText += text[i];
						}
					}
					return fixedText;
				}

				static std::string OpenNamedTag(std::string tag, std::string name, bool finish) {
					return OpenNamedTag(tag, name, finish, false);
				}

				static std::string EmptyNamedTag(std::string tag, std::string name) {
					return OpenNamedTag(tag, name, true, true);
				}

				static std::string EmptyNamedValuePair(std::string tag, std::string name, std::string value) {
					return OpenNamedTag(tag, name, false, false) + Attrib("value", value, true, true);
				}

				static std::string OpenNamedTag(std::string tag, std::string name, bool finish, bool empty) {
					return '<' + tag + " name = \"" + name + '\"' + (finish ? (empty ? "/>" : ">") : " ");
				}

				static std::string Attrib(std::string name, std::string value) {
					return Attrib(name, value, false, false);
				}

				static std::string Attrib(std::string name, std::string value, bool closeTag, bool empty) {
					return name + "=\"" + SanitizeText(value) + "\"" + (closeTag ? (empty ? "/>" : ">") : " ");
				}

				static std::string CloseTag(std::string tag) {
					return "</" + tag + '>';
				}

				static std::string Indentation(int level) {
					std::string indent;
					indent.resize(level, '\t');
					return indent;
				}
			};
		}
	}
}