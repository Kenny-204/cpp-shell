#include "cd.h"

#include <cstdlib>
#include <cerrno>
#include <cstring>

using namespace std;

void changeDir(string givenPath) {
    if (givenPath.empty() || givenPath == "~") {
        const char* home = getenv("HOME");
        if (!home || *home == '\0') {
            cerr << "cd: HOME is not set\n";
            return;
        }
        givenPath = home;
    }

    filesystem::path newPath(givenPath);
    if (!newPath.is_absolute()) newPath = filesystem::absolute(newPath);

    error_code ec;
    filesystem::current_path(newPath, ec);
    if (ec) cerr << "cd: " << givenPath << ": " << ec.message() << "\n";
}
