#include "../include/CLI11.hpp"
#include "../include/event.hpp"
#include "../include/global.hpp"
#include "../include/registry.hpp"
#include "../include/utils/config.hpp"
#include "../include/utils/logger.hpp"
#include "../include/utils/version.hpp"

#include <chrono>
#include <csignal>
#include <dpp/appcommand.h>
#include <dpp/cache.h>
#include <dpp/cluster.h>
#include <dpp/dpp.h>
#include <dpp/intents.h>
#include <dpp/misc-enum.h>
#include <dpp/presence.h>
#include <thread>

namespace skyo {
dpp::cluster* bot;
command_map_t cmds;
alias_map_t aliases;
config::Config cfg;
utils::Cooldown cooldown;
presence_cache_t presence_cache;
args_map_t args_value;

bool should_exit = false, cc_clicked = false;
std::chrono::steady_clock::time_point last_clicked;

void handle_signal(int signal) {
    if (signal == SIGINT) {
        auto now  = std::chrono::steady_clock::now();
        auto diff = now - last_clicked;
        if (!cc_clicked) {
            cc_clicked   = true;
            last_clicked = now;

            skyo::bot->log(dpp::ll_info, "⚠️Press Ctrl+C again within 3 seconds to exit");
        } else if (cc_clicked && diff < chrono::seconds(3)) {
            should_exit = true;
        } else {
            cc_clicked = false;
        }
    }
}

void register_slashcommand() {
    skyo::bot->on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            float ws_ping = bot->get_shard(event.shard)->websocket_ping;
            int ping_ms   = static_cast<int>(ws_ping * 1000);
            event.reply("Pong! " + to_string(ping_ms) + "ms");
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

    skyo::utils::setup_logger(*skyo::bot);

    // Initialize events
    skyo::events::init();

    // Register commands & aliases
    skyo::registry::init();

    // Register slash commands
    skyo::register_slashcommand();

    // Register signal handler
    std::signal(SIGINT, skyo::handle_signal);

    // Start bot
    skyo::bot->start(dpp::st_return);

    while (!skyo::should_exit) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    bot->log(dpp::ll_info, "Shutting down bot...");

    skyo::bot->shutdown();
    exit(0);
}
} // namespace skyo

int main(int argc, char* argv[]) {
    CLI::App app { "Skyo++ Discord Bot" };
    app.description("Skyo++: A modern Discord bot using D++.");
    app.footer("Made with 💙 by Keyou");
    argv = app.ensure_utf8(argv);

    auto formatter = std::make_shared<CLI::Formatter>();
    formatter->column_width(20);
    app.formatter(formatter);

    bool version = false, sync = false, dpu = false, verbose = false;
    app.add_flag("-v,--version", version, "Show version information");
    app.add_flag("-s,--sync", sync, "Check update");
    app.add_flag("-P,--dpu", dpu, "Disable on_presence_update event");
    app.add_flag("-V,--verbose", verbose, "Enable verbose logging output");

    CLI11_PARSE(app, argc, argv);

    skyo::args_value["--version"] = version;
    skyo::args_value["--sync"]    = sync;
    skyo::args_value["--dpu"]     = dpu;
    skyo::args_value["--verbose"] = verbose;

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
