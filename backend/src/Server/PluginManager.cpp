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
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto status = ErrorCode::Success;
    for(const auto &entry : fs::directory_iterator(lib_path)) {
        auto entry_path = entry.path();
        #ifdef _WIN32
            if(entry.is_regular_file() && entry_path.extension() == ".dll") {
                if(load_plugin_windows(entry_path.string()) != ErrorCode::Success) {
                    status = ErrorCode::PluginLoadFailed;
                }
            }
        #else
            if(entry.is_regular_file() && entry_path.extension() == ".so") {
                if(load_plugin_unix(entry_path.string()) != ErrorCode::Success) {
                    status = ErrorCode::PluginLoadFailed;
                }
            }
        #endif
    }
    return status;
}

#ifdef _WIN32
ErrorCode PluginManager::load_plugin_windows(const std::string &path){
    std::shared_lock lock(mutex_);
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
    IConverterFactory* instance = create_plugin();
    if(instance == nullptr){
        ErrorHandler::log_to_file("Failed to create plugin instance in " + path);
        FreeLibrary(handle);
        return ErrorCode::PluginLoadFailed;
    }
    plugins_[{instance->support_mime_type(), instance->target_format()}] = PluginInfo(instance, handle);
    return ErrorCode::Success;


}
#else
ErrorCode PluginManager::load_plugin_unix(const std::string &path){

    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    if(!handle) {
        ErrorHandler::log_to_file("Failed to load plugin: " + path + " - " + dlerror());
        return ErrorCode::PluginLoadFailed;
    }
    using CreatePluginFunc = IConverterFactory* (*)();
    dlerror();
    CreatePluginFunc create_plugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "create_plugin"));
    if(!create_plugin) {
        ErrorHandler::log_to_file("Failed to find create_plugin function in " + path + " - " + dlerror());
        dlclose(handle);
        return ErrorCode::PluginLoadFailed;
    }
    IConverterFactory* instance = create_plugin();
    if(instance == nullptr){
        ErrorHandler::log_to_file("Failed to create plugin instance in " + path);
        dlclose(handle);
        return ErrorCode::PluginLoadFailed;
    }
    plugins_[{instance->support_mime_type(), instance->target_format()}] = PluginInfo(instance, handle);

    return ErrorCode::Success;
}
#endif

 IConverterFactory* PluginManager::get_converter(const std::string &mime_type, 
                                                 const std::string &target_format){

    std::shared_lock lock(mutex_);
    auto it = plugins_.find({mime_type, target_format});
    if(it != plugins_.end()){
        return it->second.instance.get();
    }
    return nullptr;
}


PluginManager::~PluginManager(){
    #ifdef _WIN32

        for(auto& kv : plugins_){
            kv.second.instance.reset();
            FreeLibrary(kv.second.handle);
        }
    #else
        for(auto& kv : plugins_){
            kv.second.instance.reset();
            dlclose(kv.second.handle);
        }

    #endif
}
