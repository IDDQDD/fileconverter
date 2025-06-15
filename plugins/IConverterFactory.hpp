
#include <boost/asio.hpp>
#include <string>
class IConverterFactory {
public:
    virtual ~IConverterFactory() = default;
    virtual const void* convert(const void *data, std::size_t size )  = 0;
    virtual std::string support_mime_type() const = 0;
    virtual std::string target_format() const = 0;
};