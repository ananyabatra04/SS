CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra -Wstrict-prototypes `pkg-config --cflags gmp` -gdwarf-4
LFLAGS = `pkg-config --libs gmp` -gdwarf-4
KEYGEN_OBJS = keygen.o numtheory.o randstate.o ss.o
ENCRYPT_OBJS = encrypt.o numtheory.o randstate.o ss.o
DECRYPT_OBJS = decrypt.o numtheory.o randstate.o ss.o

all: cryptography

cryptography: keygen encrypt decrypt
       
keygen: $(KEYGEN_OBJS)
	$(CC) $(CFLAGS) -o keygen $(KEYGEN_OBJS) $(LFLAGS) 
encrypt: $(ENCRYPT_OBJS)
	$(CC) $(CFLAGS) -o encrypt $(ENCRYPT_OBJS) $(LFLAGS) -gdwarf-4
decrypt: $(DECRYPT_OBJS)
	 $(CC) $(CFLAGS) -o decrypt $(DECRYPT_OBJS) $(LFLAGS)
%.o : %.c 
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f keygen encrypt decrypt $(KEYGEN_OBJS) $(ENCRYPT_OBJS) $(DECRYPT_OBJS)
format:
	clang-format -i -style=file *.[ch]
