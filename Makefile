TARGET = fpe.so
CFLAGS = -g
HEADERS = fpe.c build_table.c rank.c unrank.c to_dfa.c to_nfa.c fpeproc.c
OBJECTS = $(patsubst %.c, %.o, $(HEADERS)) aescrypt.o aeskey.o aes_modes.o aestab.o 
NFA = to_nfa.c to_dfa.c fpeproc.c
YY = fpe.c fpe.h rank.c unrank.c build_table.c aescrypt.c aes.h aestab.c aes_modes.c aeskey.c $(NFA)


default: $(TARGET)
all: default

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c -fPIC $< -o $@

aes: aescrypt.c aeskey.c aestab.c aes_modes.c
	gcc -c -O2 -fPIC -fomit-frame-pointer aescrypt.c aeskey.c aestab.c aes_modes.c

$(TARGET): $(OBJECTS)
	gcc -shared -o $@ $(OBJECTS)

