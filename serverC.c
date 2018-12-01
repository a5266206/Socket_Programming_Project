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
#include <math.h>

int main()
{
	//create UDP server socket credit from Beeji's Guide page 29, 30
	int serverCSocket;
	struct sockaddr_in serverCaddr, AWSaddr;
	socklen_t serverCaddr_length, AWS_length;
	serverCaddr_length = sizeof(serverCaddr);
	AWS_length = sizeof(AWSaddr);

	serverCSocket = socket(PF_INET, SOCK_DGRAM, 0);

	//ServerC descriptor credit from Beej's Guide page 19 the old code
	serverCaddr.sin_family = AF_INET;
	serverCaddr.sin_port = htons(23544);
	serverCaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverCaddr.sin_zero, '\0', sizeof(serverCaddr.sin_zero));
	//AWS descriptor credit from Beej's Guide page 19 the old code
	AWSaddr.sin_family = AF_INET;
	AWSaddr.sin_port = htons(24544);
	AWSaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(AWSaddr.sin_zero, '\0', sizeof(AWSaddr.sin_zero));

	// bind UDP port
	if(bind(serverCSocket, (struct sockaddr *)&serverCaddr, serverCaddr_length) == -1){
		printf("Error! bind on server C \n");
	}
	printf("The Server C is up and running using UDP on port <23544>. \n");

	while(1){
		//receive message from AWS on serverC socket
		//0:Link_ID 1:File size 2:Signal power 3:Bandwidth 
		//4:Length 5:Velocity 6:Noise Power
		double buff[10];
		if(recvfrom(serverCSocket, buff, sizeof(buff), 0, (struct sockaddr*)&AWSaddr, &AWS_length) == -1){
			printf("Error! receive buff from AWS on server C \n");
		}
		printf("The Server C received link informatilon of link <%.0f>, file size <%.0f>, and signal power <%.0f>. \n", buff[0], buff[1], buff[2]);
		
		//calculate the results
		double Tp = (buff[4])/(buff[5]*10);
		double NP = pow(10,buff[6]/10)/1000;
		double SP = pow(10,buff[2]/10)/1000;
		double C = buff[3]*1000000*log10(1+SP/NP)/log10(2);
		double Tt = (buff[1]/C)*1000;
		double Delay = Tp+Tt;
		//0:delay 1:Tt 2:Tp
		double results[3] = {-1.0};
		results[0] = Delay;
		results[1] = Tt;
		results[2] = Tp;

		// printf("C = %f , SP = %f, NP = %f \n", C, SP, NP);
		// printf("Delay = %f, Tt = %f, Tp = %f \n", results[0], results[1], results[2]);
		printf("The Server C finished the calculation for link <%.0f>. \n", buff[0]);

		//send the results over sercerC socket
		if(sendto(serverCSocket, results, sizeof(results), 0, (struct sockaddr*)&AWSaddr, AWS_length) == -1){
			printf("Error! send results to AWS on server C \n");
		}
		printf("The Server C finished sending the output to AWS. \n\n");
	}
	close(serverCSocket);
	return 0;
}