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
		std::string	flags;

	    int _num_users;
	    int _max_users;

	    std::vector<client_info> users;
	    std::vector<client_info> operatorUsers;
	    std::vector<client_info> invitedUsers;
		std::vector<std::string> flag_o;

	    bool invite_only;
	    bool has_password;
	    bool topic_limited;
	    bool users_limit;
	    bool has_key;
		bool have_op;

		int	num_user() const
		{
			return _num_users;
		}
		int operatorCount() const
		{
			return operatorUsers.size();
		}

	    bool	channelHasName(const std::string& name) const;
		bool	isMember(const client_info& user);
		bool	isMemberOperator(const client_info& user_operator);
	    
		void	DeleteUserFromChannel(ft_irc& irc, int i);
		void	next_operator(void);
	    void	addUser(ft_irc &irc, int i);
	    void	addOperatorUser(const std::string& oper_name, const std::string& nick, int sock);
	    void	removeUser(const std::string& nick);
		void	removeOperator(const std::string& oper);
		void	removeInvited(const std::string& invited);
		
		~Channel();
};

#endif
