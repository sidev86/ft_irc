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
		channel.topic_limited = true;
	else if (option[0] == '-')
		channel.topic_limited = false;
}

int param_is_numeric(const std::string& param)
{
	int i = 0;
	
	while(param[i] != '\0')
	{
		if (param[i] < '0' || param[i] > '9')
			return 0;
		i++;
	}
	return 1;
}

int set_users_limit_mode(ft_irc& irc, int i, const std::string& option, Channel& channel, const std::string& option_param)
{
	int num_users;
	std::string message;
	
	if (option[0] == '-')
	{
		if (!option_param.empty())
		{
			send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
			return 0;
		}
		else
		{
			channel.users_limit = false;
			channel._max_users = -1;
		}
	}
	else if (option[0] == '+')
	{
		if (option_param.empty())
		{
			send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
			return 0;
		}
		channel.users_limit = true;
		
		//Check if the parameter is numeric
		if (!param_is_numeric(option_param))
		{
			send_error_message(irc, i, "", " :MODE +l: Invalid parameter value.", irc.client[i].client_sock);
			return 0;
		}
		std::stringstream(option_param) >> num_users;
		channel._max_users = num_users;
	}
	return 1;
}

int set_key_mode(const std::string& option, Channel& channel, const std::string& key, ft_irc& irc, int i)
{
	std::string message;
	
	if (option[0] == '-')
	{
		channel.has_key = false;
		channel._key = "";
	}
	else if (option[0] == '+')
	{
		if (channel.has_key == true)
		{
			message = channel._name + " :Channel key already set";
			send_error_message(irc, i, "467", message, irc.client[i].client_sock);
			return 1;
		}
		else if (!key.empty())
		{
			channel.has_key = true;
			channel._key = key;
		}
		else
		{
			send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
			return 1;
		}
	}
	return 0;
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

void show_mode(ft_irc& irc, int i2, const std::string& channel_name)
{
	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	std::string message = channel_name;
	removeChars(ch_iter->flags, '+');
	if (ch_iter->flags[0] != 0)
	{
		ch_iter->flags = "+" + ch_iter->flags;
		message += " " + ch_iter->flags;
		for (unsigned long int i = 1; i < ch_iter->flags.size(); i++)
		{
			if (ch_iter->flags[i] == 'i')
				continue;
			else if (ch_iter->flags[i] == 't')
				continue;
			else if (ch_iter->flags[i] == 'k' && ch_iter->isMember(irc.client[i2]) == true)
				message += " " + ch_iter->_key;
			else if (ch_iter->flags[i] == 'o')
			{
				for (std::vector<std::string>::const_iterator it = ch_iter->flag_o.begin(); it != ch_iter->flag_o.end(); ++it)
				{
					const std::string& op = *it;
					message += " &" + op;
				}
			}
			else if (ch_iter->flags[i] == 'l')
			{
				std::ostringstream oss;
				oss << ch_iter->_max_users;
				std::string user_limit = oss.str();
				message += " " + user_limit;
			}
		}
	}
	send_error_message(irc, i2, "324", message, irc.client[i2].client_sock);
}

void mode_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string option, const std::string& option_param)
{
	std::string message;
	unsigned long int t;
/* 	if (irc.channels.empty())
	{
		message = ":No such channel";
		send_error_message(irc, i, "403", message, irc.client[i].client_sock);
		return;
	} */
	std::vector<Channel>::iterator ch_iter = findChannel(channel_name, irc.channels);
	if (ch_iter == irc.channels.end()) 
	{
		message = ":No such channel";
		send_error_message(irc, i, "403", message, irc.client[i].client_sock);
		return;
	}
	if (option.empty())
	{
		show_mode(irc, i, channel_name);
		return ;
	}
	// Control if the user is on channel. if not -> ERR_NOTONCHANNEL
	
	if (ch_iter->isMember(irc.client[i]) == false)
	{
		message = channel_name + " :They're not on that channel";
		send_error_message(irc, i, "442", message, irc.client[i].client_sock);
		return;
	}

	// Control if who sended cmd is a channel operator
	if (!isOperator(oper_name, ch_iter->operatorUsers)) 
	{
		message =  ":They're not channel operator.";
		send_error_message(irc, i, "482", message, irc.client[i].client_sock);
		return;
	}
	if (!valid_option(option) || option.length() > 2)
	{
		if (option[1] == 'b')
			return;
		message = option + " :is unknown mode char to me";
		send_error_message(irc, i, "472", message, irc.client[i].client_sock);
		return;
	}
	
	if (option[1] == 'i')
		set_invite_mode(option, *ch_iter);
	else if (option[1] == 't')
		set_topic_mode(option, *ch_iter);
	else if (option[1] == 'l')
	{
		if (!set_users_limit_mode(irc, i, option, *ch_iter, option_param))		
			return;	
	}
	else if (option[1] == 'k')
	{
		if (set_key_mode(option, *ch_iter, option_param, irc, i) == 1)
			return;
	}
	else if (option[1] == 'o')
	{
		if (option_param.empty())
		{
			send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
			return;
		}
		else if (!set_operator_mode(option, *ch_iter, option_param))
		{
			send_error_message(irc, i, "401", ":No such nick.", irc.client[i].client_sock);
			return;
		}
		ch_iter->flag_o.push_back(option_param);
	}
	ch_iter->flags = ch_iter->flags + option[1];
 	message = ch_iter->_name + " " + option;
 	if (option[1] == 'o')
 		message += " " + option_param;
	for (t = 0; t < ch_iter->users.size(); t++)
		client_message_in_channel(irc, *ch_iter, i, (int)t, "MODE", message);
}
