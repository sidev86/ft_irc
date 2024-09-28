#include "../header/ft_irc.hpp"

void    exit_c(ft_irc irc, int i, std::vector<Channel>::iterator& ch_iter, std::string nick)
{
    size_t t = 0;
    for (t = 0; t < ch_iter->users.size(); t++)
    client_message_in_channel(irc, *ch_iter, i, (int)t, "PART", ch_iter->_name);
    ch_iter->removeUser(nick);
	ch_iter->removeInvited(nick);
	for (std::vector<client_info>::iterator it = ch_iter->operatorUsers.begin(); it != ch_iter->operatorUsers.end(); ++it)
	{
		if (it->nick == nick)
		{
			ch_iter->operatorUsers.erase(it);
			break;
		}
	}
	if (ch_iter->_num_users <= 0)
		ch_iter = irc.channels.erase(ch_iter);
	else if (ch_iter->operatorCount() == 0)
	{
		ch_iter->next_operator();
		std::string message = ch_iter->_name + " +o " + ch_iter->operatorUsers[0].nick;
			for (t = 0; t < ch_iter->users.size(); t++)
		client_message_in_channel(irc, *ch_iter, i, (int)t, "PART", message);
		update_channel_list(irc, *ch_iter);
	}
}

void remove_user_from_channels(ft_irc& irc, int i)
{
    for (std::vector<Channel>::iterator it = irc.channels.begin(); it != irc.channels.end();)
    {
        it->DeleteUserFromChannel(irc, i);
        if (it->_num_users <= 0)
		    it = irc.channels.erase(it);
		else if (it->operatorCount() == 0)
			it->next_operator();
        else
        {
            exit_c(irc, i, it, irc.client[i].nick);
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

void quitting_channels(ft_irc& irc, int i)
{
    std::string message = irc.client[i].nick + " has quit (Read error :Connection reset by peer)";
	notify_quit(irc, i, message);
    remove_user_from_channels(irc, i);
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