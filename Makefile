# Compiler and flags
CXX     := c++
CXXFLAGS:= -Wall -Werror -std=c++17 -g -fsanitize=address -Wpedantic #-Wextra

# Project name
NAME    := webserv

# Directories
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

# Find all .cpp files in src/ and subdirectories
SRCS    := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS    := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Include directories
INCLUDES := -I$(INC_DIR)

# Default target
all: $(NAME)

# Link the executable
$(NAME): $(OBJS)
	@echo "Linking $@"
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean object files
clean:
	@echo "Cleaning object files..."
	rm -rf $(OBJ_DIR)

# Clean everything
fclean: clean
	@echo "Removing executable..."
	rm -f $(NAME)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re
