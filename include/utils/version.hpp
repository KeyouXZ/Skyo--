#pragma once
#include <dpp/dpp.h>

#define VERSION "0.1.0"

namespace skyo::utils {
std::string get_local_version();
std::string get_remote_version(const std::string& url);
void check_version();
inline std::string version = VERSION;
}  // namespace skyo::utils
