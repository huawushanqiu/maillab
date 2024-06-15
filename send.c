#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include "base64_utils.h"

#define MAX_SIZE 4095

char buf[MAX_SIZE+1];

// receiver: mail address of the recipient
// subject: mail subject
// msg: content of mail body or path to the file containing mail body
// att_path: path to the attachment
void send_mail(const char* receiver, const char* subject, const char* msg, const char* att_path)
{
    const char* end_msg = "\r\n.\r\n";
    const char* host_name = "smtp.163.com"; // TODO: Specify the mail server domain name
    const unsigned short port = 25; // SMTP server port
    const char* user = "17323015681@163.com"; // TODO: Specify the user
    const char* pass = "GQPOXDAQWGRTLFBP"; // TODO: Specify the password
    const char* from = "17323015681@163.com"; // TODO: Specify the mail address of the sender
    char dest_ip[16]; // Mail server IP address
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

    // TODO: Create a socket, return the file descriptor to s_fd, and establish a TCP connection to the mail server
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

    // Send EHLO command and print server response
    const char* EHLO = "EHLO 163.com\r\n"; // TODO: Enter EHLO command here
    send(s_fd, EHLO, strlen(EHLO), 0);
    // TODO: Print server response to EHLO command

    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
 
    // TODO: Authentication. Server response should be printed out.
    
    const char* AUTH = "AUTH login\r\n";
    printf("%lu\n", strlen(AUTH));
    send(s_fd, AUTH, strlen(AUTH), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    
    char *encode_user = encode_str(user);
    //查看encode_str实现方式可知encode_user是user长度两倍，base64编码会增加33%的长度
    //正常情况下可以添加回车符，但是如果传入的user或者pass为空可能就会出问题，这里暂时先不管
    size_t user_len = strlen(encode_user);
    //这里太奇怪了，不知道为什么末尾多了一个看不见的字符，删掉之后终于成功登陆了，但还是很奇怪
    *(encode_user + user_len - 1) = '\0';
    strcat(encode_user, "\r\n");
    send(s_fd, encode_user, strlen(encode_user), 0);
    free(encode_user);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
    
    char *encode_pass = encode_str(pass);
    size_t pass_len = strlen(encode_pass);
    *(encode_pass + pass_len - 1) = '\0';
    strcat(encode_pass, "\r\n");
    send(s_fd, encode_pass, strlen(encode_pass), 0);
    free(encode_pass);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);


    // TODO: Send MAIL FROM command and print server response
    
    char mail_from[100] = "";
    sprintf(mail_from, "MAIL FROM:<%s>\r\n", from);
    send(s_fd, mail_from, strlen(mail_from), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
 
    
    // TODO: Send RCPT TO command and print server response
    
    char rcpt[100] = "";
    sprintf(rcpt, "RCPT TO:<%s>\r\n", receiver);
    send(s_fd, rcpt, strlen(rcpt), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
 
    
    // TODO: Send DATA command and print server response

    const char* DATA_HEAD = "data\r\n";
    send(s_fd, DATA_HEAD, strlen(DATA_HEAD), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
 

    // TODO: Send message data 

    //data如何添加附件还需要学习
    //先统一不加\r\n 
    char data[MAX_SIZE] = "";
    char data_from[MAX_SIZE] = "";
    char data_to[MAX_SIZE] = "";
    const char* MIME_VERSION = "MIME-Version: 1.0";
    sprintf(data_from, "From: %s", from);
    sprintf(data_to, "To: %s", receiver);
    const char* MIXED = "Content-Type: multipart/mixed; ";
    const char* PLAIN = "Content-Type: text/plain; ";
    const char* OCTET = "Content-Type: application/octet-stream; ";
    char* boundary = "qwertyuiopasdfghjklzxcvbnm";
    char data_subject[MAX_SIZE] = "";
    sprintf(data_subject, "Subject: %s", subject);
    const char* PREAMBLE = "This is the preamble. The user agent ignores it. Have a nice day.";
    const char* BASE64 = "Content-Transfer-Encoding: base64";
    
    //添加邮件头部
    sprintf(data, "%s\r\n%s\r\n%s\r\n%sboundary=%s\r\n%s\r\n\r\n%s\r\n\r\n", data_from, data_to, MIME_VERSION, MIXED, boundary, data_subject, PREAMBLE);
    //通过fopen简单粗暴地判断文件名
    FILE* message;
    char message_data[MAX_SIZE];
    //邮件内容是文本文件，附件是二进制文件
    if((message = fopen(msg, "rb")) == NULL){
      //表示是直接输入的信息
      //有点怀疑这样写的正确性
      //注意信息头和信息之间要多空一行
      sprintf(data, "%s--%s\r\n%s\r\n%s\r\n\r\n%s\r\n\r\n", data, boundary, PLAIN, BASE64, encode_str(msg));
    }else{
      //表示是文件名
      //直接读取全部文件数据（默认小于MAX_SIZE）
      fgets(message_data, MAX_SIZE, message);
      sprintf(data, "%s--%s\r\n%s\r\n%s\r\n\r\n%s\r\n\r\n", data, boundary, PLAIN, BASE64, encode_str(message_data));
      fclose(message);
    }
    
    //接着添加附件（如果有的话）
    FILE* attachment;
    FILE* encode_attachment;
    //跟踪数据长度
    //好像懂了，由于普通二进制文件存在\0字符，所以如果需要字符流传输就需要先重新编码
    int data_len;
    char attachment_data[MAX_SIZE];
    if((attachment = fopen(att_path, "rb")) == NULL){
      //表示没有附件
    }else{
      //表示有附件
      //直接读取全部文件数据（默认小于MAX_SIZE）
      //由于附件为二进制文件，所以不能用fgets,而要用fread
      //需要创建一个临时文件，否则就是个野指针，就会出现段错误
      encode_attachment = tmpfile();
      encode_file(attachment, encode_attachment);
      fseek(encode_attachment, 0, SEEK_SET);
      int size = fread(attachment_data, 1, MAX_SIZE, encode_attachment);
      printf("%d\n", size);
      //添加结束符
      printf("%s\n%lu\n", attachment_data, strlen(attachment_data));
      printf("%s\n", att_path);
      sprintf(data, "%s--%s\r\n%s\r\n%s\r\n\r\n%s\r\n\r\n--%s", data, boundary, OCTET, BASE64, attachment_data, boundary);
      //sprintf会覆盖目标指针，所以添加字符串时需要先复制本体，不要忘记
      fclose(attachment);
      fclose(encode_attachment);
    }
    
    
    // TODO: Message ends with a single period
    
    //结尾加上句号
    strcat(data, end_msg);
    send(s_fd, data, strlen(data), 0);
    if ((r_size = recv(s_fd, buf, MAX_SIZE, 0)) == -1)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    buf[r_size] = '\0'; // Do not forget the null terminator
    printf("%s", buf);
 


    // TODO: Send QUIT command and print server response
    
    const char* QUIT = "quit\r\n";
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
    int opt;
    char* s_arg = NULL;
    char* m_arg = NULL;
    char* a_arg = NULL;
    char* recipient = NULL;
    const char* optstring = ":s:m:a:";
    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch (opt)
        {
        case 's':
            s_arg = optarg;
            break;
        case 'm':
            m_arg = optarg;
            break;
        case 'a':
            a_arg = optarg;
            break;
        case ':':
            fprintf(stderr, "Option %c needs an argument.\n", optopt);
            exit(EXIT_FAILURE);
        case '?':
            fprintf(stderr, "Unknown option: %c.\n", optopt);
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, "Unknown error.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (optind == argc)
    {
        fprintf(stderr, "Recipient not specified.\n");
        exit(EXIT_FAILURE);
    }
    else if (optind < argc - 1)
    {
        fprintf(stderr, "Too many arguments.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        recipient = argv[optind];
        send_mail(recipient, s_arg, m_arg, a_arg);
        exit(0);
    }
}
