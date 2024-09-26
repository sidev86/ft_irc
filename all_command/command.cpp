#include "../header/ft_irc.hpp"

void    commands(ft_irc &irc, int i)
{
    std::string message = first_command(irc) + " Unknown command";
    std::string target;

    if (first_command(irc) == "KICK" || first_command(irc) == "TOPIC" || first_command(irc) == "INVITE" || first_command(irc) == "MODE")
    	operator_command(irc, i);
    else if (first_command(irc) == "JOIN" || first_command(irc) == "PART" || first_command(irc) == "QUIT")
    	channel_command(irc, i);
    else if (first_command(irc) == "LIST")
    	list_command(irc, i);
    else if (first_command(irc) == "PRIVMSG")
    {
        target = trim(second_command(irc));
        privmsg_command(irc, i, target);
    }
    else
        send_error_message(irc, i, "421", message, irc.client[i].client_sock);
}

void    autentication(ft_irc &irc, int i)
{
    std::string cmd = first_command(irc);
    std::string message =":You have not registered";
    if (cmd == "\0")
        return ;
    if (cmd == "PASS" && irc.client[i].is_pass == false)
        process_pass_command(irc, i);
    else if ((cmd == "PASS" && irc.client[i].is_pass == true) || (cmd == "USER" && irc.client[i].is_user == true))
    {
        send_error_message(irc, i, "462", ":You may not reregister", irc.client[i].client_sock);
        return ;
    }
    else if (cmd == "NICK" && irc.client[i].is_pass == true)
        nick_command(irc, i);
    else if (cmd == "USER" && irc.client[i].is_pass == true)
        user_command(irc, i);
    else
        send_error_message(irc, i, "451", message, irc.client[i].client_sock);
    if (irc.client[i].is_nick == true && irc.client[i].is_pass == true && \
    irc.client[i].is_user == true)
    {
        welcome_msg(irc, i);
        irc.client[i].authenticated = true;
    }
}

int registretion(ft_irc &irc, int i)
{
    if (!irc.client[i].authenticated)
        autentication(irc, i);
    else
    {
        if (first_command(irc) == "PING")
        {
            const std::string &messages = "PONG :" + second_command(irc);
            send(irc.client[i].client_sock, messages.c_str(), messages.size(), 0);
        }
        if (first_command(irc) == "WHO" || first_command(irc) == "USERHOST")
            return (0);
        if (first_command(irc) == "NICK" || first_command(irc) == "USER" || first_command(irc) == "PASS")
            send_error_message(irc, i, "462", ":You may not reregister", irc.client[i].client_sock);
        else
            commands(irc, i);
    }
    return (0);
}

int handle_command(ft_irc &irc, int i)
{
    if (registretion(irc, i) == 1)
        return (1);
    return 0;
}
