#include "../header/ft_irc.hpp"

void client_message(ft_irc &irc, int i, const std::string &command, const std::string &ex_message)
{
    std::string message;
    if (command.empty())
        message = ":" + irc.client[i].nick + "!" + irc.client[i].user + "@" + irc.client[i].host;
    else
        message = ":" + irc.client[i].nick + "!" + irc.client[i].user + "@" + irc.client[i].host + " " + command;
    message = message + " " + ex_message;
    message = message + "\r\n";
    send(irc.client[i].client_sock, message.c_str(), message.length(), 0);
}

void send_error_message(ft_irc &irc, int i, const std::string err_code, const std::string &message, int sock)
{
    std::string msg_error;
    if (!irc.client[i].server.empty() && !irc.client[i].nick.empty())
        msg_error = ":" + irc.client[i].server + " " + err_code + " " + irc.client[i].nick + " " + message + "\r\n";
    else
        msg_error = ":unknown " + err_code + " * " + message + "\r\n";
    send(sock, msg_error.c_str(), msg_error.size(), 0);
}
void welcome_msg(ft_irc &irc, int i)
{
    std::string buffer;

    buffer = ":" + irc.client[i].server + " 001 " + irc.client[i].nick + " :Welcome to the Internet Relay Network " + irc.client[i].nick + "!" + irc.client[i].user + "@" + irc.client[i].host + irc.client[i].realname +"\r\n";
    send(irc.client[i].client_sock, buffer.c_str(), buffer.length(), 0);

    buffer = ":" + irc.client[i].server + " 002 " + irc.client[i].nick + " :Your host is " + irc.client[i].server + ", running version 1.0.0\r\n";
    send(irc.client[i].client_sock, buffer.c_str(), buffer.length(), 0);

    buffer = ":" + irc.client[i].server + " 003 " + irc.client[i].nick + " :This server was created on " + __DATE__ + " at " + __TIME__ + "\r\n";
    send(irc.client[i].client_sock, buffer.c_str(), buffer.length(), 0);

    buffer = ":" + irc.client[i].server + " 004 " + irc.client[i].nick + " " + irc.client[i].server + " 1.0.0 ao mtov\r\n";
    send(irc.client[i].client_sock, buffer.c_str(), buffer.length(), 0);
}

void    colored_message(const std::string message, const std::string color)
{
    std::cout << color << message << RESET <<std::endl;
}
