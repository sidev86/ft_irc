#include "../../header/ft_irc.hpp"

void invite_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& nick_name, const std::string& channel_name)
{
	std::string message;

	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	if (ch_iter == irc.channels.end()) 
	{
		message = nick_name + " :No such nick/channel";
		send_error_message(irc, i, "401", message, irc.client[i].client_sock);
		return;
	}
	
	if (ch_iter->isMember(irc.client[i]) == false)
	{
		message = channel_name + " :You’re not on that channel";
		send_error_message(irc, i, "442", message, irc.client[i].client_sock);
		return;
	}
	std::vector<client_info>::iterator user_it = findUserInChannel(nick_name, ch_iter->users);
	if (user_it != ch_iter->users.end() || oper_name == nick_name) 
	{
		message = ":is already on channel";
		send_error_message(irc, i, "443", message, irc.client[i].client_sock);
		return;
	}
	if (!isOperator(oper_name, ch_iter->operatorUsers)) 
	{
		message =  channel_name + " :You’re not channel operator";
		send_error_message(irc, i, "482", message, irc.client[i].client_sock);
		return;
	}

	if (!nick_exist(irc.client, nick_name))
	{
		message = nick_name + " :No such nick/channel";
		send_error_message(irc, i, "401", nick_name + " :No such nick/channel", irc.client[i].client_sock);
		return;
	}
	int i2 = get_user_index(irc.client, nick_name);
	if (i2 != -1)
	{		
		ch_iter->invitedUsers.push_back(irc.client[i2]);
		message = ":" + irc.client[i].nick + "!" + irc.client[i].user + "@" + irc.client[i].host;
		message =  message + " INVITE " + nick_name + " :" + channel_name + "\r\n";
    	send(irc.client[i2].client_sock, message.c_str(), message.length(), 0);
	}
	std::string invite =  nick_name + " " + channel_name + " :Invite successful";
	send_error_message(irc, i, "341", invite, irc.client[i].client_sock);
}
