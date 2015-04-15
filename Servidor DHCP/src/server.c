#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 67
#define PORT_2 68
#define DHCP_CLIENT_PORT        68
#define DHCP_SERVER_PORT        67

#define DHCP_XID_LEN		4
#define DHCP_SECS_LEN		2
#define DHCP_FLAGS_LEN		2

#define DHCP_MAXMAC		6
#define DHCP_CHADDR_LEN         10
#define DHCP_SNAME_LEN          64
#define DHCP_FILE_LEN           128
#define DHCP_OPTIONS_LEN        512
#define DHCP_MIN_OPTIONS_LEN    68
#define DHCP_MAXIP		4

// DHCP
struct dhcp_msg {
  unsigned char op;                       // Peticion o respuesta
  unsigned char htype;                    // Tipo de Hardware
  unsigned char hlen;                     // Direccion fisica
  unsigned char hops;                     // Numero de saltos por maquina al cliente
  unsigned char xid[DHCP_XID_LEN];         // Transaccion ID
  unsigned char secs[DHCP_SECS_LEN];                    // Segundos desde tratado de arrancar la maquina 
  unsigned char flags[DHCP_FLAGS_LEN];                   // Bandera 
  struct in_addr ciaddr;                  // Client IP address (if already in use) - IP del cliente, 0.0.0.0 si no tiene
  struct in_addr yiaddr;                  // Client IP address (if already in use) - My IP
  struct in_addr siaddr;                  // Client IP address (if already in use) - IP del servidor
  struct in_addr giaddr;                  // Client IP address (if already in use) - IP del Gateway
  unsigned char chaddr[DHCP_MAXMAC + DHCP_CHADDR_LEN];  // Direccion fisica del cliente - MAD Adress
  char sname[DHCP_SNAME_LEN];             // Nombre del servidor DHCP
  char file[DHCP_FILE_LEN];               // Nombre del arch de arranque
  unsigned char options[0];               // Optional parameters (actual length dependent on MTU).
};

void printNCharToHexStart(char values[], int numbytes, int startindex){
	int i;
	printf("0x");
	for (i = startindex; i < numbytes; i++){
		printf("%02x", values[i]);
	}
	printf("\n");
}

void printNCharToHex(char values[], int numbytes){
	int i;
	printf("0x");
	for (i = 0; i < numbytes; i++){
		printf("%02x", values[i]);
	}
	printf("\n");
}

void printCharToHex(char values[]){
	int i;
	printf("0x");
	for (i = 0; i < strlen(values); i++){
		printf("%02x", values[i]);
	}
	printf("\n");
}

int responder (struct dhcp_msg message){
	int socket_descriptor; // Descriptor del socket
	int on = 1;
	int iter = 0;
	struct sockaddr_in address; 

	//Inicia la estructura de direcciones de socket para los protocolos
	memset(&(address.sin_zero), '\0', 8);
	
	//address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_family = AF_INET;	
	address.sin_addr.s_addr = inet_addr("255.255.255.255");
	address.sin_port = htons(PORT_2);
	
	//Crear socket UDP
	socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (setsockopt(socket_descriptor, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0){
		perror("Error - BROADCAST setsockopt()");
		return 1;
    }

    //-----------------------------------------------------------
    // Asigno al mensaje original DHCP; una nueva direccion IP
    int iax;
    //Oferta:    
    /*
    	message.op = 2;
    	message.htype = 1;
    	message.hlen = 6;
    	message.hops = 0;    	
		for (iax = 0; iax < DHCP_SECS_LEN; iax++){
			message.secs[iax] = 0;
		}
		for (iax = 0; iax < DHCP_FLAGS_LEN; iax++){
			message.flags[iax] = 0;
		}
    	message.ciaddr.s_addr = inet_addr("0.0.0.0");
    //*/
    // ACK
    	message.op = 2;
    	message.htype = 0x1; // 5
    	message.hlen = 6;
    	message.hops = 0;
		for (iax = 0; iax < DHCP_SECS_LEN; iax++){
			message.secs[iax] = 0;
		}		
		message.flags[0] = 128;
		message.flags[1] = 0;
		message.ciaddr.s_addr = inet_addr("190.180.170.160");
    	message.yiaddr.s_addr = inet_addr("0.0.0.0");
    	message.siaddr.s_addr = inet_addr("0.0.0.0");
    	message.giaddr.s_addr = inet_addr("190.190.190.190");
    	//message.giaddr.s_addr = inet_addr("0.0.0.0");    	
    	for (iax = 6; iax < (DHCP_MAXMAC + DHCP_CHADDR_LEN); iax++){
			message.chaddr[iax] = 0;
		}    	
    	//sprintf(message.sname, " ");//SoraTF-VPCEA37FL
    	for (iax = 0; iax < DHCP_SNAME_LEN; iax++){
			message.sname[iax] = 0;
		}    	
    	for (iax = 0; iax < DHCP_FILE_LEN; iax++){
			message.file[iax] = 0;
		}

	//Enviar datos
	if (sendto(socket_descriptor, &message, sizeof(message), 0, (struct sockaddr *) &address, sizeof(address)) == -1){
		perror("Client - sendto"); // Si algo salio mal al enviar el mensaje
		return 2;
	}
	//-----------------------------------------------------------

	//Cerrar el socket
	printf("Respuesta enviada\n");
	close(socket_descriptor);	
	return 0;
}

int main (int argc, char *argv[]){
	int sin_len;
	char mensaje[256];
	int socket_descriptor;
	struct sockaddr_in sin;
	printf("Esperando datos de remitente\n");

	//Iniciar estructura de direcciones de socket para protocolos
	memset(&(sin.sin_zero), '\0', 8);
 	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(PORT);
	sin_len = sizeof(sin);

	//Crea un socket UDP y unirlo al puerto
	socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
	bind(socket_descriptor, (struct sockaddr *)&sin, sizeof(sin));
		//READ or WRITE FILE
	struct dhcp_msg message;
	//while(1){		
		recvfrom(socket_descriptor, &message, sizeof(message), 0, (struct sockaddr *)&sin, &sin_len);
		int i = 0;
		printf("=========================\n");
		printf("OP: 0x%02x\n", message.op);
		printf("HTYPE:0x%02x \n", message.htype);
		printf("HLEN: 0x%02x\n", message.hlen);
		printf("HOPS: 0x%02x\n", message.hops);
		printf("XID: "); printNCharToHex(message.xid, DHCP_XID_LEN);
		printf("SECS: "); printNCharToHex(message.secs, DHCP_SECS_LEN);
		printf("FLAGS: "); printNCharToHex(message.flags, DHCP_FLAGS_LEN);
		printf("CIADDR: %s\n", inet_ntoa(message.ciaddr));
		printf("YIADDR: %s\n", inet_ntoa(message.yiaddr));
		printf("SIADDR: %s\n", inet_ntoa(message.siaddr));
		printf("GIADDR: %s\n", inet_ntoa(message.giaddr));
		printf("MACADD: %02x:%02x:%02x:%02x:%02x:%02x\n", message.chaddr[0], message.chaddr[1], message.chaddr[2], message.chaddr[3], message.chaddr[4], message.chaddr[5]);
		printf("HADDR: "); printNCharToHexStart(message.chaddr, 10, 6);
		printf("=========================\n");	
	//}

	sleep(1);
	printf("-------------------------------------\n");
	responder (message);
	printf("-------------------------------------\n");
	close(socket_descriptor);	
	return 0;	
}
