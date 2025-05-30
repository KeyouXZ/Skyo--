#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "../core/command.hpp"

using namespace std;

namespace skyo::utils {
string join(const vector<string> &vec, const string &separator);
string format_usage(const command::Command &command);
int levenshtein(const string &s1, const string &s2);
string find_closest_command(
    const unordered_map<string, command::Command> &commands,
    const string &input);
}  // namespace skyo::utils
