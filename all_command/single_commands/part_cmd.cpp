#include "../../header/ft_irc.hpp"

void part_command(ft_irc& irc, int i, const std::string& nick, const std::string& channel_name)
{
	std::string message;
	long unsigned int t;
	
	std::cout << nick << " PART COMMAND " << channel_name << std::endl;
	//irc = irc;
	//i = i;
	
	// Check if channel exists
	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	if (ch_iter == irc.channels.end()) 
	{
		send_error_message(irc, i, "403", ":No such channel.", irc.client[i].client_sock);
		return;
	}
	
	// Find the user in channel
	std::vector<client_info>::iterator user_it = findUserInChannel(nick, ch_iter->users);
	if (user_it == ch_iter->users.end()) 
	{
		send_error_message(irc, i, "441", ":They're not on that channel.", irc.client[i].client_sock);
		return;
	}
	// If channel exists send a message to all clients of the channel that the user leaved channel
	message =  channel_name;
	if (extract_message(second_command(irc)) != "")
		message += " :" + extract_message(second_command(irc));
	for (t = 0; t < ch_iter->users.size(); t++)
		client_message_all_users(irc, i, (int)t, "PART", message);
	
	// Remove user from list of user and in case from operator users if user is an operator
	ch_iter->removeUser(nick);
	ch_iter->removeInvited(nick);
	for (std::vector<client_info>::iterator it = ch_iter->operatorUsers.begin(); it != ch_iter->operatorUsers.end(); ++it)
    {
        if (it->nick == nick)
        {
            ch_iter->operatorUsers.erase(it);
            break;
        }
    }
	if (ch_iter->_num_users <= 0 || ch_iter->operatorCount() <= 0)
		ch_iter = irc.channels.erase(ch_iter);
}
