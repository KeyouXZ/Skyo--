#include <dpp/dpp.h>
#include <dpp/intents.h>
#include <dpp/presence.h>

#include "include/event.hpp"
#include "include/global.hpp"
#include "include/registry.hpp"
#include "include/utils/config.hpp"
#include "include/utils/logger.hpp"

namespace skyo {
dpp::cluster* bot;
command_map_t cmds;
alias_map_t aliases;
config::Config cfg;
utils::Cooldown cooldown;
}  // namespace skyo

int main() {
    skyo::cfg = skyo::config::read();
    skyo::bot = new dpp::cluster(skyo::cfg.config.token, dpp::i_all_intents);

    // skyo::bot->on_log(dpp::utility::cout_logger());
    skyo::utils::setup_logger(*skyo::bot);

    // Initialize events
    skyo::events::init();

    // Register commands & aliases
    skyo::registry::init();

    skyo::bot->start(dpp::st_wait);
    return 0;
}
