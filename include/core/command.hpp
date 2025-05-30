#pragma once
#include <string>
#include <vector>

#include "argument.hpp"
#include "context.hpp"

using namespace std;

namespace skyo::command {
struct Usage {
    string name;
    command::ArgType type;
    bool required = true;
    bool remainder = false;
};

enum Category { GENERAL, ECONOMY, FUN, UTILITY, DEVELOPER };

enum ContextType { Guild, DM, All };

struct Command {
    string name;
    string description;
    Category category;
    unsigned int cooldown;
    vector<string> aliases;
    vector<Usage> usage;
    ContextType context;
    function<void(const command::Context &)> callback;
};

}  // namespace skyo::command
