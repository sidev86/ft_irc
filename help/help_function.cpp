#include "../header/ft_irc.hpp"

void init_poll(ft_irc &irc)
{
    struct pollfd pfd; //creo struct pollfd la inizializzo e la inserico nel vettore
    pfd.fd = irc.server.server_sock; //definisco socket pfd
    pfd.events = POLLIN; //permetto l'ascolto degli eventi
    pfd.revents = 0; //indica che per ora non sono accaduti eventi
    irc.p_fds.push_back(pfd); //inserisco pfd nel vettore
}

void    colored_message(const std::string message, const std::string color)
{
    std::cout << color << message << RESET <<std::endl;
}

bool enough_elements(const std::string &input)
{
    std::istringstream iss(input);
    std::string word;
    int count = 0;

    while (iss >> word)
    {
        if (count == 0 && (word != "NICK" && word != "USER"))
            return (false);
        count++;
    }
    return (count == 2);
}

bool    check_info(ft_irc irc, int i)
{
    return (enough_elements(irc.client[i].nick) && enough_elements(irc.client[i].user));
}

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

    if (!rest_of_command.empty() && rest_of_command[0] == ' ')
        rest_of_command.erase(0, 1);
    if (rest_of_command.empty())
        return ("no");
    if (!rest_of_command.empty() && rest_of_command[0] == ':')
        rest_of_command.erase(0, 1);
    return rest_of_command;
}