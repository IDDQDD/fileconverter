#pragma once
#include <map>
#include <string>
#include <shared_mutex>
#include "../plugins/IConverterFactory.hpp"
#include "ErrorHandler.hpp"

using PluginKey = std::pair<std::string, std::string>; // Pair of mime_type and target_format
class PluginManager {
    std::map<PluginKey, IConverterFactory*> plugins_; // Map to store loaded plugins
    std::shared_mutex mutex_;
    const std::string lib_path = "../plugins"; // Path to the plugins directory

    public:
        PluginManager() = default;
        IConverterFactory* get_converter(const std::string &mime_type, const std::string &target_format);
        ErrorCode LoadPlugins();
    private:
        ErrorCode load_plugin_windows(const std::string &path);
        ErrorCode load_plugin_unix(const std::string &path);
};