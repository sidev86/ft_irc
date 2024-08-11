#include "../header/ft_irc.hpp"


	
void kick_command(const std::string& oper_name, const std::string& channel_name, const std::string& user_name, std::vector<Channel>& channels) 
{
	std::cout << "Kick function" << std::endl;
	std::cout << oper_name << " " << channel_name << " " << user_name << std::endl;
	std::string message;
	// Trova il canale
	std::vector<Channel>::iterator it = findChannel(channel_name, channels);
	if (it == channels.end()) 
	{
		std::cout << "Error: Channel not found!" << std::endl;
		return;
	}
	std::cout << "Canale Trovato." << std::endl;

	// Trova l'utente nel canale
	std::vector<client_info>::iterator user_it = findUserInChannel(user_name, it->users);
	if (user_it == it->users.end()) 
	{
		std::cout << "Error: User not found!" << std::endl;
		return;
	}
	std::cout << "Utente Trovato." << std::endl;

	// Controlla se l'operatore è un operatore del canale
	if (!isOperator(oper_name, it->operatorUsers)) 
	{
		std::cout << "Error: User cannot execute kick on this channel. Not an operator" << std::endl;
		return;
	}
	std::cout << "Operator Trovato." << std::endl;

	// Rimuovi l'utente dal canale
	it->removeUser(user_name);
	std::cout << "Utente " << user_name << " rimosso dal canale " << it->_name << "." << std::endl;

	// Invia messaggio di notifica a tutti gli utenti del canale
	message = "User " + user_name + " has been kicked from channel.";
	sendToAll(it->users, message);
}
