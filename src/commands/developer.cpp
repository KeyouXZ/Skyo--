#include "../../include/commands/developer.hpp"

#include <dpp/presence.h>
#include <dpp/snowflake.h>
#include <dpp/user.h>

#include <sstream>

#include "../../include/global.hpp"

using namespace dpp;
using namespace std;

namespace skyo::commands::developer {
Command pres = command::Command(
    {"pres",
     "Asd",
     Category::DEVELOPER,
     0,
     {},
     {{"user", ArgType::USER, true}},
     ContextType::All,
     [](const Context &context) {
         user usr = *get<shared_ptr<user>>(context.args[0].value);
         auto it = presence_cache.find(usr.id);
         if (it != presence_cache.end()) {
             presence rich = it->second;
             ostringstream msg;
             msg << "- **Name:** " << usr.username << "\n";
             msg << "- **ID:** " << rich.user_id << "\n";
             msg << "- **Activity:** ";
             for (auto in : rich.activities) {
                 string ty;
                 switch (in.type) {
                     case at_game:
                         ty = "Playing";
                         break;
                     case at_streaming:
                         ty = "Streaming";
                         break;
                     case at_listening:
                         ty = "Listening to";
                         break;
                     case at_watching:
                         ty = "Watching";
                         break;
                     case at_competing:
                         ty = "Competing at";
                         break;
                     case at_custom:
                         ty = in.name;
                         break;
                     default:
                         ty = "N/A (" + to_string((int)in.type) + ")";
                 }
                 msg << ty << " " << in.name << ", ";
             }
             msg << "\n";
             string status;
             switch (rich.status()) {
                 case ps_offline:
                     status = "Offline";
                     break;
                 case ps_online:
                     status = "Online";
                     break;
                 case ps_dnd:
                     status = "Do not Distrub";
                     break;
                 case ps_idle:
                     status = "Idle";
                     break;
                 case ps_invisible:
                     status = "Invisible";
                     break;
                 default:
                     status = "N/A (" + to_string((int)rich.status()) + ")";
             }
             msg << "- **Status:** " << status;

             context.event.reply(msg.str());
             return;
         } else {
             context.event.reply("Cache not found!");
             return;
         }
     }});
}  // namespace skyo::commands::developer
