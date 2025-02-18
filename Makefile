# ---------------------------------------------------------------------------- #

TARGET	= hello_world

CC		= clang

CFLAGS	= -Wall -Wextra -Werror -Wstrict-prototypes		\
		  -Wmissing-prototypes -Wpedantic -std=gnu17	\
		  -fstack-protector-strong -O0 # No optimizations

SRC		= src/main.c
OBJ		= $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)
	@echo "Build Succeeded"

.c.o:
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(TARGET)

re: clean all

.PHONY: all clean fclean re
