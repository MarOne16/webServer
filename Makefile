NAME   = webserv

FLAGS  = -Wall -Wextra -Werror -std=c++98 # -g  -fsanitize=address
CC     = c++
FILES  = ./configParsing/getLocation.cpp \
		./cgi_bin/cgi.cpp \
		./cgi_bin/exec_file.cpp \
		./configParsing/configParsing.cpp \
		./configParsing/parsingTools.cpp \
		./configParsing/hashMapParsing.cpp \
		./Response/mainResponse.cpp \
		./Response/requese.cpp \
		./Response/response.cpp \
		./Response/tools.cpp \
		./serveurs-clients/serverTools.cpp \
		./serveurs-clients/servers2.cpp 


OBJS   = $(FILES:.cpp=.o)
HEADER = ./Response/webserver.hpp \
		 ./cgi_bin/cgi.cpp \
		 ./configParsing/configParsing.hpp \

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