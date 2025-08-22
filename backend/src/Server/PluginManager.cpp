#include "../include/Server/PluginManager.hpp"
#include "../include/Server/ErrorHandler.hpp"
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
namespace fs = std::filesystem;
ErrorCode PluginManager::LoadPlugins() {
    ErrorCode result = ErrorCode::Success;
    for(const auto &entry : fs::directory_iterator(lib_path)) {
        #ifdef _WIN32
            if(load_plugin_windows(entry.path().string()) != ErrorCode::Success) {
                result = ErrorCode::PluginLoadFailed;
            }
        #else
            if(load_plugin_unix(entry.path().string()) != ErrorCode::Success)
                result = ErrorCode::PluginLoadFailed;
        #endif
    }
 
}

#ifdef _WIN32
ErrorCode PluginManager::load_plugin_windows(const std::string &path){
    HMODULE handle = LoadLibrary(path.c_str());
    if(!handle) {
        ErrorHandler::log_to_file("Failed to load plugin: " + path + " - " + std::to_string(GetLastError()));
        return ErrorCode::PluginLoadFailed;
    }
    using CreatePluginFunc = IConverterFactory* (*)();
    CreatePluginFunc create_plugin = reinterpret_cast<CreatePluginFunc>(GetProcAddress(handle, "create_plugin"));
    if(!create_plugin) {
        ErrorHandler::log_to_file("Failed to find create_plugin function in " + path + " - " + std::to_string(GetLastError()));
        FreeLibrary(handle);
        return ErrorCode::PluginLoadFailed;
    }
    plugins_[{create_plugin()->support_mime_type(), create_plugin()->target_format()}] = create_plugin();
    return ErrorCode::PluginLoadFailed;


}
#else
ErrorCode PluginManager::load_plugin_unix(const std::string &path){
    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    if(!handle) {
        ErrorHandler::log_to_file("Failed to load plugin: " + path + " - " + dlerror());
        return ErrorCode::PluginLoadFailed;
    }
    using CreatePluginFunc = IConverterFactory* (*)();
    CreatePluginFunc create_plugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "create_plugin"));
    if(!create_plugin) {
        ErrorHandler::log_to_file("Failed to find create_plugin function in " + path + " - " + dlerror());
        dlclose(handle);
        return ErrorCode::PluginLoadFailed;
    }
    plugins_[{create_plugin()->support_mime_type(), create_plugin()->target_format()}] = create_plugin();
    return ErrorCode::PluginLoadFailed;
}
#endif
