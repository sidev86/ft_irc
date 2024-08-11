#include "../../header/ft_irc.hpp"
//controllare con piu client

void quit_command(ft_irc &irc, int i)
{
    if (second_command(irc) == "no")
        client_message(irc, i, "QUIT", "");
    else
        client_message(irc, i, "QUIT", second_command(irc));
    close(irc.client[i].client_sock);
    //ciclo struttura poolfd
    for (std::vector<struct pollfd>::iterator it = irc.p_fds.begin(); it != irc.p_fds.end(); ++it)
    {
        //controllo se pfd e uguale a quello del client sock e in caso affermativo lo cancello
        if (it->fd == irc.client[i].client_sock)
        {
            irc.p_fds.erase(it);
            break;
        }
    }
    //cancello il client all iesima posizione
    irc.client.erase(irc.client.begin() + i);
    //aggiorno i pfd in base ai client rimanenti
    for (std::size_t j = i; j < irc.client.size(); ++j)
        irc.p_fds[j].fd = irc.client[j].client_sock;
}
