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
	//create TCP socket credit from Beej's Guide page 26
	//client
	int AWSSocket_client, child_AWSSocket_client;
	struct sockaddr_in AWSaddr; //address
	struct sockaddr_storage clientaddr;
	socklen_t clientaddr_length;
	clientaddr_length = sizeof(AWSaddr);

	AWSSocket_client = socket(PF_INET, SOCK_STREAM, 0);
	//AWS descriptor credit from Beej's Guide page 19 the old code
	AWSaddr.sin_family = AF_INET;
	AWSaddr.sin_port = htons(25544);
	AWSaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(AWSaddr.sin_zero, '\0', sizeof(AWSaddr.sin_zero));
	if(bind(AWSSocket_client, (struct sockaddr*)&AWSaddr, sizeof(AWSaddr)) == -1){
		printf("Error! bind on AWS \n");
	}
	//create TCP socket credit from Beej's Guide page 26
	//monitor
	int AWSSocket_monitor, child_AWSSocket_monitor;
	struct sockaddr_in AWSaddr_monitor; //address
	struct sockaddr_storage monitoraddr;
	socklen_t monitoraddr_length;
	monitoraddr_length = sizeof(AWSaddr_monitor);

	AWSSocket_monitor = socket(PF_INET, SOCK_STREAM, 0);
	//AWS descriptor credit from Beej's Guide page 19 the old code
	AWSaddr_monitor.sin_family = AF_INET;
	AWSaddr_monitor.sin_port = htons(26544);
	AWSaddr_monitor.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(AWSaddr_monitor.sin_zero, '\0', sizeof(AWSaddr_monitor.sin_zero));
	if(bind(AWSSocket_monitor, (struct sockaddr*)&AWSaddr_monitor, sizeof(AWSaddr_monitor)) == -1){
		printf("Error! monitor bind on AWS \n");
	}
	printf("The AWS is up and running. \n");
	// listen from client
	if(listen(AWSSocket_client, 10) == -1){
		printf("Error! listen on AWS \n");
	}
	// listen from monitor
	if(listen(AWSSocket_monitor, 10) == -1){
		printf("Error! listen on AWS \n");
	}
	while(1){
		// accept connection from client
		child_AWSSocket_client = accept(AWSSocket_client, (struct sockaddr*)&clientaddr, &clientaddr_length);
		if(child_AWSSocket_client == -1){
			printf("Error! accept connection from client on AWS \n");
		}
		// accept connection from monitor
		child_AWSSocket_monitor = accept(AWSSocket_monitor, (struct sockaddr*)&monitoraddr, &monitoraddr_length);
		if(child_AWSSocket_monitor == -1){
			printf("Error! accept connection from monitor on AWS \n");
		}
		// receive message from client
		int message[3];
		if(recv(child_AWSSocket_client, message, sizeof(message), 0) == -1){
			printf("Error! receive message from client on AWS \n");
		}
		printf("The AWS received link ID=<%d>, size=<%d>, and power=<%d> from the client using TCP over port <25544>. \n", message[0], message[1], message[2]);	
	
		//send message to the monitor
		if(send(child_AWSSocket_monitor, message, sizeof(message), 0) == -1){
			printf("Error! send message to monitor on AWS \n");
		}
		printf("The AWS sent ID=<%d>, size=<%d>, and power=<%d> to the monitor using TCP over port <26544>. \n", message[0], message[1], message[2]);
		
		//Create UDP client socket credit from the Beej's programming guide page 29, 30.
		int UDPSocket;
		struct sockaddr_in AWSaddr_UDP, serverAaddr, serverBaddr, serverCaddr;
		socklen_t AWS_UDP_length, serverAaddr_length, serverBaddr_length, serverCaddr_length;
		AWS_UDP_length = sizeof(AWSaddr_UDP);
		serverAaddr_length = sizeof(serverAaddr);
		serverBaddr_length = sizeof(serverBaddr);
		serverCaddr_length = sizeof(serverCaddr);

		UDPSocket = socket(PF_INET, SOCK_DGRAM, 0);
		//Set up UDP socket descriptor credit from Beej's Guide page 19 the old code
		//AWS_UDP descriptor 
		AWSaddr_UDP.sin_family = AF_INET;
		AWSaddr_UDP.sin_port = htons(24544);
		AWSaddr_UDP.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(AWSaddr_UDP.sin_zero, '\0', sizeof(AWSaddr_UDP.sin_zero));
		//ServerA descriptor
		serverAaddr.sin_family = AF_INET;
		serverAaddr.sin_port = htons(21544);
		serverAaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(serverAaddr.sin_zero, '\0', sizeof(serverAaddr.sin_zero));
		//ServerB descriptor
		serverBaddr.sin_family = AF_INET;
		serverBaddr.sin_port = htons(22544);
		serverBaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(serverBaddr.sin_zero, '\0', sizeof(serverBaddr.sin_zero));
		//ServerC descriptor
		serverCaddr.sin_family = AF_INET;
		serverCaddr.sin_port = htons(23544);
		serverCaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(serverCaddr.sin_zero, '\0', sizeof(serverCaddr.sin_zero));

		//connect with link ID to Server A
		int linkID_data[1];
		linkID_data[0] = message[0];
		if(sendto(UDPSocket, linkID_data, sizeof(linkID_data), 0, (struct sockaddr*)&serverAaddr, serverAaddr_length) == -1){
			printf("Error! send link ID to ServerA on AWS \n");
		}
		printf("The AWS sent link ID=<%d> to Backend-Server<A> using UDP over port <24544>. \n", message[0]);
		double buffA[5];
		if(recvfrom(UDPSocket, buffA, sizeof(buffA), 0, (struct sockaddr*)&serverAaddr, &serverAaddr_length) == -1){
			printf("Error! receive buffA from serverA on AWS \n");
		}

		// connect with link ID to ServerB
		if(sendto(UDPSocket, linkID_data, sizeof(linkID_data), 0, (struct sockaddr*)&serverBaddr, serverBaddr_length) == -1){
			printf("Error! send link ID to ServerB on AWS \n");
		}
		printf("The AWS sent link ID=<%d> to Backend-Server<B> using UDP over port <24544>. \n", message[0]);
		double buffB[5];
		if(recvfrom(UDPSocket, buffB, sizeof(buffB), 0, (struct sockaddr*)&serverBaddr, &serverBaddr_length) == -1){
			printf("Error! receive buffB from serverB on AWS \n");
		}
		// print link ID matches
		if(buffA[0] == -1.0){
			printf("The AWS received <0> matches from Backend-Server<A> using UDP over port <24544>. \n");
		}else{
			printf("The AWS received <1> matches from Backend-Server<A> using UDP over port <24544>. \n");
		}
		if(buffB[0] == -1.0){
			printf("The AWS received <0> matches from Backend-Server<B> using UDP over port <24544>. \n");
		}else{
			printf("The AWS received <1> matches from Backend-Server<B> using UDP over port <24544>. \n");
		}

		// find match or not
		double monitor_buff[3] = {-1.0};
		if(buffA[0] == -1.0 && buffB[0] == -1.0){ // not match
			//send no match to the client
			double noMatch[1];
			noMatch[0] = buffA[0];
			if(send(child_AWSSocket_client, noMatch, sizeof(noMatch), 0) == -1){
				printf("Error! send no match message to client on AWS \n");
			}
			//sent no match to the monitor
			if(send(child_AWSSocket_monitor, monitor_buff, sizeof(monitor_buff), 0) == -1){
				printf("Error! send no match message to monitor on AWS \n");
			}
			printf("The AWS sent No Match to the monitor and the client using TCP over ports <25544> and <26544>, respectively. \n");
		}else{ //match
			//connect and calculate the result to serverC
			double cal_data[10] = {0.0};
			//0:Link_ID 1:File size 2:Signal power 3:Bandwidth 
			//4:Length 5:Velocity 6:Noise Power
			if(buffA[0] == -1.0){
				cal_data[0] = buffB[0];
				cal_data[1] = message[1];
				cal_data[2] = message[2];
				cal_data[3] = buffB[1];
				cal_data[4] = buffB[2];
				cal_data[5] = buffB[3];
				cal_data[6] = buffB[4];
			}else{ //buffB[0]==-1.0
				cal_data[0] = buffA[0];
				cal_data[1] = message[1];
				cal_data[2] = message[2];
				cal_data[3] = buffA[1];
				cal_data[4] = buffA[2];
				cal_data[5] = buffA[3];
				cal_data[6] = buffA[4];
			}
			// send data to serverC
			if(sendto(UDPSocket, cal_data, sizeof(cal_data), 0, (struct sockaddr*)&serverCaddr, serverCaddr_length) == -1){
				printf("Error! send data to ServerC on AWS \n");
			}
			printf("The AWS sent link ID=<%.0f>, size=<%.0f>, power=<%.0f>, and link information to Backend-Server<C> using UDP over port <24544>. \n", cal_data[0], cal_data[1], cal_data[2]);
			double cal_results[3];
			// receive calculated results from serverC
			if(recvfrom(UDPSocket, cal_results, sizeof(cal_results), 0, (struct sockaddr*)&serverCaddr, &serverCaddr_length) == -1){
				printf("Error! receive data from ServerC on AWS \n");
			}
			printf("The AWS received outputs from Backend-Server<C> using UDP over port <24544>. \n");
			// sent delay to the client
			double buff[1];
			buff[0] = cal_results[0];
			if(send(child_AWSSocket_client, buff, sizeof(buff), 0) == -1){
				printf("Error! send buff to client on AWS \n");
			}
			printf("The AWS sent delay=<%.2f>ms to the client using TCP over port <25544>. \n", buff[0]);
			// sent results to the monitor
			//0:Delay , 1:Tt , 2:Tp
			monitor_buff[0] = cal_results[0];
			monitor_buff[1] = cal_results[1];
			monitor_buff[2] = cal_results[2];
			if(send(child_AWSSocket_monitor, monitor_buff, sizeof(monitor_buff), 0) == -1){
				printf("Error! send message to monitor on AWS \n");
			}
			printf("The AWS sent detailed results to the monitor using TCP over port <26544>. \n");
		}
		printf("\n");
		close(UDPSocket);
		close(child_AWSSocket_client);
		close(child_AWSSocket_monitor);
	}	
	close(AWSSocket_client);
	close(AWSSocket_monitor);
	return 0;
}