#include <dpp/cache.h>

#include "../../include/global.hpp"

using namespace dpp;

namespace skyo::events {
void on_presence_update() {
    bot->on_presence_update([](const dpp::presence_update_t &event) {
        presence_cache[event.rich_presence.user_id] = event.rich_presence;
    });
}
}  // namespace skyo::events
