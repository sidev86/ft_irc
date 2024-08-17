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
	std::string message; 
	
	if (!check_channel_name(channel_name))
	{
		std::string message = ":No such channel";
        	send_error_message(irc, i, "403", message, irc.client[i].client_sock);
        	return;
	}
	
	// Find channel
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it == irc.channels.end()) 
	{
		// Channel not exist, so create
		Channel new_channel(channel_name);
		irc.channels.push_back(new_channel);
		it = irc.channels.end() - 1; // Ottieni l'iteratore al nuovo canale
		// Add user to new channel
		it->addUser(user_name);

		message = user_name + " has joined the newly created channel " + channel_name;
		client_message(irc, i, "JOIN", message);
		
		//std::cout << "Created new channel: " << channel_name << std::endl;
		//std::cout << user_name << " has joined the newly created channel " << channel_name << std::endl;
	}
	else
	{
		// Aggiungi l'utente al canale esistente
		it->addUser(user_name);
		std::cout << user_name << " has joined the channel " << channel_name << std::endl;
	}

	// Send notification to client
	message = "Welcome to the channel " + channel_name;
	client_message(irc, i, "JOIN", message);
	
	// TODO check type of channel. if is invite-only, only users that received an invite from operator can join
	
	// TODO if user was banned on this channel cannot join again
	
	
		
}
