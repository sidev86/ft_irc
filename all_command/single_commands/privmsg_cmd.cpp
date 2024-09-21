#include "../../header/ft_irc.hpp"
#include "../../header/Channel.hpp"
const size_t MAX_RECIPIENTS = 3;

void sendMessageToUser(ft_irc& irc, const std::string& sender, const std::string reciver, const std::string& message)
{
    int userIndex = get_user_index(irc.client, sender);
    if (userIndex != -1 && userIndex < static_cast<int>(irc.client.size()))
    {
        // L'utente esiste, invia il messaggio
        std::string full_message = reciver + " " + message; // `message` già include i caratteri di newline
        client_message(irc, userIndex, "PRIVMSG", full_message);
    }
}

void sendMessageToChannel(ft_irc& irc, int t, const std::string& channelName, const std::string& message, client_info& sender)
{
    // Trova il canale
    std::vector<Channel>::iterator channelIt = findChannel(channelName, irc.channels);
    if (channelIt != irc.channels.end())
    {
        // Verifica se l'utente è membro del canale
        if (!channelIt->isMember(sender))
        {
            std::string errMsg = "404 " + sender.nick + " " + channelName + " :Cannot send to channel\r\n";
            send(sender.client_sock, errMsg.c_str(), errMsg.size(), 0);
            return;
        }
        // Il canale esiste
        std::string privmsg = ":" + sender.nick + " PRIVMSG " + channelName + " " + message + "\r\n";

        // Invia il messaggio a tutti gli utenti del canale
        for (size_t i = 0; i < channelIt->users.size(); i++)
        {
            if (channelIt->users[i].nick != irc.client[t].nick)
            {
                client_info user = channelIt->users[i];
                send(user.client_sock, privmsg.c_str(), privmsg.size(), 0);
            }
        }
    }
}

int invalid_command(ft_irc& irc, int i, const std::string& recipients)
{
    std::stringstream ss(recipients);
    std::vector<std::string> individual_target;
    std::string individual_targets;
    while (std::getline(ss, individual_targets, ','))
    {
        size_t dot = individual_targets.find(":");
        // Stampa il target
        // Controlla se ':' è stato trovato
        if (dot != std::string::npos && dot > 0) // Assicura che ci sia qualcosa prima del ':'
        {
            // Verifica se il carattere prima del ':' è uno spazio
            if (individual_targets[dot - 1] != ' ')
            {
                send_error_message(irc, i, "421", ":Unknown command", irc.client[i].client_sock);
                return (1);
            }
        }
    }
    return (0);
}

void privmsg_command(ft_irc& irc, int i, const std::string& target)
{
    if (target.empty() || target[0] == ':')
    {
        std::string errMsg = "411 " + irc.client[i].nick + " :No recipient given " + first_command(irc) + "\r\n";
        send(irc.client[i].client_sock, errMsg.c_str(), errMsg.size(), 0);
        return;
    }
    size_t colon_pos = target.find(" :");
    std::string msg;
    std::string recipients;
    if (colon_pos == std::string::npos)
    {
        msg = "";
        recipients = target;
    }
    else
    {
        msg = target.substr(colon_pos + 2);
        recipients = target.substr(0, colon_pos);
    }
    if (recipients.find(",") != std::string::npos || recipients.find(" ") != std::string::npos)
    {
        // Se c'è uno spazio senza virgola, è un errore di comando
        if (recipients.find(" ") != std::string::npos && recipients.find(",") == std::string::npos)
        {
            std::string errMsg = "421 " + irc.client[i].nick + " " + first_command(irc) + " :Unknown command\r\n";
            send(irc.client[i].client_sock, errMsg.c_str(), errMsg.size(), 0);
            return;
        }
    }
    if (invalid_command(irc, i, recipients) == 1)
        return;
    if (msg.empty())
    {
        std::string errMsg = "412 " + irc.client[i].nick + " :No text to send\r\n";
        send(irc.client[i].client_sock, errMsg.c_str(), errMsg.size(), 0);
        return;
    }
    std::stringstream ss(recipients);
    std::vector<std::string> individual_targets;
    std::string individual_target;
    while (std::getline(ss, individual_target, ','))
    {
        individual_target.erase(0, individual_target.find_first_not_of(" \n\r\t"));
        individual_target.erase(individual_target.find_last_not_of(" \n\r\t") + 1);

        if (!individual_target.empty())
            individual_targets.push_back(individual_target);
    }

    // Verifica il numero di destinatari
    if (individual_targets.size() > MAX_RECIPIENTS)
    {
        std::string errMsg = "407 " + irc.client[i].nick + " :Too many recipients\r\n";
        send(irc.client[i].client_sock, errMsg.c_str(), errMsg.size(), 0);
        return;
    }
    for (size_t j = 0; j < individual_targets.size(); ++j)
    {
        const std::string& target = individual_targets[j];
        // Se il destinatario è un canale
        if (target[0] == '#')
        {
            std::vector<Channel>::iterator it = findChannel(target, irc.channels);
            if (it == irc.channels.end())
            {
                std::string errMsg = "403 " + target + " :No such channel\r\n";
                send(irc.client[i].client_sock, errMsg.c_str(), errMsg.size(), 0);
                continue;
            }
            sendMessageToChannel(irc, i, target, ":" + msg, irc.client[i]);
        }
        else
        { // Se il destinatario è un utente
            int userIndex = get_user_index(irc.client, target);
            if (userIndex == -1)
            {
                std::string errMsg = "401 " + target + " :No such nick/channel\r\n";
                send(irc.client[i].client_sock, errMsg.c_str(), errMsg.size(), 0);
                continue;
            }
            if (individual_target != irc.client[i].nick)
                sendMessageToUser(irc, target, irc.client[i].nick, ":" + msg);
        }
    }
}