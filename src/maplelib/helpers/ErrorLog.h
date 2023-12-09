#pragma once

#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>


#include <locale>
#include <codecvt>

namespace MapleLib {
    namespace Helpers {
        enum class ErrorLevel {
            MissingFeature,
            IncorrectStructure,
            Critical,
            Crash
        };
        struct ErrorMaple {
            ErrorLevel level;
            std::string message;

            ErrorMaple(ErrorLevel level, std::string message) : level{ level }, message{ message } {}
        };

        class ErrorLogger {
        private:
            static std::list<ErrorMaple> errorList;
        public:
            static void Log(ErrorLevel level, std::string message) {
                errorList.push_back(ErrorMaple(level, message));
            }

            static bool ErrorsPresent() {
                return errorList.size() > 0;
            }

            static void ClearErrors() {
                errorList.clear();
            }
            static void SaveToFile(std::wstring filename);
            static void SaveToFile(std::string filename);
        };


    }
}