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

int main()
{
	//create TCP client socket credit from Beej's Guide page 19 the old code.
	int monitorSocket;
	struct sockaddr_in AWSaddr;
	printf("The monitor is up and running. \n");
	while(1){
		monitorSocket = socket(PF_INET, SOCK_STREAM, 0);

		AWSaddr.sin_family = AF_INET;
		AWSaddr.sin_port = htons(26544);
		AWSaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(AWSaddr.sin_zero, '\0', sizeof(AWSaddr.sin_zero));

		//establish a connection to the server
		if(connect(monitorSocket, (struct sockaddr*)&AWSaddr, sizeof(AWSaddr)) == -1){
			printf("Error! connect to the AWS \n");
		}
		//receive message
		int message[3];
		if(recv(monitorSocket, message, sizeof(message), 0) == -1){
			printf("Error! receive message from AWS on moniotr \n");
		}
		printf("The monitor received link ID=<%d>, size=<%d>, and power=<%d> from the AWS. \n", message[0], message[1], message[2]);
		//receive answer
		//0:Delay , 1:Tt , 2:Tp
		double buff[3];
		if(recv(monitorSocket, buff, sizeof(buff), 0) == -1){
			printf("Error! receive answer from AWS on moniotr \n");
		}
		if(buff[0] == -1.0){
			printf("Found no matches for link <%d>. \n", message[0]);
		}else{
			printf("The result for link <%d>: \nTt = <%.2f>ms,\nTp = <%.2f>ms,\nDelay = <%.2f>ms \n", message[0], buff[1], buff[2], buff[0]);
		}
		printf("\n");
		close(monitorSocket);
	}
	return 0;
}