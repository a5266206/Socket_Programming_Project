#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	//create TCP client socket credit from Beej's Guide page 19 the old code.
	int clientSocket;
	struct sockaddr_in AWSaddr;

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	AWSaddr.sin_family = AF_INET;
	AWSaddr.sin_port = htons(25544);
	AWSaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(AWSaddr.sin_zero, '\0', sizeof(AWSaddr.sin_zero));

	printf("The client is up and running. \n");

	//establish a connection to the server
	if(connect(clientSocket, (struct sockaddr*)&AWSaddr, sizeof(AWSaddr)) == -1){
		printf("Error connect to the AWS \n");
	}
	//Data communications
	//Send function
	int message[3];
	message[0] = atoi(argv[1]);
	message[1] = atoi(argv[2]);
	message[2] = atoi(argv[3]);
	send(clientSocket, message, sizeof(message), 0);
	printf("The client send ID=<%d>, size=<%d>, and power=<%d> to AWS. \n", message[0], message[1], message[2]);
	
	//Receive function
	double buff[1];
	if(recv(clientSocket, buff, sizeof(buff), 0) == -1){
		printf("Error receive on client \n");
	}
	if(buff[0] == -1.0){
		printf("Found no matches for link <%d>. \n", message[0]);
	}else{
		printf("The delay for link <%d> is <%.2f>ms. \n", message[0], buff[0]);
	}
	printf("\n");
	close(clientSocket);
	return 0;
}