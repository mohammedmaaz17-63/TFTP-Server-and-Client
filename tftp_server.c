#include "tftp.h"   

// Function prototype to handle each client request
void handle_client(int sockfd,
                   struct sockaddr_in client_addr,
                   socklen_t client_len,
                   tftp_packet *packet);


int main()
{
    int sockfd;                                              // Variable to store server socket file descriptor

    struct sockaddr_in server_addr, client_addr;             // Structures for server and client addresses

    socklen_t client_len = sizeof(client_addr);              // Variable to store size of client address

    tftp_packet packet;                                      // Structure to store incoming TFTP packets


    sockfd = socket(AF_INET, SOCK_DGRAM, 0);                 // Create UDP socket using IPv4

    if(sockfd < 0)                                           // Check if socket creation failed
    {
        perror("socket");                                    // Print error message
        exit(1);                                             // Exit program
    }


    struct timeval tv;                                       // Structure used to define socket timeout

    tv.tv_sec = TIMEOUT_SEC;                                 // Set timeout seconds (defined in macro)

    tv.tv_usec = 0;                                          // Set microseconds part to 0


    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));  
                                                             // Set socket receive timeout option to avoid blocking forever


    server_addr.sin_family = AF_INET;                        // Specify IPv4 address family

    server_addr.sin_port = htons(PORT);                      // Convert port number to network byte order

    server_addr.sin_addr.s_addr = INADDR_ANY;  
                                                             // Accept connections from any network interface


    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));  
                                                             // Bind socket to server address and port


    printf("TFTP Server listening on port %d...\n",PORT);  
                                                             // Print message indicating server is ready


    while(1)                                                 // Infinite loop to continuously listen for client requests
   {
        int n = recvfrom(sockfd,&packet,BUFFER_SIZE,0,
                    (struct sockaddr *)&client_addr,&client_len);
                                                             // Receive packet from client and store client address


        if(n < 0)                                            // Check if receiving packet failed
       {
            if(errno == EWOULDBLOCK || errno == EAGAIN)
            {
                continue;                                    // Ignore timeout errors and continue waiting
            }

            perror("recvfrom failed");                       // Print error if another error occurred
            continue;                                        // Continue listening for next packet
       }


        handle_client(sockfd,client_addr,client_len,&packet);  
                                                             // Call function to process client request
   }


   close(sockfd);                                            // Close server socket (normally never reached due to infinite loop)
}




void handle_client(int sockfd,
                   struct sockaddr_in client_addr,
                   socklen_t client_len,
                   tftp_packet *packet)
{

    int opcode = ntohs(packet->opcode);  
                                                             // Convert opcode from network byte order to host byte order


    char *filename = packet->body.request.filename;  
                                                             // Extract filename requested by client


    if(opcode == RRQ)                                        // Check if request is Read Request (GET)
    {
        printf("Client requested GET %s\n",filename);  
                                                             // Print requested filename

        send_file(sockfd,client_addr,client_len,filename);  
                                                             // Send file to client
    }


    else if(opcode == WRQ)                                   // Check if request is Write Request (PUT)
   {
        printf("Client requested PUT %s\n",filename);  
                                                             // Print filename client wants to upload


        tftp_packet ack;                                     // Create packet structure for ACK


        ack.opcode = htons(ACK);  
                                                             // Set opcode as ACK and convert to network byte order


        ack.body.ack_packet.block_number = htons(0);  
                                                             // ACK block number 0 (first acknowledgement for WRQ)


        sendto(sockfd,&ack,sizeof(ack),0,
           (struct sockaddr *)&client_addr,client_len);
                                                             // Send ACK packet to client


        receive_file(sockfd,client_addr,client_len,filename);  
                                                             // Start receiving file from client
   }   
}