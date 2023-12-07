#pragma once

#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

namespace MapleLib {
    namespace Helpers {
        enum ErrorLevel {
            MissingFeature,
            IncorrectStructure,
            Critical,
            Crash
        };
        struct Error {
            ErrorLevel level;
            std::string message;

            Error(ErrorLevel level, std::string message) : level{ level }, message{ message } {}
        };

        class ErrorLogger {
        private:
            static std::list<Error> errorList;
        public:
            static void Log(ErrorLevel level, std::string message) {
                errorList.push_back(Error(level, message));
            }

            static bool ErrorsPresent() {
                return errorList.size() > 0;
            }

            static void ClearErrors() {
                errorList.clear();
            }

            static void SaveToFile(std::string filename) {
                //using (StreamWriter sw = new StreamWriter(File.Open(filename, FileMode.Append, FileAccess.Write, FileShare.Read)))
                std::fstream sw{ filename, std::ios::app };

                if (sw.is_open()) {
                    std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                    sw << "Starting error log on " << std::ctime(&end_time) << std::endl;
                    for (Error& e : errorList) {
                        switch (e.level) {
                        case ErrorLevel::MissingFeature: {
                            sw << "Missing feature: ";
                            break;
                        }
                        case ErrorLevel::IncorrectStructure: {
                            sw << "Incorrect structure: ";
                            break;
                        }
                        case ErrorLevel::Critical:
                            sw << "Critical: ";
                            break; {
                            }
                        case ErrorLevel::Crash: {
                            sw << "Crash:\t";
                            break;
                        }
                        }

                        sw << e.message;
                    }
                    sw << "\n\n";
                    sw.close();
                }
                else {
                    std::cout << "this shouldn't happen, immediately debug \n";
                    std::cout << "this shouldn't happen, immediately debug \n";
                    std::cout << "this shouldn't happen, immediately debug \n";
                    //?
                }

            }
        };


    }
}