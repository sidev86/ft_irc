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
        while (it != irc.channels.end())
        {
            it->DeleteUserFromChannel(irc, i);

            if (it->_num_users <= 0 || it->operatorCount() <= 0)
                it = irc.channels.erase(it);
            else
                ++it;
        }
    }
    std::string secondCmd = second_command(irc);
    if (secondCmd.empty())
        client_message(irc, i, "QUIT", "");

    close(irc.client[i].client_sock);

    for (std::vector<struct pollfd>::iterator it = irc.p_fds.begin(); it != irc.p_fds.end(); ++it)
    {
        if (it->fd == irc.client[i].client_sock)
        {
            it = irc.p_fds.erase(it);
            break;
        }
    }
    
    irc.client.erase(irc.client.begin() + i);
}