#include "ft_irc.hpp"


std::vector<Channel>::iterator findChannel(const std::string& channel_name, std::vector<Channel>& channels) 
{
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) 
    {
        if (it->channelHasName(channel_name)) 
            return it;  // Canale trovato
    }
    return channels.end();  // Canale non trovato
}	
	
	
std::vector<client_info>::iterator findUserInChannel(const std::string& user_name, std::vector<client_info>& users) 
{
    for (std::vector<client_info>::iterator it = users.begin(); it != users.end(); ++it) 
    {
        if (it->clientHasName(user_name))
            return it; 
    }
    return users.end();
}

bool isOperator(const std::string& oper_name, std::vector<client_info>& operatorUsers) 
{
    for (std::vector<client_info>::iterator it = operatorUsers.begin(); it != operatorUsers.end(); ++it) 
    {
        if (it->clientHasName(oper_name)) 
            return true;
    }
    return false;
}

void sendToAll(std::vector<client_info>& users, const std::string& message) 
{
   std::cout << "message to send to all => " << message << std::endl;
   for (std::vector<client_info>::iterator it = users.begin(); it != users.end(); ++it) 
    {
        if (it->authenticated) 
        {
            if (send(it->client_sock, message.c_str(), message.size(), 0) == -1) 
                std::cerr << "Error in send notification message to user with socket fd " << it->client_sock << std::endl;
        }
    }
}

void sendToClient(client_info& client, const std::string& message)
{
	std::cout << "message to send => " << message << std::endl;
	if (client.authenticated) 
        {
            if (send(client.client_sock, message.c_str(), message.size(), 0) == -1) 
                std::cerr << "Error in send notification message to user with socket fd " << client.client_sock << std::endl;
        }
}


