#pragma once

#include <string>
#include "json.hpp"

class ConfigLoader
{
    public:
        nlohmann::json open(const std::string &path);
        void save(std::string &path);
    private:
        nlohmann::json config;
};
