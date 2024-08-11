#include "../header/ft_irc.hpp"

void    commands(ft_irc &irc, int i)
{
    std::string message = first_command(irc) + " :Unknown command";
    if (first_command(irc) == "QUIT")
        quit_command(irc, i);
    else if (first_command(irc) == "KICK" || first_command(irc) == "TOPIC" || first_command(irc) == "INVITE" || first_command(irc) == "MODE")
    	operator_command(irc);
    else
        send_error_message(irc, i, "421", message, irc.client[i].client_sock);
}

void    autentication(ft_irc &irc, int i)
{
    std::string message = first_command(irc) + " :Unknown command";
    if (first_command(irc) == "PASS" && irc.client[i].is_pass == false)
        process_pass_command(irc, i);
    else if ((first_command(irc) == "PASS" && irc.client[i].is_pass == true) || (first_command(irc) == "USER" && irc.client[i].is_user == true))
    {
        send_error_message(irc, i, "462", ": You are already registered", irc.client[i].client_sock);
        return ;
    }
    else if (first_command(irc) == "NICK")
        nick_command(irc, i);
    else if (first_command(irc) == "USER")
        user_command(irc, i);
    else
        send_error_message(irc, i, "421", message, irc.client[i].client_sock);
    if (irc.client[i].is_nick == true && irc.client[i].is_pass == true && \
    irc.client[i].is_user == true)
    {
        welcome_msg(irc, i);
        irc.client[i].authenticated = true;
    }
}

int registretion(ft_irc &irc, int i)
{
    //se il client non e autenticato
    if (!irc.client[i].authenticated)
        autentication(irc, i);
    else
    {
        std::cout << CYAN <<" Client[" << i << "]: " << RESET;
        colored_message(irc.buffer, CYAN);
        //altrimenti se e autenticato a diritto di usare tutti i comandi disponibili
        if (first_command(irc) == "NICK" || first_command(irc) == "USER" || first_command(irc) == "PASS")
            send_error_message(irc, i, "462", ": You are already registered", irc.client[i].client_sock);
        else
            commands(irc, i);
    }
    return (0);
}

// Funzione per gestire i comandi generali
int handle_command(ft_irc &irc, int i)
{
    //prendo il primo client che sempre i-- dato e fd partira da i++
    i--;
    if (registretion(irc, i) == 1)
        return (1);
    return 0;
}
