# sys/socket.h
Header que fornece a interface POSIX para criação e gerenciamento de sockets, incluindo funções, tipos e macros utilizados para comunicação de rede.
No Linux, os sockets seguem o conceito de que "tudo é um arquivo": as operações de rede utilizam descritores de arquivo (file descriptors) semelhantes aos de arquivos comuns.

## socket()
Cria um ponto final de comunicação (endpoint) e retorna um descritor de arquivo associado a ele.
Syntax:
`int socket(int domain, int type, int protocol);`
Onde:
- *domain* : (Domínio/Família de endereços); Define o protocolo de comunicação (ex: AF_INET para IPv4, AF_INET6 para IPv6, ou AF_UNIX para comunicação local).
- *type* : (Tipo de comunicação); Define o comportamento da transmissão (ex: SOCK_STREAM para conexões orientadas a fluxo/TCP ou SOCK_DGRAM para datagramas/UDP).
- *protocol* : (Protocolo específico); Define o protocolo exato a ser usado dentro daquele domínio e tipo. Passar 0 faz o sistema escolher o padrão automático (como TCP para SOCK_STREAM).

Ex. para TCP/IPv4 :
```C
int fd = socket(AF_INET, SOCK_STREAM, 0);
```

**Retorno:** Inteiro >= 0, sendo o descritor do socket; -1 para erro.

Basicamente, socket() cria endpoint e retorna file descriptor.

## bind
Associa o socket a um endereço IP e a uma porta local específicos.
Syntax:
`int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`

Onde:
- *sockfd* : O descritor de arquivo do socket que você quer associar a um endereço (criado previamente com a função socket()).
- *addr* : Um ponteiro para uma estrutura de endereço, que contém o endereço IP e a porta local que o socket vai usar, normalmente struct sockaddr_in para IPv4, definida em <netinet/in.h>.
- *addrlen* : O tamanho em bytes da estrutura de endereço passada no segundo argumento (geralmente calculado com sizeof(struct sockaddr_in)).

**Retorno:** 0 para Success; e -1 para erro (e define a variável global errno com o código do problema).

## listen()
Coloca um socket criado e associado a uma porta em modo passivo, preparando-o para aceitar conexões de entrada de clientes.

Syntax:
`int listen(int sockfd, int backlog);`
Onde:
- *sockfd* : O descritor de arquivo do socket (retornado pela função socket() e já vinculado a um endereço pela função bind()).
- *backlog* : O tamanho máximo da fila de conexões pendentes que podem aguardar aceitação.

**Retorno** : 0 para Success; e -1 em caso de erro (com o código de erro gravado em errno).

## accept()
Normalmente do lado do servidor. Extrai a primeira conexão pendente na fila de um socket que está ouvindo (escuta), criando um novo socket dedicado para essa comunicação
Syntax:
`int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`
Onde:
- *sockfd* : O descritor de arquivo do socket do servidor configurado para escutar conexões (listen).
- *addr* : Um ponteiro para uma estrutura sockaddr que receberá o endereço IP e a porta do cliente conectado.
- *addrlen* : Um ponteiro para uma variável do tipo socklen_t que contém o tamanho da estrutura addr. Tido anteriormente a chamada por "*socklen_t len = sizeof(client_addr);*"

**Retorno** : Retorna um novo File Descriptor (>= 0), do novo socket; Retorna -1 para erro (defie a variável global errno).

## connect()
Normalmente do lado do cliente. Estabelece uma conexão com um endpoint remoto. Em TCP, tal função inicia o processo de estabelecimento da conexão TCP.
Syntax:
`int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);`

Onde:
- *sockfd* : O descritor do arquivo do socket criado pela função socket().
- *serv_addr* : Um ponteiro para a estrutura sockaddr que contém o endereço e a porta do servidor de destino.
- *addrlen* : O tamanho em bytes da estrutura de endereço apontada por serv_addr.

**Return** : 0 para Success; -1 para erro ou falha (saved on errno).

## send()
Envia bytes através de um socket conectado.
Syntax:
`ssize_t send(int sockfd, const void *buf, size_t len, int flags);`
Onde:
- *sockfd* : O descritor de arquivo do socket conectado que vai enviar os dados.
- *buf* : Um ponteiro para o buffer que contém a mensagem ou os dados que você deseja transmitir.
- *len* : O tamanho total dos dados a serem enviados, medido em bytes.
- *flags* : Opções adicionais de transmissão (geralmente definidas como 0 para comportamento padrão).

**Return:** Retorna o número de bytes enviados, porém o valor pode ser menor que o len solicitado, portanto, para implementação robusta, deve-se tratar envio parcial. E -1 para erro (errno user).

## recv()
Recebe bytes de um socket.
Syntax:
`ssize_t recv(int sockfd, void *buf, size_t len, int flags);`
Onde:
- *sockfd* : O descritor de arquivo do socket conectado de onde os dados serão lidos.
- *buf* : O ponteiro para o buffer de memória onde a mensagem recebida será armazenada.
- *len* : O tamanho máximo, em bytes, que o buffer pode suportar.
- *flags* :  Modificadores do comportamento da função (geralmente definidos como 0 ou combinados via operador lógico OR, como MSG_PEEK ou MSG_OOB).

**Return:**
- \>0 : Número de bytes efetivamente recebidos pelo socket se bem-sucedido;
- 0 : Conexão foi encerrada pelo outro lado;
- -1 : Erro, definindo errno com código correspondente.

OBS: O recv() não preserva fronteiras de mensagens porque o TCP gerencia os dados como um fluxo contínuo de bytes, sem saber onde uma mensagem termina e outra começa.

## shutdown()
Desativa parcial ou totalmente a comunicação de um socket.
Sintaxe:
`int shutdown(int socket, int how);`
Onde:
- *sockfd* : O file descriptor do socket que irá ser desligado;
- *how* : Define o tipo de encerramento da conexão. Pode receber os seguintes valores (macros):
	- SHUT_RD (0): desabilita recepções adicionais;
	- SHUT_WR (1): desabilita transmissões adicionais;
	- SHUT_RDWR (2):  desabilita ambas.
 
**Return:**
- 0 : Success;
- -1 : Erro (define cód. em errno).

OBS: shutdown() é diferente de close(), pois shutdown() controla apenas a direção da comunicação do socket.

## Fluxograma
**Server:**
*socket() -> bind() -> listen() -> accept() -> recv() / send() -> shutdown() -> close()*

**Client:**
*socket() -> connect() -> send() / recv() -> shutdown() -> close()*


# netinet/in.h
Define estruturas de dados, constantes, conversões de dados (byte order <-> host byte order) e funções para a família de endereços de internet (IPv4 e IPv6) na programação de sockets.

## Funções de conversão
A rede utiliza obrigatoriamente o padrão Big-Endian (byte mais significativo primeiro), enquanto a maioria dos computadores modernos (arquiteturas x86/x64) utiliza Little-Endian (byte menos significativo primeiro). 
Portanto, para enviar portas de rede(sin_port), inteiros binarios (int, short) ou structs(raw structs) do computador pela rede, deve-se fazer a conversão. Para strings não há a necessidade, visto que cada caractere ocupa apenas um byte.

### htons()
Host TO Network Short; Converte um inteiro de 16 bits da ordem de bytes da máquina para a ordem de bytes da rede.
Normalmente usado ao preencher a estrutura sockaddr_in para configurar um servidor ou cliente.
**Syntax:**
`uint16_t htons(uint16_t hostshort);`
Onde:
- *hosthsort* : O número inteiro de 16 bits (geralmente uma porta de rede, como 80, 443 ou 8080) no formato do seu computador.

**Return:** valor equivalente em network byte order. Não retorna -1 para erro, pois é uma conversão númerica.

### ntohs()
Network TO Host Short; É o inverso de htons(), converte 16 bits de network byte order para host byte order.

**Syntax**:
`uint16_t ntohs(uint16_t netshort);`
Onde:
- *netshort* : O número de 16 bits (geralmente uma porta de rede, como 80 ou 443) que está atualmente no formato da rede.

**Return:** valor convertido para host byte order.Não possui uma condição de erro convencional.

### htonl()
Host TO Network Long; Converte um valor de 32 bits de host byte order para network byte order. Sendo especialmente relevante para endereços IPv4.

**Syntax:**
`uint32_t htonl(uint32_t hostlong);`
Onde:
- *hostlong* : valor inteiro de 32 bits que será convertido para network byte order.

**Return:** O valor de 32bits convertido em formato network byte order (pronto para ser enviado pela rede).

### ntohl()
Network TO Host Long; converte um número inteiro de 32 bits do formato network byte order para o formato host byte order.

**Syntax:**
`uint32_t ntohl(uint32_t netlong);`
Onde:
- *netlong* : Um número inteiro de 32 bits (uint32_t) sem sinal, representando o valor na ordem de bytes da rede.

**Return:** O valor convertido para a ordem de bytes do host local.

## Structs e macros
### struct sockaddr_in e in_addr
É usada para armazenar endereços de rede IPv4 em programação de sockets.

**Struct:**
```C
struct sockaddr_in {
    sa_family_t sin_family;     // Família do endereço (deve ser AF_INET)
    in_port_t   sin_port;       // Porta TCP ou UDP (em Network Byte Order)
    struct in_addr sin_addr;    // Endereço IP (em Network Byte Order)
    unsigned char sin_zero[8];  // Preenchimento (padding) para alinhar com struct sockaddr
};

struct in_addr {
    uint32_t s_addr;            // Endereço IPv4 de 32 bits
};

```

Onde:
* **`sin_family`**: Identifica a família de endereços. Use sempre o valor `AF_INET`.
* **`sin_port`**: O número da porta de comunicação (camada de transporte). Precisa estar convertida para a ordem de bytes da rede usando funções como `htons()`.
* **`sin_addr`**: Uma estrutura do tipo `in_addr` que guarda o endereço IP de 32 bits, também em ordem de bytes da rede (usando `inet_pton()` ou `htons/htonl` ).
* **`sin_zero`**: Campo de padding tradicional presente para compatibilidade de tamanho com struct sockaddr; normalmente não possui significado funcional para o programa.

## Macro INADDR_ANY
Usado para indicar que o servidor deve aceitar conexões destinadas a qualquer endereço local apropriado.
Ex.:
```C
struct sockaddr_in addr;

addr.sin_addr.s_addr = htonl(INADDR_ANY);
```


# arpa/inet.h
Usada principalmente para conversão e manipulação de endereços IP em programas de rede.

## inet_pton()
Converte um endereço de rede IP (IPv4 ou IPv6) em formato de texto (string) para o formato binário estruturado (struct in_addr ou in6_addr). O termo "pton" significa presentation to network.

**Syntax:**
`int inet_pton(int af, const char *restrict src, void *restrict dst);`

Onde:
- *af* : Address Family; A família do endereço. Os valores suportados são:
	- AF_INET : Para endereços IPv4 (ex: "192.168.1.1").
	- AF_INET6 : Para endereços IPv6 (ex: "2001:db8::1").

- *src* : Source; Ponteiro para a string de texto contendo o endereço IP que você deseja converter.
- *dst* : Destination; Ponteiro para o buffer de memória onde a estrutura binária convertida será armazenada.
	- Se af for AF_INET, deve apontar para uma struct in_addr.
	- Se af for AF_INET6, deve apontar para uma struct in6_addr.

**Return:**
- 1 : Success; Conversão bem sucedida;
- 0 : Formato inválido, A string fornecida em src não continha um endereço de rede válido para a família especificada (af).
- -1 : Erro interno. O errno é definida como EAFNOSUPPORT se a família de endereços fornecida em af não for suportada.

## inet_ntop()
Converte um endereço de rede de internet (em formato binário/estruturado) para o formato de texto legível (como IPv4 ou IPv6).

**Syntax:**
`const char *inet_ntop(int af, const void *restrict src, char *restrict dst, socklen_t size);`
Onde:
- *af* : O domínio do endereço. Use AF_INET para IPv4 ou AF_INET6 para IPv6.
- *src* : Ponteiro para a estrutura de dados binária que contém o endereço de rede (geralmente uma estrutura struct in_addr para IPv4 ou struct in6_addr para IPv6).
- *dst* : Ponteiro para o buffer de texto (string) onde o endereço convertido será armazenado.
- *size* : O tamanho disponível no buffer dst. Para evitar erros, utilize as constantes padrão:
	- INET_ADDRSTRLEN (16 bytes) para IPv4.
	- INET6_ADDRSTRLEN (46 bytes) para IPv6.
 
**Return:**
- Success : Retorna um ponteiro para o buffer dst que agora contém a string do endereço.
- Erro : Retorna NULL, e o código do erro é definido em errno (por exemplo, EAFNOSUPPORT se a família de endereços for inválida ou ENOSPC se o tamanho do buffer for muito pequeno).

# unistd.h
Reúne interfaces para operações de baixo nível envolvendo arquivos, processos, diretórios, descritores de arquivo, pipes, execução de programas, controle do processo etc.

Para uso em relação aos sockets, a função essencial é *close()*, para fechar um descritor de arquivo;

## close()
Fecha um descritor de arquivo ativo.

**Syntax**
`int close(int fd);`
Onde:
- *fd* : O descritor de arquivo (file descriptor) que você deseja fechar. Ele é o número inteiro retornado por funções como open(), creat(), pipe() ou socket().

**Return:**
- 0 : Sucesso ao fechar o descritor;
- -1 : Erro; O código do erro específico é armazenado na variável global errno (por exemplo, EBADF se o descritor for inválido).
