NAME   = webserv
#  -fsanitize=address
FLAGS  = -Wall -Wextra -Werror -std=c++98  -g3    -fsanitize=address
CC     = c++
FILES  = ./configParsing/getLocation.cpp \
		./configParsing/configParsing.cpp \
		./configParsing/parsingTools.cpp \
		./Response/mainResponse.cpp \
		./Response/requese.cpp \
		./Response/response.cpp \
		./Response/tools.cpp \
		./serveurs-clients/serverTools.cpp \
		./serveurs-clients/servers2_copy.cpp 


OBJS   = $(FILES:.cpp=.o)
HEADER = ./Response/webserver.hpp

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