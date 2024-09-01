#include "../../header/ft_irc.hpp"

/*           ERR_NEEDMOREPARAMS              RPL_CHANNELMODEIS
           ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_KEYSET
           RPL_BANLIST                     RPL_ENDOFBANLIST
           ERR_UNKNOWNMODE                 ERR_NOSUCHCHANNEL
           ERR_USERSDONTMATCH              RPL_UMODEIS
           ERR_UMODEUNKNOWNFLAG
*/
int valid_option(const std::string& option)
{
	if (option[0] == '-' || option[0]  == '+')
	{
		if (option[1] == 'i' || option[1] == 'o' || option[1] == 'k' || option[1] == 't' || option[1] == 'l')
			return 1;
	}
	return 0;
}


void set_invite_mode(const std::string& option, Channel& channel)
{
	if (option[0] == '+')
		channel.invite_only = true;
	else
		channel.invite_only = false;
}

void set_topic_mode(const std::string& option, Channel& channel)
{
	if (option[0] == '+')
		channel.topic_all_users = false;
	else
		channel.topic_all_users = true;
}

void set_users_limit_mode(const std::string& option, Channel& channel, const std::string& option_param)
{
	int num_users;
	
	if (option[0] == '-')
	{
		channel.users_limit = false;
		channel._max_users = -1;
	}
	else if (option[0] == '+')
	{
		channel.users_limit = true;
		std::stringstream(option_param) >> num_users;
		
		//TODO check if the number is valid
		
		channel._max_users = num_users;
	}
}

void set_key_mode(const std::string& option, Channel& channel, const std::string& key)
{
	if (option[0] == '-')
	{
		channel.has_key = false;
		channel._key = "";
	}
	else if (option[0] == '+')
	{
		if (!key.empty())
		{
			channel.has_key = true;
			channel._key = key;
		}
	}
}


int set_operator_mode(const std::string& option, Channel& channel, const std::string user)
{
	std::vector<client_info>::iterator user_it; 
	
	if (option[0] == '+')
	{
		user_it = findUserInChannel(user, channel.users);
		if (user_it == channel.users.end()) 
			return 0;
		channel.operatorUsers.push_back(*user_it); 
	}
	else if (option[0] == '-')
	{
		user_it = findUserInChannel(user, channel.operatorUsers);
		if (user_it == channel.operatorUsers.end()) 
			return 0;
		channel.operatorUsers.erase(user_it);	
	}
	return 1;
}

void mode_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string option, const std::string& option_param)
{
	std::string message;
	
	
	// Find the channel
	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	if (ch_iter == irc.channels.end()) 
	{
		message = ":No such nick/channel";
		send_error_message(irc, i, "401", message, irc.client[i].client_sock);
		return;
	}
	
	// Control if who sended cmd is a channel operator
	if (!isOperator(oper_name, ch_iter->operatorUsers)) 
	{
		message =  ":You're not channel operator.";
		send_error_message(irc, i, "482", message, irc.client[i].client_sock);
		return;
	}
	
	if (!valid_option(option))
	{
		message = option + " :is unknown mode char to me";
		send_error_message(irc, i, "472", message, irc.client[i].client_sock);
		return;
	}
	
	if (option[1] == 'i')
		set_invite_mode(option, *ch_iter);
	else if (option[1] == 't')
		set_topic_mode(option, *ch_iter);
	else if (option[1] == 'l')
		set_users_limit_mode(option, *ch_iter, option_param);
	else if (option[1] == 'k')
		set_key_mode(option, *ch_iter, option_param);
	else if (option[1] == 'o')
		if (!set_operator_mode(option, *ch_iter, option_param))
			send_error_message(irc, i, "401", ":No such nick.", irc.client[i].client_sock);
}
