#pragma once

constexpr double ENGINE_TIMESTEP = 1.0/60.0;
constexpr double MENU_TIMESTEP = 1.0/60.0;
constexpr double NETWORKING_TIMESTEP = 1/60.0;

constexpr unsigned int VIEWPORT_WIDTH = 960;

constexpr int STAIRCASE_STEPSIZE = -6;

constexpr int PLAYER_LIMIT = 16;

constexpr double PI = 3.1415;

constexpr auto NULL_SPRITE = "NULL_SPRITE";

constexpr auto LOBBY_MESSAGE_TYPE_REGISTER = "b5dae2e8-424f-9ed0-0fcb-8c21c7ca1352";
constexpr auto LOBBY_MESSAGE_TYPE_UNREGISTER = "488984ac-45dc-86e1-9901-98dd1c01c064";
constexpr auto LOBBY_MESSAGE_TYPE_LIST = "297d0df4-430c-bf61-640a-640897eaef57";
constexpr auto VANGUARD_IDENTIFIER = "4fd0319b-5868-4f24-8b77-568cbb18fde9";
constexpr auto COMPATIBILITY_IDENTIFIER = "c41aaa23-52e6-4160-9566-5110b71c645e";