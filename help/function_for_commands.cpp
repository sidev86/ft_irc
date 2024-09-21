#include "../header/ft_irc.hpp"

std::string first_command(ft_irc irc)
{
    std::string command;
    std::stringstream ss(irc.buffer);
    ss >> command;
    return (command);
}

std::string second_command(ft_irc irc)
{
    std::string first_word;
    std::stringstream ss(irc.buffer);

    ss >> first_word;

    std::string rest_of_command;
    std::getline(ss, rest_of_command);

    return rest_of_command;
}

std::string user_list(Channel& channel_name)
{
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
	return (message);
}

void update_channel_list(ft_irc& irc, Channel& channel_name)
{
	std::string message = user_list(channel_name);
	std::string end_users = channel_name._name + " :End of /NAMES list";
    for (unsigned int op = 0;op != channel_name.operatorUsers.size(); op++)
    {
        if (findUserInChannel(channel_name.operatorUsers[op].nick, channel_name.operatorUsers) != channel_name.operatorUsers.end())
        {
            send_error_message(irc, op, "353", message, channel_name.operatorUsers[op].client_sock);
            send_error_message(irc, op, "366", end_users, channel_name.operatorUsers[op].client_sock);
        }
    }
	for (unsigned int t = 0; t < channel_name.users.size(); t++)
	{
    	if ((findUserInChannel(channel_name.users[t].nick, channel_name.users) != channel_name.users.end())
        && channel_name.users[t].nick != channel_name.operatorUsers[0].nick)
		{
			send_error_message(irc, t, "353", message, channel_name.users[t].client_sock);
			send_error_message(irc, t, "366", end_users, channel_name.users[t].client_sock);
		}
	}
}
