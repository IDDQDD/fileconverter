#include "Server/SettingsUtils.hpp"
#include "Server/ErrorHandler.hpp"



ErrorCode SettingsProvider::settings_provider(std::string filename) {
    const auto settings = read_settings_from_json_file(filename);
    if(!settings.has_value() || !settings->is_object()){
            ErrorHandler::log("Failed to read settings file.");
            return ErrorCode::FAILURE;
    }
    const auto &root = settings->as_object();
    const json::object *srv_obj = &root;
    const json::object *err_obj = &root;
    const json::object *conn_obj = &root;
    if(set_error_handling_settings(err_obj) == ErrorCode::FAILURE ||
       set_server_settings(srv_obj) == ErrorCode::FAILURE ||
       set_connection_settings(conn_obj) == ErrorCode::FAILURE){
        return ErrorCode::FAILURE;
       }
    return ErrorCode::SUCCESS;
}

std::optional<json::value> SettingsProvider::read_settings_from_json_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        ErrorHandler::log("Could not open settings file: " + filename);
        return std::nullopt;
    }

    boost::system::error_code ec;
    auto result = json::parse(file, ec);
    if (ec) {
        ErrorHandler::log("JSON parse error in " + filename + ": " + ec.message());
        return std::nullopt;
    }
    return result;
}

bool SettingsProvider::has_field_in_json(const json::object *obj, std::string_view key){
    if(auto it = obj->if_contains(key); it && (it->is_string() || it->is_int64())){
        return true;
    }
    return false;
}



ConnectionSettings& SettingsProvider::get_connection_settings() const {
    return connection_settings_;
}
ServerSettings& SettingsProvider::get_server_settings() const {
    return server_settings_;
}

  ErrorCode SettingsProvider::set_server_settings(const json::object *serv_obj) {

        if(auto *srv = root.if_contains("server"); srv && srv->is_object())
           srv_obj = &srv->as_object();

        if(!has_field_in_json(*srv_obj, "address")){
            ErrorHandler::log("Settings file is missing 'address' setting.");
            return ErrorCode::FAILURE;
        }
        if(!has_field_in_json(*srv_obj, "port")){
            ErrorHandler::log("Settings file is missing 'port' setting.");
            return ErrorCode::FAILURE;
        }

        if(!has_field_in_json(*srv_obj, "host")){
            ErrorHandler::log("Settings file is missing 'host' setting.");
            return ErrorCode::FAILURE;
        }

        if(!has_field_in_json(*srv_obj, "threads")){
            ErrorHandler::log("Settings file is missing 'threads' setting.");
            return ErrorCode::FAILURE;
        }
    return ErrorCode::SUCCESS;
  }
  ErrorCode SettingsProvider::set_connection_settings(const json::object *conn_obj) {

     if(auto *cn = conn_obj->if_contains("connection"); cn && cn->is_object())
            conn_obj = &cn->as_object();

        if(!has_field_in_json(*conn_obj, "read_message_max") ||
           !has_field_in_json(*conn_obj, "auto_fragment")){ {
            ErrorHandler::log("Missing or invalid 'connection' setting");
                return ErrorCode::FAILURE;
        }
    return ErrorCode::SUCCESS;
  }
  ErrorCode SettingsProvider::set_error_handling_settings(const json::object *err_obj) {

       if(auto *eh = err_obj.if_contains("ErrorHandling"); eh && eh->is_object())
            err_obj = &eh->as_object();

        if(!has_field_in_json(*err_obj, "log_file")) {
            ErrorHandler::log("Missing or invalid 'log_file' setting.");
            return ErrorCode::FAILURE;
        }

    return ErrorCode::SUCCESS;
  }

