#pragma once

#include <string>
#include "json.hpp"

class ConfigLoader
{
    public:
        nlohmann::json requestconfig();
        nlohmann::json requestconfig(const std::string &path);
        void saveconfig(nlohmann::json &config);

    protected:

    private:
        const std::string CONFIG_PATH = "config.json";
};
