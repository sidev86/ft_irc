#include "../header/ft_irc.hpp"

/*
se crei 2 client ogniuno crea il suo canale e
esempio
client 1 #SAS
client 2 #SOS
e client 1 entra in #SOS
e client 2 entra in #SAS
se poi il primo client esce.
poi non posso creare una altra connessione con lo stesso perche non mi entra in accepts_connection
e leakka perche indice del client e negativo
*/
int process_incoming_data(ft_irc &irc, int i)
{
    if (irc.server_suspended)
        return 0;
    ssize_t bytes = recv(irc.p_fds[i].fd, irc.buffer, sizeof(irc.buffer) - 1, 0);
    if (bytes <= 0)
    {
        close(irc.p_fds[i].fd);
        irc.p_fds.erase(irc.p_fds.begin() + i);
        irc.client.erase(irc.client.begin() + i);

        if (bytes < 0)
        {
            perror("recv");
            std::cerr << "🚨Error: (connection closed)🚨" << std::endl;
        }

        return 1;
    }

    irc.buffer[bytes] = '\0';
    if (first_command(irc) == "CAP" && trim(second_command(irc)) == "LS 302")
        return 0;

    if (handle_command(irc, i) == 1)
        return 1;

    return 0;
}


int accept_connections(ft_irc &irc)
{
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

    irc.client.push_back(new_client);
    init_poll(irc, new_client.client_sock);
    return 0;
}

int set_sock(ft_irc &irc,int i)
{
    int reuse = 1;
    if (non_blocking_server(irc.client[i].client_sock) == 1)
        return 1;
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
    size_t i = 0;
    while (i < irc.p_fds.size())
    {
        if (irc.server_suspended)
        {
            i++;
            continue;
        }
        if (irc.p_fds[i].revents & POLLIN)
        {
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

int poll_and_handle(ft_irc &irc)
{
    int result = 0;

    if (irc.server_suspended)
        return 0;
    memset(irc.buffer, 0, sizeof(irc.buffer));
    do
    {
        result = poll(irc.p_fds.data(), irc.p_fds.size(), -1);
    } 
    while (result < 0 && errno == EINTR);
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

int handle_client(ft_irc &irc)
{
    if (irc.server_suspended)
        return 0;
    if (poll_and_handle(irc) == 1)
        return 1;
    else
        return 0;
}
