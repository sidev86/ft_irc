#include "../header/ft_irc.hpp"

std::string extract_message(const std::string &buffer)
{
    size_t pos = buffer.find(':');
    if (pos != std::string::npos) {
        return buffer.substr(pos + 1);
    }
    return "";
}

bool    nickmember(const std::string nick, Channel &channel)
{
    if (channel.users.empty() != 0)
    {
        for (size_t i = 0; i < channel.users.size(); i++)
        {
            if (channel.users[i].nick == nick)
                return true;
        }
    }
    if (channel.operatorUsers.empty() != 0)
    {
        for (size_t i = 0; i < channel.operatorUsers.size(); i++)
        {
            if (channel.operatorUsers[i].nick == nick)
                return true;
        }
    }
    return false;
}

bool clienthadnick(const std::string nick, ft_irc &irc)
{
    for (size_t i = 0; i < irc.client.size(); i++)
    {
        if (irc.client[i].nick == nick)
            return (true);
    }
    return (false);
}

void init_poll(ft_irc &irc, int &sock)
{
    struct pollfd pfd;
    pfd.fd = sock;
    pfd.events = POLLIN;
    pfd.revents = 0;
    irc.p_fds.push_back(pfd);
}

int find_char(const std::string &str, char char_to_find)
{
    size_t position = str.find(char_to_find);
    if (position != std::string::npos)
        return 1;
    else
        return 0;
}

void removeChars(std::string& str, const char charsToRemove)
{
    str.erase(std::remove(str.begin(), str.end(), charsToRemove), str.end());
}

bool enough_elements(const std::string &input)
{
    std::istringstream iss(input);
    std::string word;
    int count = 0;

    while (iss >> word)
    {
        if (count == 0 && (word != "NICK" && word != "USER"))
            return (false);
        count++;
    }
    return (count == 2);
}

bool    check_info(ft_irc irc, int i)
{
    return (enough_elements(irc.client[i].nick) && enough_elements(irc.client[i].user));
}

std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(' ');
    size_t end = str.find_last_not_of(' ');
    if (start == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

bool Channel::isMember(const client_info& user)
{
    return std::find(users.begin(), users.end(), user) != users.end();
}

bool Channel::isMemberOperator(const client_info& user_operator)
{
    return std::find(operatorUsers.begin(), operatorUsers.end(), user_operator) != operatorUsers.end();
}