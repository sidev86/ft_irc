#include "../../header/ft_irc.hpp"


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
	//TODO add a control to check if the topic is changeable
	if (!new_topic.empty() && !isOperator(oper_name, it->operatorUsers)) 
	{
		send_error_message(irc, i, "482", ":You're not channel operator.", irc.client[i].client_sock);
		return;
	}
	// Change the topic
	else if (!new_topic.empty() && isOperator(oper_name, it->operatorUsers))
	{
		it->_topic = new_topic;
		message = "Topic of the channel modified. New topic => " + it->_topic;
		client_message(irc, i, "TOPIC", message);
	}	
	// View the current topic
	else
	{
		message = "Topic of the channel => " + it->_topic;
		client_message(irc, i, "TOPIC", message);
	}
		
}


