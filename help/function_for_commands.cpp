#include "../header/ft_irc.hpp"

std::string first_command(ft_irc irc)
{
    std::string command;
    std::stringstream ss(irc.buffer);
    ss >> command;
    return (command);
}

std::string second_command(ft_irc irc)
{
    std::string first_word;
    std::stringstream ss(irc.buffer);

    ss >> first_word;

    std::string rest_of_command;
    std::getline(ss, rest_of_command);

    return rest_of_command;
}