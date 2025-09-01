#include <exception>
#include <iostream>
#include <vector>

#include "Server/SettingsUtils.hpp"
#include "Server/ServerCore.hpp"
#include "Server/ErrorHandler.hpp"
#include "Server/PluginManager.hpp"

int main(int argc, char* argv[])
{
    try {
        const auto& settings = read_from_json_file("backend/config/settings.json");
        if(!settings.has_value()){
            ErrorHandler::log("Failed to read settings file.");
            return EXIT_FAILURE;
        }
        const auto &settings_obj = settings.value().as_object();
        if(!settings_obj.contains("server")){
            ErrorHandler::log("Settings file contains deprecated 'server' section.");
            return EXIT_FAILURE;
        }
        const auto &server = get_value_from_json(settings_obj, "server");
        if(!server.has_value() || !server->is_object()){
            ErrorHandler::log("Settings file is missing 'server' section.");
            return EXIT_FAILURE;
        }
        const auto& server_obj = server.value().as_object();
        if(!server_obj.contains("threads")){
            ErrorHandler::log("Settings file is missing 'threads' setting.");
            return EXIT_FAILURE;
        }
        const auto &error_file = get_value_from_json(settings_obj, "ErrorHandling");
        if(!error_file.has_value() || !error_file->is_object()){
            ErrorHandler::log("Settings file is missing 'ErrorHandling' section.");
            return EXIT_FAILURE;
        }
        const auto& error_obj = error_file->as_object();
        if(!error_obj.contains("log_file")){
            ErrorHandler::log("Settings file is missing 'log_file' setting.");
            return EXIT_FAILURE;
        }
        const auto &threads = get_value_from_json(server_obj, "threads");
        if(!threads.has_value()){
            ErrorHandler::log("Missing or invalid 'threads' setting.");
            return EXIT_FAILURE;
        }
        const auto &log_file = get_value_from_json(error_obj, "log_file");
        if(!log_file.has_value()){
            ErrorHandler::log("Missing or invalid 'log_file' setting.");
            return EXIT_FAILURE;
        }
        ErrorHandler::set_log_file(log_file->as_string());

        auto plugin_manager = std::make_shared<PluginManager>();
        if (plugin_manager->LoadPlugins() != ErrorCode::Success) {
            ErrorHandler::log_to_file("Some plugins failed to load!");
            return EXIT_FAILURE;
        } // Load plugins from the specified directory

        // The io_context is required for all I/O
        const int64_t threads_count = threads->as_int64();
        net::io_context ioc(threads_count);

        // Create and launch a listening 
        std::make_shared<ServerCore>(ioc, settings_obj, plugin_manager)->run();

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> t;
        for (int64_t i = 1; i < threads_count; ++i)
            t.emplace_back([&ioc] { ioc.run(); });

        ioc.run();
        for(auto& th : t)
            if(th.joinable())
                th.join();
    
    } catch (const boost::system::system_error& e) {
        ErrorHandler::log_to_file("System error: " + std::string(e.what()));
        std::cerr << "System error: " << e.what() << std::endl;
    } catch (...) {
        ErrorHandler::log_to_file("Unknown exception occurred.");
        std::cerr << "Unknown exception occurred." << std::endl;
    }
    return EXIT_SUCCESS;
}