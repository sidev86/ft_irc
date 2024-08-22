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
	    std::string _key;
	    unsigned int _num_users;
	    int _max_users;
	    std::vector<client_info> users; // Usa client_info qui
	    std::vector<client_info> operatorUsers;
	    std::vector<client_info> invitedUsers;
	    //MODES
	    bool invite_only;
	    bool has_password;
	    bool topic_all_users;
	    bool users_limit;
	    bool has_key;
	    
	    bool channelHasName(const std::string& name) const;
	    
	    void addUser(const std::string& username);
	    void addOperatorUser(const std::string& oper_name);
	    void removeUser(const std::string& username);
};

#endif
