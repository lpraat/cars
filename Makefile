DEBUG=-Wall -Wextra -g -O0 -DDEBUG -fsanitize=address 
INCLUDE=-I./src

debug:
	$(CC) $(INCLUDE) $(DEBUG) main.c -o build/main

clean:
	rm -r build/*