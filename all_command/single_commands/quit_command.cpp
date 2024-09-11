#include "../../header/ft_irc.hpp"
/*
implementare messaggio di addi agli utenti con cui ha avuto PRIVMSG
*/

void quit_command(ft_irc &irc, int i, const std::string& comment)
{
    std::string message;
    unsigned long int t;
    
    if (!irc.channels.empty())
    {
    	// Notify all users in same channels of quitting user
    	for (t = 0; t < irc.client.size(); t++)
	{
		for (std::vector<Channel>::iterator ch_it = irc.channels.begin(); ch_it != irc.channels.end(); ch_it++)
		{
			if (findUserInChannel(irc.client[i].nick, ch_it->users) != ch_it->users.end() && findUserInChannel(irc.client[t].nick, ch_it->users) != ch_it->users.end() && i != (int)t)
			{
				if (!irc.client[t].quit_received)
				{
					irc.client[t].quit_received = true;
					message = ":" + comment;
					client_message_all_users(irc, i, (int)t, "QUIT", message);
				}
			}
		}
		irc.client[t].quit_received = false;
	}
        std::vector<Channel>::iterator it = irc.channels.begin();
        while (it != irc.channels.end())
        {
		it->DeleteUserFromChannel(irc, i);

		if (it->_num_users <= 0 || it->operatorCount() <= 0)
			it = irc.channels.erase(it);
		else
		{
		
			++it;
		}
            
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
