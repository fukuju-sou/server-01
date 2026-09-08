CC = gcc
CFLAGS = -Wall -Wextra -std=c17
LDFLAGS = -pthread

TARGET = http_server

SRCS = \
	main.c \
	http/http_server.c \
	http/http_client.c \
	http/http_parser.c \
	http/http_response.c \
	http/http_router.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
