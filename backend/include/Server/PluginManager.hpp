#pragma once
#include <map>
#include <string>
#include <shared_mutex>
#include "../plugins/IConverterFactory.hpp"
#include "ErrorHandler.hpp"

struct PluginInfo{

    std::unique_ptr<IConverterFactory> instance;
    #ifdef _WIN32
        PluginInfo() : instance(nullptr), handle(nullptr) {}
        PluginInfo(IConverterFactory* inst, HMODULE h) : instance(inst), handle(h) {}
        HMODULE handle;
    #else
        PluginInfo(): instance(nullptr), handle(nullptr) {}
        PluginInfo(IConverterFactory* inst, void* h) : instance(inst), handle(h) {}
        void* handle;
    #endif
};

using PluginKey = std::pair<std::string, std::string>; // Pair of mime_type and target_format
class PluginManager {
    std::shared_mutex mutex_; // Mutex for thread-safe access to the plugins map
    std::map<PluginKey, PluginInfo> plugins_; // Map to store loaded plugins
    const std::string lib_path = "../plugins"; // Path to the plugins directory

    public:
        PluginManager() = default;
        IConverterFactory* get_converter(const std::string &mime_type, const std::string &target_format);
        ErrorCode LoadPlugins();
    private:
        ErrorCode load_plugin_windows(const std::string &path);
        ErrorCode load_plugin_unix(const std::string &path);
};