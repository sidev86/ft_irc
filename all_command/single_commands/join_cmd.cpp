#include "../../header/ft_irc.hpp"


bool	check_channel_name(const std::string& channel_name)
{
	 if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
		return false;
	else
		return true;
}

void join_command(ft_irc& irc, int i, const std::string& channel_name, const std::string& user_name)
{
	//Controlla validità del nome canale
	if (!check_channel_name(channel_name))
	{
		std::string message = ":No such channel";
        	send_error_message(irc, i, "403", message, irc.client[i].client_sock);
        	return;
	}
	
	// Trova canale
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it == irc.channels.end()) 
	{
		// Canale non trovato, quindi crealo
		Channel new_channel(channel_name);
		irc.channels.push_back(new_channel);
		it = irc.channels.end() - 1; // Ottieni l'iteratore al nuovo canale
		// Aggiungi l'utente al nuovo canale
		//client_info& client = irc.client[i];
		it->addUser(user_name);

		std::cout << "Created new channel: " << channel_name << std::endl;
		std::cout << user_name << " has joined the newly created channel " << channel_name << std::endl;
	}
	else
	{
		// Aggiungi l'utente al canale esistente
		//client_info& client = irc.client[i];
		it->addUser(user_name);
		std::cout << user_name << " has joined the channel " << channel_name << std::endl;
	}

	// Invia messaggio di conferma al client
	std::string message = "Welcome to the channel " + channel_name;
	client_message(irc, i, "JOIN", message);
		
}
