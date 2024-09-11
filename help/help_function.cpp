#include "../header/ft_irc.hpp"

std::string extract_message(const std::string &buffer)
{
    size_t pos = buffer.find(':');
    if (pos != std::string::npos) {
        return buffer.substr(pos + 1);
    }
    return "";
}

void init_poll(ft_irc &irc, int &sock)
{
    struct pollfd pfd; //creo struct pollfd
    pfd.fd = sock; //associo il socket a fd cosi che pfd lo monitori
    pfd.events = POLLIN; //permetto l'ascolto degli eventi
    pfd.revents = 0; //definisce che pfd dovra monitorare i dati inviati
    irc.p_fds.push_back(pfd); //inserisco pfd nel vettore
}

void removeChars(std::string& str, const char charsToRemove)
{
    str.erase(std::remove(str.begin(), str.end(), charsToRemove), str.end());
}

bool enough_elements(const std::string &input)
{
    std::istringstream iss(input);
    std::string word;
    int count = 0;

    while (iss >> word)
    {
        if (count == 0 && (word != "NICK" && word != "USER"))
            return (false);
        count++;
    }
    return (count == 2);
}

bool    check_info(ft_irc irc, int i)
{
    return (enough_elements(irc.client[i].nick) && enough_elements(irc.client[i].user));
}

std::string trim(const std::string& str)
{
    // Trova la prima posizione di un carattere non spazio
    size_t start = str.find_first_not_of(' ');
    // Trova l'ultima posizione di un carattere non spazio
    size_t end = str.find_last_not_of(' ');

    // Se la stringa è tutta spazi o vuota, ritorna una stringa vuota
    if (start == std::string::npos) {
        return "";
    }

    // Ritorna la sottostringa tra 'start' e 'end'
    return str.substr(start, end - start + 1);
}
