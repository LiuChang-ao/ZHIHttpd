#include <unp.h>

#define HTTPD_PORT SERV_PORT
#define MAX_MSG_LEN 4096

ssize_t read_nbytes(int fd, char *buffer, ssize_t n) {
    if (buffer == NULL) return -1;
    ssize_t count = 0;
    while (count < n) {
        ssize_t per_count = read(fd, buffer + count, n - count);
        if (per_count < 0) {
            if (errno == EINTR)
                per_count = 0;
            else
                return -1;
        } else if (per_count == 0) {
            break;
        }
        count += per_count;
    }
    return count;
}

ssize_t write_nbytes(int fd, const char *buffer, ssize_t n) {
    if (buffer == NULL) return -1;
    ssize_t count = 0;
    while (count < n) {
        ssize_t per_count = write(fd, buffer + count, n - count);
        if (per_count < 0 && errno == EINTR) {
            per_count = 0;
        } else {
            return -1;
        }
        count += per_count;
    }
    return count;
}

void handle_request(int client) {
    char client_msg[MAX_MSG_LEN];
    ssize_t recv_bytes_cnt = read_nbytes(client, client_msg, MAX_MSG_LEN);
    write_nbytes(STDOUT_FILENO, client_msg, recv_bytes_cnt);
    write_nbytes(STDOUT_FILENO, "\n", 1);
    char ans_msg[MAX_MSG_LEN];
    sprintf(ans_msg, "You send me %zd bytes.\n", recv_bytes_cnt);
    write_nbytes(client, ans_msg, (ssize_t) strlen(ans_msg));
}

int main(int argc, char **argv) {
    printf("hello world.");
    fflush(stdout);
    int listen_fd, conn_fd;
    pid_t child_pid;
    socklen_t client_len;
    struct sockaddr_in client_addr, serv_addr;
    listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = HTTPD_PORT;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listen_fd, (SA *) &serv_addr, sizeof(serv_addr));
    Listen(listen_fd, LISTENQ);
    while (1) {
        client_len = sizeof(client_addr);
        conn_fd = Accept(listen_fd, (SA *) &client_addr, &client_len);
        if ((child_pid = Fork()) == 0) {
            Close(listen_fd);
            handle_request(conn_fd);
            exit(0);
        }
        Close(conn_fd);
    }
    return 0;
}