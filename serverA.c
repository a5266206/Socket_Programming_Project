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
	int LINK_ID, BW;
	double Length, Velocity, Noise_Power;
	int match = 0;

	//create UDP server socket credit from Beej's Guide page 29, 30
	int serverASocket;
	struct sockaddr_in serverAaddr, AWSaddr;
	socklen_t serverAaddr_length, AWS_length;
	serverAaddr_length = sizeof(serverAaddr);
	AWS_length = sizeof(AWSaddr);

	serverASocket = socket(PF_INET, SOCK_DGRAM, 0);

	//ServerA descriptor credit from Beej's Guide page 19 the old code
	serverAaddr.sin_family = AF_INET;
	serverAaddr.sin_port = htons(21544);
	serverAaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAaddr.sin_zero, '\0', sizeof(serverAaddr.sin_zero));
	//AWS descriptor credit from Beej's Guide page 19 the old code
	AWSaddr.sin_family = AF_INET;
	AWSaddr.sin_port = htons(24544);
	AWSaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(AWSaddr.sin_zero, '\0', sizeof(AWSaddr.sin_zero));

	// bind UDP port
	if(bind(serverASocket, (struct sockaddr *)&serverAaddr, serverAaddr_length) == -1){
		printf("Error! bind on server A \n");
	}
	printf("The Server A is up and running using UDP on port <21544>. \n");

	while(1){
		// receive link ID from AWS	
		int buf[1];
		if(recvfrom(serverASocket, buf, sizeof(buf), 0, (struct sockaddr*)&AWSaddr, &AWS_length) == -1){
			printf("Error! receive LINK_ID from AWS on server A \n");
		}
		int recv_ID = buf[0];
		printf("The Server A received input <%d>. \n", recv_ID);
		//load database_a.csv file
		FILE* ptr = fopen("database_a.csv", "r");
		if( ptr == NULL){
			printf("Error! file open\n");
		}
		while((fscanf(ptr, "%d, %d, %lf, %lf, %lf", &LINK_ID, &BW, &Length, &Velocity, &Noise_Power)) != EOF){
			if(LINK_ID == recv_ID){
				match = 1;
				break;
			}
		}
		fclose(ptr);
		//put results to a buff for return
		double buff[5] = {-1.0};
		if(match == 1){
			buff[0] = LINK_ID;
			buff[1] = BW;
			buff[2] = Length;
			buff[3] = Velocity;
			buff[4] = Noise_Power;
		}
		printf("The Server A has found <%d> match. \n", match);
		if(sendto(serverASocket, buff, sizeof(buff), 0, (struct sockaddr*)&AWSaddr, AWS_length) == -1){
			printf("Error! send information to AWS on server A \n");
		}
		printf("The Server A finished sending the output to AWS. \n\n");
		match = 0;
	}
	close(serverASocket);
	return 0;
}