#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include <vector>
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h> 

class client_info;

class Channel
{
	public:
	    Channel(std::string name);
	    ~Channel();
	    std::string _name;
	    std::string _topic;
	    unsigned int _num_users;
	    std::vector<client_info> users; // Usa client_info qui
	    std::vector<client_info> operatorUsers;
	    
	    bool channelHasName(const std::string& name) const 
	    {
		return _name == name;
	    }
	    
	    void addUser(const std::string& username);
	    void addOperatorUser(const std::string& oper_name);
	    void removeUser(const std::string& username);
	};

void kick_command(const std::string& oper_name, const std::string& channel_name, const std::string& user_name, std::vector<Channel>& channels);


#endif
