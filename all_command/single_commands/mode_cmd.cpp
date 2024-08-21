#include "../../header/ft_irc.hpp"


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
		channel.topic_settable = true;
	else
		channel.topic_settable = false;
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
		channel.has_key = true;
		channel._key = key;
	}
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
	
	
	


}
