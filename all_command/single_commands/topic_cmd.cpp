#include "../../header/ft_irc.hpp"
void set_view_topic(ft_irc& irc, int i, Channel& channel, const std::string new_topic)
{
	std::string message;
	unsigned long int t;
	
	if (new_topic.empty())
	{
		channel._topic = "";
		message = ":" + irc.client[i].server + " 331 " + irc.client[i].nick + " " + channel._name + " :No topic is set";
		for (t = 0; t < channel.users.size(); t++)
			client_message(irc, t, "TOPIC", message);
		return ;
	}
	if (!new_topic.empty())
		channel._topic = new_topic;
	if (!channel._topic.empty())
	{
		message = ":" + irc.client[i].server + " 332 " + irc.client[i].nick + " " + channel._name + " :" + channel._topic;
		
		if(!new_topic.empty())
		{
			for (t = 0; t < channel.users.size(); t++)
				client_message(irc, t, "TOPIC", message);
		}
		else
			client_message(irc, i, "TOPIC", message);
	}
	else
	{
		message = ":" + irc.client[i].server + " 331 " + irc.client[i].nick + " " + channel._name + " :No topic is set";
		client_message(irc, i, "", message);
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
		oper_it = findUserInChannel(oper_name, it->users);
	else
	{
		send_error_message(irc, i, "403", ":No such channel.", irc.client[i].client_sock);
		return;
	}
	
	//Channel found
	// If user is not operator -> cannot change topic
	message = it->_name + " :You're not on that channel";
	if (!it->topic_all_users)
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
		set_view_topic(irc, i, *it, new_topic);	
	}
}
