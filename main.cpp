#include "ft_irc.hpp"
#include <iostream>

int main() 
{
	std::vector<Channel> channels;
	channels.push_back(Channel("general"));
	
	channels.push_back(Channel("random"));
	channels.push_back(Channel("banana"));

	channels[0].addUser("pincopallo");
	channels[0].addUser("tiziocaio");
	channels[0].addUser("vader");
	channels[0]._topic = "Cucina";
	channels[0].addOperatorUser("vader");
	channels[0].addOperatorUser("luke");
	
	std::cout << "Numero di utenti del canale " <<  channels[0]._name << " => " << channels[0]._num_users << std::endl;
	if (channels[0]._num_users > 0)
	{
		std::cout << "\n=== Utenti ===\n" << std::endl;
		for (unsigned int x = 0; x < channels[0]._num_users; x++)
		{
			std::cout << channels[0].users[x].user << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << "Numero di utenti del canale " <<  channels[1]._name << " => " << channels[1]._num_users << std::endl;
	kick_command("luke", "general", "pincopallo", channels);
	std::cout << "Numero di utenti del canale " <<  channels[0]._name << " => " << channels[0]._num_users << std::endl;
	kick_command("vader", "unknown", "pincopallo", channels);
	kick_command("vader", "banana", "pincopallo", channels);
	
	std::cout << "\n\n\n";
	topic_command("vader", "unknown", "Sport", channels);
	topic_command("pincopallo", "general", "Sport", channels);
	topic_command("pincopallo", "general", "", channels);
	topic_command("vader", "general", "Sport", channels);
	

	return 0;
}

