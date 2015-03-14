
#include <security/pam_appl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <setjmp.h>

#include <time.h>

struct pam_response *reply;

int null_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
        *resp = reply;
        return PAM_SUCCESS;
}

char* leerCadena(char *msj){
        char cad [100];
        int i;
        for(i = 0; i<100; i++){
                cad[i] = 10;
        }
        printf ("%s",msj);
        fgets (cad, 100, stdin);

        int con = 0;
        char c = ' ';
        while((con < (strlen(cad))) && (c != 10) && (c != 12)){
                c = cad[con] ;
                con++;
        }
        if(con > 1){
                con = con - 1;
        }
        char *cadena;
        cadena = (char *)malloc(con);
        for(i=0; i<con; i++){
                cadena[i] = cad[i];
        }
        return cadena;
}

static struct pam_conv conv = { null_conv, NULL };

int main(int argc, char *argv[]) {
        
        int retval;
        char *user, *pass;
        user = leerCadena("Introduzca su usuario:");

        pass = leerCadena("Introduzca su contraseña:");

        
        if(!authenticate("system-auth", user, pass)){
                //printf("------------------------------------\n");
            mode_t mode = 0666;

            int fd; // Descriptor del FIFO
            int len; // Bytes leidos del FIFO
            char buf[PIPE_BUF];

                //Si no existe el FIFO, lo crea 
            if((mkfifo("DAR_Fifo", mode)) < 0){
                //perror("mkFifo Fail");          
            }        

            // Apertura del FIFO para solo lectura
            if((fd = open("DAR_Fifo", O_RDONLY)) < 0){
                perror("Open Fail");
                exit(EXIT_FAILURE);
            }

            int flag = 0;
            //int con = 0;
                // Lectura y visualizacion de la salida del FIFO hasta EOF
            char cadObj[] = "/media/edfortis";

            time_t now;
            struct tm *tm;
            now = time(0);
            if ((tm = localtime (&now)) == NULL) {
                //printf ("Error extracting time stuff\n");
                return 1;
            }
            int timeOut = (tm->tm_sec) + 5; // Tiempo de espera...
            int timeBarre = (tm->tm_sec);
            
            while(flag == 0){

                now = time(0);
                if ((tm = localtime (&now)) == NULL) {
                    printf ("Error en la preparación del tiempo\n");
                    return 1;
                }
                if((tm->tm_sec) > timeOut){
                    flag = 2;
                }

                if((len = read (fd, buf, PIPE_BUF-1)) > 0){
                    int tam = 0;
                    char cx = buf[tam];;
                    while((tam < (PIPE_BUF-2)) && (cx != 10 && cx != 12)){
                        tam++;
                        cx = buf[tam];
                    }
                    int ix;
                    char *cadAx = (char *)malloc(tam);
                    for(ix = 0; ix < tam; ix++){
                        cadAx[ix] = buf[ix];
                        if((tam <= 1)){
                            ix = tam;
                        }
                    }
                    
                    int result = strncmp(cadObj, cadAx, tam);
                    if(result == 0){
                        
                        if(strlen(cadObj) == strlen(cadAx)){                            
                            flag = 1;
                        }                    
                    } else {
                        flag = 0;
                    }
                }
            }
            close(fd);
            printf("------------------------------------------------------------------------\n");
            if(flag == 1){                
                printf("El usuario, contraseña y anexo son correctos\n");
            }else if(flag == 2){
                printf("El anexo no es correcto\n");
            }
            now = time(0);
            if ((tm = localtime (&now)) == NULL) {
                printf ("Error extracting time stuff\n");
                return 1;
            }
            
        }else{
            printf("El usuario y contraseña no son correctos\n");            
        }
        
        exit(EXIT_SUCCESS);
}   

int authenticate(char *service, char *user, char *pass) {

        pam_handle_t *pamh = NULL;
        int retval = pam_start(service, user, &conv, &pamh);

        if (retval == PAM_SUCCESS) {

                reply = (struct pam_response *)malloc(sizeof(struct pam_response));
                reply[0].resp = pass;
                reply[0].resp_retcode = 0;

                retval = pam_authenticate(pamh, 0);

                //if (retval == PAM_SUCCESS)
                        //fprintf(stdout, "Authenticated\n");
                //else
                        //fprintf(stdout, "Not Authenticated\n");

                pam_end(pamh, PAM_SUCCESS);

                return ( retval == PAM_SUCCESS ? 0:1 );

        }

        return ( retval == PAM_SUCCESS ? 0:1 );
}

