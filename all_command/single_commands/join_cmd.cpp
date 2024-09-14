#include "../../header/ft_irc.hpp"

/*:NICK!USER@localhost JOIN #SUS
:server 353 NICK = #SUS :@NICK 
:server 366 NICK #SUS :End of /NAMES list
:server 331 NICK #SUS :No topic is set*/

const int MAX_CHANNELS_PER_USER = 3;  // Numero massimo di canali per utente
bool check_channel_name(const std::string& channel_name)
{
    if (channel_name.empty() || channel_name[0] != '#'\
        || channel_name.size() < 4 || channel_name[1] == '.')
        return false;
    else
        return true;
}

int	join_to_channel(ft_irc& irc, Channel& channel, const std::string& nick, int i)
{
	if (!userAlreadyInChannel(channel, nick))
	{
		/*channel.addUser(nick);
		if (userReceivedInvite(channel, nick))
			channel.removeInvited(nick);*/
		irc.msg = nick + " :" + channel._name;
		return (1);
	}
	else
	{
		irc.msg = irc.client[i].nick + " " + channel._name + " :You are already on channel";
		send_error_message(irc, i, "443", irc.msg, irc.client[i].client_sock);
		return (0);
	}
}

void	reply_to_channel(ft_irc& irc, int i, std::vector<Channel>::iterator it)
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
	//unsigned long int t;
	
	//std::cout << "users size = " << it->users.size() << std::endl;
	//for (t = 0; t < it->users.size(); t++)
	send_error_message(irc, i, "353", message, irc.client[i].client_sock);
	message =it->_name + " :End of /NAMES list";
	//for (t = 0; t < it->users.size(); t++)
	send_error_message(irc, i, "366", message, irc.client[i].client_sock);
	std::string num = "332";
	if (it->_topic.empty())
	{
		num = "331";
		message =it->_name + " :No topic is set";
	}
	else
		message =it->_name + " :" + it->_topic;
	send_error_message(irc, i, num, message, irc.client[i].client_sock);
}

int try_join(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, std::vector<Channel>::iterator it, const std::string& key)
{
	if (it->users_limit && it->_num_users >= it->_max_users)
	{
		//client_message(irc, i, "JOIN", message);
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
	/*if (!irc.msg.empty())
	{
		//Send to all clients in channel
		for (t = 0; t < it->users.size(); t++)
				client_message_all_users(irc, i, (int)t, "JOIN", irc.msg);
	}*/
	return (1);
}

int join_on_existing_channel(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, std::vector<Channel>::iterator it, const std::string& key)
{
	std::vector<client_info>::iterator user_it = findUserInChannel(nick, it->users);
	if (user_it != it->users.end())
	{
		irc.msg =irc.client[i].nick + " " + it->_name + " :You are already on channel";
		send_error_message(irc, i, "443", irc.msg, irc.client[i].client_sock);
		return (0);
	}
	// If channel is invite-only, only users that received an invite from operator can join
	
	if (!it->invite_only || (it->invite_only && userReceivedInvite(*it, nick)))
	{
		if (!try_join(irc, i, channel_name, nick, it, key))
			return (0);
	}
	else
	{
		//client_message(irc, i, "JOIN", message);
		irc.msg = channel_name + " :Cannot join channel (+i)";
		send_error_message(irc, i, "473", irc.msg, irc.client[i].client_sock);
		return (0);	
	}
	return (1);
}

void	create_channel(ft_irc &irc, int i, std::string channel_name, std::string nick, std::vector<Channel>::iterator &it)
{
	// Channel not exist, so create
	Channel new_channel(channel_name);
	// Add channel to the list of channels
	irc.channels.push_back(new_channel);
	// Obtain iterator to the new channel
	it = irc.channels.end() - 1;
	// Add user to new channel
	it->addUser(irc, i);
	it->addOperatorUser(nick, irc.client[i].nick);
	it->have_op = true;
	it->has_key = false;
}

void join_command(ft_irc& irc, int i, const std::string& channel_name, const std::string& nick, const std::string& key)
{
	// Verifica se l'utente ha raggiunto il limite massimo di canali
    int user_channels = 0;
    unsigned long int t;
    
    for (std::vector<Channel>::iterator it = irc.channels.begin(); it != irc.channels.end(); ++it)
    {
        if (userAlreadyInChannel(*it, nick))
            user_channels++;
    }
    // Se l'utente ha raggiunto il limite massimo, invia ERR_TOOMANYCHANNELS
    if (user_channels >= MAX_CHANNELS_PER_USER)
    {
        irc.msg =irc.client[i].nick + " " + channel_name + " :You have joined too many channels";
		send_error_message(irc, i, "405", irc.msg, irc.client[i].client_sock);
        return;
    }
	if (!check_channel_name(channel_name))
	{
		irc.msg = ":No such channel";
        	send_error_message(irc, i, "403", irc.msg, irc.client[i].client_sock);
        	return;
	}
	// Trova il canale
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	std::cout << key << std::endl;
	if (it == irc.channels.end())
	{
		if (!key.empty())
		{
			send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
			return;
		}
		create_channel(irc, i, channel_name, nick, it);
	}
	else // Il canale esiste già
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
	/*if (second_command(irc).empty())
		client_message(irc, i, "JOIN", "");
	else
		client_message(irc, i, "JOIN", channel_name);*/
	reply_to_channel(irc, i, it);
	irc.msg = "";
}
