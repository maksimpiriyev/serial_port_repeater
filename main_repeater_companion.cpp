//
//  main.cpp
//  SerialPortRepeater
//
//  Created by Maksim Piriyev on 7/7/16.
//  Copyright © 2016 Maksim. All rights reserved.
//

#include <iostream>
#include <thread>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "serial_port.h"
#include <mavlink/common/mavlink.h>
#define BUFFER_LENGTH 1000



static SerialPort* serial1,*serial2;

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void
signal_callback_handler(int signum)
{
    serial1->close_serial();
    serial2->close_serial();
    exit(signum);
}

int main(int argc, const char * argv[]) {
    
    int baudRate1 = 115200,baudRate2=115200;
    const char* port1,*port2;
    //const char* pipe = NULL;
    
    printf("%d\n",argc);
    if(argc==3){
        port1 = argv[1];
        port2 = argv[2];
    }else if(argc>=5){
        port1 = argv[1];
        port2 = argv[3];
        sscanf(argv[2], "%d",&baudRate1);
        sscanf(argv[4], "%d",&baudRate2);
    }
    
    try{
        static SerialPort sp1(port1,baudRate1),sp2(port2,baudRate2);
        sp1.open_serial();
        sp2.open_serial();
        serial1 = &sp1; serial2 = &sp2;
        
        signal(SIGINT, signal_callback_handler);
        
        
        static char buffer1[BUFFER_LENGTH],buffer2[BUFFER_LENGTH];
        const int L = 0;
        thread([](){
            while(true){
                int l = sp2._read_port(buffer2,BUFFER_LENGTH);
                if(l>0){
                    sp1._write_port(buffer2, l);
                    
                }
                usleep(5*1000);
            }
            
        }).detach();
        
        
        while(true){
            int l = sp1._read_port(buffer1,BUFFER_LENGTH);
            if(l>0){
                sp2._write_port(buffer1, l);
                static mavlink_message_t msg;
                static mavlink_status_t status;
                
                //  printf("Bytes Received: %d\nDatagram: ", (int)recsize);
                for (int i = 0; i < l; ++i)
                {
                    if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status))
                    {
                        
                        // Packet received
                        printf("\nReceived packet: SYS: %d, COMP: %d, LEN: %d, MSG ID: %d\n", msg.sysid, msg.compid, msg.len, msg.msgid);
                    }
                }
            }
            usleep(5*1000);
            
        }
        
        
    }catch(...){ }
    
    return 0;
}

