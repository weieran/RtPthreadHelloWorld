/*
 * can_main.cpp
 *
 *  Created on: May 28, 2016
 *      Author: weieran
 *
 * Data copied from: https://en.wikipedia.org/wiki/SocketCAN
 */

#include "can_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

namespace
{

struct can_frame frame;
int can_socket;

void write_can_data()
{
  int nbytes;


  frame.can_id  = 0x123;
  frame.can_dlc = 2;
  frame.data[0] = 0x11;
  frame.data[1] = 0x22;

  nbytes = write(can_socket, &frame, sizeof(struct can_frame));

  printf("Wrote %d bytes\n", nbytes);
}

}

int init_can()
{
    struct sockaddr_can addr;

    struct ifreq ifr;

    char ifname[] = "vcan0";

    if((can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
	    perror("Error while opening socket");
	    return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(can_socket, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

    if(bind(can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
	    perror("Error in socket bind");
	    return -2;
    }
    return 0;
}

void can_main()
{

  write_can_data();

}

