#include "redirections.h"
#include "execute.h"
#include "pipes.h"

#include <iostream>

bool handleRedirections(vector<string> input) {
    if (input.empty()) return false;

    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == ">" || input[i] == ">>") {
            if (i + 1 >= input.size()) {
                cerr << "cpp-shell: missing redirection filename\n";
                return true;
            }
            if (i == 0) {
                cerr << "cpp-shell: missing command for redirection\n";
                return true;
            }

            vector<string> command(input.begin(), input.begin() + i);
            string output = handlePipes(command);

            ios_base::openmode mode = ios::out;
            if (input[i] == ">>") mode |= ios::app;
            ofstream file(input[i + 1], mode);
            if (!file) {
                cerr << "cpp-shell: cannot open " << input[i + 1] << "\n";
                return true;
            }
            file << output;
            if (!file) cerr << "cpp-shell: error writing " << input[i + 1] << "\n";
            return true;
        }
    }
    return false;
}
