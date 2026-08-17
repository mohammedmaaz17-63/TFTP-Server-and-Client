#ifndef TFTP_H
#define TFTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <errno.h>


#define PORT 6969
#define BUFFER_SIZE 516
#define TIMEOUT_SEC 5

typedef enum {
    RRQ = 1,
    WRQ = 2,
    DATA = 3,
    ACK = 4,
    ERROR = 5
} tftp_opcode;

typedef struct {
    uint16_t opcode;
    union {

        struct {
            char filename[256];
            char mode[8];
        } request;

        struct {
            uint16_t block_number;
            char data[512];
        } data_packet;

        struct {
            uint16_t block_number;
        } ack_packet;

        struct {
            uint16_t error_code;
            char error_msg[512];
        } error_packet;

    } body;

} tftp_packet;


void send_file(int sockfd, struct sockaddr_in client_addr,
               socklen_t client_len, char *filename);

void receive_file(int sockfd, struct sockaddr_in client_addr,
                  socklen_t client_len, char *filename);

#endif