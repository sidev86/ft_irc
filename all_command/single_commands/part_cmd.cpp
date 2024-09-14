#include "../../header/ft_irc.hpp"

/*sisteamre la stampa della lista di utenti se un utente esce dal canale
konversationfa roba strana*/
void update_channel_list(ft_irc& irc, int i, Channel& channel_name)
{
	(void)i;
	std::string message;
	std::string users_list;
	
	message =" = " + channel_name._name + " :";
	for (std::vector<client_info>::iterator op_it = channel_name.operatorUsers.begin(); op_it != channel_name.operatorUsers.end(); ++op_it)
		users_list += "@" + op_it->nick + " ";
	std::vector<client_info>::iterator op_it = channel_name.operatorUsers.begin();
	bool is_operator = false;
	for (std::vector<client_info>::iterator user_it = channel_name.users.begin(); user_it != channel_name.users.end(); ++user_it)
	{
		for (;op_it != channel_name.operatorUsers.end(); ++op_it)
		{
			if (user_it->nick == op_it->nick)
			{
				is_operator = true;
				break;
			}
		}
		if (is_operator == true)
		{
			is_operator = false;
			continue;
		}
		users_list += user_it->nick + " ";
	}

	message += users_list;
	unsigned long int t;
	for (t = 0; t < channel_name.users.size(); t++)
		send_error_message(irc, t, "353", message, irc.client[t].client_sock);
	message =channel_name._name + " :End of /NAMES list";
	for (t = 0; t < channel_name.users.size(); t++)
		send_error_message(irc, t, "366", message, irc.client[t].client_sock);
}

void part_command(ft_irc& irc, int i, const std::string& nick, const std::string& channel_name)
{
	std::string message;
	long unsigned int t;
	
	//std::cout << nick << " PART COMMAND " << channel_name << std::endl;
	
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
		client_message_in_channel(irc, *ch_iter, i, (int)t, "PART", message);
	
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
	update_channel_list(irc, i, *ch_iter);
}
