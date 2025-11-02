#include "../../include/core/command.hpp"
#include "../../include/global.hpp"
#include "../../include/utils/helper.hpp"

#include <dpp/channel.h>
#include <dpp/colors.h>
#include <dpp/message.h>
#include <dpp/unicode_emoji.h>
#include <string>

using namespace skyo::command;

namespace skyo::commands::general {
inline std::string category_to_string(Category cat) {
    switch (cat) {
        case Category::GENERAL:
            return "GENERAL";
        case Category::DEVELOPER:
            return "DEVELOPER";
        case Category::UTILITY:
            return "UTILITY";
        default:
            return "UNKNOWN";
    }
}

Command ping = Command(
    { "ping",
      "Check the bot's latency.",
      Category::GENERAL,
      5,
      {},
      {},
      ContextType::All,
      [](const Context &context) {
          int latency = context.event.from()->websocket_ping * 1000;
          context.event.reply("Pong! " + std::to_string(latency) + "ms");
      } });

Command help = Command(
    { "help",
      "Displays a list of commands or detailed info about a specific command.",
      Category::GENERAL,
      5,
      {},
      { { "command", ArgType::STRING, false } },
      ContextType::All,
      [](const Context &context) {
          if (context.args.size() == 1) {
              // Get Command
              std::string input = std::get<std::string>(context.args[0].value);
              auto it           = cmds.find(input);
              if (it == cmds.end()) {
                  std::string suggestion = utils::find_closest_command(cmds, input);
                  if (!suggestion.empty()) {
                      context.event.reply("Did you mean `" + suggestion + "`?");
                  } else {
                      context.event.reply("Command not found.");
                  }
                  cooldown.preventCooldown(context.message.author.id, "help");
                  return;
              }

              Command &cmd = it->second;

              dpp::embed embed =
                  dpp::embed()
                      .set_title(string(dpp::unicode_emoji::books) + " help: `" + cmd.name + "`")
                      .set_color(dpp::colors::blue)
                      .add_field(
                          string(dpp::unicode_emoji::pushpin) + " Description",
                          cmd.description.empty() ? "No description available" : cmd.description)
                      .add_field(
                          string(dpp::unicode_emoji::paperclip) + string(" Alias") +
                              (cmd.aliases.size() > 1 ? "es" : ""),
                          cmd.aliases.empty() ? "No aliases available"
                                              : utils::join(cmd.aliases, ", "))
                      .add_field(
                          string(dpp::unicode_emoji::memo) + " Usage", utils::format_usage(cmd))
                      .add_field(
                          string(dpp::unicode_emoji::stopwatch) + " Cooldown",
                          cmd.cooldown == 0 ? "No cooldown" : to_string(cmd.cooldown) + " seconds")
                      .set_footer(
                          dpp::embed_footer()
                              .set_text("Requested by " + context.message.author.username)
                              .set_icon(context.message.author.get_avatar_url()));

              context.event.reply(dpp::message(context.message.channel_id, embed));
          } else {
              dpp::embed embed = dpp::embed()
                                     .set_title("📜 Command List")
                                     .set_color(dpp::colors::blue)
                                     .set_description("Use `help <command>` for details.\n");

              std::map<std::string, std::vector<std::pair<std::string, std::string>>>
                  sorted_commands;

              for (const auto &[name, cmd] : cmds) {
                  if (cmd.category == Category::DEVELOPER) continue;

                  std::string category = category_to_string(cmd.category);
                  std::string description =
                      cmd.description.empty() ? "No description" : cmd.description;

                  sorted_commands[category].emplace_back(cmd.name, description);
              }

              for (auto &[category, commands] : sorted_commands) {
                  std::sort(commands.begin(), commands.end());

                  std::ostringstream ss;
                  for (const auto &[cmd_name, desc] : commands) {
                      ss << "`" << cmd_name << "` - " << desc << "\n";
                  }

                  embed.add_field(category, ss.str(), false);
              }

              context.event.reply(dpp::message(context.message.channel_id, embed));
          }
      } });
} // namespace skyo::commands::general
