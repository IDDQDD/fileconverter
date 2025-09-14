#pragma once
#include <boost/json.hpp>
#include <tuple>
#include <fstream>
#include <string>
#include <optional>
#include <map>
#include <variant>
#include "ErrorHandler.hpp"
#include "Settings.hpp"
namespace json = boost::json;
class SettingsProvider{
private:
ConnectionSettings connection_settings_{};
ServerSettings server_settings_{};

  ErrorCode set_server_settings(const json::object *serv_obj);
  ErrorCode set_connection_settings(const json::object *conn_obj);
  ErrorCode set_error_handling_settings(const json::object *err_obj);
  ErrorCode set_plugin_settings(const json::object *plg_obj);
  bool has_field_in_json(const json::object *obj, std::string_view field);
  std::optional<json::value> read_settings_from_json_file(const std::string& filename);

  
  public:
  SettingsProvider() = default;
  ErrorCode settings_provider(const std::string filename);
  const ConnectionSettings& get_connection_settings() const;
  const ServerSettings& get_server_settings() const;
};


