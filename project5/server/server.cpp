
#include <string>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#include <pthread.h>
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <sys/wait.h>
#include <streambuf>
#include <sstream>
#include <fstream>
#include <thread>

#include "transmitter.h"
#include "Receiver.h"

#define MAX 10

using namespace std;

// Declaring global variables
int sum_B = 0;
int consumerCount1 = 0;

//holds the total length of the intext.txt to be process
int lengthInput = -1;

//
char repChar;


// Shared queues
queue<char> Q1;
queue<char> Q2;
queue<char> Q3;


void *reader(void *);

void *character(void *);

void *toUpper(void *);

void *writer(void *);

// mutex lock avoids race conditon Q
pthread_mutex_t mutex;

//signals for all threads
pthread_cond_t cond;

string message;
string input;

void *reader(void *) {
    
    static int readerCount = 0;
//    std::string intext.txt = "this is a test intext.txt message";
    int num = 0;
    lengthInput = input.length();

    while (num < lengthInput) {

        // Getting the lock on queue using mutex
        pthread_mutex_lock(&mutex);

        //executes if Q is not full
        if (Q1.size() < MAX) {

//            cout << "read:  " << intext.txt[num] << endl;

            // Pushing the char into queue
            Q1.push(input[num]);

            num++;

            //broadcast done with cond
            pthread_cond_broadcast(&cond);
        }
            // else If some other thread is exectuing, wait
        else {
//            cout << ">> reader is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);
        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }

}

// Function definition for consumer thread B
void *character(void *) {

    int num = 0;
    while (1) {

        // Getting the lock on queue1 using mutex
        pthread_mutex_lock(&mutex);

        // Pop only when queue has at least 1 element and queue 2 has less than 10 elements
        if (Q1.size() > 0 && Q2.size() < MAX) {
            // Get the data from the front of queue
            char data = Q1.front();

//            cout << "B thread consumed: " << data << endl;

            // Pop the consumed data from queue
            Q1.pop();

            //if data is a blank replace
            if (data == ' ') {
                Q2.push(repChar);
            } else {
                Q2.push(data);
            }

            num++;

            pthread_cond_broadcast(&cond);
        } else if (num == lengthInput) {
            pthread_mutex_unlock(&mutex);
            return NULL;

        }
            // If some other thread is exectuing, wait
        else {
//            cout << "B is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);
        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }
}

// Function definition for consumer thread B
void *toUpper(void *) {
    int num = 0;
    while (1) {

        // Getting the lock on queue2 using mutex
        pthread_mutex_lock(&mutex);

        // Pop only when queue has at least 1 element
        if (Q2.size() > 0 && Q3.size() < MAX) {
            // Get the data from the front of queue
            char data = Q2.front();

//            cout << "toUpper thread consumed: " << data << endl;

            // Pop the consumed data from queue
            Q2.pop();


            //if lowercase char
            if (data <= 'z' && data >= 'a') {
                //adjust ascii
                data = data - 32;
            }

            Q3.push(data);

            num++;

            pthread_cond_broadcast(&cond);
        } else if (num == lengthInput) {
            pthread_mutex_unlock(&mutex);
            return NULL;

        }
            // If some other thread is exectuing, wait
        else {
//            cout << "upper is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);

        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }
}

// Function definition for consumer thread B
void *writer(void *) {
//    cout << "toUpper thread consumed: ";

    int num = 0;
    while (1) {

        // Getting the lock on queue2 using mutex
        pthread_mutex_lock(&mutex);

        // Pop only when queue has at least 1 element
        if (Q3.size() > 0) {
            // Get the data from the front of queue
            char data = Q3.front();

//            cout << "writeter: " << data << endl;

            message = message + data;

            // Pop the consumed data from queue
            Q3.pop();


            num++;

            pthread_cond_broadcast(&cond);
        } else if (num == lengthInput) {
            pthread_mutex_unlock(&mutex);
            return NULL;

        }
            // If some other thread is exectuing, wait
        else {
//            cout << "writer is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);
        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }
}


void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    int bufferSize = 10240;

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[bufferSize];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

     //if no port
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     fprintf(stdout, "Run client by providing host and port\n");

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");

    //void bzero(void *s, size_t n); [Option End]
    //The bzero() function shall place n zero-valued bytes in the area pointed to by s
     bzero((char *) &serv_addr, sizeof(serv_addr));

    //int atoi (const char * str);
    //converts string to integer
     portno = atoi(argv[1]);


     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     //bind function binds the socket to the address and
     //port number specified in addr(custom data structure).
     // if binding was succesful
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

    //It puts the server socket in a passive mode,
    //where it waits for the client to approach the server to make a connection. T
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     /*accept extracts the first connection request on the queue of
         pending connections for the listening socket, sockfd, creates a new connected socket,
         and returns a new file descriptor referring to that socke */
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
        error("ERROR on accept");


    pid_t c_pid = fork();

    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (c_pid > 0) {
        cout << "decoder process - started " << getpid() << endl;

        bzero(buffer,bufferSize);


         //  read - read from client
         n = read(newsockfd, buffer, bufferSize);
         if (n < 0)
            error("decoder process - ERROR reading from socket");


        //holds the intext.txt so it doesnt change
        char bufHold[bufferSize];
        memcpy(bufHold, buffer, bufferSize);

        string str = bufHold;

        cout << "decoder process - received message: \n" << str << endl;

        // decode message
        recApp{str};

        std::ifstream recfile("filename.outf");
        std::stringstream buf;
        if (recfile.is_open()) {
            buf << recfile.rdbuf();
        }

        repChar = buf.str().at(0);
        input = buf.str().substr(1);


        recfile.close();

        // Declaring integers used to
        // identify the thread in the system
        pthread_t readerThread, characterThread, upperThread, writeThread;

        // Function to create a threads
        // (pthread_create() takes 4 arguments)
        int retreader = pthread_create(&readerThread,
                                       NULL, reader, NULL);
        int retcharacter = pthread_create(&characterThread,
                                          NULL, *character, NULL);
        int retupper = pthread_create(&upperThread,
                                      NULL, *toUpper, NULL);
        int retwrite = pthread_create(&writeThread,
                                      NULL, writer, NULL);


        // pthread_join suspends execution of the calling
        // thread until the target thread terminates
        if (!retreader)
            pthread_join(readerThread, NULL);
        if (!retcharacter)
            pthread_join(characterThread, NULL);
        if (!retupper)
            pthread_join(upperThread, NULL);
        if (!retwrite)
            pthread_join(writeThread, NULL);

        cout << "decoder process - upper case message: \n" << message << endl;

        // write upper message to file
        ofstream upperfile("upper.outf");
        upperfile << message;
        upperfile.close();

        exit(EXIT_SUCCESS);

    } else {
        cout << "encoder process - started " << getpid() << endl;

        std::stringstream buf;
 
        // encode message when upper file is ready
        while (1) {
            std::ifstream upperfile("upper.outf");
            if(upperfile.good()) {
                transApp{"upper.outf"};
                upperfile.close();
                remove("upper.outf");
                break;
            } else {
                cout << "encoder process - waiting ..." << endl;
                //usleep(30000);
                this_thread::sleep_for(chrono::milliseconds(3000) );
            }
        }

        
        std::stringstream encodedMessage;
        std::ifstream upperBinfile("filename.binf");
        if (upperBinfile.is_open()) {
            encodedMessage << upperBinfile.rdbuf();
        }
        upperBinfile.close();
       
        cout << "encoder process - encodedMessage : \n" << encodedMessage.str() << endl;


        // execve(serverEncoder, encoderArgs, nullptr);
        // send message back to client
        n = write(newsockfd, encodedMessage.str().data(), strlen(encodedMessage.str().data()));
        if (n < 0)
           error("encoder process - ERROR writing to socket");
        

        exit(EXIT_SUCCESS);
    } 

    close(newsockfd);
    close(sockfd);
    exit(EXIT_SUCCESS);

}



int count(char *fileName, char sought) {

    puts("counting the number of entered charaters in the following file: ");
    puts(fileName);

    int charCount = 0;
    FILE *file;
    FILE *tempFile;


    char *line = NULL;
    size_t len = 0;
    ssize_t read;


//opens file
    file = fopen(fileName, "r");
    if (file == NULL) {
        puts("Error!");
        exit(1);
    }


    int currentLine = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        currentLine++;

        for (int i = 0; line[i] != '\0'; i++) {
            //if lowercase char
            if (line[i] == sought) {
                charCount++;
            }
        }
        // printf("%s", line);
        // fprintf(tempFile,"%s", line);
    }


    fclose(file);

    tempFile = fopen("fileChar.txt", "w");
    if (tempFile == NULL) {
        puts("Error!");
        exit(1);
    }

    fprintf(tempFile, "%d", charCount);


    fclose(tempFile);
    return charCount;
}

//many comments taken from https://www.geeksforgeeks.org/socket-programming-cc/
