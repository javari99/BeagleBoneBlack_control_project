// Definition of the ServerSocket class 
// PBM-CEIT  

#ifndef MySocket_class
#define MySocket_class
#include <iostream>
#include <sys/socket.h>
#include <string.h>
using namespace std;

class mysocket
{
    int nPort;
    struct sockaddr_in serv_addr, cli_addr;
    int sockfd, portno;
public:
    mysocket (int port){
        nPort=port;
    }
    mysocket(){}
    bool Open(){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            cout<<"ERROR opening socket"; return 0;
        }
        memset((char *) &serv_addr, 0, sizeof(serv_addr)); 
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(nPort);
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            cout<<"ERROR on binding"<<endl;  close(sockfd);
            return 0;
        }
        listen(sockfd,5);
        
        return 1;
    }
    void Accept( mysocket &client ){
        int newSock;
        socklen_t clilen;
        clilen = sizeof(cli_addr);
        newSock = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
        if (newSock < 0) 
            cout << "ERROR on accept"<<endl;    
        client.sockfd = newSock;
    }
    int Read(char *buffer, int lenmax){
        int n = read(this->sockfd,buffer,lenmax);
        if (n < 0) cout << "ERROR reading from socket"<<endl;
        return n;
    }
    int Send(const char *buffer){
        int n=send(this->sockfd,buffer,strlen(buffer),0);
        return n;
    }
    void Close(){
        close(sockfd);
    }
    
};


#endif