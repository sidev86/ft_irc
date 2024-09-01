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
        if (it != irc.channels.end())
        {
            std::stringstream num_users_ss;
            num_users_ss << it->_num_users;
            message = irc.client[i].server + " 322 " + irc.client[i].nick + " " + it->_name + " " + num_users_ss.str() + " :" + it->_topic;
        }
        else
            message = irc.client[i].server + " 401 " + irc.client[i].nick + " " + single_channel + " :No such channel";
        client_message(irc, i, "", message);
    }
}


void	list_command(ft_irc& irc, int i)
{
    std::string message = irc.client[i].server + " 321 " + irc.client[i].nick + " Channel :Users Name";
    client_message(irc, i, "", message);
    if (irc.channels.empty())
    {
        message = irc.client[i].server + " 323 " + irc.client[i].nick + " :End of /LIST";
        client_message(irc, i, "", message);
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
            message = irc.client[i].server + " " + "322" + " " + irc.client[i].nick + " " + it->_name + " " + ss.str() + " : " + it->_topic;
            client_message(irc, i, "", message);
        }
    }
    message = irc.client[i].server + " 323 " + irc.client[i].nick + " :End of /LIST";
    client_message(irc, i, "", message);
}