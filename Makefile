CPP = c++
CPPFLAGS = -g -std=c++98 -Wall -Werror -Wextra
NAME = ircserv
SRCS =	main.cpp \
		help/function_for_commands.cpp \
		help/registration.cpp \
		help/messages.cpp \
		help/ausiliary.cpp \
		all_command/command.cpp \
		handling_connections/handle_client.cpp \
		handling_connections/handle_server.cpp \
		all_command/single_commands/quit_command.cpp \
		all_command/operator_command.cpp \
		all_command/Channel.cpp \
		all_command/operator_cmd_functions.cpp \
		all_command/single_commands/kick_cmd.cpp \
		all_command/single_commands/topic_cmd.cpp
		
		
		
OBJS = $(SRCS:.cpp=.o)

RESET = \033[0m
BOLD = \033[1m
RED = \033[31m
RED = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
MAGENTA = \033[35m
CYAN = \033[36m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(BOLD)$(CYAN)[Compilazione binario]:$(RESET) $@"
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJS)
	@echo "$(RED)[Successo]:$(RESET) $(NAME) è stato creato con successo!"

%.o: %.cpp
	@echo "$(BOLD)$(BLUE)[Compilazione oggetto]:$(RESET) $<"
	$(CPP) $(CPPFLAGS) -c $< -o $@
	@echo "$(YELLOW)[Successo]:$(RESET) $@ è stato creato con successo!"

clean:
	clear
	@echo "$(BOLD)$(RED)[Pulizia]:$(RESET) Rimozione file oggetto e binari"
	rm -f $(OBJS) $(NAME)
	@echo "$(RED)[Pulizia completata]$(RESET)"

fclean: clean

re: fclean all

.PHONY: all clean fclean re
