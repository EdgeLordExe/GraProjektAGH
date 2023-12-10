#ifndef HPP_CONSOLE
#define HPP_CONSOLE
#include <string>
#include <vector>


class Console{
    public:
    std::vector<std::string> history;

    std::string current_command;

    void Draw();

    void ParseCommand();

    bool toggled = false;


};

#endif
