#include "../../include/core/command.hpp"
#include "../../include/global.hpp"
#include "../../include/utils/helper.hpp"

#include <chrono>
#include <ctime>
#include <dpp/cache.h>
#include <dpp/colors.h>
#include <dpp/discordevents.h>
#include <dpp/guild.h>
#include <dpp/message.h>
#include <dpp/presence.h>
#include <dpp/snowflake.h>
#include <dpp/unicode_emoji.h>
#include <dpp/user.h>
#include <memory>
#include <sstream>
#include <vector>

using namespace std;
using namespace dpp;
using namespace skyo::command;

string status_to_string(const presence_status &status) {
    switch (status) {
        case ps_offline:
            return "Offline";
        case ps_online:
            return "Online";
        case ps_dnd:
            return "Do not Distrub";
        case ps_idle:
            return "Idle";
        case ps_invisible:
            return "Invisible";
        default:
            return "N/A (" + to_string((int)status) + ")";
    }
}

string activity_to_string(const activity &activity) {
    switch (activity.type) {
        case at_game:
            return "Playing";
        case at_streaming:
            return "Streaming";
        case at_listening:
            return "Listening to";
        case at_watching:
            return "Watching";
        case at_competing:
            return "Competing at";
        case at_custom:
            return activity.name;
        default:
            return "N/A (" + to_string((int)activity.type) + ")";
    }
}
/*
 * @value 1 is Verification Level
 * @value 2 is MFA Level
 * @value 3 is Content Filter
 * @value 4 is NSFW Level
 * */
vector<string> security_to_string(const guild &server) {
    vector<string> result;
    switch (server.verification_level) {
        case ver_low:
            result.push_back("Low");
            break;
        case ver_medium:
            result.push_back("Medium");
            break;
        case ver_high:
            result.push_back("High");
            break;
        case ver_very_high:
            result.push_back("Very High");
            break;
        default:
            result.push_back("None");
    }

    result.push_back(server.mfa_level == mfa_elevated ? "Elevated" : "None");

    switch (server.explicit_content_filter) {
        case expl_all_members:
            result.push_back("All Members");
            break;
        case expl_members_without_roles:
            result.push_back("Members without roles");
            break;
        default:
            result.push_back("Disabled");
    }

    switch (server.nsfw_level) {
        case nsfw_age_restricted:
            result.push_back("Age Restricted");
            break;
        case nsfw_explicit:
            result.push_back("Explicit");
            break;
        case nsfw_safe:
            result.push_back("Safe");
            break;
        default:
            result.push_back("Default");
    }

    return result;
}

string afk_timeout_to_string(const guild_afk_timeout_t &timeout) {
    switch (timeout) {
        case afk_off:
            return "Disabled";
        case afk_60:
            return "1 Minute";
        case afk_300:
            return "5 Minutes";
        case afk_900:
            return "15 Minutes";
        case afk_1800:
            return "30 Minutes";
        default:
            return "1 Hour";
    }
}

namespace skyo::commands::utility {
Command avatar = Command(
    { "avatar",
      "Get the avatar of a user",
      Category::UTILITY,
      5,
      {},
      { { "user", ArgType::USER, false } },
      ContextType::All,
      [](const Context &context) {
          user usr;
          embed msg_embed;

          usr = context.args.size() == 1 ? *get<shared_ptr<user>>(context.args[0].value)
                                         : context.message.author;

          msg_embed =
              embed()
                  .set_title(usr.username + "'s avatar")
                  .set_image(usr.get_avatar_url(4096))
                  .set_footer(
                      embed_footer().set_text("Requested by " + context.message.author.username))
                  .set_timestamp(time(nullptr))
                  .set_color(rand() % 0xFFFFFF);

          message msg = message(context.message.channel_id, msg_embed);
          msg.add_component(
              component().add_component(
                  component()
                      .set_label("Download")
                      .set_type(cot_button)
                      .set_style(cos_link)
                      .set_url(usr.get_avatar_url(4096))
                      .set_id("down")));

          context.event.send(msg);
      } });

Command botinfo = Command(
    { "botinfo",
      "Shows information about the bot",
      Category::UTILITY,
      5,
      { "bi" },
      {},
      ContextType::All,
      [](const Context &context) {
          user self = bot->me;

          // information
          long create_at = static_cast<long>(floor(self.get_creation_time()));

          auto now                      = chrono::system_clock::now();
          chrono::duration<long> uptime = chrono::duration_cast<chrono::seconds>(now - start_time);

          string uptime_format;
          int days    = uptime.count() / 86400;
          int hours   = (uptime.count() % 86400) / 3600;
          int minutes = (uptime.count() % 3600) / 60;
          int seconds = uptime.count() % 60;
          if (days > 0) uptime_format += to_string(days) + "d ";
          if (hours > 0) uptime_format += to_string(hours) + "h ";
          if (minutes > 0) uptime_format += to_string(minutes) + "m ";
          uptime_format += to_string(seconds) + "s ";

          vector<string> dev_names;
          for (auto id : cfg.config.developers) {
              if (const user *usr = find_user(id)) {
                  usr ? dev_names.push_back(usr->global_name) : dev_names.push_back("Unknown");
              }
          }

          ostringstream info;
          info << "- **ID:** " << self.id << "\n";
          info << "- **Username:** " << self.username << "\n";
          info << "- **Tag:** " << self.discriminator << "\n";
          info << "- **Created:** <t:" << create_at << ":F> (<t:" << create_at << ":R>)\n";
          info << "- **Developer:** " << utils::join(dev_names, ", ") << "\n";
          info << "- **Prefixes:** " << utils::join(cfg.config.prefix, " ") << "\n";
          info << "- **Uptime:** " << uptime_format << "\n";
          info << "- **Guilds:** " << get_guild_count() << "\n";
          info << "- **Users:** " << get_user_count() << "\n";
          info << "- **Channels:** " << get_channel_count();

          embed emd =
              embed()
                  .set_title(
                      string(unicode_emoji::robot) + " " +
                      (!self.global_name.empty() ? self.global_name : self.username) +
                      "'s information")
                  .add_field(string("🔍") + " Bot info", info.str())
                  .set_color(colors::blue)
                  .set_thumbnail(self.get_avatar_url(128))
                  .set_footer(
                      embed_footer().set_text("Requested by " + context.message.author.username))
                  .set_timestamp(time(nullptr));

          context.event.send(message(context.message.channel_id, emd));
      } });

Command userinfo = Command(
    { "userinfo",
      "Get information about a user",
      Category::UTILITY,
      5,
      { "ui" },
      { { "user", ArgType::GUILD_MEMBER, false } },
      ContextType::Guild,
      [](const Context &context) {
          bool has_arg_user   = context.args.size() == 1;
          guild_member member = has_arg_user
              ? *get<shared_ptr<dpp::guild_member>>(context.args[0].value)
              : context.message.member;
          user user           = has_arg_user ? *find_user(member.user_id) : context.message.author;

          // User Info
          long create_at = static_cast<long>(user.get_creation_time());
          auto rich_it   = presence_cache.find(member.user_id);
          presence rich;
          string status = "N/A", activities = "N/A";
          if (rich_it != presence_cache.end()) {
              rich   = rich_it->second;
              status = status_to_string(rich.status());

              activities = string();
              vector<string> activity;
              for (auto in : rich.activities) {
                  activity.push_back(activity_to_string(in) + " " + in.name);
              }
              activities = utils::join(activity, ", ");
          }

          ostringstream user_info;
          user_info << "- **ID:** " << user.id << "\n";
          user_info << "- **Username:** " << user.username << "\n";
          user_info << "- **Created:** <t:" << create_at << ":F> (<t:" << create_at << ":R>)"
                    << "\n";
          user_info << "- **Is Bot:** " << (user.is_bot() ? "Yes" : "No") << "\n";
          user_info << "- **Activity:** " << activities;

          // Member Info
          long join_at                   = static_cast<long>(member.joined_at);
          string nickname                = member.get_nickname();
          vector<snowflake> member_roles = member.get_roles();
          string roles;

          role *highest_role = nullptr;
          for (snowflake aui : member_roles) {
              role *rl = find_role(aui);
              if (rl == nullptr) continue;

              if (highest_role == nullptr || rl->position > highest_role->position)
                  highest_role = rl;
              roles += rl->get_mention() + " ";
          }

          ostringstream member_info;
          member_info << "- **Nickname:** " << (nickname.empty() ? "-" : nickname) << "\n";
          member_info << "- **Joined:** <t:" << join_at << ":F> (<t:" << join_at << ":R>)\n";
          member_info << "- **Status:** " << status << "\n";
          member_info << "- **Highest Role:** "
                      << (highest_role != nullptr ? highest_role->get_mention() : "Unknown")
                      << "\n";
          member_info << "- **Roles:** " << roles;

          embed emd =
              embed()
                  .set_title(
                      string(unicode_emoji::pushpin) + " " + user.username + "'s information")
                  .set_color(colors::blue)
                  .set_thumbnail(user.get_avatar_url(128))
                  .add_field(string("👤") + " User info", user_info.str())
                  .add_field(string("👥") + " Member info", member_info.str())
                  .set_timestamp(time(nullptr))
                  .set_footer(
                      embed_footer().set_text("Requested by " + context.message.author.username));
          message msg = message(context.message.channel_id, emd);
          context.event.send(msg);
      } });

Command serverinfo = Command(
    { "serverinfo",
      "Get information about the server",
      Category::UTILITY,
      5,
      { "si" },
      {},
      ContextType::Guild,
      [](const Context &context) {
          guild server     = *find_guild(context.message.guild_id);
          long create_at   = static_cast<long>(server.get_creation_time());
          user *owner_user = find_user(server.owner_id);
          string owner     = owner_user ? ("<@" + to_string(owner_user->id) + ">") : "Unknown";

          ostringstream server_info;
          server_info << "- **ID:** " << server.id << "\n";
          server_info << "- **Name:** " << server.name << "\n";
          server_info << "- **Description:** "
                      << (server.description.empty() ? "-" : server.description) << "\n";
          server_info << "- **Created:** <t:" << create_at << ":F> (<t:" << create_at << ":R>)\n";
          server_info << "- **Owner:** " << owner << "\n";
          server_info << "- **Region:** ";

          ostringstream security_info;
          vector<string> security_result = security_to_string(server);
          security_info << "- **Verification Level:** " << security_result[0] << "\n";
          security_info << "- **2FA Requirement:** " << security_result[1] << "\n";
          security_info << "- **Content Filter:** " << security_result[2] << "\n";
          security_info << "- **NSFW Level:** " << security_result[3];

          auto members = server.members;
          int human = 0, bot = 0;
          for (auto &[id, member] : members) {
              if (const user *u = find_user(id)) {
                  (u->is_bot() ? bot : human)++;
              }
          }

          string member_info =
              "- **Humans:** " + to_string(human) + "\n- **Bot:** " + to_string(bot);

          string nitro_info = "- **Boost:** " + to_string(server.premium_subscription_count) +
              "\n- **Tier:** " + to_string((int)server.premium_tier);

          auto channels = server.channels;
          int text = 0, voice = 0, thread = 0, category = 0;
          bool run = false;
          for (snowflake ch_id : channels) {
              const channel *ch = find_channel(ch_id);

              if (ch == nullptr) continue;

              if (ch->is_text_channel()) text++;
              else if (ch->is_voice_channel()) {
                  voice++;
                  if (!run) {
                      string reg = ch->rtc_region;
                      server_info << (reg.empty() ? "N/A" : reg);
                      run = !run;
                  }
              } else if (ch->is_news_channel())
                  text++;
              else if (ch->is_group_dm())
                  thread++;
              else if (ch->is_category())
                  category++;
          }

          ostringstream channel_info;
          channel_info << "- **Text:** " << text << "\n";
          channel_info << "- **Voice:** " << voice << "\n";
          channel_info << "- **Thread:** " << thread << "\n";
          channel_info << "- **Category:** " << category;

          string afk_info = "- **AFK Channel:** " +
              (server.afk_channel_id == 0 ? "Disabled" : to_string(server.afk_channel_id)) +
              "\n- **AFK Timeout:** " + afk_timeout_to_string(server.afk_timeout);

          auto emojis = server.emojis;
          int req = 0, anm = 0;
          for (snowflake emoji_id : emojis) {
              if (const emoji *emoji = find_emoji(emoji_id)) {
                  (emoji->is_animated() ? anm : req)++;
              }
          }
          string emoji_info =
              "- **Regular:** " + to_string(req) + "\n- **Animated:** " + to_string(anm);

          string roles = "- **Roles:** " + to_string(server.roles.size());

          embed emb =
              embed()
                  .set_title(string(unicode_emoji::castle) + " " + server.name + " information")
                  .add_field(string(unicode_emoji::pushpin) + " General", server_info.str())
                  .add_field(string(unicode_emoji::shield) + " Security", security_info.str())
                  .add_field(
                      string("👥") + " Members (" + to_string(server.member_count) + ")",
                      member_info)
                  .add_field(string(unicode_emoji::large_blue_diamond) + " Nitro", nitro_info)
                  .add_field(
                      string(unicode_emoji::book) + " Channels & Categories (" +
                          to_string(server.channels.size()) + ")",
                      channel_info.str())
                  .add_field(string(unicode_emoji::crescent_moon) + " AFK", afk_info)
                  .add_field(
                      string(unicode_emoji::smile) + " Emojis (" + to_string(server.emojis.size()) +
                          ")",
                      emoji_info)
                  .add_field(string(unicode_emoji::performing_arts) + " Roles", roles)
                  .set_color(colors::blue)
                  .set_footer(
                      embed_footer().set_text("Requested by " + context.message.author.username))
                  .set_timestamp(time(nullptr));

          message msg = message(context.message.channel_id, emb);

          context.event.send(msg);
      } });
} // namespace skyo::commands::utility
