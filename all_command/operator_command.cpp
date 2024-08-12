#include "../header/ft_irc.hpp"


int check_number_of_arguments(std::string command)
{
	std::stringstream ss(command);
	std::string first_wd;
	std::string word;
	
	int num_args = 0;
	
	ss >> first_wd;
	
	while (ss >> word)
		num_args++;
	if (first_wd == "KICK" && (num_args == 2 || num_args == 3))
		return 1;
	else if (first_wd == "TOPIC" && (num_args == 1 || num_args == 2))
		return 1;
	else
		return 0;
	return 0;
}


void	send_to_operator_function(ft_irc& irc, int i)
{
	std::stringstream ss(irc.buffer);
	std::string word;
	std::string args[10];
	ss >> word;
	int index = 0;
	
	while(ss >> args[index])
	{
		index++;
	}
	
	if (word == "KICK")
	{
		kick_command(irc, i, "vader", args[0], args[1]);
	}
	else if (word == "TOPIC")
	{
		topic_command("vader", args[0], args[1], irc.channels);
	}
	
	
	
}

void	operator_command(ft_irc& irc, int i)
{
	irc = irc;
	std::cout << "Operator command: " << irc.buffer << std::endl;

	//splittare il buffer ovvero il comando, contare gli argomenti del comando e se sono
	//più di quelli previsti dò errore
	if (!check_number_of_arguments(irc.buffer))
		send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
	
	
	send_to_operator_function(irc, i);
	
	
	//se il numero di argomenti è giusto richiamo funzione che esegue istruzioni relative al comando

}
