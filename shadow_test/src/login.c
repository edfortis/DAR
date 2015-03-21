#include <security/pam_appl.h>
#include <libusb-1.0/libusb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 1024



struct pam_response *reply;



int null_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {

        *resp = reply;
        return PAM_SUCCESS;

}

static struct pam_conv conv = { null_conv, NULL };




int main(int argc, char *argv[]) {
	

        int retval, datos = 0;
        char *user, *pass;
        datos = escuchar();
        
        if(datos > 0){
        
        if(argc == 3) {

                user = argv[1];
                pass = strdup(argv[2]);
                

        } else { 

                fprintf(stderr, "Usage: login [username] [password] \n");
                exit(1);

        }
       
		
        return authenticate("system-auth", user, pass);
		}else{
				printf(" Ingresa la USB apropiada \n");
		}
        

} 

int escuchar() 
{
		int t,bandera = 0;
        char *tuberia = "DAR_FIFO";
        char msj[MAX_BUF];
        char *msj2;

        //abre la tuberia
        t = open(tuberia,O_RDONLY);
        //lee el mensaje desde la tuberia y lo almacena
        read(t,msj,MAX_BUF);
        
        int i=0;
        char* str = "1";
		while( i < MAX_BUF )
		{
			if( msj[i] == *str )
			{
				bandera = 1;
				
			}
			
			++i;
		}
        //imprime el valor que comparte el otro proceso
        printf("Recibido: %s \n",msj);
        //cerramos el tubo
        close(t);

return bandera;
}






 




int authenticate(char *service, char *user, char *pass) {

        pam_handle_t *pamh = NULL;
        int bandera = 0;
        int retval = pam_start(service, user, &conv, &pamh);

        if (retval == PAM_SUCCESS) {

                reply = (struct pam_response *)malloc(sizeof(struct pam_response));
                reply[0].resp = pass;
                reply[0].resp_retcode = 0;

                retval = pam_authenticate(pamh, 0);

                if (retval == PAM_SUCCESS)
							
						fprintf(stdout, "Authenticated\n");
						

                else
						
                        fprintf(stdout, "Not Authenticated\n");

                pam_end(pamh, PAM_SUCCESS);

                return ( retval == PAM_SUCCESS ? 0:1 );

        }

        return ( retval == PAM_SUCCESS ? 0:1 );
}



