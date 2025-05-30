#include <dpp/cache.h>
#include <dpp/colors.h>
#include <dpp/guild.h>
#include <dpp/message.h>
#include <dpp/presence.h>
#include <dpp/snowflake.h>
#include <dpp/unicode_emoji.h>
#include <dpp/user.h>

#include <chrono>
#include <ctime>
#include <memory>
#include <sstream>
#include <vector>

#include "../../include/core/command.hpp"
#include "../../include/global.hpp"
#include "../../include/utils/helper.hpp"

using namespace std;
using namespace dpp;
using namespace skyo::command;

namespace skyo::commands::utility {
Command avatar = Command(
    {"avatar",
     "Get the avatar of a user",
     Category::UTILITY,
     5,
     {},
     {{"user", ArgType::USER, false}},
     ContextType::All,
     [](const Context &context) {
         user usr;
         embed msg_embed;

         usr = context.args.size() == 1
                   ? usr = *get<shared_ptr<user>>(context.args[0].value)
                   : context.message.author;

         msg_embed = embed()
                         .set_title(usr.username + "'s avatar")
                         .set_image(usr.get_avatar_url(4096))
                         .set_footer(embed_footer().set_text(
                             "Requested by " + context.message.author.username))
                         .set_timestamp(time(nullptr))
                         .set_color(rand() % 0xFFFFFF);

         message msg = message(context.message.channel_id, msg_embed);
         msg.add_component(
             component().add_component(component()
                                           .set_label("Download")
                                           .set_type(cot_button)
                                           .set_style(cos_link)
                                           .set_url(usr.get_avatar_url(4096))
                                           .set_id("down")));

         context.event.send(msg);
     }});

Command botinfo = Command(
    {"botinfo",
     "Shows information about the bot",
     Category::UTILITY,
     5,
     {"bi"},
     {},
     ContextType::All,
     [](const Context &context) {
         user self = bot->me;

         // information
         long create_at = static_cast<long>(floor(self.get_creation_time()));

         auto now = chrono::system_clock::now();
         chrono::duration<long> uptime =
             chrono::duration_cast<chrono::seconds>(now - start_time);

         ostringstream uptime_format;
         int days = uptime.count() / 86400;
         int hours = (uptime.count() % 86400) / 3600;
         int minutes = (uptime.count() % 3600) / 60;
         int seconds = uptime.count() % 60;
         if (days > 0) uptime_format << days << "d ";
         if (hours > 0) uptime_format << hours << "h ";
         if (minutes > 0) uptime_format << minutes << "m ";
         uptime_format << seconds << "s ";

         vector<string> dev_names;
         for (auto id : cfg.config.developers) {
             user *usr = find_user(id);
             if (usr) {
                 dev_names.push_back(usr->global_name);
             } else {
                 dev_names.push_back("Unknown");
             }
         }

         ostringstream info;
         info << "- **ID:** " << self.id << "\n";
         info << "- **Username:** " << self.username << "\n";
         info << "- **Tag:** " << self.discriminator << "\n";
         info << "- **Created:** <t:" << create_at << ":F> (<t:" << create_at
              << ":R>)\n";
         info << "- **Developer:** " << utils::join(dev_names, ", ") << "\n";
         info << "- **Prefixes:** " << utils::join(cfg.config.prefix, " ")
              << "\n";
         info << "- **Uptime:** " << uptime_format.str() << "\n";
         info << "- **Guilds:** " << get_guild_count() << "\n";
         info << "- **Users:** " << get_user_count() << "\n";
         info << "- **Channels:** " << get_channel_count();

         embed emd =
             embed()
                 .set_title(string(unicode_emoji::robot) + " " +
                            (!self.global_name.empty() ? self.global_name
                                                       : self.username) +
                            "'s information")
                 .add_field(string("🔍") + " Bot info", info.str())
                 .set_color(colors::blue)
                 .set_thumbnail(self.get_avatar_url(128))
                 .set_footer(embed_footer().set_text(
                     "Requested by " + context.message.author.username))
                 .set_timestamp(time(nullptr));

         context.event.send(message(context.message.channel_id, emd));
     }});

Command userinfo = Command(
    {"userinfo",
     "Get information about a user",
     Category::UTILITY,
     5,
     {"ui"},
     {{"user", ArgType::GUILD_MEMBER, false}},
     ContextType::Guild,
     [](const Context &context) {
         bool has_arg_user = context.args.size() == 1;
         guild_member member =
             has_arg_user
                 ? *get<shared_ptr<dpp::guild_member>>(context.args[0].value)
                 : context.message.member;
         user user =
             has_arg_user ? *find_user(member.user_id) : context.message.author;

         // User Info
         long create_at = static_cast<long>(user.get_creation_time());

         ostringstream user_info;
         user_info << "- **ID:** " << user.id << "\n";
         user_info << "- **Username:** " << user.username << "\n";
         user_info << "- **Created:** <t:" << create_at
                   << ":F> (<t:" << create_at << ":R>)" << "\n";
         user_info << "- **Is Bot:** " << (user.is_bot() ? "Yes" : "No")
                   << "\n";
         user_info << "- **Activity:** " << "N/A";

         // Member Info
         long join_at = static_cast<long>(member.joined_at);
         string nickname = member.get_nickname();
         vector<snowflake> member_roles = member.get_roles();
         string roles;

         role *highest_role = nullptr;
         for (snowflake aui : member_roles) {
             role *rl = find_role(aui);
             if (rl == nullptr) continue;
             if (highest_role == nullptr ||
                 rl->position > highest_role->position)
                 highest_role = rl;
             roles += rl->get_mention() + " ";
         }

         ostringstream member_info;
         string status = "N/A";
         /*switch (user_presence.status()) {
             case ps_offline:
                 status = "Offline";
             case ps_online:
                 status = "Online";
             case ps_dnd:
                 status = "Don't not Distrub";
             case ps_idle:
                 status = "Idle";
             case ps_invisible:
                 status = "Invisible";
             default:
                 status = "N/A";
         }*/
         member_info << "- **Nickname:** "
                     << (nickname.empty() ? "-" : nickname) << "\n";
         member_info << "- **Joined:** <t:" << join_at << ":F> (<t:" << join_at
                     << ":R>)\n";
         member_info << "- **Status:** " << status << "\n";
         member_info << "- **Highest Role:** "
                     << (highest_role != nullptr ? highest_role->get_mention()
                                                 : "Unknown")
                     << "\n";
         member_info << "- **Roles:** " << roles;

         embed emd =
             embed()
                 .set_title(string(unicode_emoji::pushpin) + " " +
                            user.username + "'s information")
                 .set_color(colors::blue)
                 .set_thumbnail(user.get_avatar_url(128))
                 .add_field(string("👤") + " User info", user_info.str())
                 .add_field(string("👥") + " Member info", member_info.str())
                 .set_timestamp(time(nullptr))
                 .set_footer(embed_footer().set_text(
                     "Requested by " + context.message.author.username));
         message msg = message(context.message.channel_id, emd);
         context.event.send(msg);
     }});

Command serverinfo = Command(
    {"serverinfo",
     "Get information about the server",
     Category::UTILITY,
     5,
     {"si"},
     {},
     ContextType::Guild,
     [](const Context &context) {
         guild server = *find_guild(context.message.guild_id);
         long create_at = static_cast<long>(server.get_creation_time());
         string owner = find_user(server.owner_id)->username;

         ostringstream server_info;
         server_info << "- **ID:** " << server.id << "\n";
         server_info << "- **Name:** " << server.name << "\n";
         server_info << "- **Description:** "
                     << (server.description.empty() ? "-" : server.description)
                     << "\n";
         server_info << "- **Created:** <t:" << create_at
                     << ":F> (<t:" << create_at << ":R>)\n";
         server_info << "- **Owner:** " << (owner.empty() ? "Unknown" : owner)
                     << "\n";
         server_info << "- **Region:** ";

         ostringstream security_info;
         string ver_level;
         switch (server.verification_level) {
             case ver_low:
                 ver_level = "Low";
             case ver_medium:
                 ver_level = "Medium";
             case ver_high:
                 ver_level = "High";
             case ver_very_high:
                 ver_level = "Very High";
             default:
                 ver_level = "None";
         }
         string mfa_level = mfa_elevated ? "Elevated" : "None";

         string cnt_filter;
         switch (server.explicit_content_filter) {
             case expl_all_members:
                 cnt_filter = "All Members";
             case expl_members_without_roles:
                 cnt_filter = "Members without roles";
             default:
                 cnt_filter = "Disabled";
         }
         string nsfw_level;
         switch (server.nsfw_level) {
             case nsfw_age_restricted:
                 nsfw_level = "Age Restricted";
             case nsfw_explicit:
                 nsfw_level = "Explicit";
             case nsfw_safe:
                 nsfw_level = "Safe";
             default:
                 nsfw_level = "Default";
         }
         security_info << "- **Verification Level:** " << ver_level << "\n";
         security_info << "- **2FA Requirement:** " << mfa_level << "\n";
         security_info << "- **Content Filter:** " << cnt_filter << "\n";
         security_info << "- **NSFW Level:** " << nsfw_level;

         ostringstream member_info;
         auto members = server.members;
         int human = 0, bot = 0;
         for (auto &member : members) {
             user user = *find_user(member.first);
             user.is_bot() ? bot++ : human++;
         }

         member_info << "- **Humans:** " << human << "\n";
         member_info << "- **Bot:** " << bot;

         ostringstream nitro_info;
         nitro_info << "- **Boost:** " << server.premium_subscription_count
                    << "\n";
         nitro_info << "- **Tier:** " << (int)server.premium_tier;

         ostringstream channel_info;
         auto channels = server.channels;
         int text = 0, voice = 0, thread = 0, category = 0;
         bool run = false;
         for (snowflake ch_id : channels) {
             channel *ch = find_channel(ch_id);
             if (ch->is_text_channel())
                 text++;
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
         channel_info << "- **Text:** " << text << "\n";
         channel_info << "- **Voice:** " << voice << "\n";
         channel_info << "- **Thread:** " << thread << "\n";
         channel_info << "- **Category:** " << category;

         ostringstream afk_info;
         string afk_timeout;
         switch (server.afk_timeout) {
             case afk_off:
                 afk_timeout = "Disabled";
             case afk_60:
                 afk_timeout = "1 Minute";
             case afk_300:
                 afk_timeout = "5 Minutes";
             case afk_900:
                 afk_timeout = "15 Minutes";
             case afk_1800:
                 afk_timeout = "30 Minutes";
             case afk_3600:
                 afk_timeout = "1 Hour";
         }
         afk_info << "- **AFK Channel:** "
                  << (server.afk_channel_id =
                          0 ? "Disabled" : to_string(server.afk_channel_id))
                  << "\n";
         afk_info << "- **AFK Timeout:** " << afk_timeout;

         ostringstream emoji_info;
         auto emojis = server.emojis;
         int req = 0, anm = 0;
         for (snowflake emoji_id : emojis) {
             emoji *emoji = find_emoji(emoji_id);
             if (emoji->is_animated())
                 anm++;
             else
                 req++;
         }
         emoji_info << "- **Regular:** " << req << "\n";
         emoji_info << "- **Animated:** " << anm;

         ostringstream roles;
         roles << "- **Roles:** " << server.roles.size();

         embed emb =
             embed()
                 .set_title(string(unicode_emoji::castle) + " " + server.name +
                            " information")
                 .add_field(string(unicode_emoji::pushpin) + " General",
                            server_info.str())
                 .add_field(string(unicode_emoji::shield) + " Security",
                            security_info.str())
                 .add_field(string("👥") + " Members (" +
                                to_string(server.member_count) + ")",
                            member_info.str())
                 .add_field(
                     string(unicode_emoji::large_blue_diamond) + " Nitro",
                     nitro_info.str())
                 .add_field(string(unicode_emoji::book) +
                                " Channels & Categories (" +
                                to_string(server.channels.size()) + ")",
                            channel_info.str())
                 .add_field(string(unicode_emoji::crescent_moon) + " AFK",
                            afk_info.str())
                 .add_field(string(unicode_emoji::smile) + " Emojis (" +
                                to_string(server.emojis.size()) + ")",
                            emoji_info.str())
                 .add_field(string(unicode_emoji::performing_arts) + " Roles",
                            roles.str())
                 .set_color(colors::blue)
                 .set_footer(embed_footer().set_text(
                     "Requested by " + context.message.author.username))
                 .set_timestamp(time(nullptr));

         message msg = message(context.message.channel_id, emb);

         context.event.send(msg);
     }});
}  // namespace skyo::commands::utility
