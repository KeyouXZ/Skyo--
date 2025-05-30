#pragma once
#include <dpp/dispatcher.h>
#include <dpp/message.h>

#include <string>

#include "argument.hpp"

using namespace std;

namespace skyo::command {
struct Command;

struct Context {
    shared_ptr<Command> cmd;
    dpp::message message;
    dpp::message_create_t event;
    vector<Argument> args;

    template <typename T>
    optional<T> getArg(size_t index) const {
        if (index >= args.size() || !args[index].is<T>()) {
            return nullopt;
        }
        return args[index].get<T>();
    }

    bool isArgType(size_t index, ArgType type) const {
        return index < args.size() && args[index].type == type;
    }

    bool isArgTypeString(size_t index, const string& typeStr) const {
        return index < args.size() && args[index].getTypeString() == typeStr;
    }
};
}  // namespace skyo::command
