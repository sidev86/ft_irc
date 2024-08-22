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
		message = ":No such channel";
        	send_error_message(irc, i, "403", message, irc.client[i].client_sock);
        	return;
	}
	
	// Find channel
	std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it == irc.channels.end()) 
	{
		// Channel not exist, so create
		Channel new_channel(channel_name);
		
		// Add channel to the list of channels
		irc.channels.push_back(new_channel);
		
		// Obtain iterator to the new channel
		it = irc.channels.end() - 1; 
		
		// Add user to new channel
		it->addUser(user_name);
		it->addOperatorUser(user_name);

		message = user_name + " has joined the newly created channel " + channel_name;
		client_message(irc, i, "JOIN", message);
	}
	else //Channel already exist
	{
		// If channel is invite-only, only users that received an invite from operator can join
		
		if (it->invite_only == false || (it->invite_only == true && userReceivedInvite(*it, user_name)))  // && user is not in list of invited users (create function)
		{
			// Add user to existing channel
			it->addUser(user_name);
			message = user_name + " has joined the channel " + channel_name;
			//TODO send to all clients in channel
			
		}
		else
		{
			client_message(irc, i, "JOIN", message);
			message = channel_name + " :Cannot join channel (+i)";
			send_error_message(irc, i, "473", message, irc.client[i].client_sock);
			return;	
		}
		
	}

	// Send notification to client
	message = "Welcome to the channel " + channel_name;
	client_message(irc, i, "JOIN", message);
	
		
}
