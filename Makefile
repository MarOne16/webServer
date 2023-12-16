NAME   = webserv

FLAGS  = -Wall -Wextra -Werror -std=c++98 -Ofast
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
		./Response/tools2.cpp \
		./serveurs-clients/serverTools.cpp \
		./serveurs-clients/servers.cpp \
		./serveurs-clients/Tools_Serevers.cpp\
		./serveurs-clients/Tols_Second.cpp\


OBJS   = $(FILES:.cpp=.o)
HEADER = ./Response/webserver.hpp \
		 ./cgi_bin/cgi.hpp \
		 ./configParsing/configParsing.hpp \
		 ./serveurs-clients/servers.hpp 

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