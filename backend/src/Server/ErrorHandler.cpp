#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../include/Server/ErrorHandler.hpp"
#include "../include/Server/SettingsUtils.hpp"

namespace time= boost::posix_time;
std::string ErrorHandler::filename = "error.log";
void ErrorHandler::set_log_file(std::string_view file) {
    filename = file;
    }

void ErrorHandler::log(const std::string& message){
    std::cerr << "Error: " << message << std::endl;
    }

void ErrorHandler::log_to_file(const std::string &message) {
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety when writing to the log file
    std::ofstream file(filename, std::ios::app);
        if(!file.is_open()) {
            std::cerr << "Error opening log file: " << filename << std::endl;
            return;
        } else {
            file << time::second_clock::local_time() << " : " << message << std::endl;
            std::cerr << "Logged to file: " << filename << std::endl;
        }
    }