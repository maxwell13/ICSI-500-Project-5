/*
 * Simple client to work with server.c program.
 * Host name and port used by server are to be
 * passed as arguments.
 *
 * To test: Open a terminal window.
 * At prompt ($ is my prompt symbol) you may
 * type the following as a test:
 *
 * $./client 127.0.0.1 54554
 * Please enter the message: Programming with sockets is fun!
 * I got your message
 * $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <fstream>

#include "transmitter.h"
#include "Receiver.h"

using namespace std;


void clientError(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{

    string filename;
    cout << "Please enter file name: ";
    cin >> filename;

    char clientDecoder[] = "ClientDecoder";
    char * decoderArgs[] = {clientDecoder, nullptr};

    char clientEncoder[] = "ClientEncoder";
    char * encoderArgs[] = {clientEncoder, strdup(filename.data()), nullptr};



    int bufferSize = 10240;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[bufferSize];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        clientError("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)     &serv_addr,sizeof(serv_addr)) < 0)
        clientError("ERROR connecting");



    pid_t c_pid = fork();

    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (c_pid > 0) {

        cout << "encoder process - started " << getpid() << endl;

        // encode file
        transApp{filename};

        // read encoded content
        std::ifstream binfile("filename.binf");
        std::stringstream buf;
        if (binfile.is_open()) {
            buf << binfile.rdbuf();
        }

        cout << "encoder process - message to send:\n " << buf.str() << endl;

        // send encoded content
        n = write(sockfd, buf.str().data(), strlen(buf.str().data()));
        if (n < 0)
            clientError("encoder process - ERROR writing to socket");
    
        exit(EXIT_SUCCESS);

    } else {

        cout << "decoder process - started " << getpid() << endl;
        bzero(buffer,bufferSize);

        //reads from socket
        n = read(sockfd, buffer, bufferSize);
        if (n < 0)
            clientError("decoder process - ERROR reading from socket");
    
        string str = buffer;

        cout << "decoder process - received message:\n " << str << endl;

        recApp{str};

        cout << "decoded message saved in filename.out" << endl;
        
        // execve(clientDecoder, decoderArgs, nullptr);
        exit(EXIT_SUCCESS);
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}
