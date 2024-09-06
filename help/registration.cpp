#include "../header/ft_irc.hpp"

int is_valid_hostname(const std::string &str)
{
    if (str == "*")
        return 0;
    if (!std::isalnum(str[0]) || !std::isalnum(str[str.length() - 1]))
        return 1;
    for (size_t i = 0; i < str.size(); ++i)
    {
        char c = str[i];
        if (!std::isalnum(c) && c != '-' && c != '.')
            return 1;
        if (c == '-')
        {
            if ((i == 0 || i == str.size() - 1) && (str[i - 1] == '-' || str[i + 1] == '-'))
                return 1;
        }
        if (c == '.')
        {
            if ((i == 0 || i == str.size() - 1) && (str[i - 1] == '.' || str[i + 1] == '.'))
                return 1;
        }
    }
    return 0;
}

int check_realname(ft_irc &irc, int i, std::string &error_msg)
{
    if (irc.client[i].realname[0] != ':')
    {
        send_error_message(irc, i, "461", error_msg, irc.client[i].client_sock);
        return (1);
    }
    return (0);
}

int handle_user(ft_irc &irc, int i)
{
    std::string error_msg;
    error_msg = first_command(irc) + " :Not enough parameters";
    std::stringstream ss(trim(second_command(irc)));
    ss >> irc.client[i].user >> irc.client[i].host >> irc.client[i].server;
    std::getline(ss, irc.client[i].realname);
    irc.client[i].realname = trim(irc.client[i].realname);
    irc.client[i].host = trim(irc.client[i].host);
    irc.client[i].server = trim(irc.client[i].server);
    irc.client[i].user = trim(irc.client[i].user);
    if (irc.client[i].user.empty() || irc.client[i].server.empty() || irc.client[i].host.empty() || irc.client[i].realname.empty())
    {
        send_error_message(irc, i, "461", error_msg, irc.client[i].client_sock);
        return (1);
    }
    if (check_realname(irc, i, error_msg) == 1)
    {
        return (1);
    }
    if (irc.client[i].host.length() > 253 || is_valid_hostname(irc.client[i].host) == 1)
    {
        send_error_message(irc, i, "461", error_msg, irc.client[i].client_sock);
        return (1);
    }
    if (is_valid_hostname(irc.client[i].server) == 1)
    {
        send_error_message(irc, i, "461", error_msg, irc.client[i].client_sock);
        return (1);
    }
    return (0);
}

int cont_check_nick(ft_irc &irc, int i, const std::string &nick)
{
    std::string error_msg;
    if (nick.length() > 9)
    {
        error_msg = nick + " :Erroneous nickname";
        send_error_message(irc, i, "432", error_msg, irc.client[i].client_sock);
        return (1);
    }
    int alpha = 0;
    int num = 0;
    for (unsigned int j = 0; j < nick.length(); j++)
    {
        if (std::isalpha(nick[j]))
            alpha = 1;
        else if (std::isdigit(nick[j]))
            num = 1;
        if (!std::isalnum(nick[j]) && nick[j] != '-' && nick[j] != '.')
        {
            error_msg = nick + " :Erroneous nickname";
            send_error_message(irc, i, "432", error_msg, irc.client[i].client_sock);
            return (1);
        }
    }
    if (alpha == 0 || num == 0 || nick[0] == '.' || nick[nick.length() - 1] == '.' || \
    nick[0] == '-' || nick[nick.length() - 1] == '-')
    {
        error_msg = nick + " :Erroneous nickname";
        send_error_message(irc, i, "432", error_msg, irc.client[i].client_sock);
        return (1);
    }
    irc.client[i].is_nick = true;
    return (0);
}

int check_nick(const std::string &nick, ft_irc &irc, int i)
{
    if (nick.empty())
    {
        send_error_message(irc, i, "431", ": no nickname given", irc.client[i].client_sock);
        return (1);
    }
    else if (i >= 1)
    {
        for (int j = 0; j < i; j++)
        {
            if (irc.client[j].nick == nick)
            {
                send_error_message(irc, i, "433", nick + " :Nickname is already in use", irc.client[i].client_sock);
                return 1;
            }
        }
    }
    if (cont_check_nick(irc, i, nick) == 1)
        return (1);
    return (0);
}


void    process_pass_command(ft_irc &irc, int i)
{
    //non inserisco la password
    std::string pass = trim((second_command(irc)));
    if (pass[0] == ':')
        pass = extract_message(pass);
    if (pass.empty())
    {
        std::string message = first_command(irc) + " : Not enough parameters";
        send_error_message(irc, i, "461", message, irc.client[i].client_sock);
    }
    else if (pass == irc.pass_server)
        irc.client[i].is_pass = true;
    else
        send_error_message(irc, i, "464", ": Password incorrect", irc.client[i].client_sock);
}

int user_command(ft_irc &irc, int i)
{
    int u_client = handle_user(irc, i);
    if (u_client == 0 && irc.client[i].is_pass == true)
    {
        irc.client[i].is_user = true;
        return (0);
    }
    return (0);
}

void nick_command(ft_irc &irc, int i)
{
    std::string nick = trim(second_command(irc));
    int u_nick = check_nick(nick, irc, i);
    if (u_nick == 0 && irc.client[i].is_pass == true && \
    (irc.client[i].is_nick == false || irc.client[i].is_nick == true))
    {
        irc.client[i].nick = nick;
        return ;
    }
}