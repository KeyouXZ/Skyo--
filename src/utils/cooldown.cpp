#include "../../include/utils/cooldown.hpp"

#include <dpp/message.h>
#include <dpp/snowflake.h>

namespace skyo::utils {
Cooldown::Cooldown() {}

void Cooldown::setCooldown(const dpp::snowflake user_id,
                           const std::string &command, int64_t duration) {
    // Get current time in milliseconds since epoch
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();

    // Set cooldown end time
    cooldowns[{user_id, command}] = now + duration;
}

bool Cooldown::isOnCooldown(const dpp::snowflake user_id,
                            const std::string &command) {
    auto it = cooldowns.find({user_id, command});

    if (it == cooldowns.end()) {
        return false;
    }

    // Get current time in milliseconds since epoch
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();

    return it->second > now;
}

int64_t Cooldown::getCooldown(const dpp::snowflake user_id,
                              const std::string &command) {
    auto it = cooldowns.find({user_id, command});

    if (it == cooldowns.end()) {
        return 0;
    }

    // Get current time in milliseconds since epoch
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();

    return std::max<int64_t>(0, it->second - now);
}

void Cooldown::preventCooldown(const dpp::snowflake user_id,
                               const std::string &command) {
    prevent[{user_id, command}] = true;
}

bool Cooldown::isCooldownPrevented(const dpp::snowflake user_id,
                                   const std::string &command) {
    auto it = prevent.find({user_id, command});
    bool isThis = (it != prevent.end()) ? it->second : false;

    // Remove the entry if it exists
    if (it != prevent.end()) {
        prevent.erase(it);
    }

    return isThis;
}
}  // namespace skyo::utils
