NAME = webserv

SRC_DIR = src/
HPP_DIR = includes/
OBJ_DIR = obj/

INCLUDES = -I$(HPP_DIR)
INCLUDES += -Ilibft

OS := $(shell uname)
ifeq ($(OS), Linux)
	INCLUDES += -Iincludes_linux
endif
ifeq ($(OS), Darwin)
	INCLUDES += -Iincludes_mac
endif

# Setting the compiler and the default linker program
CC = clang++
# main compilation options
CFLAGS = -Wall -Wextra -Werror -std=c++98
CFLAGS += -g
# options for pre-processor (-I, -include, -D ... )
CPPFLAGS = -MMD $(INCLUDES)

SRC =	main.cpp \
		Log.cpp \
		ConfigServer.cpp \
		ConfigLocation.cpp \
		Config.cpp \
		DefaultPage.cpp \
		Autoindex.cpp \
		Client.cpp \
		Response.cpp \
		Core.cpp \
		cgi.cpp \
		cgi2.cpp \
		base64.cpp

OBJ = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))
# add list of dependency files
DEP = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.d))


.PHONY: all clean fclean mkdir re

$(shell mkdir -p $(OBJ_DIR))

all: $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
		$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# include dependency files
-include $(DEP)

$(NAME): $(OBJ)
		$(CC) $(OBJ) -o $(NAME)

clean:
		/bin/rm -rf $(OBJ_DIR)

fclean: clean
		/bin/rm -f $(NAME)

mkdir: 
		$(shell mkdir -p $(OBJ_DIR))

leaks:
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(NAME)

re: fclean mkdir all