CC = gcc

CFLAGS = -Wall -Wextra -std=c17

TARGET = http_server

SRCS = \
	main.c \
	http/http_server.c \
	http/http_parser.c \
	http/http_response.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)