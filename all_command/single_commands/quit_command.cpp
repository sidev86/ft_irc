#include "../../header/ft_irc.hpp"


void remove_client_from_channels(ft_irc& irc, int i, std::string& quit_message)
{
	std::string message;
	
	quit_message = quit_message;
	i = i;
	for (std::vector<Channel>::iterator it = irc.channels.begin(); it != irc.channels.end(); it++)
	{
		it->DeleteUserFromChannel(irc, i);
		if (it->_num_users == 0)
		{
			std::cout << "no more users" << std::endl;
			it = irc.channels.erase(it);
			break;
		}
		
		if (it->operatorCount() == 0)
		{
			it->next_operator();
			message = it->_name + " +o " + it->operatorUsers[0].nick;
			for (size_t t = 0; t < it->users.size(); t++)
				client_message_in_channel(irc, *it, i, (int)t, "MODE", message);
		}
		for (size_t t = 0; t < it->users.size(); t++)
		{
			client_message_in_channel(irc, *it, i, (int)t, "QUIT", quit_message);
			reply_to_channel(irc, t, *it);
		}
		
	}
}


void quit_command(ft_irc &irc, int i, const std::string& comment)
{
	std::string message;
	std::cout << "**QUIT command**" << std::endl;
	if (!comment.empty())
		message = irc.client[i].nick + " has quit: " + comment;
	else
		message = irc.client[i].nick + " has quit: Client exited";
	
	
	remove_client_from_channels(irc, i, message);

	close(irc.client[i].client_sock);
	for (std::vector<struct pollfd>::iterator it = irc.p_fds.begin(); it != irc.p_fds.end(); ++it)
	{
		if (it->fd == irc.client[i].client_sock)
		{
		    it = irc.p_fds.erase(it);
		    break;
		}
	}
	irc.client.erase(irc.client.begin() + i);
}
