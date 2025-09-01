#pragma once
#include <boost/json.hpp>
#include <tuple>
#include <fstream>
#include <string>
#include <optional>
#include "Server/ErrorHandler.hpp"
namespace json = boost::json;
inline std::optional<json::value> read_from_json_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        ErrorHandler::log("Could not open settings file: " + filename);
        return std::nullopt;
    }

    boost::system::error_code ec;
    auto result = json::parse(file, ec);
    if (ec) {
        ErrorHandler::log("JSON parse error in " + filename + ": " + ec.message());
        return std::nullopt;
    }
    return result;
}


inline  std::optional<json::value> get_value_from_json(const json::object& obj, std::string_view key){
    if(auto it = obj.if_contains(key)){
        return *it;
    }
    return std::nullopt;
}