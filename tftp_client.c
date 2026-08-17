#include "tftp.h"  
#include "tftp_client.h"   



int main() {
    char command[256];                           // Buffer to store user command from terminal
    tftp_client_t client;                        // Structure variable to store client details (socket, server address etc.)

    memset(&client, 0, sizeof(client));          // Initialize entire client structure with zeros to avoid garbage values

                                                 // Main loop for command-line interface
    while (1) {                                  // Infinite loop to continuously accept commands from user
        printf("tftp> ");                        // Display TFTP command prompt

        fgets(command, sizeof(command), stdin);  // Read command entered by user from keyboard

                                                 // Remove newline character
        command[strcspn(command, "\n")] = 0;     // Replace newline '\n' with NULL character to clean the string

                                                 // Process the command
        process_command(&client, command);       // Call function to interpret and execute the command
    }

    return 0;                                    // End of main function
}



                                                 // Function to process commands
void process_command(tftp_client_t *client, char *command) 
{
    char cmd[20], arg[100], ip[50];              // Variables to store command type, filename argument and server IP
    int port;                                    // Variable to store server port number

    if(sscanf(command,"connect %s %d",ip,&port)==2)   // Check if command is "connect <ip> <port>"
    {
        connect_to_server(client,ip,port);            // Call function to connect to server
    }

    else if(sscanf(command,"get %s",arg)==1)           // Check if command is "get <filename>"
    {
        get_file(client,arg);                          // Call function to download file from server
    }

    else if(sscanf(command,"put %s",arg)==1)           // Check if command is "put <filename>"
    {
        put_file(client,arg);                          // Call function to upload file to server
    }

    else if(strcmp(command,"quit")==0)                 // Check if command is "quit"
    {
        disconnect(client);                            // Disconnect from server
        exit(0);                                       // Exit program
    }

    else
        printf("Invalid command\n");                   // Display error if command is not recognized
}



                                                       // This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client, char *ip, int port) {

    // Create UDP socket
    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);   // Create UDP socket using IPv4 protocol

    if (client->sockfd < 0)                            // Check if socket creation failed
    {
        perror("socket");                              // Print error message
        exit(1);                                       // Terminate program
    }
  

    // Set socket timeout option
    struct timeval timeout;                            // Structure used to specify timeout value

    timeout.tv_sec = TIMEOUT_SEC;                      // Timeout seconds defined in macro
    timeout.tv_usec = 0;                               // Microseconds set to zero

    if (setsockopt(client->sockfd, SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout)) < 0)     // Set receive timeout option for socket
    {
        perror("setsockopt");                          // Print error message if option fails
        exit(1);                                       // Exit program
    }


    // Set up server address
    memset(&client->server_addr, 0, sizeof(client->server_addr)); // Clear server address structure

    client->server_addr.sin_family = AF_INET;                     // Specify IPv4 address family

    client->server_addr.sin_port = htons(port);                   // Convert port number to network byte order

    if (inet_pton(AF_INET, ip, &client->server_addr.sin_addr) <= 0) // Convert IP string to binary form
    {
        printf("Invalid IP address\n");                           // Display error if IP is invalid
        exit(1);                                                  // Exit program
    }

    client->server_len = sizeof(client->server_addr);             // Store size of server address structure

    // Store server IP
    strcpy(client->server_ip, ip);                                // Copy IP address into client structure

    printf("Connected to server %s:%d\n",ip,port);                // Display successful connection message

}



void put_file(tftp_client_t *client, char *filename)
{
    send_request(client->sockfd,client->server_addr,filename,WRQ);       // Send Write Request (WRQ) to server

    receive_request(client->sockfd,client->server_addr,filename,WRQ);    // Start sending file after server ACK
}



void get_file(tftp_client_t *client, char *filename) 
{
    // Send RRQ and receive file 
    send_request(client->sockfd,client->server_addr,filename,RRQ);       // Send Read Request (RRQ) to server

    receive_request(client->sockfd,client->server_addr,filename,RRQ);    // Receive file data from server
}



void disconnect(tftp_client_t *client) 
{
    // close fd
    close(client->sockfd);                                               // Close the client socket file descriptor

    printf("Disconnected\n");                                            // Display disconnection message
   
}



void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
    tftp_packet packet;                                                 // Declare packet structure for TFTP communication

    packet.opcode = htons(opcode);                                      // Store operation code (RRQ or WRQ) in network byte order

    strcpy(packet.body.request.filename, filename);                     // Copy filename into packet

    strcpy(packet.body.request.mode, "octet");                          // Set transfer mode to "octet" (binary mode)

    sendto(sockfd, &packet, sizeof(packet), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));       // Send packet to server using UDP
}



void receive_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
    if(opcode == RRQ)                                                     // If operation is Read Request
    {
        receive_file(sockfd, server_addr, sizeof(server_addr), filename); // Receive file from server
    }
    else                                                                  // Otherwise operation is Write Request
    {
        send_file(sockfd, server_addr, sizeof(server_addr), filename);    // Send file to server
    }
}