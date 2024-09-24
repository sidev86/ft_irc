#include "../header/ft_irc.hpp"
extern ft_irc global_irc;

void handle_sigtstp(int signal)
{
    (void)signal;

    if (!global_irc.server_suspended) 
    {
        std::cout << "Received SIGTSTP (Ctrl+Z), suspending server..." << std::endl;
        global_irc.server_suspended = true;
        std::string suspend_msg = "\nServer is suspended. Please wait...\n";
        for (size_t i = 0; i < global_irc.client.size(); ++i)
            send(global_irc.client[i].client_sock, suspend_msg.c_str(), suspend_msg.size(), 0);
    } 
    else 
    {
        std::cout << "Received SIGTSTP (Ctrl+Z), resuming server..." << std::endl;
        global_irc.server_suspended = false;
        std::string resume_msg = "Server has resumed.\n";
        for (size_t i = 0; i < global_irc.client.size(); ++i)
            send(global_irc.client[i].client_sock, resume_msg.c_str(), resume_msg.size(), 0);
    }
}

void handle_termination(int signal)
{
    (void)signal;

    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    global_irc.server_running = false;

    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    sigaddset(&block_set, SIGTSTP);
    sigprocmask(SIG_BLOCK, &block_set, NULL);

    for (size_t i = 0; i < global_irc.client.size(); ++i)
    {
        std::string shutdown_msg = "Server is shutting down...\n";
        send(global_irc.client[i].client_sock, shutdown_msg.c_str(), shutdown_msg.size(), 0);
        close(global_irc.client[i].client_sock);
    }
    global_irc.client.clear();

    if (global_irc.server.server_sock != -1)
    {
        close(global_irc.server.server_sock);
        global_irc.server.server_sock = -1;
    }
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);
}
