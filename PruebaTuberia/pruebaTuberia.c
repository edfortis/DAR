// gcc usb_exp.c -lusb-1.0
//==========================================
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>


#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>


int main(int argc, char *argv[]) 
{
	int i = 0;
	i = obtener_usb();
	printf("%d \n",i);
	return i;
	
}

 int obtener_usb()
 {
   libusb_device                    **devList = NULL;
   libusb_device                    *devPtr = NULL;
   libusb_device_handle             *devHandle = NULL;
   struct libusb_device_descriptor  devDesc;

   unsigned char              strDesc[256];
   ssize_t                    numUsbDevs = 0;      // pre-initialized scalars
   ssize_t                    idx = 0;
   int                        retVal = 0;
   int 						  bandera = 0;

   retVal = libusb_init (NULL);
   //========================================================================
   // Get the list of USB devices visible to the system.
   //========================================================================
   numUsbDevs = libusb_get_device_list (NULL, &devList);
   
    while (idx < numUsbDevs)
   {
	   
	   devPtr = devList[idx];

      retVal = libusb_open (devPtr, &devHandle);
      if (retVal != LIBUSB_SUCCESS)
         break;

      //=====================================================================
      // Get the device descriptor for this device.
      //=====================================================================

      retVal = libusb_get_device_descriptor (devPtr, &devDesc);
      if (retVal != LIBUSB_SUCCESS)
         break;

	    if (devDesc.iSerialNumber > 0)
      {
		 
         retVal = libusb_get_string_descriptor_ascii
                  (devHandle, devDesc.iSerialNumber, strDesc, 256);
         if (retVal < 0)
            break;
         

         //printf ("  iSerialNumber = %s\n", strDesc);
         if (strcmp(strDesc,"4C530302001204104550") == 0)
		 {
			//1 = conectada
			bandera = 1;
		 }
      }
     
      
      libusb_close (devHandle);
      devHandle = NULL;
      idx++;
   }
   
   if (devHandle != NULL)
   {
      //========================================================================
      // Close device if left open due to break out of loop on error.
      //========================================================================
	 
      libusb_close (devHandle);
   }   
	 
   libusb_exit (NULL);

return bandera;

}



 
