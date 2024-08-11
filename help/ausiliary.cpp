#include "../header/ft_irc.hpp"

void init_poll(ft_irc &irc, int &sock)
{
    struct pollfd pfd; //creo struct pollfd
    pfd.fd = sock; //associo il socket a fd cosi che pfd lo monitori
    pfd.events = POLLIN; //permetto l'ascolto degli eventi
    pfd.revents = 0; //definisce che pfd dovra monitorare i dati inviati
    irc.p_fds.push_back(pfd); //inserisco pfd nel vettore
}

