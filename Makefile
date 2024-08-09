NAME = kick
CPP = c++
CPPFLAGS = -g -Wall -Wextra -Werror -std=c++98
SRCS = main.cpp kick_cmd.cpp topic_cmd.cpp Channel.cpp operator_cmd_functions.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
