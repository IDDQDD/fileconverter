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
        
        
        ErrorHandler::set_log_file(log_file->as_string());

        auto plugin_manager = std::make_shared<PluginManager>();
        if (plugin_manager->LoadPlugins() != ErrorCode::Success) {
            ErrorHandler::log_to_file("Some plugins failed to load!");
            return EXIT_FAILURE;
        } // Load plugins from the specified directory
        auto sp = std::make_shared<SettingsProvider>();
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