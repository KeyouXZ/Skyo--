#include "../../include/utils/version.hpp"

#include <curl/curl.h>

#include <regex>

namespace skyo::utils {
std::string get_local_version() { return version; }

static size_t write_callback(void* contents, size_t size, size_t nmemb,
                             std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

std::string get_remote_version(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "SkyoBot/1.0");

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    std::regex version_regex(R"verdef(#define\s+VERSION\s+"([\d\.]+)")verdef");
    std::smatch match;

    if (std::regex_search(response, match, version_regex)) {
        return match[1];
    }

    return "unknown";
}

void check_version() {
    const std::string local = get_local_version();
    const std::string remote = get_remote_version(
        "https://raw.githubusercontent.com/KeyouXZ/Skyo--/main/include/utils/"
        "version.hpp");

    if (remote == "unknown" || remote == "404: Not Found") {
        std::cout << "⚠️Unable to check latest version.\n";
        return;
    }

    if (local != remote) {
        std::cout << "🔔 Update available! Local: v" << local << ", Latest: v"
                  << remote << "\n";
    } else {
        std::cout << "✅ You are using the latest version: v" << local << "\n";
    }
}
}  // namespace skyo::utils
