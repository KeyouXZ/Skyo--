#include "../../include/utils/config.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace skyo::config {

inline void to_json(json& j, GeneralConfig const& p) {
    j = json{
        {"Token", p.token}, {"Prefix", p.prefix}, {"Developers", p.developers}};
}
inline void from_json(json const& j, GeneralConfig& p) {
    j.at("Token").get_to(p.token);
    j.at("Prefix").get_to(p.prefix);
    j.at("Developers").get_to(p.developers);
}

inline void to_json(json& j, EconomyConfig const& e) {
    j = json{{"Currency", e.currency}, {"Full Currency", e.full_currency}};
}
inline void from_json(json const& j, EconomyConfig& e) {
    j.at("Currency").get_to(e.currency);
    j.at("Full Currency").get_to(e.full_currency);
}

Config read() {
    Config config;
    std::string generalPath = "configs/General.json";
    std::string economyPath = "configs/Economy.json";

    fs::create_directories("configs");

    try {
        if (!fs::exists(generalPath)) {
            config.config.token = "YOUR_TOKEN_HERE";
            config.config.prefix = {"!"};
            config.config.developers = {123456789012345678};

            std::ofstream o(generalPath);
            o << std::setw(4) << json(config.config) << "\n";
        }
        {
            std::ifstream i(generalPath);
            json j;
            i >> j;
            config.config = j.get<GeneralConfig>();
        }

        if (!fs::exists(economyPath)) {
            config.economy.currency = "Sb";
            config.economy.full_currency = "Skyorb";
            std::ofstream o(economyPath);
            o << std::setw(4) << json(config.economy) << "\n";
        }
        {
            std::ifstream i(economyPath);
            json j;
            i >> j;
            config.economy = j.get<EconomyConfig>();
        }
    } catch (std::exception const& ex) {
        std::cerr << "[Config Error] " << ex.what() << "\n";
    }
    return config;
}
}  // namespace skyo::config
