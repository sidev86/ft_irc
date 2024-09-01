#ifndef FT_IRC_HPP
#define FT_IRC_HPP


#define RESET "\033[0m"
#define RED  "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN  "\033[36m"

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <signal.h>
#include "Channel.hpp"

class Channel;

class client_info
{
    public:
        std::string nick;
        std::string user;
        std::string host;
        std::string server;
        std::string realname;
        bool    authenticated;
        bool    is_nick;
        bool    is_user;
        bool    is_pass;
        bool clientHasName(const std::string& name) const 
        {
            return nick == name;
        }
        int client_sock;
        struct sockaddr_in client_addr;
        socklen_t client_len;
};

class server_info
{
    public:
        int server_sock;
        struct sockaddr_in server_addr;
};

class ft_irc
{
    public:
        std::string port;
        std::string pass_server;
        std::string msg;
        std::vector<struct pollfd> p_fds;
        server_info server;
        char buffer[512];
        std::vector<client_info> client;
        std::vector<Channel> channels;
        bool server_suspended;
        bool server_running;
};
int     handle_server(ft_irc &irc);
int     handle_client(ft_irc &irc);
int     handle_command(ft_irc &irc, int i);
int     handle_user(ft_irc &irc, int i);
int     check_nick(const std::string &nick, ft_irc &irc, int i);
int     is_valid_hostname(const std::string &str);
int     user_command(ft_irc &irc, int i);
int     non_blocking_server(int sockfd);
int     check_number_of_arguments(std::string command);
int     nick_exist(std::vector<client_info>& clients, const std::string& nickname);
int     get_user_index(std::vector<client_info>& clients, const std::string& nickname);

std::string first_command(ft_irc irc);
std::string second_command(ft_irc irc);
std::string trim(const std::string& str);
std::string extract_message(const std::string &buffer);

std::vector<Channel>::iterator	findChannel(const std::string& channel_name, std::vector<Channel>& channels);
std::vector<client_info>::iterator	findUserInChannel(const std::string& user_name, std::vector<client_info>& users);

void    part_command(ft_irc& irc, int i, const std::string& user_name, const std::string& channel_name);
void    handle_termination(int signal);
void    handle_sigtstp(int signal);
void	list_command(ft_irc& irc, int i);
void	sendToAll(std::vector<client_info>& users, const std::string& message);
void	sendToClient(client_info& client, const std::string& message);
void	operator_command(ft_irc& irc, int i);
void	channel_command(ft_irc& irc, int i);
void	send_to_command_function(ft_irc& irc, int i);
void    process_pass_command(ft_irc &irc, int i);
void    quit_command(ft_irc &irc, int i);
void    client_message(ft_irc &irc, int i, const std::string &command, const std::string &ex_message);
void    send_error_message(ft_irc &irc, int i, const std::string err_code, const std::string &message, int sock);
void    init_poll(ft_irc &irc, int &sock);
void    colored_message(const std::string message, const std::string color);
void    welcome_msg(ft_irc &irc, int i);
void    nick_command(ft_irc &irc, int i);
void	kick_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& user_name);
void	topic_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& new_topic);
void	invite_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string& user_name);
void    join_command(ft_irc& irc, int i, const std::string& channel_names, const std::string& user_name, const std::string& key);
void	mode_command(ft_irc& irc, int i, const std::string& oper_name, const std::string& channel_name, const std::string option, const std::string& option_param);
void    privmsg_command(ft_irc& irc, int i, const std::string& target);


bool	isOperator(const std::string& oper_name, std::vector<client_info>& operatorUsers);
bool	userReceivedInvite(Channel& channel, const std::string& nick);
bool    userAlreadyInChannel(Channel& channel, const std::string& nick);

#endif
