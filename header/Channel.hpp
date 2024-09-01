#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class ft_irc;
class client_info;

class Channel
{
	public:
		Channel();
	    Channel(std::string name);

	    std::string _name;
	    std::string _topic;
	    std::string _key;

	    int _num_users;
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

	    void DeleteUserFromChannel(ft_irc& irc, int i);
	    void addUser(ft_irc &irc, int i);
	    void addOperatorUser(const std::string& oper_name, const std::string& nick);
	    void removeUser(const std::string& nick);
		void removeOperator(const std::string& oper);
		void removeInvited(const std::string& invited);
		
		~Channel();
};

#endif
