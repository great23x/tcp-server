#include <stdio.h>
#include <winsock.h>

#define PORT 6969
#define MAX_BUFFER 1024

// Error log and die function
void err_die(char* s){
    int err = errno;
    fprintf(stderr, "%s\n", s);
    WSACleanup();
    exit(EXIT_FAILURE);
}

int main() {
    // Variables
    int nRet;
    char send_buffer[MAX_BUFFER] = {0},rec_buffer[MAX_BUFFER] = {0};
    // WSA Variable
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        err_die("WSA Variable Failed");
    }

    // Initialize the socket
    int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_fd < 0) {
        err_die("Socket Failed To Open");
    }

    // Socket address structure
    struct sockaddr_in adrr = {
        adrr.sin_family = AF_INET,
        adrr.sin_port = htons(PORT),
    };
    adrr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(adrr.sin_zero), 0, sizeof(adrr.sin_zero));

    //Connect
    nRet = connect(client_fd, (struct sockaddr *) &adrr, sizeof(struct sockaddr));
    if (nRet >= 0) {
        recv(client_fd, rec_buffer, MAX_BUFFER,0);
        printf("%s",rec_buffer);
        while(1){
            printf("ME:");
            fgets(send_buffer, MAX_BUFFER, stdin);
            send(client_fd, send_buffer, MAX_BUFFER, 0);
            recv(client_fd, rec_buffer, MAX_BUFFER, 0);
            printf("SERVER: %s",rec_buffer);
        }
    } else {
        err_die("Connection Failed..");
    }

}
