#pragma once
#include <dpp/dpp.h>
#include <dpp/presence.h>
#include <dpp/snowflake.h>

#include <any>
#include <chrono>
#include <unordered_map>

#include "core/command.hpp"
#include "utils/config.hpp"
#include "utils/cooldown.hpp"

namespace skyo {
using command_map_t = std::unordered_map<std::string, command::Command>;
using alias_map_t = std::unordered_map<std::string, std::string>;
using presence_cache_t = std::unordered_map<dpp::snowflake, dpp::presence>;
using args_map_t = std::unordered_map<std::string, any>;

extern dpp::cluster* bot;
extern config::Config cfg;
extern command_map_t cmds;
extern alias_map_t aliases;
extern utils::Cooldown cooldown;
inline chrono::time_point start_time = chrono::system_clock::now();
extern presence_cache_t presence_cache;
extern args_map_t args_value;
}  // namespace skyo
