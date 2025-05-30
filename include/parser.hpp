#pragma once
#include <dpp/dispatcher.h>
#include <dpp/message.h>

#include <string>
#include <vector>

#include "core/argument.hpp"
#include "core/command.hpp"

using namespace std;

namespace skyo::parser {
string arg_type_to_string(command::ArgType type);
vector<string> tokenize(const string &message);
bool convert_arg(vector<command::Argument> &args, const string &input,
                 command::ArgType type, string &message);
vector<command::Argument> parse_args(const string &message,
                                     const vector<command::Usage> &usage,
                                     dpp::message_create_t event);
string prefix_handler(string message);
}  // namespace skyo::parser
