#include "ErrorLog.h"

namespace MapleLib {
    namespace Helpers {
        std::list<ErrorMaple> ErrorLogger::errorList

        void ErrorLogger::SaveToFile(std::wstring filename) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            SaveToFile(converter.to_bytes(filename));
        }
        void ErrorLogger::SaveToFile(std::string filename) {
            //using (StreamWriter sw = new StreamWriter(File.Open(filename, FileMode.Append, FileAccess.Write, FileShare.Read)))
            std::fstream sw{ filename, std::ios::app };

            if (sw.is_open()) {
                std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                sw << "Starting error log on " << std::ctime(&end_time) << std::endl;
                for (ErrorMaple& e : errorList) {
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
    }
}