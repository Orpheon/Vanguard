#include <fstream>
#include <string>

#include "configloader.h"
#include "json.hpp"

nlohmann::json ConfigLoader::requestconfig()
{
    return requestconfig(CONFIG_PATH);
}

nlohmann::json ConfigLoader::requestconfig(const std::string &path)
{
    std::ifstream configfile(path);
    nlohmann::json config;
    config << configfile;
    configfile.close();
    return config;
}

void ConfigLoader::saveconfig(nlohmann::json& config)
{
    std::ofstream configfile(CONFIG_PATH);
    configfile << config.dump(4) << std::endl;
    configfile.close();
}
