#pragma once
#include <dpp/message.h>
#include <dpp/snowflake.h>

#include <cstdint>
#include <string>

namespace skyo::utils {
struct pair_hash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

class Cooldown {
   private:
    std::unordered_map<std::pair<dpp::snowflake, std::string>, int64_t,
                       pair_hash>
        cooldowns;
    std::unordered_map<std::pair<dpp::snowflake, std::string>, bool, pair_hash>
        prevent;

   public:
    Cooldown();
    void setCooldown(const dpp::snowflake user_id, const std::string &command,
                     int64_t duration);
    bool isOnCooldown(const dpp::snowflake user_id, const std::string &command);
    int64_t getCooldown(const dpp::snowflake user_id,
                        const std::string &command);
    void preventCooldown(const dpp::snowflake user_id,
                         const std::string &command);
    bool isCooldownPrevented(const dpp::snowflake user_id,
                             const std::string &command);
};
}  // namespace skyo::utils
