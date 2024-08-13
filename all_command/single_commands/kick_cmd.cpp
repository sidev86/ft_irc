#include "../../header/ft_irc.hpp"


	
void kick_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& user_name) 
{
	//std::cout << "Kick function" << std::endl;
	
	//client_message(irc, i, "KICK", "Kick function");
	
	
	std::cout << oper_name << " " << channel_name << " " << user_name << std::endl;
	std::string message;
	
	// Find the channel
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it == irc.channels.end()) 
	{
		//std::cout << "Error: Channel not found!" << std::endl;
		send_error_message(irc, i, "403", ":No such channel.", irc.client[i].client_sock);
		return;
	}
	
	//Channel found
	
	// Find the user in channel
	std::vector<client_info>::iterator user_it = findUserInChannel(user_name, it->users);
	if (user_it == it->users.end()) 
	{
		send_error_message(irc, i, "441", ":They're not on that channel.", irc.client[i].client_sock);
		return;
	}
	//User found

	// Control if who sended cmd is a channel operator
	if (!isOperator(oper_name, it->operatorUsers)) 
	{
		send_error_message(irc, i, "482", ":You're not channel operator.", irc.client[i].client_sock);
		return;
	}
	std::cout << "Operator Trovato." << std::endl;

	// Remove user from channel
	it->removeUser(user_name);
	//std::cout << "Utente " << user_name << " rimosso dal canale " << it->_name << "." << std::endl;

	// Invia messaggio di notifica a tutti gli utenti del canale
	message = "User " + user_name + " has been kicked from channel.";
	client_message(irc, i, "KICK", message);
	sendToAll(it->users, message);
}
