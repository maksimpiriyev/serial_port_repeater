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
#define BUFFER_LENGTH 1000



static SerialPort* serial1,*serial2;
static int pipe_fd = -1;
static int pipe_dronee = -1;
// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void
signal_callback_handler(int signum)
{
    serial1->close_serial();
    serial2->close_serial();
    close(pipe_fd);
    exit(signum);
}

int main(int argc, const char * argv[]) {
    
    int baudRate1 = 115200;
    const char* port1;
    //const char* pipe = NULL;
    
    port1 = argv[1];
    sscanf(argv[2], "%d",&baudRate1);
    
    
    try{
        static SerialPort sp1(port1,baudRate1);
        sp1.open_serial();
        serial1 = &sp1;
        
        signal(SIGINT, signal_callback_handler);
        
        
        static char buffer1[BUFFER_LENGTH];
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

