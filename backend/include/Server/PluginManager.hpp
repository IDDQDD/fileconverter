#pragma once
#include <map>
#include <string>
#include "../plugins/IConverterFactory.hpp"

using PluginKey = std::pair<std::string, std::string>; // Pair of mime_type and target_format
class PluginManager {
    std::map<PluginKey, IConverterFactory*> plugins_; // Map to store loaded plugins
    const std::string lib_path = "../plugins"; // Path to the plugins directory
    public:
        PluginManager();
        IConverterFactory* get_converter(const std::string &mime_type, const std::string &target_format);
        void LoadPlugins();
    private:
        int load_plugin_windows(const std::string &path);
        int load_plugin_unix(const std::string &path);
};