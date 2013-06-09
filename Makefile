CFLAGS = -g

fpe: fpe.o 
  gcc $(CFLAGS) -o

aes: aescrypt.c aes.h brg_types.h brg_endian.h aesopt.h aestab.h

aeskey.o: aeskey.c aesopt.h aestab.h

aes_modes.o: aes_modes.c aesopt.h

aestab.o: aestab.c aestab.h aesopt.h aes.h
