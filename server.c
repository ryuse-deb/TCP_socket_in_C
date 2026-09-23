#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(5000);
	
	int ssockfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(ssockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	
	listen(ssockfd, 5);
	
	socklen_t c_len = sizeof(client_addr);
	int c_sockfd;
	char buff[100];
	
	while(1) {
		c_sockfd = accept(ssockfd, (struct sockaddr*) &client_addr, &c_len);
		//while(1) {
		recv(c_sockfd, buff, sizeof(buff), 0);
		printf("Msg recebida: %s\n", buff);
		
		//if(buff[0] == '0') close(c_sockfd); }
		close(c_sockfd);
		}
	
	return 0;
}

