#include <boost/json.hpp>
#include <fstream>
namespace json = boost::json;
inline boost::json::value read_from_file( const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        ErrorHandler::log("Could not open settings file: " + filename);
    }
    return json::parse(file);
}