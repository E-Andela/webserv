CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++11

SERVER = server
CLIENT = client

CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp

OBJ_DIR = obj
CLIENT_OBJ = $(OBJ_DIR)/client.o
SERVER_OBJ = $(OBJ_DIR)/server.o

all: $(SERVER) $(CLIENT)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(CLIENT_OBJ): $(CLIENT_SRC) | $(OBJ_DIR)
	$(CC) $(FLAGS) -c $< -o $@

$(SERVER_OBJ): $(SERVER_SRC) | $(OBJ_DIR)
	$(CC) $(FLAGS) -c $< -o $@

$(SERVER): $(SERVER_OBJ)
	$(CC) $(FLAGS) $^ -o $@

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -f $(OBJ)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(SERVER) $(CLIENT)

re: fclean all

.PHONY: all clean fclean re