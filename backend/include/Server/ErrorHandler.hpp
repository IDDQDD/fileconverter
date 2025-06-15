#include <iostream>
#include <string>
#include <fstream>

class ErrorHandler {
    static std::string filename;    
    public:
    static void log(const std::string& message);
    static void set_log_file(std::string_view file);
    static void log_to_file(const std::string &message);
   };
