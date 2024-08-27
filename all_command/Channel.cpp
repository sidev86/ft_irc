#include "../header/Channel.hpp"
#include "../header/ft_irc.hpp"  

Channel::Channel(std::string name) 
{
	this->_name = name; 
	this->_num_users = 0;
	this->_max_users = -1;
	this->_topic = "";
	this->invite_only = false;
	this->has_password = false; 
	this->topic_all_users = false;
	this->users_limit = false;
	this->has_key = false;
	
}
Channel::~Channel() {}

void Channel::addUser(const std::string& username) 
{
    client_info newClient;
    newClient.user = username;
    users.push_back(newClient);
    _num_users++;
}

void Channel::addOperatorUser(const std::string& oper_name)
{
    client_info newOperator;
    newOperator.user = oper_name;
    operatorUsers.push_back(newOperator);
}

void Channel::removeUser(const std::string& username)
{
    for (std::vector<client_info>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (it->user == username)
        {
            users.erase(it);
            _num_users--;
            break;
        }
    }
}

void Channel::removeOperator(const std::string& oper)
{
    for (std::vector<client_info>::iterator it = operatorUsers.begin(); it != operatorUsers.end(); ++it)
    {
        if (it->user == oper)
        {
            operatorUsers.erase(it);
            break;
        }
    }
}



bool Channel::channelHasName(const std::string& name) const 
{
	return _name == name;
}

