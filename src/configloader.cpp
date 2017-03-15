#include <fstream>
#include <string>

#include "configloader.h"
#include "json.hpp"

nlohmann::json ConfigLoader::requestconfig()
{
    std::ifstream configfile(CONFIG_DIR);
    nlohmann::json config;
    config << configfile;
    configfile.close();
    return config;
}

void ConfigLoader::saveconfig(nlohmann::json& config)
{
    std::ofstream configfile(CONFIG_DIR);
    configfile << config.dump(4) << std::endl;
    configfile.close();
}
