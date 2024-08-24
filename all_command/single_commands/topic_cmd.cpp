#include "../../header/ft_irc.hpp"

void set_view_topic(ft_irc& irc, int i, Channel& channel, const std::string new_topic)
{
	std::string message;
	if (!new_topic.empty())
	{
		channel._topic = new_topic;
		message = "Topic of the channel modified. New topic => " + channel._topic;
		client_message(irc, i, "TOPIC", message);
	}
	else
	{
		message = "Topic of the channel => " + channel._topic;
		client_message(irc, i, "TOPIC", message);
	}
}


void topic_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& new_topic)
{
	std::string message;
	std::vector<client_info>::iterator oper_it;
	
	// Find channel
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	
	// Find user in channel
	if (it != irc.channels.end())
	{
		oper_it = findUserInChannel(oper_name, it->users);
		if (oper_it == it->users.end())
		{
			irc.msg = channel_name + " :You’re not on that channel";
			send_error_message(irc, i, "442", irc.msg, irc.client[i].client_sock);
			return;
		}
	}		
	else
	{
		send_error_message(irc, i, "403", ":No such channel.", irc.client[i].client_sock);
		return;
	}
	
	//Channel found
	
	// If user is not operator -> cannot change topic
	if (!it->topic_all_users)
	{
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
		set_view_topic(irc, i, *it, new_topic);	
	}	
		
}


