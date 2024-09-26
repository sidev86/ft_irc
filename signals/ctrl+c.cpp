#include "../header/ft_irc.hpp"

void remove_user_from_channels(ft_irc& irc, int i)
{
    for (std::vector<Channel>::iterator it = irc.channels.begin(); it != irc.channels.end();)
    {
        it->DeleteUserFromChannel(irc, i);
        if (it->_num_users <= 0)
		    it = irc.channels.erase(it);
		else if (it->operatorCount() <= 0)
			it->next_operator();
        else
        {
			update_channel_list(irc, *it);
            ++it;
        }
    }
}

void notify_quit(ft_irc& irc, int i, const std::string& message)
{
    for (std::vector<Channel>::iterator ch_it = irc.channels.begin(); ch_it != irc.channels.end(); ++ch_it)
    {
        if (findChannel(ch_it->_name, irc.channels) != irc.channels.end())
        {
            for (unsigned int t = 0; t < ch_it->users.size(); t++)
            {
                if ((findUserInChannel(irc.client[i].nick, ch_it->users) != ch_it->users.end() || findUserInChannel(irc.client[t].nick, ch_it->users) != ch_it->users.end()) &&
                ch_it->_num_users > 1)
                    client_message_all_users(irc, i, t, "", message);
            }
        }
    }
}

void exit_from_all_channels(ft_irc& irc, int i)
{
	for (std::vector<Channel>::iterator ch_it = irc.channels.begin(); ch_it != irc.channels.end(); ++ch_it)
    	{
        	if (findChannel(ch_it->_name, irc.channels) != irc.channels.end())
        	{
        		std::cout << "channel name ==>" << ch_it->_name << std::endl;
        		part_command(irc, i, irc.client[i].nick, ch_it->_name);
        		//update_channel_list(irc, *ch_it);
        	}
	}
}


void quitting_channels(ft_irc& irc, int i)
{
    std::cout << "quitting" << std::endl;
    std::string message = irc.client[i].nick + " has quit (Read error :Connection reset by peer)";
	notify_quit(irc, i, message);
	
	exit_from_all_channels(irc, i);	
	
    //remove_user_from_channels(irc, i);
    
    

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
