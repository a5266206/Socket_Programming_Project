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
	int serverBSocket;
	struct sockaddr_in serverBaddr, AWSaddr;
	socklen_t serverBaddr_length, AWS_length;
	serverBaddr_length = sizeof(serverBaddr);
	AWS_length = sizeof(AWSaddr);

	serverBSocket = socket(PF_INET, SOCK_DGRAM, 0);

	//ServerA descriptor credit from Beej's Guide page 19 the old code
	serverBaddr.sin_family = AF_INET;
	serverBaddr.sin_port = htons(22544);
	serverBaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverBaddr.sin_zero, '\0', sizeof(serverBaddr.sin_zero));
	//AWS descriptor credit from Beej's Guide page 19 the old code
	AWSaddr.sin_family = AF_INET;
	AWSaddr.sin_port = htons(24544);
	AWSaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(AWSaddr.sin_zero, '\0', sizeof(AWSaddr.sin_zero));

	// bind UDP port
	if(bind(serverBSocket, (struct sockaddr *)&serverBaddr, serverBaddr_length) == -1){
		printf("Error! bind on server B \n");
	}
	printf("The Server B is up and running using UDP on port <22544>. \n");

	while(1){
		// receive link ID from AWS	
		int buf[1];
		if(recvfrom(serverBSocket, buf, sizeof(buf), 0, (struct sockaddr*)&AWSaddr, &AWS_length) == -1){
			printf("Error! receve LINK_ID from AWS on server B \n");
		}
		int recv_ID = buf[0];
		printf("The Server B received input <%d>. \n", recv_ID);
		//load database_b.csv file
		FILE* ptr = fopen("database_b.csv", "r");
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
		double buff[5] = {-1.0};
		if(match == 1){
			buff[0] = LINK_ID;
			buff[1] = BW;
			buff[2] = Length;
			buff[3] = Velocity;
			buff[4] = Noise_Power;
		}
		printf("The Server B has found <%d> match. \n", match);
		//send buff to AWS
		if(sendto(serverBSocket, buff, sizeof(buff), 0, (struct sockaddr*)&AWSaddr, AWS_length) == -1){
			printf("Error! send information to AWS on server B \n");
		}
		printf("The Server B finished sending the output to AWS. \n\n");
		match = 0;
	}
	close(serverBSocket);
	return 0;
}