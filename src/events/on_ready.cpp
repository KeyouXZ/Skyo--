#include <dpp/dispatcher.h>
#include <dpp/misc-enum.h>
#include <dpp/presence.h>

#include "../../include/global.hpp"

namespace skyo::events {
void on_ready() {
    bot->on_ready([](const dpp::ready_t &_) {
        dpp::presence pres =
            dpp::presence(dpp::ps_dnd, dpp::at_game, "with you");
        bot->set_presence(pres);

        if (!any_cast<bool>(skyo::args_value["--dpu"]))
            presence_cache[bot->me.id] = pres;

        bot->log(dpp::ll_info, "Logged in as " + bot->me.username + "#" +
                                   to_string(bot->me.discriminator));
    });
}
}  // namespace skyo::events
