#include "../../header/ft_irc.hpp"

void quit_command(ft_irc &irc, int i, const std::string& comment)
{
    std::string message;
    unsigned long int t;
    if (!irc.channels.empty())
    {
        std::vector<Channel>::iterator it = irc.channels.begin();
        while (it != irc.channels.end())
        {
			it->DeleteUserFromChannel(irc, i);
			if (it->_num_users <= 0)
				it = irc.channels.erase(it);
			else if (it->operatorCount() == 0 && it->_num_users > 0)
			{
				it->next_operator();
				message = it->_name + " +o " + it->operatorUsers[0].nick;
				for (t = 0; t < it->users.size(); t++)
				{
					irc.client[t].quit_received = false;
					if (!irc.client[t].quit_received == false)
					{
						irc.client[t].quit_received = true;
						if (comment.empty())
							message = ":Client Quit";
						else
							message = ":" + comment;
						if (!it->operatorUsers.empty() && !it->users.empty())
							client_message_all_users(irc, i, (int)t, "QUIT", message);
					}
					std::cout << "saaaaaas" << std::endl;
					client_message_in_channel(irc, *it, i, (int)t, "MODE", message);
					std::cout << "fine saaas" << std::endl;
				}
				std::cout << "update\n";
				update_channel_list(irc, *it);
			}
			else
				++it;
        }
    }
    std::string secondCmd = second_command(irc);
    if (secondCmd.empty())
		client_message(irc, i, "QUIT", "");
    close(irc.client[i].client_sock);
    for (std::vector<struct pollfd>::iterator it = irc.p_fds.begin(); it != irc.p_fds.end(); ++it)
    {
        if (it->fd == irc.client[i].client_sock)
        {
            it = irc.p_fds.erase(it);
            break;
        }
    }
    irc.client.erase(irc.client.begin() + i);
}
