#pragma once
#include "IConverterFactory.hpp"

class Text2Pdf : public IConverterFactory {
    const void *data;
    public:
        Text2Pdf() = default;
        ~Text2Pdf() override = default;

        std::string support_mime_type() const override;
        std::string target_format() const override;
        const void* convert(const void *data, std::size_t size) override;
};