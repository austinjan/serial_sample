CC = gcc
CFLAGS = -Wall -Wextra

all: sender receiver

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

receiver: receiver.c
	$(CC) $(CFLAGS) -o receiver receiver.c

test: sender receiver
	@echo "Starting receiver in the background..."
	./receiver &
	@sleep 1
	@echo "Sending test command..."
	./sender
	@echo "Testing complete."
	@killall receiver

clean:
	rm -f sender receiver
