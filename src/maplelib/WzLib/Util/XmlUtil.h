#pragma once

#include <string>
#include <array>
#include <vector>

namespace MapleLib {
	namespace WzLib {
		namespace Util {
			class XmlUtil {
			private:
				static constexpr std::array<std::pair<wchar_t, std::wstring>, 5> characterMap{
					std::pair<wchar_t, std::wstring>{L'"', L"&quot;"},
					std::pair<wchar_t, std::wstring>{L'\'', L"&apos;"},
					std::pair<wchar_t, std::wstring>{L'&', L"&amp;"},
					std::pair<wchar_t, std::wstring>{L'<', L"&lt;"},
					std::pair<wchar_t, std::wstring>{L'>', L"&gt;"}
				};

			public:
				static std::wstring SanitizeText(std::wstring text) {
					std::wstring fixedText{};
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

				static std::wstring OpenNamedTag(std::wstring tag, std::wstring name, bool finish) {
					return OpenNamedTag(tag, name, finish, false);
				}

				static std::wstring EmptyNamedTag(std::wstring tag, std::wstring name) {
					return OpenNamedTag(tag, name, true, true);
				}

				static std::wstring EmptyNamedValuePair(std::wstring tag, std::wstring name, std::wstring value) {
					return OpenNamedTag(tag, name, false, false) + Attrib(L"value", value, true, true);
				}

				static std::wstring OpenNamedTag(std::wstring tag, std::wstring name, bool finish, bool empty) {
					return L'<' + tag + L" name = \"" + name + L'\"' + (finish ? (empty ? L"/>" : L">") : L" ");
				}

				static std::wstring Attrib(std::wstring name, std::wstring value) {
					return Attrib(name, value, false, false);
				}

				static std::wstring Attrib(std::wstring name, std::wstring value, bool closeTag, bool empty) {
					return name + L"=\"" + SanitizeText(value) + L"\"" + (closeTag ? (empty ? L"/>" : L">") : L" ");
				}

				static std::wstring CloseTag(std::wstring tag) {
					return L"</" + tag + L'>';
				}

				static std::wstring Indentation(int level) {
					std::wstring indent;
					indent.resize(level, '\t');
					return indent;
				}
			};
		}
	}
}