#include "../header/ft_irc.hpp"

// Funzione per leggere i dati in arrivo dal socket e processarli
int process_incoming_data(ft_irc &irc, int i)
{
    //legge dati del client
    ssize_t bytes = recv(irc.p_fds[i].fd, irc.buffer, sizeof(irc.buffer) -1, 0);
    if (bytes <= 0)
    {
        if (bytes < 0)
            perror("recv");
        close(irc.p_fds[i].fd); //chiude connession
        irc.p_fds.erase(irc.p_fds.begin() + i); //rimuobe l'fd monitorato da poll
        irc.client.erase(irc.client.begin() + i); //rimuove il client dal vettore
        std::cout << "🚨Error: \n(connection closed)🚨" << std::endl;
        return 1;
    }
    irc.buffer[bytes] = '\0';
    if (first_command(irc) == "CAP" && second_command(irc) == "LS 302")
        return 0;
    if (handle_command(irc, i) == 1)
        return 1;
    return 0;
}

int accept_connections(ft_irc &irc)
{
    //inizzializzo client
    client_info new_client;
    new_client.client_len = sizeof(new_client.client_addr);
    new_client.client_sock = accept(irc.server.server_sock, (struct sockaddr *)&new_client.client_addr, &new_client.client_len);
    new_client.is_nick = false;
    new_client.is_user = false;
    new_client.is_pass = false;
    new_client.authenticated = false;
    if (new_client.client_sock < 0)
    {
        colored_message("🚨Error: \n(accept failed)🚨", RED);
        perror("accept");
        return 1;
    }
    // Aggiungi il nuovo client al vettore dei client
    irc.client.push_back(new_client);
    init_poll(irc, new_client.client_sock);
    return 0;
}

int set_sock(ft_irc &irc,int i)
{
    int reuse = 1;
    if (non_blocking_server(irc.client[i].client_sock) == 1)
        return 1;
    //setta il socket in modo che se esci lo puoi subito riutilizzare lo stesso
    if (setsockopt(irc.client[i].client_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "🚨Error: \n(setsockopt failed)🚨" << std::endl;
        perror("setsockopt");
        close(irc.client[i].client_sock);
        return 1;
    }
    return 0;
}

int pfd_connections(ft_irc &irc)
{
    for (size_t i = 0; i < irc.p_fds.size();)
    {
        //controlla se l'utente invia messaggi
        if (irc.p_fds[i].revents & POLLIN)
        {
            //se fds[i] == server_sock, nuovo client accettazione connessione
            if (irc.p_fds[i].fd == irc.server.server_sock)
            {
                if (accept_connections(irc) == 1)
                {
                    if (set_sock(irc, i) == 1)
                        return 1;
                    i++;
                    continue;
                }
                
            }
            else
            {
                if (process_incoming_data(irc, i) == 1)
                    continue;
            }
        }
        i++;
    }
    return 0;
}

// Funzione per eseguire il poll e gestire gli eventi in arrivo
int poll_and_handle(ft_irc &irc)
{
    memset(irc.buffer, 0, sizeof(irc.buffer)); //inizializza il buffer a 0
    //faccio si che pool monitori tutti gli fd all'infinito
    int result = poll(irc.p_fds.data(), irc.p_fds.size(), -1);
    if (result < 0)
    {
        colored_message("🚨Error: \n(poll failed)🚨", RED);
        perror("poll");
        return 1;
    }
    if (pfd_connections(irc) == 1)
        return 1;
    return 0;
}

// Funzione principale per gestire il client
int handle_client(ft_irc &irc)
{
    if (poll_and_handle(irc) == 1)
        return 1;
    else
        return 0;
}
