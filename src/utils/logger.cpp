#include "../../include/utils/logger.hpp"

#include <fstream>
#include <iostream>

namespace skyo::utils {

void setup_logger(dpp::cluster& bot) {
    bot.on_log([](const dpp::log_t& event) {
        std::cout << "[" << event.severity << "] " << event.message
                  << std::endl;

        if (event.severity >= dpp::ll_warning) {
            std::ofstream logfile("bot.log", std::ios::app);
            logfile << "[" << event.severity << "] " << event.message
                    << std::endl;
        }
    });
}
}  // namespace skyo::utils
