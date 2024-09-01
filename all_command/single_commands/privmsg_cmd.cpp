#include "../../header/ft_irc.hpp"
#include "../../header/Channel.hpp"

/*ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
           ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
           ERR_NOSUCHNICK
           RPL_AWAY
*/

void sendMessageToUser(ft_irc& irc, const std::string& nick, const std::string& message) {
    int userIndex = get_user_index(irc.client, nick);
    std::cout << "Debug: User Index = " << userIndex << std::endl;

    if (userIndex != -1 && userIndex < static_cast<int>(irc.client.size())) {
        // L'utente esiste, invia il messaggio
        std::string full_message = message; // `message` già include i caratteri di newline
        client_message(irc, userIndex, "PRIVMSG", full_message);
    } else {
        // L'utente non esiste, invia un errore se l'indice è valido
        if (userIndex != -1) {
            std::string errMsg = "401 " + nick + " :No such nick/channel";
            client_message(irc, userIndex, "ERROR", errMsg); // Usa un comando di errore appropriato
        }
    }
}

void sendMessageToChannel(ft_irc& irc, const std::string& channelName, const std::string& message, client_info& sender) {
    // Trova il canale
    std::vector<Channel>::iterator channelIt = findChannel(channelName, irc.channels);
    
    if (channelIt != irc.channels.end()) {
        // Il canale esiste
        std::string privmsg = ":" + sender.nick + " PRIVMSG " + channelName + message + "\r\n";
        
        // Debug: stampa gli utenti del canale
        std::cout << "Debug: Channel " << channelName << " found. Users count: " << channelIt->users.size() << std::endl;
        
        // Invia il messaggio a tutti gli utenti del canale
        for (size_t i = 0; i < channelIt->users.size(); i++) {
            client_info user = channelIt->users[i];

            std::cout << "User Nick: " << user.nick << " Socket FD: " << user.client_sock << std::endl;

            // Non inviare il messaggio al mittente
            if (user.client_sock != sender.client_sock)
            {
                    std::cout << "Sending message to user " << user.nick << " on socket " << user.client_sock << std::endl;
                    send(user.client_sock, privmsg.c_str(), privmsg.size(), 0);
            }
        }

    } else {
        // Il canale non esiste, invia un errore al mittente
        std::string errMsg = "403 " + channelName + " :No such channel\r\n";
        send(sender.client_sock, errMsg.c_str(), errMsg.size(), 0);
    }
}


// Funzione per gestire il comando PRIVMSG
void privmsg_command(ft_irc& irc, int i, const std::string& target) {
    // Separare il nome del canale o del target dal messaggio
    size_t pos = target.find(' ');
    if (pos == std::string::npos) {
        std::cerr << "Error: Invalid target format" << std::endl;
        return;
    }
    std::string channel_name = target.substr(0, pos);
    std::string msg = target.substr(pos + 1);

    if (channel_name[0] == '#')
    { // Se è un canale
        // Cerca il canale
        std::vector<Channel>::iterator it = findChannel(channel_name, irc.channels);
        if (it == irc.channels.end()) {
            std::cerr << "Channel not found: " << channel_name << std::endl;
            // Gestisci il caso in cui il canale non venga trovato
            return;
        }

        // Invia il messaggio a tutti i membri del canale
        std::string full_message = msg;
        sendMessageToChannel(irc, channel_name, full_message, irc.client[i]);
    }
    else// Se è un utente
        sendMessageToUser(irc, channel_name, msg);
}