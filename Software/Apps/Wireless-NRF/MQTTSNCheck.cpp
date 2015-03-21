#include "MQTTSNCheck.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
//this is a very simple packet checker. it simply checks the actual length and the expected length.
bool MQTTSNCheck::verifyPacket(void *(&payload), uint8_t &length) {
    const uint8_t* buf = reinterpret_cast<const uint8_t*> (payload);
    if (buf[0] == 0x00) {
          fprintf(stderr, "ERROR: PACKET INVALID: packet length header is not valid\n");
        return false;
    }
    if (buf[0] == 0x01) {
        fprintf(stderr, "ERROR: PACKET INVALID: packet length is too long to process\n");
        return false;
    }
    if (buf[0] != length) {
       fprintf(stderr, "ERROR: PACKET INVALID: read %d bytes but header states length is %d\n", (int)length, (int) buf[0]);
        return false;
    }
   
    
/*route to fix a bug in RSMB Server. where publish is padded with a LOT of unneeded bits causing packet size to be more than doubled.*/
    if (buf[1] == 0x0C)
    {
        if (buf[0] > 8)
        {
            //if it is bigger than the minmum length
            int end = 0;
            for (int t = 8; t<length; t++) // loop from the 2nd data byte to the last.
            {  end ++;
                if (buf[t] == 0x00)
                {
                    //ITS A NULL BYTE!
                 break;   
                }
            }
            if (end > 0)
            {
                //we have found some null bits starting at end + 7.
             uint8_t newlen =  7 + end; // set new packet length.
             
              char newbuf[newlen];
              newbuf[0] = newlen;
              for (int q=1; q<newlen;q++)
              {
                  newbuf[q] = buf[q];
              }
              memcpy(payload, newbuf, newlen);
        //      payload = newbuf;
               length = newlen;
            }
        }
    }
    
    fprintf(stdout,"0x");
    for (int q = 0; q < length; q++)
       fprintf(stdout, "%.2x", buf[q]);

     
       fprintf(stdout," \n");
  
       
    return true;

}
