#pragma once
#include <dpp/dpp.h>
#include <dpp/misc-enum.h>

#define MAX_LOG_SIZE 1024 * 1024 * 2;  // 2MB
#define LOG_FILE "bot.log";

namespace skyo::utils {
std::string severity_to_string(dpp::loglevel severity);
std::string color_for_severity(dpp::loglevel severity);
std::string emoji_for_severity(dpp::loglevel severity);
std::string current_timestamp();
void rotate_logs_if_needed();
void setup_logger(dpp::cluster& bot);

inline size_t max_log_size = MAX_LOG_SIZE;
inline std::string log_file = LOG_FILE;
}  // namespace skyo::utils
