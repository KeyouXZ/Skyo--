#include <dpp/dispatcher.h>

#include "../../include/global.hpp"

namespace skyo::events {
void on_ready() {
    bot->on_ready([](const dpp::ready_t &_) {
        bot->set_presence(dpp::presence(dpp::ps_dnd, dpp::at_game, "with you"));
    });
}
}  // namespace skyo::events
