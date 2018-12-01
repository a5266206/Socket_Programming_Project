CC = gcc

all:
	$(CC) -o awsoutput aws.c
	$(CC) -o client client.c
	$(CC) -o monitoroutput monitor.c
	$(CC) -o serverAoutput serverA.c
	$(CC) -o serverBoutput serverB.c
	$(CC) -o serverCoutput serverC.c -lm

aws:
	./awsoutput

monitor:
	./monitoroutput

serverA:
	./serverAoutput

serverB:
	./serverBoutput

serverC:
	./serverCoutput
