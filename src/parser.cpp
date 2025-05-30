#include "../include/parser.hpp"

#include <dpp/cache.h>
#include <dpp/channel.h>
#include <dpp/guild.h>
#include <dpp/snowflake.h>

#include <regex>

#include "../include/core/argument.hpp"
#include "../include/global.hpp"

using namespace std;
using namespace dpp;
using namespace skyo::command;

namespace skyo::parser {
string arg_type_to_string(ArgType type) {
    switch (type) {
        case ArgType::STRING:
            return "string";
        case ArgType::INT:
            return "int";
        case ArgType::UINT:
            return "uint";
        case ArgType::DOUBLE:
            return "double";
        case ArgType::FLOAT:
            return "float";
        case ArgType::LONG:
            return "long";
        case ArgType::CHAR:
            return "char";
        case ArgType::SHORT:
            return "short";
        case ArgType::BYTE:
            return "byte";
        case ArgType::LONG_LONG:
            return "long long";
        case ArgType::UNSIGNED_LONG:
            return "unsigned long";
        case ArgType::UNSIGNED_LONG_LONG:
            return "unsigned long long";
        case ArgType::UNSIGNED_CHAR:
            return "unsigned char";
        case ArgType::UNSIGNED_SHORT:
            return "unsigned short";
        case ArgType::VOID_TYPE:
            return "void";
        case ArgType::BOOLEAN:
            return "boolean";
        case ArgType::USER:
            return "user";
        case ArgType::CHANNEL:
            return "channel";
        case ArgType::GUILD_MEMBER:
            return "guild member";
        default:
            return "unknown(" + std::to_string(static_cast<int>(type)) + ")";
    }
}

vector<string> tokenize(const string &message) {
    vector<string> args;
    regex regex(R"delim("([^"]*)"|(\S+))delim");

    smatch match;
    string::const_iterator searchStart = message.cbegin();

    while (regex_search(searchStart, message.cend(), match, regex)) {
        if (match[1].matched) {
            args.push_back(match[1].str());  // quoted string
        } else {
            args.push_back(match[2].str());  // unquoted word
        }
        searchStart = match.suffix().first;
    }

    return args;
}

bool convert_arg(vector<Argument> &args, const string &input, ArgType type,
                 string &message, const snowflake &guild_id) {
    try {
        const regex channel_tag(R"(^<#(\d+)>$)");
        const regex user_tag(R"(^<@(\d+)>$)");
        const regex c_u_tag(R"(^(\d+)$)");
        smatch match;

        switch (type) {
            case ArgType::INT:
                args.push_back(Argument(stoi(input)));
                return true;
            case ArgType::UINT:
                args.push_back(
                    Argument(static_cast<unsigned int>(stoul(input))));
                return true;
            case ArgType::DOUBLE:
                args.push_back(Argument(stod(input)));
                return true;
            case ArgType::FLOAT:
                args.push_back(Argument(stof(input)));
                return true;
            case ArgType::LONG:
                args.push_back(Argument(stol(input)));
                return true;
            case ArgType::LONG_LONG:
                args.push_back(Argument(stoll(input)));
                return true;
            case ArgType::UNSIGNED_LONG:
                args.push_back(Argument(stoul(input)));
                return true;
            case ArgType::UNSIGNED_LONG_LONG:
                args.push_back(Argument(stoull(input)));
                return true;
            case ArgType::CHAR:
                args.push_back(Argument(input.empty() ? '\0' : input[0]));
                return true;
            case ArgType::BOOLEAN: {
                string lowered;
                transform(input.begin(), input.end(), back_inserter(lowered),
                          ::tolower);

                if (lowered == "true" || lowered == "1" || lowered == "yes" ||
                    lowered == "on") {
                    args.push_back(Argument(true));
                    return true;
                } else if (lowered == "false" || lowered == "0" ||
                           lowered == "no" || lowered == "off") {
                    args.push_back(Argument(false));
                    return true;
                } else {
                    message = "Invalid boolean input: " + input;
                    return false;
                }
                return false;
            }
            case ArgType::STRING:
                args.push_back(Argument(input));
                return true;
            case ArgType::CHANNEL:
                if (regex_match(input, match, channel_tag) ||
                    regex_match(input, match, c_u_tag)) {
                    channel *ch = find_channel(stoull(match[1].str()));
                    if (ch == nullptr) {
                        message = "Channel not found!";
                        return false;
                    }
                    args.push_back(Argument(make_shared<channel>(*ch)));
                    return true;
                } else {
                    return false;
                }
            case ArgType::USER:
                if (regex_match(input, match, user_tag) ||
                    regex_match(input, match, c_u_tag)) {
                    user *usr = find_user(stoull(match[1].str()));
                    if (usr == nullptr) {
                        message = "User not found!";
                        return false;
                    }
                    args.push_back(Argument(make_shared<user>(*usr)));
                    return true;
                } else {
                    return false;
                }
            case ArgType::GUILD_MEMBER:
                if (regex_match(input, match, user_tag) ||
                    regex_match(input, match, c_u_tag)) {
                    guild_member member =
                        find_guild_member(guild_id, stoull(match[1].str()));
                    if (member.user_id.empty()) {
                        message = "Member not found!";
                        return false;
                    }
                    args.push_back(Argument(make_shared<guild_member>(member)));
                    return true;
                } else {
                    return false;
                }
            default:
                message = "ArgType not supported: " + arg_type_to_string(type);
                return false;
        }
    } catch (...) {
        message = "Failed to parsing \"" + input + "\" as " +
                  arg_type_to_string(type);
        return false;
    }
}

vector<Argument> parse_args(const string &message, const vector<Usage> &usage,
                            message_create_t event) {
    vector<Argument> args;
    vector<string> token;

    token = tokenize(message);

    // Handle Remainder
    bool has_remainder = !usage.empty() && usage.back().remainder;
    if (has_remainder && token.size() - 1 > usage.size()) {
        size_t remainder_index = usage.size() - 1;
        string remainder;

        for (size_t i = remainder_index; i < token.size() - 1; ++i) {
            if (i > remainder_index) remainder += " ";
            remainder += token[i + 1];
        }

        token.erase(token.begin() + 1 + remainder_index, token.end());
        token.push_back(remainder);
    }

    /* cout << "Token: ";
    for (string tkn : token) {
        cout << "[ " + tkn + " ] ";
    }
    cout << endl;*/

    // Too many arguments
    if (token.size() - 1 > usage.size()) {
        event.reply("Too many arguments provided. Expected: " +
                    to_string(usage.size()));
        return {};
    }

    // Count required arguments
    size_t required_count = 0;
    for (const auto &u : usage) {
        if (u.required) required_count++;
    }

    // Too few arguments
    if (token.size() - 1 < required_count) {
        event.reply("Too few parameters provided. Expected at least " +
                    to_string(required_count) + ".");
        return {};
    }

    // Type Handler
    if (token.size() > 1) {
        args.push_back(Argument(token[0]));

        size_t loop_count = std::min(token.size() - 1, usage.size());
        for (size_t i = 0; i < loop_count; ++i) {
            string msg;
            bool success = convert_arg(args, token[i + 1], usage[i].type, msg,
                                       event.msg.guild_id);

            if (!success && msg.empty()) {
                event.reply("Type mismatch for parameter " + to_string(i + 1) +
                            ". Expected " + arg_type_to_string(usage[i].type));
                return {};
            } else if (!msg.empty()) {
                event.reply(msg);
                return {};
            }
        }
    } else {
        args.push_back(Argument(token[0]));
    }

    return args;
}

string prefix_handler(string message) {
    for (const auto &prefix : cfg.config.prefix) {
        if (message.find(prefix) == 0) {
            return prefix;
        }
    }
    return string();
}

}  // namespace skyo::parser
