#pragma once

#include <string>
#include "json.hpp"

class ConfigLoader
{
    public:
        nlohmann::json requestconfig();
        void saveconfig(nlohmann::json &config);

    protected:

    private:
        const std::string CONFIG_DIR = "config.json";
};
