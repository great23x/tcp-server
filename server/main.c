#include <stdio.h>
#include <winsock2.h>

#define PORT 6969
#define BACKLOG 100
#define MAX_BUFFER 1024

void processMessage(int fd, int* clientArr){
    char buffer[MAX_BUFFER] = {0};
    int nRet = recv(fd, buffer, MAX_BUFFER,0);
    if(nRet >= 0){
        printf("[CLIENT %d]: %s",fd,buffer);
        for(int i=0; i < MAX_BUFFER; ++i){
            buffer[i] = toupper(buffer[i]);
        }
        send(fd, buffer, MAX_BUFFER,0);
    }
    else{
        printf("Something went wrong.. Closing connection with client: %d\n",fd);
        closesocket(fd);
        for(int i=0; i < BACKLOG; ++i){
            if(clientArr[i]==fd){
                clientArr[i]=0;
                break;
            }
        }
    }
}

void processReq(int server_fdc,int* clientArr, fd_set* fr){
    if(FD_ISSET(server_fdc, fr)){
        struct sockaddr_in client_addr = {};
        int sock_len = sizeof(client_addr);
        int conn_fd = accept(server_fdc, (struct sockaddr*)&client_addr, &sock_len);
        if(conn_fd > 0){
            int full=1;
            printf("Client %d Connected\n",conn_fd);
            for(int i=0; i < BACKLOG; ++i){
                if(clientArr[i] == 0){
                    full = 0;
                    clientArr[i] = conn_fd;
                    send(conn_fd, "Connected to Server\n",22,0);
                    break;
                }
            }
            if(full){
                printf("Server capacity is full..\n");
            }
        }
    }
    else{
        for(int i=0; i < BACKLOG; ++i){
            if(FD_ISSET(clientArr[i],fr)){
                processMessage(clientArr[i], clientArr);
            }
        }
    }
}

void err_die(char* s){
    int err = errno;
    fprintf(stderr, "%s\n",s);
    WSACleanup();
    exit(EXIT_FAILURE);
}

int main() {
    int nRet;
    WSADATA ws;
    if(WSAStartup(MAKEWORD(2,2), &ws) < 0){
        err_die("WSA Variable");
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_fd < 0){
        err_die("Socket");
    }

    struct sockaddr_in adrr = {
        adrr.sin_family = AF_INET,
        adrr.sin_port = htons(PORT),
    };
    adrr.sin_addr.s_addr = INADDR_ANY;
    memset(&(adrr.sin_zero), 0, sizeof(adrr.sin_zero));

    int optVal=0, optLen=sizeof(optVal);
    nRet = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optLen);
    if(nRet < 0){
        err_die("setsockopt");
    }

    nRet = bind(server_fd, (struct sockaddr*)&adrr, sizeof(struct sockaddr));
    if(nRet < 0){
        err_die("Binding");
    }

    nRet = listen(server_fd, BACKLOG);
    if(nRet < 0){
        err_die("Listening Failed");
    }

    fd_set fr, fw, fe;
    struct timeval tv = {
        tv.tv_sec = 2,
        tv.tv_usec = 0
    };
    int clientArr[BACKLOG]={0};
    printf("Server started\n");
    while(1){
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);
        FD_SET(server_fd, &fr);
        FD_SET(server_fd, &fe);
        for(int i=0; i < BACKLOG; ++i){
            if(clientArr[i]){
                FD_SET(clientArr[i],&fr);
                FD_SET(clientArr[i], &fe);
            }
        }

        nRet = select(server_fd+1, &fr, &fw, &fe, &tv);
        if(nRet > 0){
            processReq(server_fd, clientArr, &fr);
        }
        else if(nRet == 0){
            printf("Waiting for connection..\n");
        }
        else{
            err_die("Failed..");
        }
    }
}
