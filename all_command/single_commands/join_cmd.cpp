#include "../../header/ft_irc.hpp"

const int MAX_CHANNELS_PER_USER = 3;
bool check_channel_name(const std::string& channel_name)
{
    if (channel_name.empty() || channel_name.size() < 4)
        return false;
    else
	{
		if (channel_name[0] != '#' && channel_name[0] != '&' && channel_name[0] != '+')
			return false;
		for (unsigned int i = 1; i < channel_name.size(); i++)
		{
			if (channel_name[i] == ',' || channel_name[i] == ':')
				return false;
			else if (channel_name[i] == '\\')
			{
				if (channel_name[i + 1] == 'n' || channel_name[i + 1] == 'r' || channel_name[i + 1] == '0')
					return false;
			}
		}
        return true;
	}
}

int	join_to_channel(ft_irc& irc, Channel& channel, const std::string& nick, int i)
{
	if (!userAlreadyInChannel(channel, nick))
	{
		irc.msg = nick + " :" + channel._name;
		return (1);
	}
	else
	{
		irc.msg = " " + channel._name + " :You are already on channel";
		send_error_message(irc, i, "443", irc.msg, irc.client[i].client_sock);
		return (0);
	}
}

void	reply_to_channel(ft_irc& irc, int i, Channel &channel_name)
{
	std::string message;
	update_channel_list(irc, channel_name);
	std::string num = "332";
	if (channel_name._topic.empty())
	{
		num = "331";
		message =channel_name._name + " :No topic is set";
	}
	else
		message =channel_name._name + " :" + channel_name._topic;
	send_error_message(irc, i, num, message, irc.client[i].client_sock);
}

int try_join(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, std::vector<Channel>::iterator it, const std::string& key)
{
	if (it->users_limit && it->_num_users >= it->_max_users)
	{
		irc.msg = channel_name + " :Cannot join channel (+l)";
		send_error_message(irc, i, "471", irc.msg, irc.client[i].client_sock);
		return (0);	
	}
	else if((it->users_limit && it->_num_users < it->_max_users) || !it->users_limit)
	{
		if (!join_to_channel(irc, *it, nick, i))
			return (0);
	}
	if (it->has_key) 
	{
		if (!key.empty() && key == it->_key)
		{
			if (!join_to_channel(irc, *it, nick, i))
				return (0);
		}
		else
		{
			irc.msg = channel_name + " :Cannot join channel (+k)";
			send_error_message(irc, i, "475", irc.msg, irc.client[i].client_sock);
			return (0);	
		}
	}
	else
	{
		if (!join_to_channel(irc, *it, nick, i))
			return (0);
	}
	return (1);
}

int join_on_existing_channel(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, std::vector<Channel>::iterator it, const std::string& key)
{
	std::vector<client_info>::iterator user_it = findUserInChannel(nick, it->users);
	if (user_it != it->users.end())
	{
		irc.msg =" " + it->_name + " :You are already on channel";
		send_error_message(irc, i, "443", irc.msg, irc.client[i].client_sock);
		return (0);
	}	
	if (!it->invite_only || (it->invite_only && userReceivedInvite(*it, nick)))
	{
		if (!try_join(irc, i, channel_name, nick, it, key))
			return (0);
	}
	else
	{
		irc.msg = channel_name + " :Cannot join channel (+i)";
		send_error_message(irc, i, "473", irc.msg, irc.client[i].client_sock);
		return (0);	
	}
	return (1);
}

void	create_channel(ft_irc &irc, int i, std::string channel_name, std::string nick, std::vector<Channel>::iterator &it)
{
	Channel new_channel(channel_name);
	irc.channels.push_back(new_channel);
	it = irc.channels.end() - 1;
	it->addUser(irc, i);
	it->addOperatorUser(nick, irc.client[i].nick, irc.client[i].client_sock);
	it->have_op = true;
	it->has_key = false;
}

void join_command(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, const std::string& key)
{
    int user_channels = 0;
    unsigned long int t;
    
    for (std::vector<Channel>::iterator it = irc.channels.begin(); it != irc.channels.end(); ++it)
    {
        if (userAlreadyInChannel(*it, nick))
            user_channels++;
    }
    if (user_channels >= MAX_CHANNELS_PER_USER)
    {
        irc.msg =" " + channel_name + " :You have joined too many channels";
		send_error_message(irc, i, "405", irc.msg, irc.client[i].client_sock);
        return;
    }
	if (!check_channel_name(channel_name))
	{
		irc.msg = ":No such channel";
		send_error_message(irc, i, "403", irc.msg, irc.client[i].client_sock);
		return;
	}
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it == irc.channels.end())
	{
		if (!key.empty())
		{
			send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
			return;
		}
		create_channel(irc, i, channel_name, nick, it);
	}
	else
	{
		if (!key.empty() && it->has_key == false)
		{
			send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
			return;
		}
		if (!join_on_existing_channel(irc, i, channel_name, nick, it, key))
			return;
	}
	if (!userAlreadyInChannel(*it, nick))
		it->addUser(irc, i);
	
	irc.msg = channel_name;
	for (t = 0; t < it->users.size(); t++)
		client_message_in_channel(irc, *it, i, (int)t, "JOIN", irc.msg);
	reply_to_channel(irc, i, *it);
	irc.msg = "";
}
