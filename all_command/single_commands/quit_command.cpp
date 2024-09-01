#include "../../header/ft_irc.hpp"
/*con canali manda il messaggio a tutti i canali
e poi lo rimuove dai canali a cui fa parte
esempio: (:nick!user@host QUIT :Quit: Gone fishing)
implemntare rimuovi canale se user sono finiti
se un operatore di un canale con piu utenti esce il canale si elimina
*/

void quit_command(ft_irc &irc, int i)
{
    if (!irc.channels.empty())
    {
        std::vector<Channel>::iterator it = irc.channels.begin();
        it->DeleteUserFromChannel(irc, i);
    }
    std::string secondCmd = second_command(irc);
    if (secondCmd.empty())
        client_message(irc, i, "QUIT", "");
    else
    {
        std::string message = trim(secondCmd);
        client_message(irc, i, "QUIT", message);
    }
    close(irc.client[i].client_sock);
    for (std::vector<struct pollfd>::iterator it = irc.p_fds.begin(); it != irc.p_fds.end(); ++it)
    {
        if (it->fd == irc.client[i].client_sock)
        {
            irc.p_fds.erase(it);
            break;
        }
    }
    irc.client.erase(irc.client.begin() + i);
    for (std::size_t j = i; j < irc.client.size(); ++j)
        irc.p_fds[j].fd = irc.client[j].client_sock;
}

