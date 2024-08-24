#include "../../header/ft_irc.hpp"


bool	check_channel_name(const std::string& channel_name)
{
	 if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
		return false;
	else
		return true;
}


void join_to_channel(ft_irc& irc, Channel& channel, const std::string& user_name)
{
	if (!userAlreadyInChannel(channel, user_name))
	{
		//channel.addUser(user_name);
		irc.msg = user_name + " has joined the channel " + channel._name;
	}
}

void join_command(ft_irc& irc, int i, const std::string& channel_name, const std::string& user_name, const std::string& key)
{
	//std::string message; 
	
	if (!check_channel_name(channel_name))
	{
		irc.msg = ":No such channel";
        	send_error_message(irc, i, "403", irc.msg, irc.client[i].client_sock);
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
		
		it->addOperatorUser(user_name);
		irc.msg = user_name + " has joined the newly created channel " + channel_name;
		client_message(irc, i, "JOIN", irc.msg);
	}
	else //Channel already exist
	{
		std::vector<client_info>::iterator user_it = findUserInChannel(user_name, it->users);
		if (user_it != it->users.end()) 
			return;
		// If channel is invite-only, only users that received an invite from operator can join
		
		if (!it->invite_only || (it->invite_only && userReceivedInvite(*it, user_name)))
		{
			if (it->users_limit && it->_num_users >= it->_max_users)
			{
				//client_message(irc, i, "JOIN", message);
				irc.msg = channel_name + " :Cannot join channel (+l)";
				send_error_message(irc, i, "471", irc.msg, irc.client[i].client_sock);
				return;	
			}
			else if((it->users_limit && it->_num_users < it->_max_users) || !it->users_limit)
			{
				join_to_channel(irc, *it, user_name);
			}
			if (it->has_key) 
			{
				if (!key.empty() && key == it->_key)
				{
					join_to_channel(irc, *it, user_name);
				}
				else
				{
					irc.msg = channel_name + " :Cannot join channel (+k)";
					send_error_message(irc, i, "475", irc.msg, irc.client[i].client_sock);
					return;	
				}
			}
			else
			{
				join_to_channel(irc, *it, user_name);
			}
			
			if (!irc.msg.empty())
				client_message(irc, i, "JOIN", irc.msg);
			//TODO send to all clients in channel
			
		}
		else
		{
			//client_message(irc, i, "JOIN", message);
			irc.msg = channel_name + " :Cannot join channel (+i)";
			send_error_message(irc, i, "473", irc.msg, irc.client[i].client_sock);
			return;	
		}
		
	}

	// Send notification to client
	if (!userAlreadyInChannel(*it, user_name))
	{
		it->addUser(user_name);
		irc.msg = "Welcome to the channel " + channel_name;
		client_message(irc, i, "JOIN", irc.msg);
	}
	irc.msg = "";
	
		
}
