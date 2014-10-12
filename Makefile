GCC = gcc
CFLAGS = -O

all: receiver_main sender_main

receiver_main: receiver_main.c
	$(GCC) $(CFLAGS) $? -o $@

sender_main: sender_main.c
	$(GCC) $(CFLAGS) $? -o $@

clean:
	rm receiver_main sender_main