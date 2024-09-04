#include "../../header/ft_irc.hpp"

bool	check_channel_name(const std::string& channel_name)
{
	 if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
		return false;
	else
		return true;
}

int	join_to_channel(ft_irc& irc, Channel& channel, const std::string& nick, int i)
{
	if (!userAlreadyInChannel(channel, nick))
	{
		//channel.addUser(nick);
		if (userReceivedInvite(channel, nick))
			channel.removeInvited(nick);
		irc.msg = nick + " has joined the channel " + channel._name;
		return (0);
	}
	else
	{
		irc.msg = ":" + irc.client[i].server + " 443 " + irc.client[i].nick + " " + channel._name + ":You are already on channel";
		client_message(irc, i, "", irc.msg);
		return (1);
	}
}

void	reply_to_channel(ft_irc& irc, int i, std::vector<Channel>::iterator it)
{
	std::string message;
	std::string users_list;
	
	message = ":" + irc.client[i].server + " 353 " + irc.client[i].nick  + " = " + it->_name + " :";
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
	client_message(irc, i, "", message);
	message = ":" + irc.client[i].server + " 366 " + irc.client[i].nick + " " + it->_name + " :End of /NAMES list";
	client_message(irc, i, "", message);
	if (it->_topic.empty())
		message = ":" + irc.client[i].server + " 331 " + irc.client[i].nick + " " + it->_name + " :No topic is set";
	else
		message = ":" + irc.client[i].server + " 332 " + irc.client[i].nick + " " + it->_name + " :" + it->_topic;
	client_message(irc, i, "", message);
}

int invite_only(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, std::vector<Channel>::iterator it, const std::string& key)
{
	if (it->users_limit && it->_num_users >= it->_max_users)
	{
		//client_message(irc, i, "JOIN", message);
		irc.msg = channel_name + " :Cannot join channel (+l)";
		send_error_message(irc, i, "471", irc.msg, irc.client[i].client_sock);
		return (1);	
	}
	else if((it->users_limit && it->_num_users < it->_max_users) || !it->users_limit)
	{
		if (join_to_channel(irc, *it, nick, i) == 1)
			return (1);
	}
	if (it->has_key) 
	{
		if (!key.empty() && key == it->_key)
		{
			if (join_to_channel(irc, *it, nick, i) == 1)
				return (1);
		}
		else
		{
			irc.msg = channel_name + " :Cannot join channel (+k)";
			send_error_message(irc, i, "475", irc.msg, irc.client[i].client_sock);
			return (1);	
		}
	}
	else
	{
		if (join_to_channel(irc, *it, nick, i) == 1)
			return (1);
	}
	if (!irc.msg.empty())
		client_message(irc, i, "JOIN", irc.msg);
	//TODO send to all clients in channel
	return (0);
}

int channel_alredy_exist(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, std::vector<Channel>::iterator it, const std::string& key)
{
	std::vector<client_info>::iterator user_it = findUserInChannel(nick, it->users);
	if (user_it != it->users.end())
	{
		irc.msg = ":" + irc.client[i].server + " 443 " + irc.client[i].nick + " " + it->_name + ":You are already on channel";
		client_message(irc, i, "", irc.msg);
		return (1);
	}
	// If channel is invite-only, only users that received an invite from operator can join
	
	if (!it->invite_only || (it->invite_only && userReceivedInvite(*it, nick)))
	{
		if (invite_only(irc, i, channel_name, nick, it, key) == 1)
			return (1);
	}
	else
	{
		//client_message(irc, i, "JOIN", message);
		irc.msg = channel_name + " :Cannot join channel (+i)";
		send_error_message(irc, i, "473", irc.msg, irc.client[i].client_sock);
		return (1);	
	}
	return (0);
}

void	channel_not_exist(ft_irc &irc, int i, std::string channel_name, std::string nick, std::vector<Channel>::iterator &it)
{
	// Channel not exist, so create
	Channel new_channel(channel_name);
	
	// Add channel to the list of channels
	irc.channels.push_back(new_channel);
	
	if (second_command(irc).empty())
		client_message(irc, i, "JOIN", "");
	else
		client_message(irc, i, "JOIN", channel_name);
	// Obtain iterator to the new channel
	it = irc.channels.end() - 1; 
	
	// Add user to new channel
	it->addUser(irc, i);
	it->addOperatorUser(nick, irc.client[i].nick);

}

void join_command(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, const std::string& key)
{

	if (!check_channel_name(channel_name))
	{
		irc.msg = ":No such channel";
        	send_error_message(irc, i, "403", irc.msg, irc.client[i].client_sock);
        	return;
	}
	// Trova il canale
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it == irc.channels.end()) 
		channel_not_exist(irc, i, channel_name, nick, it);
	else // Il canale esiste già
	{
		if (channel_alredy_exist(irc, i, channel_name, nick, it, key) == 1)
			return;
	}
	
	if (!userAlreadyInChannel(*it, nick))
		it->addUser(irc, i);

	irc.msg = "";
	reply_to_channel(irc, i, it);
	std::cout << "\n**CHANNEL USERS**\n" << std::endl;
	for (std::vector<client_info>::iterator u_it = it->users.begin(); u_it != it->users.end(); ++u_it)
	{
		std::cout << u_it->user << std::endl;
	}
    // TODO: Verifica il tipo di canale. Se è "invite-only", solo gli utenti invitati dall'operatore possono unirsi.
}