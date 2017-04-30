#pragma once

#include <stdexcept>

#include "logging.h"
#include "json.hpp"

class Global {
    public:
        static Logger& logging()
        {
            if (internal_logger_ptr == nullptr)
            {
                throw new std::runtime_error("Attempt to log something before a logging instance was created!");
            }
            return *internal_logger_ptr;
        }
        static void provide_logging(Logger *new_logger) { internal_logger_ptr = new_logger; }

        static nlohmann::json& settings()
        {
            if (internal_settings_config == nullptr)
            {
                throw new std::runtime_error("Attempt to access global settings before those were loaded!");
            }
            return *internal_settings_config;
        }
        static void provide_settings(nlohmann::json *new_settings) { internal_settings_config = new_settings; }

    private:
        static Logger *internal_logger_ptr;
        static nlohmann::json *internal_settings_config;
};