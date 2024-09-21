#include "../header/ft_irc.hpp"

void update_user_list(ft_irc& irc, int i, std::vector<Channel>::iterator it)
{
	std::string message;
	std::string users_list;
	
	message =" = " + it->_name + " :";
	for (std::vector<client_info>::iterator op_it = it->operatorUsers.begin(); op_it != it->operatorUsers.end(); ++op_it)
		users_list += "@" + op_it->nick + " ";
	std::vector<client_info>::iterator op_it = it->operatorUsers.begin();
	bool is_operator = false;
	for (std::vector<client_info>::iterator user_it = it->users.begin(); user_it != it->users.end(); ++user_it)
	{
		for (;op_it != it->operatorUsers.end(); ++op_it)
		{
			if (user_it->nick == op_it->nick)
			{
				is_operator = true;
				break;
			}
		}
		if (is_operator == true)
		{
			is_operator = false;
			continue;
		}
		users_list += user_it->nick + " ";
	}
	message += users_list;
	unsigned long int t;
	for (t = 0; t < it->users.size(); t++)
	{
		if (it->isMember(irc.client[t]) == true || it->isMemberOperator(irc.client[t]) == true)
			send_error_message(irc, i, "353", message, irc.client[t].client_sock);
	}
    message = it->_name + " :End of /NAMES list";
	for (t = 0; t < it->users.size(); t++)
	{
		if (it->isMember(irc.client[t]) == true || it->isMemberOperator(irc.client[t]) == true)
			send_error_message(irc, i, "366", message, irc.client[t].client_sock);
	}
}

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
            update_user_list(irc, i, it);
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