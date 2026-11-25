#include "inputParser.h"

using namespace std;

vector<string> parser(string s) {
    vector<string> inputParsed;
    string word;
    bool quoted = false;
    char quote = '\0';

    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (quoted) {
            if (c == quote) {
                quoted = false;
            } else {
                word.push_back(c);
            }
        } else if (c == '\'' || c == '"') {
            quoted = true;
            quote = c;
        } else if (isspace(static_cast<unsigned char>(c))) {
            if (!word.empty()) {
                inputParsed.push_back(word);
                word.clear();
            }
        } else {
            word.push_back(c);
        }
    }

    if (quoted) {
        return {};
    }
    if (!word.empty()) inputParsed.push_back(word);
    return inputParsed;
}
