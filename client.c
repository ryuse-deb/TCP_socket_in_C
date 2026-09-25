#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
	struct sockaddr_in server_addr = {0};
	
	
	
	server_addr.sin_family		= AF_INET;
	server_addr.sin_port		= htons(5000);
	inet_pton(AF_INET, "192.168.2.101", &server_addr.sin_addr);
	
	int c_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(c_sockfd == -1) {
		puts("socket() : Error");
		return 1;
		}
	
	while(1) {	
		if(connect(c_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
			puts("connect() : erro");
			return 1;
			}
		
		
		while(1) {
			//SEND--------------------------------------------------------------
			char buff[100] = {0};
			printf("Client: ");
			fgets(buff, 100, stdin);
			buff[strcspn(buff, "\n")] = '\0';
			
			int cod_send = send(c_sockfd, buff, strlen(buff), 0);
			
			if(cod_send >= 0 && cod_send < (int)strlen(buff)) {
				puts("Envio parcial, dados possivelmente perdidos");
				}
			else if(cod_send == -1) {
				puts("send() : Erro");
				return 1;
				}


			//RECV--------------------------------------------------------------
			char buff_recv[100];
			int recv_return;
			recv_return = recv(c_sockfd, buff_recv, sizeof(buff_recv), 0);
			if(recv_return > 0) {
				buff_recv[recv_return] = '\0';
				
				if(strcmp(buff_recv, "exit") == 0) {
				if(close(c_sockfd) == -1) {
					puts("Falha ao encerrar conexão");
					return 1;
					}
				puts("Conexão encerrada");
				break;
				}
				printf("Server: %s\n", buff_recv);
				}
				
			else if(strcmp(buff_recv, "exit") == 0) {
				if(close(c_sockfd) == -1) {
					puts("Falha ao encerrar conexão");
					return 1;
					}
				puts("Conexão encerrada");
				break;
				}		
			
			else if(recv_return == 0) {
				printf("Conexão encerrada");
				return 1;
				}
			else {
				printf("recv() : Erro interno");
				return 1;
				}
				puts("");
			}
			break;
		}
	
	return 0;
}

//*socket() -> connect() -> send() / recv() -> shutdown() -> close()*
