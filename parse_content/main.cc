#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <libgen.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "parse_content.h"

int main(int argc, char* argv[])
{
    // parse input param
    if(argc <= 2) {
        printf("Useage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    char* ip = argv[1];
    int port = atoi(argv[2]);

    // prepare address
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    // create socket
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    int ret = bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);

    // accept
    struct sockaddr_in client_addr;
    socklen_t client_socklen = sizeof(client_addr);
    int fd = accept(listenfd, (struct sockaddr*)&client_addr, &client_socklen);
    if(fd < 0) {
        printf("errno is: %d\n", errno);
    }else {
        char buffer[BUFFER_SIZE];
        memset(buffer, '\0', BUFFER_SIZE);
        int data_read = 0;
        int read_index = 0;
        int checked_index = 0;
        int start_line = 0;
        CHECK_STATE check_state = CHECK_STATE_REQUESTLINE;

        while(1) {
            data_read = recv(fd, buffer+read_index, BUFFER_SIZE-read_index, 0);
            if(data_read == -1) {
                printf("reading failed\n");
                break;
            }else if(data_read == 0){
                printf("remote client has closed the connection\n");
                break;
            }
            read_index += data_read;

            // parse read data
            HTTP_CODE result = parse_content(buffer, checked_index, read_index, check_state, start_line);
            if(result == NO_REQUEST){
                continue;
            }else if(result == GET_REQUEST){
                send(fd, szret[0], sizeof(szret[0]), 0);
                break;
            }else{
                send(fd, szret[1], sizeof(szret[1]), 0);
                break;
            }
        }
        close(fd);
    }
    close(listenfd);
    return 0;
}
