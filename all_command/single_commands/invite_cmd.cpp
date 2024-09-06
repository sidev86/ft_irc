#include "../../header/ft_irc.hpp"

void invite_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& nick_name, const std::string& channel_name)
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
	
	if (ch_iter->isMember(irc.client[i]) == false)
	{
		message = channel_name + " :they're not on that channel";
		send_error_message(irc, i, "442", message, irc.client[i].client_sock);
		return;
	}
	// Check if user is already in channel
	std::vector<client_info>::iterator user_it = findUserInChannel(nick_name, ch_iter->users);
	if (user_it != ch_iter->users.end() || oper_name == nick_name) 
	{
		message = ":is already on channel";
		send_error_message(irc, i, "443", message, irc.client[i].client_sock);
		return;
	}
	
	// Control if who sended cmd is a channel operator
	if (!isOperator(oper_name, ch_iter->operatorUsers)) 
	{
		message =  ":they're not channel operator.";
		send_error_message(irc, i, "482", message, irc.client[i].client_sock);
		return;
	}
	
	// Check if nick exist in server
	if (!nick_exist(irc.client, nick_name))
	{
		//TOFIX should print the nick who was searching
		message = ":No such nick/channel";
		send_error_message(irc, i, "401", message, irc.client[i].client_sock);
		return;
	}

	// Get the index of the user invited and send a message to him
	int i2 = get_user_index(irc.client, nick_name);
	if (i2 != -1)
	{
		//TODO use RPL_INVITING cmd response to communicate invite to end-client (?)
		
		ch_iter->invitedUsers.push_back(irc.client[i2]);
		message = ":" + irc.client[i].nick + "!" + irc.client[i].user + "@" + irc.client[i].host;
		message =  message + " INVITE " + nick_name + " :" + channel_name;
		message = message + "\r\n";
    	send(irc.client[i2].client_sock, message.c_str(), message.length(), 0);
	}
	std::string invite = irc.client[i2].nick + " " + channel_name;
	send_error_message(irc, i, "341", invite, irc.client[i].client_sock);
}
