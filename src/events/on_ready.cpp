#include <dpp/dispatcher.h>
#include <dpp/presence.h>

#include "../../include/global.hpp"

namespace skyo::events {
void on_ready() {
    bot->on_ready([](const dpp::ready_t &_) {
        dpp::presence pres =
            dpp::presence(dpp::ps_dnd, dpp::at_game, "with you");
        bot->set_presence(pres);

        presence_cache[bot->me.id] = pres;
    });
}
}  // namespace skyo::events
