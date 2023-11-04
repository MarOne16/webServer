NAME   = configParsing
FLAGS  = -Wall -Wextra -Werror -std=c++98
CC     = c++
FILES  = main.cpp getLocation.cpp configParsing.cpp parsingTools.cpp
OBJS   = $(FILES:.cpp=.o)
HEADER = configParsing.hpp

all: $(NAME)

$(NAME): $(OBJS) $(HEADER)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re