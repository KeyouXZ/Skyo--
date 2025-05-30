#include <dpp/channel.h>

#include <unordered_map>
#include <vector>

#include "../../include/core/command.hpp"

using namespace std;

namespace skyo::utils {
string join(const vector<string> &vec, const string &separator) {
    if (vec.empty()) return "";

    // Estimate final size and reserve
    size_t total_size = (vec.size() - 1) * separator.size();
    for (const auto &s : vec) total_size += s.size();

    string result;
    result.reserve(total_size);

    // Build the result
    result += vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += separator;
        result += vec[i];
    }

    return result;
}

string format_usage(const command::Command &command) {
    if (!command.usage.empty()) {
        string result = command.name;

        for (const auto &arg : command.usage) {
            result += " ";
            result +=
                arg.required ? ("<" + arg.name + ">") : ("[" + arg.name + "]");
        }

        return result;
    } else {
        return command.name;
    }
}

int levenshtein(const string &s1, const string &s2) {
    const size_t m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    for (size_t i = 0; i <= m; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] =
                    std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]}) +
                    1;
        }
    }

    return dp[m][n];
}

string find_closest_command(
    const unordered_map<string, command::Command> &commands,
    const string &input) {
    vector<string> all_names;

    // Collect command names and aliases
    for (const auto &[name, cmd] : commands) {
        all_names.push_back(name);
        all_names.insert(all_names.end(), cmd.aliases.begin(),
                         cmd.aliases.end());
    }

    // Sort by Levenshtein distance to input
    sort(all_names.begin(), all_names.end(),
         [&](const string &a, const string &b) {
             return levenshtein(a, input) < levenshtein(b, input);
         });

    if (!all_names.empty() && levenshtein(all_names[0], input) <= 1) {
        return all_names[0];
    }

    return "";  // No good suggestion
}
}  // namespace skyo::utils
