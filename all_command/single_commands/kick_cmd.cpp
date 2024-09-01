#include "../../header/ft_irc.hpp"

/*
ERR_CHANOPRIVSNEEDED
JOIN #Saas
:s2!s@s JOIN Welcome to the channel #Saas
KICK #Saas samu12 : gay
:s 461 s2 :Not enough parameters.
KICK #Saas samu12 :gay
:s 441 s2 :They're not on that channel.
il commento puo essere ance piu lungo di una parola
*/
	
void kick_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& user_name) 
{	
	//std::cout << oper_name << " " << channel_name << " " << user_name << std::endl;
	std::string message;
	unsigned long int t;
	
	// Find the channel
	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	if (ch_iter == irc.channels.end()) 
	{
		send_error_message(irc, i, "403", ":No such channel.", irc.client[i].client_sock);
		return;
	}
	
	//Channel found
	
	// Find the user in channel
	std::vector<client_info>::iterator user_it = findUserInChannel(user_name, ch_iter->users);
	if (user_it == ch_iter->users.end()) 
	{
		send_error_message(irc, i, "441", ":They're not on that channel.", irc.client[i].client_sock);
		return;
	}
	//User found

	// Control if who sended cmd is a channel operator
	if (!isOperator(oper_name, ch_iter->operatorUsers)) 
	{
		send_error_message(irc, i, "482", ":You're not channel operator.", irc.client[i].client_sock);
		return;
	}

	// If channel exists send a message to all clients of the channel that the user leaved channel
	message = "User " + user_name + " has been kicked from channel.";
	for (t = 0; t < ch_iter->users.size(); t++)
		client_message(irc, t, "KICK", message);
	
	// Remove user from list of user and in case from operator users if user is an operator
	ch_iter->removeUser(user_name);
	ch_iter->removeOperator(user_name);
}