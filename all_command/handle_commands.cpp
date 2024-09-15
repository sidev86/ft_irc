#include "../header/ft_irc.hpp"

int is_comment(std::string command)
{
	int i = 0;
	while (command[i] != 0 && command[i] != ':')
		i++;
	if (command[i] == ':')
		return 0;
	else
		return 1;
}

int check_number_of_arguments(std::string command)
{
	std::stringstream ss(command);
	std::string first_wd;
	std::string word;
	int num_args = 0;

	ss >> first_wd;

	while (ss >> word)
		num_args++;
	if (first_wd == "KICK" && ((num_args > 2 && is_comment(command) == 0) || num_args == 2))
		return 1;
	else if (first_wd == "TOPIC" && ((num_args > 1 && is_comment(command) == 0) || num_args == 1))
		return 1;
	else if (first_wd == "INVITE" && num_args == 2)
		return 1;
	else if (first_wd == "JOIN" && (num_args == 1 || num_args == 2))
		return 1;
	else if (first_wd == "MODE" && (num_args == 1 || num_args == 2 || num_args == 3))
		return 1;
	else if (first_wd == "PART" && (num_args == 1 || is_comment(command) == 0))
		return 1;
	else if (first_wd == "QUIT" && (num_args == 0 || (num_args > 0 && is_comment(command) == 0)))
		return 1;
	else
		return 0;
	return 0;
}

void	send_to_command_function(ft_irc& irc, int i)
{
	std::stringstream ss(irc.buffer);
	std::string word;
	std::string args[10];
	ss >> word;
	int index = 0;
	
	while(ss >> args[index])
		index++;
	
	if (word == "KICK")
		kick_command(irc, i, irc.client[i].nick, args[0], args[1], extract_message(second_command(irc)));
	else if (word == "TOPIC")
		topic_command(irc, i,  irc.client[i].nick, args[0], extract_message(second_command(irc)));
	else if (word == "INVITE")
		invite_command(irc, i,  irc.client[i].nick, args[0], args[1]);
	else if (word == "JOIN")
		join_command(irc, i, args[0], irc.client[i].nick, trim(args[1]));
	else if (word == "MODE")
		mode_command(irc, i, irc.client[i].nick, args[0], args[1], args[2]);
	else if (word == "PART")
		part_command(irc, i, irc.client[i].nick, args[0]);
	else if (word == "QUIT")
		quit_command(irc, i, extract_message(second_command(irc)));
}

void	operator_command(ft_irc& irc, int i)
{
	//splittare il buffer ovvero il comando, contare gli argomenti del comando e se sono
	//più di quelli previsti dò errore
	if (!check_number_of_arguments(irc.buffer))
	{
		send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
		return;
	}	
	send_to_command_function(irc, i);
	//se il numero di argomenti è giusto richiamo funzione che esegue istruzioni relative al comando
}

void	channel_command(ft_irc& irc, int i)
{
	if (!check_number_of_arguments(irc.buffer))
	{
		send_error_message(irc, i, "461", ":Not enough parameters.", irc.client[i].client_sock);
		return;
	}
	send_to_command_function(irc, i);
}
