#pragma once
#include <boost/json.hpp>
#include <boost/beast/websocket.hpp>
#include "ErrorHandler.hpp"

enum SettingObjects {
    SERVER,
    CONNECTION,
    ERROR_HANDLING,
    PLUGIN
};
struct ServerSettings {
     std::string host;
     std::string address;
     int64_t port;  // Problem with type conversion; needs to be validated
     int64_t threads;
    ServerSettings() = default;
    ServerSettings(const boost::json::value srv):
        host(srv.at("host").as_string().c_str()),
        address(srv.at("address").as_string().c_str()),
        port(srv.at("port").as_int64()),
        threads(srv.at("threads").as_int64()){};
};

struct ConnectionSettings {
     int64_t read_message_max;
     bool auto_fragment;

    ConnectionSettings() = default;
    ConnectionSettings(const boost::json::value conn) :
        read_message_max(conn.at("read_message_max").as_int64()),
        auto_fragment(conn.at("auto_fragment").as_bool()) {}
};





namespace md {

    struct Metadata {
    std::string mime_type;
    std::string target_format;
    Metadata(std::string mime, std::string fmt): 
             mime_type(std::move(mime)), target_format(std::move(fmt)){}
    };

std::unique_ptr<Metadata> build_metadata(const boost::json::value v);
}; // namespace metadata




