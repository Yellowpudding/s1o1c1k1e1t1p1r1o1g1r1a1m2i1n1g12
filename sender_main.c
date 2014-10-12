#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
//struct for packet
typedef struct{
	int seq_num;
	char data[512];
}packet;
typedef struct{
	int acknum;
}ack;
int getsocket(char * hostname, unsigned short int hostUDPport, int *sockfd, struct addrinfo * p, struct addrinfo hints)
{
	struct addrinfo *servinfo;
	int rv;
	char c[20];
	sprintf(c, "%d", hostUDPport);
	if ((rv = getaddrinfo(hostname, c, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	//make sockets
	for(p = servinfo; p != NULL; p = p->ai_next){
		if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("talker: socket");
			continue;
		}
		break;
	}
	if(p==NULL){
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	return 0;
	}
}
/*Beej talker.c datagram example*/
void reliablyTransfer(char* hostname, unsigned short int hostUDPport, char* filename, unsigned long long int bytesToTransfer)
{
	FILE * openfile=fopen(filename, "r");
	//recommended buffer size 512 bytes
	int * sockfd=malloc(sizeof(int));
	struct addrinfo hints, *p;
	int numbytes;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	//argv[1] is hostname
	//need to handle errors from return value
	//keep connection and just stop sending when we're done
	int getsockerrorcheck=getsocket(hostname, hostUDPport, sockfd, p, hints);
	int fread_retval=1;
	packet* pktbuf[3]; //for now we send 3 packets at a time
	while(fread_retval>0){
		//argv[2] is the actual message
		int cw=3;
		int i=0;
		//sending loop
		for(;i<cw;i++){
			//allocate new memory for buffer to point to
			packet * newpk=malloc(sizeof(packet));
			//check for error later
			int fread_retval = fread(newpk->data, sizeof(char), 512, openfile);
			newpk->seq_num=i;
			char * packetsend=(char *)newpk;
			if((numbytes = sendto(*sockfd, packetsend, strlen(packetsend), 0, p->ai_addr, p->ai_addrlen)) == -1){
				perror("talker: sendto");
				exit(1);
			}
			pktbuf[i]=newpk;
		}
		//waiting for ack
		//1000 microsecond in 1 millisecond, so 20ms=20,000microsecond
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;
	}
	
	return;
}

int main(int argc, char** argv)
{
	unsigned short int udpPort;
	unsigned long long int numBytes;
	
	if(argc != 5)
	{
		fprintf(stderr, "usage: %s receiver_hostname receiver_port filename_to_xfer bytes_to_xfer\n\n", argv[0]);
		exit(1);
	}
	udpPort = (unsigned short int)atoi(argv[2]);
	numBytes = atoll(argv[4]);
	
	reliablyTransfer(argv[1], udpPort, argv[3], numBytes);
} 
