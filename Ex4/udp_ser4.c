/**********************************
tcp_ser.c: the source file of the server in tcp transmission 
***********************************/


#include "headsock.h"

#define BACKLOG 10

#define ERRORRATE 20

void str_ser4(int sockfd);                                                        // transmitting and receiving function

int main(void)
{
	int sockfd;
	struct sockaddr_in my_addr;

	srand(time(NULL));

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			//create socket
		printf("error in socket");              //Notice that here SOCK_DGRAM is for UDP protocol
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);  //initialize the buffer
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {           //bind socket
		printf("error in binding");
		exit(1);
	}
	printf("start receiving\n");
	while(1) {
		str_ser4(sockfd);                        // send and receive
	}
	close(sockfd);
	exit(0);
}

void str_ser4(int sockfd)
{
	//UDP FINAL
	char buf[BUFSIZE];
	FILE *fp;
	char recvs[DATALEN];
	struct ack_so ack;
	int end, n = 0;
	int len;
	int ran = 0;
	long lseek = 0;
	end = 0;
	struct sockaddr_in addr;

	len = sizeof (struct sockaddr_in);
	ack.num = 0;
	ack.len = 0;

	printf("receiving data!\n");

	while(!end) {
		if((n=recvfrom(sockfd, &recvs, DATALEN, 0, (struct sockaddr *)&addr, &len)) == -1) {
			printf("error receiving!\n");
			exit(1);
		}
		ran = rand()%100;
		if(ran>=ERRORRATE) {
			if(recvs[n-1] == '\0') {     //at the end of the file
				end = 1;
				n--;
			}

			memcpy((buf+lseek), recvs, n);
			lseek += n;
			
			ack.num = 1;
			ack.len = 0;
				
		}
		else {
			ack.num = 0;
			ack.len = 0;
		}
		if((n=sendto(sockfd, &ack, 2, 0, (struct sockaddr *)&addr, len)) == -1) {
			printf("error sending ack!\n");
			exit(1);
		}	
		
	}
	if ((fp = fopen ("myTCPreceive.txt","wt")) == NULL)
	{
		printf("File doesn't exit\n");
		exit(0);
	}
	fwrite (buf , 1 , lseek , fp);					//write data into file
	fclose(fp);
	printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);

}
