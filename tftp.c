#include "tftp.h"  


                                                                    // Function used to send a file to client
void send_file(int sockfd, struct sockaddr_in client_addr,
               socklen_t client_len, char *filename)
{
    tftp_packet packet;                                             // Declare TFTP packet structure to hold data packets

    int fd = open(filename,O_RDONLY);                               // Open the requested file in read-only mode

    if(fd < 0)                                                      // Check if file opening failed
    {
        perror("File not found");                                   // Print error message if file does not exist
        return;                                                     // Return from function without sending file
    }

    int block = 1;                                                  // Initialize block number (TFTP data blocks start from 1)

    int n;                                                          // Variable to store number of bytes read from file


    while((n = read(fd,packet.body.data_packet.data,512)) >= 0)
                                                                    // Read up to 512 bytes from file into packet data field
    {
        packet.opcode = htons(DATA);                                // Set opcode to DATA packet and convert to network byte order

        packet.body.data_packet.block_number = htons(block);
                                                                    // Store block number and convert it to network byte order


        sendto(sockfd,&packet,n+4,0,
               (struct sockaddr*)&client_addr,client_len);
                                                                    // Send DATA packet to client
                                                                    // n+4 because header contains opcode(2 bytes) + block number(2 bytes)


        recvfrom(sockfd,&packet,sizeof(packet),0,NULL,NULL);
                                                                    // Wait for ACK packet from client after sending data


        if(n < 512)                                                 // If bytes read are less than 512
            break;                                                  // It means last packet reached, so exit loop


        block++;                                                    // Increment block number for next packet
    }


    close(fd);                                                      // Close the file after sending all data


    printf("File sent successfully\n");                             // Print success message
}






                                                                   // Function used to receive file from client
void receive_file(int sockfd, struct sockaddr_in client_addr,
                  socklen_t client_len, char *filename)
{
    tftp_packet packet;                                            // Declare packet structure to receive incoming packets

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                                                                   // Open file for writing
                                                                   // O_WRONLY → write mode
                                                                   // O_CREAT → create file if it does not exist
                                                                   // O_TRUNC → overwrite file if already exists
                                                                   // 0666 → file permission (read/write for all users)


    int n;                                                         // Variable to store number of bytes received


    while(1)                                                       // Infinite loop to receive data packets
    {
        n = recvfrom(sockfd,&packet,sizeof(packet),0,NULL,NULL);
                                                                   // Receive packet from sender


        if(n < 0)                                                  // Check if receiving packet failed
        {
            perror("recvfrom");                                    // Print error message
            break;                                                 // Exit loop
        }


        if(ntohs(packet.opcode) == DATA)
                                                                   // Check if received packet is DATA packet
        {
            int block = ntohs(packet.body.data_packet.block_number);
                                                                   // Extract block number and convert from network byte order


            write(fd,packet.body.data_packet.data,n-4);
                                                                   // Write received data into file
                                                                   // n-4 removes header size (opcode + block number)


            packet.opcode = htons(ACK);
                                                                   // Change opcode to ACK to acknowledge received packet


            packet.body.ack_packet.block_number = htons(block);
                                                                   // Set ACK block number equal to received DATA block


            sendto(sockfd,&packet,sizeof(packet),0,
                   (struct sockaddr *)&client_addr,client_len);
                                                                   // Send ACK packet back to sender


            /* last packet condition */
            if(n < 516)
            {
                break;                                             // If packet size less than 516 bytes → last packet → exit loop
            }
        }
    }


    close(fd);                                                     // Close file after receiving all data


    printf("File transfer completed\n");                           // Print completion message
}