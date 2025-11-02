#include "../../include/utils/logger.hpp"

#include "../../include/global.hpp"

#include <fstream>
#include <iostream>

using namespace std;
using namespace dpp;

namespace skyo::utils {
const char* RESET  = "\033[0m";
const char* RED    = "\033[31m";
const char* YELLOW = "\033[33m";
const char* GREEN  = "\033[32m";
const char* CYAN   = "\033[36m";
const char* GRAY   = "\033[90m";

std::mutex cout_mutex;

string severity_to_string(loglevel severity) {
    switch (severity) {
        case ll_trace:
            return "TRACE";
        case ll_debug:
            return "DEBUG";
        case ll_info:
            return "INFO";
        case ll_warning:
            return "WARN";
        case ll_error:
            return "ERROR";
        case ll_critical:
            return "FATAL";
        default:
            return "LOG";
    }
}

string color_for_severity(loglevel severity) {
    switch (severity) {
        case ll_trace:
            return GRAY;
        case ll_debug:
            return CYAN;
        case ll_info:
            return GREEN;
        case ll_warning:
            return YELLOW;
        case ll_error:
        case ll_critical:
            return RED;
        default:
            return RESET;
    }
}

string emoji_for_severity(loglevel severity) {
    switch (severity) {
        case ll_trace:
            return "🔍";
        case ll_debug:
            return "🐛";
        case ll_info:
            return "✅";
        case ll_warning:
            return "⚠️ ";
        case ll_error:
            return "❌";
        case ll_critical:
            return "💀";
        default:
            return "🔔";
    }
}

string current_timestamp() {
    chrono::time_point now = chrono::system_clock::now();
    time_t t_now           = chrono::system_clock::to_time_t(now);
    tm tm                  = *localtime(&t_now);

    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void rotate_logs_if_needed() {
    namespace fs = std::filesystem;

    if (!fs::exists(log_file)) return;

    error_code ec;
    size_t size = fs::file_size(log_file, ec);
    if (ec || size < max_log_size) return;

    string rotated = log_file + ".old";

    fs::remove(rotated, ec);
    fs::rename(log_file, rotated, ec);
}

void safe_log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "\33[2K\r" // clear line
              << msg << std::endl;
}

void setup_logger(cluster& bot) {
    bot.on_log([](const log_t& event) {
        const auto color     = color_for_severity(event.severity);
        const auto emoji     = emoji_for_severity(event.severity);
        const auto level     = severity_to_string(event.severity);
        const auto timestamp = current_timestamp();

        if (!any_cast<bool>(args_value["--verbose"]) && (event.severity == ll_debug || event.severity == ll_trace))
            return;

        ostringstream logline;
        logline << GRAY << "[" << timestamp << "] " << color << "[" << level << "]" << RESET << " " << emoji << " " << event.message;

        safe_log(logline.str());

        rotate_logs_if_needed();
        ofstream logfile(log_file, ios::app);
        logfile << "[" << timestamp << "] [" << level << "] " << event.message << std::endl;
    });
}
} // namespace skyo::utils
