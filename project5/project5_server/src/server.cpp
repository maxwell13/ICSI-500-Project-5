/* Program: server.c
 * A simple TCP server using sockets.
 * Server is executed before Client.
 * Port number is to be passed as an argument.
 *
 * To test: Open a terminal window.
 * At the prompt ($ is my prompt symbol) you may
 * type the following as a test:
 *
 * $ ./server 54554
 * Run client by providing host and port
 *
 *
 */
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


#define MAX 10

using namespace std;

// Declaring global variables
int sum_B = 0;
int consumerCount1 = 0;

//holds the total length of the input to be process
int lengthInput=-1;

//
char repChar;


// Shared queues
queue<char> Q1;
queue<char> Q2;
queue<char> Q3;




void* reader(void*);
void* character(void*);
void* toUpper(void*);
void* writer(void*);

// mutex lock avoids race conditon Q
pthread_mutex_t mutex;

//signals for all threads
pthread_cond_t cond;

string message;


void* reader(void*)
{
	repChar='$';
    static int readerCount = 0;
    std::string input="this is a test input message";
    int num = 0;
    lengthInput = input.length();



    while (num < lengthInput) {

        // Getting the lock on queue using mutex
        pthread_mutex_lock(&mutex);

        //executes if Q is not full
        if (Q1.size() < MAX)
        {

            cout << "read:  " << input[num] << endl;

            // Pushing the char into queue
            Q1.push(input[num]);

            num++;

            //broadcast done with cond
            pthread_cond_broadcast(&cond);
        }
        // else If some other thread is exectuing, wait
        else {
            cout << ">> reader is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);
        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }

}

// Function definition for consumer thread B
void* character(void*)
{

	int num=0;
    while (1) {

        // Getting the lock on queue1 using mutex
        pthread_mutex_lock(&mutex);

        // Pop only when queue has at least 1 element and queue 2 has less than 10 elements
        if (Q1.size() > 0 && Q2.size() < MAX) {
            // Get the data from the front of queue
            char data = Q1.front();

            cout << "B thread consumed: " << data << endl;

            // Pop the consumed data from queue
            Q1.pop();

            //if data is a blank replace
            if(data==' ')
            {
            	Q2.push(repChar);
            }
            else
            {
            	Q2.push(data);
            }

            num++;

            pthread_cond_broadcast(&cond);
        }
        else if(num==lengthInput )
        {
        	pthread_mutex_unlock(&mutex);
        	 return NULL;

        }
        // If some other thread is exectuing, wait
        else {
            cout << "B is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);
        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }
}
// Function definition for consumer thread B
void* toUpper(void*)
{
	int num=0;
    while (1) {

        // Getting the lock on queue2 using mutex
        pthread_mutex_lock(&mutex);

        // Pop only when queue has at least 1 element
        if (Q2.size() > 0 && Q3.size() < MAX ) {
            // Get the data from the front of queue
            char data = Q2.front();

            cout << "toUpper thread consumed: " << data << endl;

            // Pop the consumed data from queue
            Q2.pop();


              //if lowercase char
                if( data <= 'z' && data >= 'a') {
                  //adjust ascii
                	data = data - 32;
                }

            	Q3.push(data);

            num++;

            pthread_cond_broadcast(&cond);
        }
        else if(num==lengthInput )
        {
        	pthread_mutex_unlock(&mutex);
        	 return NULL;

        }
        // If some other thread is exectuing, wait
        else {
            cout << "upper is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);

        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }
}

// Function definition for consumer thread B
void* writer(void*)
{
cout << "toUpper thread consumed: ";

	int num=0;
    while (1) {

        // Getting the lock on queue2 using mutex
        pthread_mutex_lock(&mutex);

        // Pop only when queue has at least 1 element
        if (Q3.size() > 0) {
            // Get the data from the front of queue
            char data = Q3.front();

            cout << "writeter: " << data << endl;

            message = message + data;

            // Pop the consumed data from queue
            Q3.pop();


            num++;

            pthread_cond_broadcast(&cond);
        }
        else if(num==lengthInput )
        {
        	pthread_mutex_unlock(&mutex);
        	 return NULL;

        }
        // If some other thread is exectuing, wait
        else {
            cout << "writer is in wait.." << endl;
            pthread_cond_wait(&cond, &mutex);
        }

        // Get the mutex unlocked
        pthread_mutex_unlock(&mutex);
    }
}










void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{




     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

//     //if no port
//     if (argc < 2) {
//         fprintf(stderr,"ERROR, no port provided\n");
//         exit(1);
//     }

//     fprintf(stdout, "Run client by providing host and port\n");

     //socket creation
    /*int sockfd = socket(domain, type, protocol)
     sockfd: socket descriptor, an integer (like a file-handle)
    domain: integer, communication domain e.g., AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol)
    type: communication type
    SOCK_STREAM: TCP(reliable, connection oriented)
    SOCK_DGRAM: UDP(unreliable, connectionless)
    protocol: Protocol value for Internet Protocol(IP), which is 0*/

//
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0)
//        error("ERROR opening socket");

//    //void bzero(void *s, size_t n); [Option End]
//    //The bzero() function shall place n zero-valued bytes in the area pointed to by s
//     bzero((char *) &serv_addr, sizeof(serv_addr));
//
//    //int atoi (const char * str);
//    //converts string to integer
//     portno = atoi(argv[1]);
//
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_addr.s_addr = INADDR_ANY;
//     serv_addr.sin_port = htons(portno);
//
//     //bind function binds the socket to the address and
//     //port number specified in addr(custom data structure).
//     // if binding was succesful
//     if (bind(sockfd, (struct sockaddr *) &serv_addr,
//              sizeof(serv_addr)) < 0)
//              error("ERROR on binding");
//
//    //It puts the server socket in a passive mode,
//    //where it waits for the client to approach the server to make a connection. T
//     listen(sockfd,5);
//     clilen = sizeof(cli_addr);
//
//     /*accept extracts the first connection request on the queue of
//         pending connections for the listening socket, sockfd, creates a new connected socket,
//         and returns a new file descriptor referring to that socke */
//     newsockfd = accept(sockfd,
//                 (struct sockaddr *) &cli_addr,
//                 &clilen);
//     if (newsockfd < 0)
//        error("ERROR on accept");
//
//
//     bzero(buffer,256);
//
//
//     //  read - read from a file descriptor
//     n = read(newsockfd,buffer,255);
//     if (n < 0)
//        error("ERROR reading from socket");
//
//         printf("Here is the message: %s\n",buffer);

    //holds the input so it doesnt change
    char bufHold[256];
    memcpy(bufHold,buffer,256);

    std::string testInput="this is a test input message";

    // Declaring integers used to
    // identify the thread in the system
    pthread_t readerThread, characterThread,upperThread,writeThread;

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
//


cout << '\n' << "the final message is " << message;
//
//     n = write(newsockfd,"I got your message",18);
//     if (n < 0)
//        error("ERROR writing to socket");
//     close(newsockfd);
//     close(sockfd);
//     return 0;
}

//
//void toUpper(char * fileName)
//{
//
//puts("perfroming lowercase to uppercase following file: ");
//puts(fileName);
//
//int c;
//FILE *file ;
//FILE *tempFile ;
//char *line = NULL;
//size_t len = 0;
//ssize_t read;
//
//
////opens file and temp file
//file = fopen(fileName, "r");
//if(file == NULL)
//   {
//      puts("Error in opening given file");
//      exit(1);
//   }
//
//
//tempFile = fopen("fileUpper.txt", "w");
//if(tempFile == NULL)
//   {
//      puts("Error! 2");
//      exit(1);
//   }
//
//
//
//
//int currentLine =0;
//
//while ((read = getline(&line, &len, file)) != -1) {
//    currentLine++;
//
//     for (int i = 0; line[i]!='\0'; i++) {
//    //if lowercase char
//      if( line[i] <= 'z' && line[i] >= 'a') {
//
//        //adjust ascii
//         line[i] = line[i] - 32;
//      }
//   }
//     // printf("%s", line);
//    fprintf(tempFile,"%s", line);
//}
//
//
//fclose(file);
//fclose(tempFile);
//
//// remove(fileName);
//// rename("temp.txt", fileName);
//
//}


int count(char * fileName, char sought)
{

puts("counting the number of entered charaters in the following file: ");
puts(fileName);

int charCount=0;
FILE *file ;
FILE *tempFile ;




char *line = NULL;
size_t len = 0;
ssize_t read;


//opens file
file = fopen(fileName, "r");
if(file == NULL)
   {
      puts("Error!");
      exit(1);
   }





int currentLine =0;

while ((read = getline(&line, &len, file)) != -1) {
    currentLine++;

     for (int i = 0; line[i]!='\0'; i++) {
    //if lowercase char
      if( line[i] == sought ) {
        charCount++;
      }
   }
     // printf("%s", line);
    // fprintf(tempFile,"%s", line);
}


fclose(file);

tempFile = fopen("fileChar.txt", "w");
if(tempFile == NULL)
   {
      puts("Error!");
      exit(1);
   }

fprintf(tempFile,"%d", charCount);


fclose(tempFile);
return charCount;
}

//many comments taken from https://www.geeksforgeeks.org/socket-programming-cc/
