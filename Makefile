CC = clang

CFLAGS = -Wall -Wextra #-Werror
CFLAGS += -ggdb # remove later

EXEC=Lazy

SRCS := $(shell find . -type f -name "*.c")

EXEC: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o ./build/$(EXEC)
