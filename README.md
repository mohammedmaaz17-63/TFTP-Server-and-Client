# TFTP Client and Server

## Project Overview

A TFTP (Trivial File Transfer Protocol) client-server application implemented in C that enables file transfer between a client and server using UDP. The client provides a command-line interface with commands such as `connect`, `get`, `put`, and `quit` to communicate with the server and perform file transfers.

The project implements Read Request (RRQ), Write Request (WRQ), DATA, and ACK packets for uploading and downloading files.

## Features

- Client-server communication using UDP
- File upload using `put`
- File download using `get`
- Server connection using `connect`
- Client termination using `quit`
- Read Request (RRQ)
- Write Request (WRQ)
- DATA packet handling
- ACK packet handling
- 512-byte data block transfer
- Socket timeout handling
- Network byte-order conversion

## Technologies Used

- C Programming
- Linux System Calls
- UDP Socket Programming
- Network Programming
- File Handling
- Structures
- Pointers
- Command Line Interface

## TFTP Communication

The project uses UDP sockets for communication between the client and server.

### File Download

```text
Client                         Server
  |                              |
  |          RRQ                 |
  |----------------------------->|
  |                              |
  |          DATA                |
  |<-----------------------------|
  |                              |
  |          ACK                 |
  |----------------------------->|
  |                              |
  |          DATA                |
  |<-----------------------------|
  |                              |
  |          ACK                 |
  |----------------------------->|

### File Upload

```text
Client                         Server
  |                              |
  |          WRQ                 |
  |----------------------------->|
  |                              |
  |          ACK                 |
  |<-----------------------------|
  |                              |
  |          DATA                |
  |----------------------------->|
  |                              |
  |          ACK                 |
  |<-----------------------------|
```

## TFTP Packet Types

| Packet | Description                |
| ------ | -------------------------- |
| RRQ    | Read Request               |
| WRQ    | Write Request              |
| DATA   | Transfers file data        |
| ACK    | Acknowledges received data |

Each DATA packet can carry up to 512 bytes of file data along with its corresponding block number.

## Client Commands

### `connect`

Connects the client to the TFTP server.

```text
connect <server-ip>
```

### `get`

Downloads a file from the server.

```text
get <filename>
```

### `put`

Uploads a file to the server.

```text
put <filename>
```

### `quit`

Terminates the client application.

```text
quit
```

## Key Challenges & Learnings

* Implemented a TFTP client-server architecture using UDP sockets for file transfer.
* Implemented RRQ and WRQ operations along with DATA and ACK packets for uploading and downloading files.
* Faced challenges while handling 512-byte data blocks, block numbers, and network byte-order conversion during file transfer.
* Implemented socket timeout handling and file operations to improve communication reliability.
* Gained practical understanding of Linux socket programming, file handling, and client-server communication.

## Project Structure

```text
TFTP-Client-Server/
│
├── tftp.c
├── tftp.h
├── tftp_client.c
├── tftp_client.h
├── tftp_server.c
├── test_files/
│   ├── test.txt
│   └── document.tftp.txt
│
└── README.md
```

## Compilation

Compile the source files using GCC:

```bash
gcc tftp.c tftp_client.c tftp_server.c -o tftp
```

Run the server and client according to the project implementation.

## Author

**Mohammed Maaz**

GitHub: [https://github.com/mohammedmaaz17-63](https://github.com/mohammedmaaz17-63)

