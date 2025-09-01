#pragma once
#include <boost/asio.hpp>
#include <optional>
#include <string>
struct ConvertResult {
    ConvertResult() : size(0) {}
    ConvertResult(std::vector<std::uint8_t>& v, std::size_t s) : data(std::move(v)), size(s) {}
    std::vector<std::uint8_t> data;
    std::size_t size;
};
class IConverterFactory {
public:
    virtual ~IConverterFactory() = default;
    virtual std::optional<ConvertResult> convert(const void *data, std::size_t size )  = 0;
    virtual std::string support_mime_type() const = 0;
    virtual std::string target_format() const = 0;
};