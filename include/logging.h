#pragma once

#include <allegro5/allegro_native_dialog.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <vector>

class Logger
{
    public:
        template <typename ... Args>
        void print(const char *filename, int line, std::string msg, Args ... args)
        {
            std::string output = "%s at line %i: " + msg;
            print_flush(format(output, filename, line, args...));
        }

        template <typename ... Args>
        void panic(const char *filename, int line, std::string msg, Args ... args)
        {
            std::string output = "Fatal Error: %s at line %i: " + msg;
            panic_flush(format(output, filename, line, args...));
        }

    protected:
        template <typename ... Args>
        std::string format(std::string msg, Args ... args)
        {
            int size = std::snprintf(nullptr, 0, msg.c_str(), args...);
            // +1 for null termination
            std::vector<char> buf(size + 1);
            std::snprintf(&buf[0], buf.size(), msg.c_str(), args...);
            return std::string(&buf[0]);
        }

        virtual void print_flush(std::string msg) = 0;
        virtual void panic_flush(std::string msg) = 0;
};

class PrintLogger : public Logger
{
    public:
        void print_flush(std::string msg)
        {
            std::cout << msg << "\n" << std::flush;
        }

        void panic_flush(std::string msg)
        {
            std::cout << msg << "\n" << std::flush;
            throw new std::runtime_error(msg);
        }
};