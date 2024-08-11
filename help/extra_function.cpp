#include "../header/ft_irc.hpp"

void init_poll(ft_irc &irc)
{
    struct pollfd pfd; //creo struct pollfd la inizializzo e la inserico nel vettore
    pfd.fd = irc.server.server_sock; //definisco socket pfd
    pfd.events = POLLIN; //permetto l'ascolto degli eventi
    pfd.revents = 0; //indica che per ora non sono accaduti eventi
    irc.p_fds.push_back(pfd); //inserisco pfd nel vettore
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

