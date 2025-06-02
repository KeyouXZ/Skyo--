#include <dpp/appcommand.h>
#include <dpp/cache.h>
#include <dpp/cluster.h>
#include <dpp/dpp.h>
#include <dpp/intents.h>
#include <dpp/presence.h>

#include "../include/CLI11.hpp"
#include "../include/event.hpp"
#include "../include/global.hpp"
#include "../include/registry.hpp"
#include "../include/utils/config.hpp"
#include "../include/utils/logger.hpp"
#include "../include/utils/version.hpp"

namespace skyo {
dpp::cluster* bot;
command_map_t cmds;
alias_map_t aliases;
config::Config cfg;
utils::Cooldown cooldown;
presence_cache_t presence_cache;
args_map_t args_value;

void register_slashcommand() {
    skyo::bot->on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong! " + to_string((int)bot->rest_ping * 1000) +
                        "ms");
        }
    });

    skyo::bot->on_ready([](const dpp::ready_t& _) {
        if (dpp::run_once<struct register_bot_commands>()) {
            skyo::bot->global_command_create(
                dpp::slashcommand("ping", "Ping pong!", bot->me.id));
        }
    });
}

void start() {
    skyo::cfg = skyo::config::read();
    skyo::bot = new dpp::cluster(skyo::cfg.config.token, dpp::i_all_intents);

    // skyo::bot->on_log(dpp::utility::cout_logger());
    skyo::utils::setup_logger(*skyo::bot);

    // Initialize events
    skyo::events::init();

    // Register commands & aliases
    skyo::registry::init();

    // Register slash commands
    skyo::register_slashcommand();

    skyo::bot->start(dpp::st_wait);
}
}  // namespace skyo

int main(int argc, char* argv[]) {
    CLI::App app{"Skyo++ Discord Bot"};
    app.description("Skyo++: A modern Discord bot using D++.");
    app.footer("Made with 💙 by Keyou");
    argv = app.ensure_utf8(argv);

    auto formatter = std::make_shared<CLI::Formatter>();
    formatter->column_width(20);
    app.formatter(formatter);

    bool version = false, sync = false, dpu = false;
    app.add_flag("-v,--version", version, "Show version information");
    app.add_flag("-s,--sync", sync, "Check update");
    app.add_flag("-P,--dpu", dpu, "Disable on_presence_update()");

    CLI11_PARSE(app, argc, argv);

    skyo::args_value["--version"] = version;
    skyo::args_value["--sync"] = sync;
    skyo::args_value["--dpu"] = dpu;

    if (version) {
        std::cout << "Skyo++ Discord Bot v" << skyo::utils::get_local_version()
                  << endl;
        return 0;
    } else if (sync) {
        skyo::utils::check_version();
        return 0;
    }

    if (dpu) {
        std::cout << "Presence Update Event Disabled" << endl;
    }

    skyo::start();
    return 0;
}
