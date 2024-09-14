#include "../../header/ft_irc.hpp"

void multi_list(ft_irc& irc, int i, std::string channel_list)
{
    std::string message;
    std::stringstream ss(channel_list);
    std::string single_channel;

    while (std::getline(ss, single_channel, ','))
    {
        // Rimuovere gli spazi all'inizio e alla fine del nome del canale
        single_channel.erase(0, single_channel.find_first_not_of(' '));
        single_channel.erase(single_channel.find_last_not_of(' ') + 1);

        std::vector<Channel>::iterator it = findChannel(single_channel, irc.channels);
        std::string num = "322";
        if (it != irc.channels.end())
        {
            std::stringstream num_users_ss;
            num_users_ss << it->_num_users;
            message = irc.client[i].nick + " " + it->_name + " " + num_users_ss.str() + " :" + it->_topic;
        }
        else
        {
            num = "403";
            message = irc.client[i].nick + " " + single_channel + " :No such channel";
        }
        send_error_message(irc, i, num, message, irc.client[i].client_sock);
    }
}


void	list_command(ft_irc& irc, int i)
{
    std::string message =irc.client[i].nick + " Channel :Users Name";
    send_error_message(irc, i, "321", message, irc.client[i].client_sock);
    if (irc.channels.empty())
    {
        message = irc.client[i].nick + " :End of /LIST";
        send_error_message(irc, i, "323", message, irc.client[i].client_sock);
        return;
    }
    std::string channel = second_command(irc);
    if (!channel.empty())
    {
        multi_list(irc, i, channel);
    }
    else
    {
        for (std::vector<Channel>::iterator it = irc.channels.begin(); it != irc.channels.end(); ++it) 
        {
            std::stringstream ss;
            ss << it->_num_users;
            message = irc.client[i].nick + " " + it->_name + " " + ss.str() + " : " + it->_topic;
            send_error_message(irc, i, "322", message, irc.client[i].client_sock);
        }
    }
    message = irc.client[i].nick + " :End of /LIST";
    send_error_message(irc, i, "323", message, irc.client[i].client_sock);
}