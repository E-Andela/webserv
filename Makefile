# Compiler and flags
CXX     := c++
CXXFLAGS:= -Wall -Werror -Wextra -Wpedantic -std=c++17 -g -fsanitize=address

NAME    := webserv

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

SRCS    := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS    := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

INCLUDES := -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning object files..."
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Removing executable..."
	rm -f $(NAME)

re: fclean all

# to run unit tests
#test:

.PHONY: all clean fclean re
