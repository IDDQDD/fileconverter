#include <exception>
#include <iostream>
#include <vector>
#include <../include/Server/ServerCore.hpp>
#include <../include/Server/ErrorHandler.hpp>
#include <../include/Server/PluginManager.hpp>

int main(int argc, char* argv[])
{
    try {
        const auto settings = read_from_file("../config/settings.json");
        const auto threads = settings.at("threads").as_int64();
        ErrorHandler::set_log_file(settings.at("log_file").as_string());

        auto plugin_manager = std::make_unique<PluginManager>();
        if (plugin_manager->LoadPlugins() != ErrorCode::Success) {
            ErrorHandler::log_to_file("Some plugins failed to load!");
            return EXIT_FAILURE;
        } // Load plugins from the specified directory

        // The io_context is required for all I/O
        net::io_context ioc(threads);

        // Create and launch a listening 
        std::make_shared<ServerCore>(ioc, settings, plugin_manager)->run();

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> t;
        for (std::size_t i = 1; i < threads; ++i)
            t.emplace_back([&ioc] { ioc.run(); });

        ioc.run();
        for(auto& th : t)
            if(th.joinable())
                th.join();
    } catch (const std::exception& e) {
        ErrorHandler::log_to_file("Exception: " + std::string(e.what()));
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (const boost::system::system_error& e) {
        ErrorHandler::log_to_file("System error: " + std::string(e.what()));
        std::cerr << "System error: " << e.what() << std::endl;
    } catch (...) {
        ErrorHandler::log_to_file("Unknown exception occurred.");
        std::cerr << "Unknown exception occurred." << std::endl;
    }
    return EXIT_SUCCESS;
}