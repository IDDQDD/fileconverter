#include <exception>
#include <iostream>
#include <vector>

#include "Server/SettingsProvider.hpp"
#include "Server/ServerCore.hpp"
#include "Server/ErrorHandler.hpp"
#include "Server/PluginManager.hpp"


static void run_io(net::io_context& ioc, const int64_t threads_count) {
    std::vector<std::thread> t;
    for (int64_t i = 1; i < threads_count; ++i)
        t.emplace_back([&ioc] { ioc.run(); });

    ioc.run();
    for(auto& th : t)
        if(th.joinable())
            th.join();
}

int main()
{
    try {
        auto plugin_manager = std::make_shared<PluginManager>();
        if (plugin_manager->LoadPlugins() != ErrorCode::SUCCESS) {
            ErrorHandler::log_to_file("Some plugins failed to load!");
            return EXIT_FAILURE;
        } // Load plugins from the specified directory

        SettingsProvider sp;
        if(sp.settings_provider("backend/config/settings.json") == ErrorCode::FAILURE) {
            ErrorHandler::log_to_file("Failed to load server settings.");
            return EXIT_FAILURE;
        } // Load server settings from the configuration file
        
        // The io_context is required for all I/O
        const int64_t threads_count = sp.get_server_settings().threads;
        net::io_context ioc(threads_count);
    
        // Create and launch a listening 
        std::make_shared<ServerCore>(ioc, sp.get_server_settings(), 
                                     sp.get_connection_settings(), plugin_manager)->run();

       run_io(ioc, threads_count);
    
    } catch (const boost::system::system_error& e) {
        ErrorHandler::log_to_file("System error: " + std::string(e.what()));
        std::cerr << "System error: " << e.what() << std::endl;
    } catch (...) {
        ErrorHandler::log_to_file("Unknown exception occurred.");
        std::cerr << "Unknown exception occurred." << std::endl;
    }
    return EXIT_SUCCESS;
}