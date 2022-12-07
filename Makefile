all:
	gcc -g server.c -o server -lpthread
	gcc -g client.c -o client -lpthread
