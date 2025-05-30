#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace skyo::config {
struct GeneralConfig {
    std::string token;
    std::vector<std::string> prefix;
    std::vector<uint64_t> developers;
};

struct EconomyConfig {
    std::string currency;
    std::string full_currency;
};

struct Config {
    GeneralConfig config;
    EconomyConfig economy;
};

Config read();
}  // namespace skyo::config
