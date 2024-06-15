#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_SIZE 65535

char buf[MAX_SIZE+1];

void recv_mail()
{
    const char* host_name = "pop.163.com"; // TODO: Specify the mail server domain name
    const unsigned short port = 110; // POP3 server port
    const char* user = "17323015681@163.com"; // TODO: Specify the user
    const char* pass = "GQPOXDAQWGRTLFBP"; // TODO: Specify the password
    char dest_ip[16];
    int s_fd; // socket file descriptor
    struct hostent *host;
    struct in_addr **addr_list;
    int i = 0;
    int r_size;

    // Get IP from domain name
    if ((host = gethostbyname(host_name)) == NULL)
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    addr_list = (struct in_addr **) host->h_addr_list;
    while (addr_list[i] != NULL)
        ++i;
    strcpy(dest_ip, inet_ntoa(*addr_list[i-1]));

    // TODO: Create a socket,return the file descriptor to s_fd, and establish a TCP connection to the POP3 server
    
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    //注意大小端转换
    servaddr.sin_port = (port << 8) + (port >> 8);
    bzero(servaddr.sin_zero, 8);
    //这个设计令我疑惑，为什么要在in_addr_t外面套一个in_addr结构体，这里半天没反应过来
    servaddr.sin_addr.s_addr = inet_addr(dest_ip);
    if(connect(s_fd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr))){
      printf("connect failed");
    }
    
    // Print welcome message
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);

    // TODO: Send user and password and print server response
    
    char user_name[100] = "USER ";
    strcat(user_name, user);
    strcat(user_name, "\r\n");
    send(s_fd, user_name, strlen(user_name), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    
    char pass_code[100] = "PASS ";
    strcat(pass_code, pass);
    strcat(pass_code, "\r\n");
    send(s_fd, pass_code, strlen(pass_code), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    

    // TODO: Send STAT command and print server response
    
    const char* STAT = "STAT\r\n";
    send(s_fd, STAT, strlen(STAT), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    

    // TODO: Send LIST command and print server response
    
    const char* LIST = "LIST\r\n";
    send(s_fd, LIST, strlen(LIST), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    

    // TODO: Retrieve the first mail and print its content
    
    const char* RETR = "RETR 1\r\n";
    send(s_fd, RETR, strlen(RETR), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    //memset(buf, '\0', MAX_SIZE);
    

    // TODO: Send QUIT command and print server response
    
    const char* QUIT = "QUIT\r\n";
    send(s_fd, QUIT, strlen(QUIT), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    

    close(s_fd);
}

int main(int argc, char* argv[])
{
    recv_mail();
    exit(0);
}
