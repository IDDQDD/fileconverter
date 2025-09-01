#pragma once
#include <boost/json.hpp>
#include <boost/beast/websocket.hpp>
#include "ErrorHandler.hpp"
#include "SettingsUtils.hpp"

struct ServerSettings {
    const std::string host;
    const std::string address;
    const unsigned short port;
    const unsigned int threads;
    ServerSettings(const boost::json::value &settings):
        host(settings.at("host").as_string().c_str()),
        address(settings.at("address").as_string().c_str()),
        port(settings.at("port").as_uint64()),
        threads(settings.at("threads").as_uint64()){};
};

struct ConnectionSettings {
    const unsigned int read_message_max;
    const bool auto_fragment;
    ConnectionSettings(const boost::json::value &settings):
        read_message_max(settings.at("read_message_max").as_uint64()),
        auto_fragment(settings.at("auto_fragment").as_bool()) {};
};
struct Metadata {
    const std::string mime_type;
    const std::string target_format;
    Metadata(const json::value &metadata):
            mime_type(metadata.at("mime_type").as_string().c_str()),
            target_format(metadata.at("target_format").as_string().c_str()) {};
};