#include <dpp/unicode_emoji.h>

#include "../../include/global.hpp"
#include "../../include/parser.hpp"

using namespace dpp;

namespace skyo::events {
void on_message_create() {
    bot->on_message_create([](const message_create_t &event) {
        // Return if the sender is the bot
        if (event.msg.author.id == bot->me.id) return;

        // Prefix handler
        string msg = event.msg.content;
        string prefix_used = parser::prefix_handler(msg);
        string command_name = msg.substr(prefix_used.size());

        if (prefix_used.empty()) return;

        // Pre-Command
        auto space_pos = command_name.find(' ');
        if (space_pos != string::npos) {
            command_name = command_name.substr(0, space_pos);
        }

        // Alias
        if (aliases.find(command_name) != aliases.end()) {
            command_name = aliases[command_name];
        }

        // Command
        auto command_it = cmds.find(command_name);

        if (command_it != cmds.end()) {
            const command::Command &cmd = command_it->second;

            // Check Context Type
            if (cmd.context == command::ContextType::Guild &&
                event.msg.is_dm()) {
                event.reply("Wrong place!");
                return;
            } else if (cmd.context == command::ContextType::DM &&
                       !event.msg.is_dm()) {
                event.reply("Wrong place!");
                return;
            }

            // check cooldown
            if (cooldown.isOnCooldown(event.msg.author.id, cmd.name)) {
                int cooldown_left =
                    cooldown.getCooldown(event.msg.author.id, cmd.name) / 1000;

                bot->message_create(
                    message(event.msg.channel_id,
                            "You can use this command again in " +
                                to_string(cooldown_left) + " seconds."),
                    [cooldown_left](const confirmation_callback_t &callback) {
                        if (!callback.is_error()) {
                            message reply_msg =
                                std::get<message>(callback.value);

                            std::thread([reply_msg, cooldown_left]() {
                                this_thread::sleep_for(
                                    chrono::seconds(cooldown_left));
                                bot->message_delete(reply_msg.id,
                                                    reply_msg.channel_id);
                            }).detach();
                        }
                    });

                return;
            }

            // Args handler
            vector<command::Argument> args = parser::parse_args(
                msg.substr(prefix_used.size()), cmd.usage, event);
            if (args.empty()) {
                // If args parsing failed, we return early
                return;
            }
            args.erase(args.begin());

            command::Context ctx(
                {make_shared<command::Command>(cmd), event.msg, event, args});

            cmd.callback(ctx);

            // Set Cooldown
            if (!cooldown.isCooldownPrevented(event.msg.author.id, cmd.name)) {
                auto msg_id = event.msg.id;
                auto channel_id = event.msg.channel_id;
                auto cooldown_duration = cmd.cooldown * 1000;

                cooldown.setCooldown(event.msg.author.id, cmd.name,
                                     cooldown_duration);
                bot->message_add_reaction(
                    event.msg.id, event.msg.channel_id,
                    unicode_emoji::hourglass_flowing_sand);

                std::thread([cmd, msg_id, channel_id, cooldown_duration]() {
                    // Sleep for cooldown duration
                    this_thread::sleep_for(
                        chrono::milliseconds(cooldown_duration));

                    // Remove reaction
                    bot->message_delete_reaction(
                        msg_id, channel_id, bot->me.id,
                        unicode_emoji::hourglass_flowing_sand);
                }).detach();
            }
        }
    });
}
}  // namespace skyo::events
