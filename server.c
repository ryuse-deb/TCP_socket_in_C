#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
	struct sockaddr_in client_addr = {0};
	struct sockaddr_in server_addr = {0};
	
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(5000);
	
	int ssockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(ssockfd == -1) {
		puts("socket() : erro"); return 1;
		}
	
	if(bind(ssockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
		puts("bind() : erro");
		return 1;
		}
	
	if(listen(ssockfd, 5) == -1) {
		puts("listen() : Erro");
		return 1;
		}
	
	socklen_t c_len = sizeof(client_addr);
	int c_sockfd;
	
	while(1) {
		c_sockfd = accept(ssockfd, (struct sockaddr*) &client_addr, &c_len);
		if(c_sockfd == -1) {
			puts("accept() : Erro");
			return 1;
			}
		
		while(1) {
			//RECV---------------------------------------------------------_
			char buff[100];
			int recv_return = recv(c_sockfd, buff, sizeof(buff), 0);
			if(recv_return > 0) {
				buff[recv_return] = '\0';				
				
				if(strcmp(buff, "exit") == 0) {
				if(close(c_sockfd) == -1) {
					puts("Falha ao encerrar conexão");
					return 1;
					}
				puts("Conexão encerrada");
				break;
				}
				printf("Client: %s\n", buff);
				}
			
			else if(recv_return == 0) {
				printf("Conexão encerrada");
				return 1;
				}
			else {
				printf("recv() : Erro interno");
				return 1;
				}
			
			
			//SEND----------------------------------------------------------
			
			char buff_send[100] = {0};
			printf("Server: ");
			fgets(buff_send, sizeof(buff_send), stdin);
			buff_send[strcspn(buff_send, "\n")] = '\0';
			
			int cod_send = send(c_sockfd, buff_send, strlen(buff_send), 0);
			
			if(cod_send >=0 && cod_send < (int)strlen(buff_send)) {
				puts("Envio parcial, dados possivelmente perdidos");
				}
			else if(cod_send == -1) {
				puts("send() : Erro");
				return 1;
				}
				puts("");
			}
			break;
		}
		
	return 0;
}

