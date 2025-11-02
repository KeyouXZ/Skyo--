#include "../include/commands/developer.hpp"
#include "../include/commands/general.hpp"
#include "../include/commands/utility.hpp"
#include "../include/global.hpp"

using namespace skyo::commands;

namespace skyo::registry {
void init() {
    // general
    cmds[general::ping.name] = general::ping;
    cmds[general::help.name] = general::help;

    // utility
    cmds[utility::avatar.name] = utility::avatar;
    cmds[utility::botinfo.name] = utility::botinfo;
    cmds[utility::userinfo.name] = utility::userinfo;
    cmds[utility::serverinfo.name] = utility::serverinfo;

    cmds[developer::pres.name] = developer::pres;

    // register aliases
    for (const auto& [name, cmd] : cmds) {
        for (const auto& alias : cmd.aliases) {
            aliases[alias] = name;
        }
    }
}
}  // namespace skyo::registry
