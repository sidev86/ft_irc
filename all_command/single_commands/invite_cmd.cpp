#include "../../header/ft_irc.hpp"

int nick_exist(std::vector<client_info>& clients, const std::string& nickname)
{
	for (std::vector<client_info>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->nick == nickname)
			return 1;
	}
	return 0;
}


int get_user_index(std::vector<client_info>& clients, const std::string& nickname)
{
	int i = 0;
	for (std::vector<client_info>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->nick == nickname)
			return i;
		i++;
	}
	return -1;
}

void invite_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& nick_name)
{
	std::string message;
	
	// Find the channel
	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	if (ch_iter == irc.channels.end()) 
	{
		message = ":No such nick/channel";
		send_error_message(irc, i, "401", message, irc.client[i].client_sock);
		return;
	}
	
	// Control if who sended cmd is a channel operator
	if (!isOperator(oper_name, ch_iter->operatorUsers)) 
	{
		message =  ":You're not channel operator.";
		send_error_message(irc, i, "482", message, irc.client[i].client_sock);
		return;
	}
	
	// Check if nick exist in server
	if (!nick_exist(irc.client, nick_name))
	{
		message = ":No such nick/channel";
		send_error_message(irc, i, "401", message, irc.client[i].client_sock);
		return;
	}
	
	// Check if user is already in channel
	std::vector<client_info>::iterator user_it = findUserInChannel(nick_name, ch_iter->users);
	if (user_it != ch_iter->users.end()) 
	{
		message = ":is already on channel";
		send_error_message(irc, i, "443", message, irc.client[i].client_sock);
		return;
	}
	
	
	// Get the index of the user invited and send a message to him
	i = get_user_index(irc.client, nick_name);
	if (i != -1)
	{
		ch_iter->invitedUsers.push_back(irc.client[i]);
		message =  oper_name + " invited you to join " + channel_name + " channel.";
		client_message(irc, i, "INVITE", message);
	}

}
