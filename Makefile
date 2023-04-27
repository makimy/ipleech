CFLAGS=-Wall -Werror -Wextra -pedantic -O3
OBJECTS=main.o parser.o scanner.o


all: ipleech

install: ipleech
	install $< /usr/local/bin/$<

uninstall: /usr/local/bin/ipleech
	rm -f $<

ipleech: $(OBJECTS) ipleech.h
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o ipleech
