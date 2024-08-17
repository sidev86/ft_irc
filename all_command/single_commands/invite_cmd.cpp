#include "../../header/ft_irc.hpp"


void invite_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& user_name)
{
	
	std::cout << oper_name << " " << channel_name << " " << user_name << std::endl;
	std::string message;
	
	// Find the channel
	/*std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
	if (it == irc.channels.end()) 
	{
		//std::cout << "Error: Channel not found!" << std::endl;
		send_error_message(irc, i, "403", ":No such channel.", irc.client[i].client_sock);
		return;
	}
	
	// Control if who sended cmd is a channel operator
	if (!isOperator(oper_name, it->operatorUsers)) 
	{
		send_error_message(irc, i, "482", ":You're not channel operator.", irc.client[i].client_sock);
		return;
	}
	std::cout << "Operator Found." << std::endl;*/
	
	message =  oper_name + " invited you to join " + channel_name + " channel.";
	client_message(irc, i, "INVITE", message);
	

	
	


}
