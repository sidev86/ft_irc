#include "../../header/ft_irc.hpp"

/*il topic puo essere cambiato da tutti lunico limite e +t 
in mode che non lo fa cambiate da tutti ma solo operator*/
void set_view_topic(ft_irc& irc, int i, Channel& channel, const std::string new_topic)
{
	std::string message;
	unsigned long int t;
	
	if (new_topic.empty())
	{
		channel._topic = "";
		message = channel._name + " :No topic is set";
	}
	else
	{
		channel._topic = new_topic;
		message =channel._name + " :" + channel._topic;
	}
	for (t = 0; t < channel.users.size(); t++)
		client_message_in_channel(irc, channel, i, (int)t, "TOPIC", message);
}

void	show_topic(ft_irc& irc, int i, const std::string& channel_name, std::string message, Channel& it)
{
	if (it._topic.empty())
	{
		message = channel_name + " :No topic is set";
		send_error_message(irc, i, "331", message, irc.client[i].client_sock);
	}
	else
	{
		message = channel_name + " :" + it._topic;
		send_error_message(irc, i, "332", message, irc.client[i].client_sock);
	}
}

void topic_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& new_topic)
{
	std::string message;
	std::vector<client_info>::iterator oper_it;
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it != irc.channels.end())
		oper_it = findUserInChannel(oper_name, it->users);
	else
	{
		send_error_message(irc, i, "403", ":No such channel.", irc.client[i].client_sock);
		return;
	}
	if (new_topic.empty() && second_command(irc).find(":") == std::string::npos)
	{
		show_topic(irc, i, channel_name, message,  *it);
		return ; 
	}
	message = it->_name + " :You're not on that channel";
	if (it->topic_limited)
	{
		if (findUserInChannel(oper_name, it->users) == it->users.end())
		{
			send_error_message(irc, i, "442", message, irc.client[i].client_sock);
			return;
		}
		if (!new_topic.empty() && !isOperator(oper_name, it->operatorUsers))
		{
			send_error_message(irc, i, "482", ":You're not channel operator.", irc.client[i].client_sock);
			return;
		}
		else
		{
			set_view_topic(irc, i, *it, new_topic);	
		}
	}
	else
	{
		if (findUserInChannel(oper_name, it->users) == it->users.end())
		{
			send_error_message(irc, i, "442", message, irc.client[i].client_sock);
			return;
		}
		set_view_topic(irc, i, *it, new_topic);	
	}
}
