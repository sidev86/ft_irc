#include "../../header/ft_irc.hpp"

void kick_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& nick_name, const std::string& comment) 
{	
	std::string message;
	unsigned long int t;
	
	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	if (ch_iter == irc.channels.end()) 
	{
		send_error_message(irc, i, "403", ch_iter->_name + ":No such channel", irc.client[i].client_sock);
		return;
	}
	std::vector<client_info>::iterator user_it = findUserInChannel(nick_name, ch_iter->users);
	if (user_it == ch_iter->users.end() || ch_iter->isMember(irc.client[i]) == false || findUserInChannel(nick_name, ch_iter->users) == ch_iter->users.end()) 
	{
		send_error_message(irc, i, "441", ch_iter->_name + ":You’re not on that channel", irc.client[i].client_sock);
		return;
	}
	if (oper_name == nick_name)
	{
		send_error_message(irc, i, "482", ch_iter->_name + " :You cannot kick yourself from a channel", irc.client[i].client_sock);
		return ;
	}
	if (!isOperator(oper_name, ch_iter->operatorUsers)) 
	{
		send_error_message(irc, i, "482", ch_iter->_name + ":You’re not channel operator", irc.client[i].client_sock);
		return;
	}
	message = channel_name + " " + nick_name;
	if (!comment.empty())
		message += " :" + comment;
	for (t = 0; t < ch_iter->users.size(); t++)
		client_message_in_channel(irc, *ch_iter, i, (int)t, "KICK", message);
	ch_iter->removeUser(nick_name);
	ch_iter->removeInvited(nick_name);
	if (ch_iter->_num_users <= 0)
		ch_iter = irc.channels.erase(ch_iter);
	else
	{
		ch_iter->next_operator();
		update_channel_list(irc, *ch_iter);
	}
}
