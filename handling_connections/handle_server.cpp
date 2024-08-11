#include "../header/ft_irc.hpp"

/*
sockfd = socket del server
viene creato un socket per avviare la connessione
*/
int create_socket(int &sockfd)
{
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        colored_message("🚨Error: \n(socket creation failed)🚨", RED);
        perror("socket");
        return 1;
    }
    return 0;
}

/*
funzione per poter fare riusare un indirizzo gia usato
ES. se io uso 6667 e esco.
senza questo mi da bind error se lo provo a riusare
*/
int set_reuse_address(int sockfd)
{
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopt");
        close(sockfd);
        return 1;
    }
    return 0;
}

void initialize_address(struct sockaddr_in &server_addr, const std::string &port)
{
    memset(&server_addr, 0, sizeof(server_addr)); //inizzializzo server_addr a 0
    server_addr.sin_family = AF_INET; //il socket sta usando il protocollo IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //il server ascoltera tutti le reti dispomibili

    std::stringstream ss(port); //creo un flusso di dati prendo la porta e la converto in binario per il server
    int port_num;
    ss >> port_num;
    server_addr.sin_port = htons(port_num);
}

/*
bind(collegare indirizzo alla porta del server)
bindo il client al server
*/
int bind_socket(int sockfd, const struct sockaddr_in &server_addr)
{
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        colored_message("🚨Error: \n(bind failed)🚨", RED);
        perror("bind");
        close(sockfd);
        return 1;
    }
    return 0;
}

/*
mette il server in ascolto dei client
*/
int start_listening(int sockfd)
{
    if (listen(sockfd, 10) < 0)
    {
        colored_message("🚨Error: \n(listen failed)🚨", RED);
        perror("listen");
        close(sockfd);
        return 1;
    }
    return 0;
}

int non_blocking_server(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL, 0); //setti una flag per il socket da usare dopo
    if (flags < 0)
    {
        colored_message("🚨Error: \n(fcntl failed)🚨", RED);
        perror("fcntl");
        close(sockfd);
        return 1;
    }
    /*
    socket non bloccante con piu client
    per esempio con ctrl+c ctrl-z ctrl-d il server non si ferma
    */
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) //imposta il socket come non bloccante
    {
        std::cerr << "🚨Error: \n(fcntl F_SETFL failed)🚨" << std::endl;
        perror("fcntl");
        close(sockfd);
        return 1;
    }
    return (0);
}

int handle_server(ft_irc &irc)
{
    if (create_socket(irc.server.server_sock) == 1)
        return 1;
    if (non_blocking_server(irc.server.server_sock) == 1)
        return 1;
    if (set_reuse_address(irc.server.server_sock) == 1)
        return 1;
    initialize_address(irc.server.server_addr, irc.port);
    if (bind_socket(irc.server.server_sock, irc.server.server_addr) == 1)
        return 1;
    if (start_listening(irc.server.server_sock) == 1)
        return 1;
    colored_message("Server: listening on port " + irc.port, YELLOW);
    init_poll(irc, irc.server.server_sock);
    while (1)
    {
        if (handle_client(irc) == 1)
            break;
    }
    close(irc.server.server_sock);
    return 0;
}
