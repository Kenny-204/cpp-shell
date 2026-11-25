#include <cstdlib>
#include <iostream>
#include <vector>
#include <filesystem>
#include "cd.h"
#include "execute.h"
#include "inputParser.h"
#include "redirections.h"
#include "pipes.h"

using namespace std;

string getPromptPath() {
    error_code ec;
    filesystem::path path = filesystem::current_path(ec);
    if (ec) return "~";
    return path.filename().empty() ? path.string() : path.filename().string();
}

void showHelp() {
    cout << "===== Shell Help =====\n"
         << "  cd [dir]        Change directory (or use HOME when omitted)\n"
         << "  cd ~            Change to the HOME directory\n"
         << "  help            Display this help message\n"
         << "  checkbg         Check background process status\n"
         << "  <cmd> &         Run a command in the background\n"
         << "  <cmd> | <cmd>   Pipe commands\n"
         << "  <cmd> > file    Redirect output (overwrite)\n"
         << "  <cmd> >> file   Redirect output (append)\n"
         << "  exit            Exit the shell\n"
         << "======================\n";
}

string handleCommand(vector<string> input) {
    if (input.empty()) return "";

    if (input[0] == "cd") {
        if (input.size() == 1) changeDir("~");
        else if (input.size() == 2) changeDir(input[1]);
        else cerr << "cd: too many arguments\n";
        return "";
    }
    if (input[0] == "checkbg") {
        checkBackgroundProcesses();
        return "";
    }
    if (input[0] == "help") {
        showHelp();
        return "";
    }
    if (input[0] == "exit") {
        cout << "BYE...\n";
        return "__CPP_SHELL_EXIT__";
    }
    if (handleRedirections(input)) return "";
    if (!input.empty() && input.back() == "&") return executeCommand(input, true);
    return handlePipes(input);
}

int main() {
    while (true) {
        cout << "[~ / " << getPromptPath() << "]$ " << flush;

        string input;
        if (!getline(cin, input)) {
            cout << "\nBYE...\n";
            break;
        }

        vector<string> parsedInput = parser(input);
        if (parsedInput.empty() && !input.empty()) {
            cerr << "cpp-shell: unterminated quote\n";
            continue;
        }

        string output = handleCommand(parsedInput);
        if (output == "__CPP_SHELL_EXIT__") break;
        if (!output.empty()) cout << output;
    }
    return 0;
}
