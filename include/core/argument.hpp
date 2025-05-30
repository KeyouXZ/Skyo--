#pragma once
#include <dpp/channel.h>
#include <dpp/guild.h>
#include <dpp/user.h>

#include <memory>

using namespace std;

namespace skyo::command {
using ArgumentValue = variant<string,              // string
                              int,                 // int
                              unsigned int,        // uint
                              double,              // double
                              float,               // float
                              long,                // long
                              char,                // char
                              short,               // short
                              unsigned char,       // byte
                              long long,           // long long
                              unsigned long,       // unsigned long
                              unsigned long long,  // unsigned long long
                              unsigned short,      // unsigned short
                              nullptr_t,  // void (represented as nullptr)
                              bool,       // boolean
                              shared_ptr<dpp::user>,         // user
                              shared_ptr<dpp::channel>,      // channel
                              shared_ptr<dpp::guild_member>  // guild member
                              >;

enum class ArgType {
    STRING,
    INT,
    UINT,
    DOUBLE,
    FLOAT,
    LONG,
    CHAR,
    SHORT,
    BYTE,
    LONG_LONG,
    UNSIGNED_LONG,
    UNSIGNED_LONG_LONG,
    UNSIGNED_CHAR,
    UNSIGNED_SHORT,
    VOID_TYPE,
    BOOLEAN,
    USER,
    CHANNEL,
    GUILD_MEMBER
};

struct Argument {
    ArgumentValue value;
    ArgType type;

    // Constructor helpers
    Argument(string val) : value(std::move(val)), type(ArgType::STRING) {}
    Argument(int val) : value(val), type(ArgType::INT) {}
    Argument(unsigned int val) : value(val), type(ArgType::UINT) {}
    Argument(double val) : value(val), type(ArgType::DOUBLE) {}
    Argument(float val) : value(val), type(ArgType::FLOAT) {}
    Argument(long val) : value(val), type(ArgType::LONG) {}
    Argument(char val) : value(val), type(ArgType::CHAR) {}
    Argument(short val) : value(val), type(ArgType::SHORT) {}
    Argument(unsigned char val) : value(val), type(ArgType::BYTE) {}
    Argument(long long val) : value(val), type(ArgType::LONG_LONG) {}
    Argument(unsigned long val) : value(val), type(ArgType::UNSIGNED_LONG) {}
    Argument(unsigned long long val)
        : value(val), type(ArgType::UNSIGNED_LONG_LONG) {}
    Argument(unsigned short val) : value(val), type(ArgType::UNSIGNED_SHORT) {}
    Argument(nullptr_t) : value(nullptr), type(ArgType::VOID_TYPE) {}
    Argument(bool val) : value(val), type(ArgType::BOOLEAN) {}
    Argument(shared_ptr<dpp::user> val)
        : value(std::move(val)), type(ArgType::USER) {}
    Argument(shared_ptr<dpp::channel> val)
        : value(std::move(val)), type(ArgType::CHANNEL) {}
    Argument(shared_ptr<dpp::guild_member> val)
        : value(std::move(val)), type(ArgType::GUILD_MEMBER) {}

    string getTypeString() const {
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
                return "unknown";
        }
    }

    template <typename T>
    T get() const {
        return get<T>(value);
    }

    template <typename T>
    bool is() const {
        return holds_alternative<T>(value);
    }
};

}  // namespace skyo::command
