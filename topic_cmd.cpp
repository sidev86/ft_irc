#include "ft_irc.hpp"


void topic_command(const std::string& oper_name, const std::string& channel_name, const std::string& new_topic,  std::vector<Channel>& channels)
{
	std::string message;
	std::vector<client_info>::iterator oper_it;
	
	// Trova il canale
	std::vector<Channel>::iterator it = findChannel(channel_name, channels);
	// Trova l'utente nel canale
	oper_it = findUserInChannel(oper_name, it->users);
	if (it == channels.end()) 
	{
		message = "Error: Channel not found!";
		
		//std::cout << message << std::endl;
		//sendToClient(*oper_it, message);
		return;
	}
	std::cout << "Canale Trovato." << std::endl;
	
	
	// Se cè da modificare topic e user non è operator -> permission denied
	if (!new_topic.empty() && !isOperator(oper_name, it->operatorUsers)) 
	{
		message = "Error: User cannot modify topic on this channel. Not an operator";
		//std::cout << message << std::endl;
		sendToClient(*oper_it, message);
		return;
	}
	// visualizzazione topic attuale
	else if (!new_topic.empty() && isOperator(oper_name, it->operatorUsers))
	{
		it->_topic = new_topic;
		message = "Topic of the channel modified. New topic => " + it->_topic;
		std::cout << message << std::endl;
		sendToAll(it->users, message);
	}	
	else
	{
		message = "Topic of the channel => " + it->_topic;
		std::cout << message << std::endl;
		sendToClient(*oper_it, message);
	}
		
}


