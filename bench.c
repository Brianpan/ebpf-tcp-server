#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define TARGET_HOST "127.0.0.1" //10.1.1.5
#define TARGET_PORT 12345
#define BENCH_COUNT 1 //1
#define BENCHMARK_RESULT_FILE "bench.txt"
#define MAX_MSG_LEN 50 //50
#define STEP 50 //50
#define MAX_INDEX MAX_MSG_LEN / STEP
#define TIMEOUT 3

static long time_res[MAX_INDEX] = {0};
static FILE *bench_fd;

static inline long time_diff_us(struct timeval *start, struct timeval *end)
{
    return ((end->tv_sec - start->tv_sec) * 1000000) +
           (end->tv_usec - start->tv_usec);
}

static void generateString(int size, char *str)
{
    printf("Generating String...\n");
    for (int i = 0; i < size - 1; i++)
        str[i] = 'A' + (rand() % 26);
    str[size - 1] = '\0';
}

static void bench(int size)
{
    int sock_fd;
    char *msg_dum = malloc(size * sizeof(char));
    char dummy[MAX_MSG_LEN];
    memset(dummy, 0, sizeof(dummy));
    struct timeval start, end;
    generateString(size, msg_dum);
  
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(-1);
    }
    
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        exit(-1);
    }

    struct sockaddr_in info = {
        .sin_family = PF_INET,
        .sin_addr.s_addr = inet_addr(TARGET_HOST),
        .sin_port = htons(TARGET_PORT),
    };

    printf("Connecting...\n");
    if (connect(sock_fd, (struct sockaddr *)&info, sizeof(info)) == -1) {
        perror("connect");
        exit(-1);
    }

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    printf("Getting the socket name...\n");
    if (getsockname(sock_fd, (struct sockaddr *)&local_addr, &addr_len) == -1) {
      perror("getsockname");
      exit(-1);
    }

    printf("Send & Recv...\n");
    gettimeofday(&start, NULL);
  
    int send_len = send(sock_fd, msg_dum, strlen(msg_dum), 0);
    if (send_len == -1) {
        perror("send");
        exit(-1);
    }
    printf("Finish Sending.\n");
    
    int recv_len = recv(sock_fd, dummy, MAX_MSG_LEN, 0);
    if (recv_len == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("recv timeout occurred\n");
        } else {
            perror("recv");
        }
        exit(-1);
    } else if (recv_len == 0) {
        printf("Connection closed by peer\n");
        exit(0);
    } else {
        dummy[recv_len] = '\0';
    }
    printf("Finish Receiving.\n");
    
    gettimeofday(&end, NULL);

    printf("SHUTDOWN sock_fd\n\n");
    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);

    if (strncmp(msg_dum, dummy, strlen(msg_dum))) {
        puts("echo message validation failed");
        exit(-1);
    }
    time_res[size / STEP - 1] += time_diff_us(&start, &end);
    free(msg_dum);
}

int main(int argc, char **argv)
{
    bench_fd = fopen(BENCHMARK_RESULT_FILE, "w");
    if (!bench_fd) {
        perror("fopen");
        return -1;
    }
    for (int size = 50; size <= MAX_MSG_LEN; size += STEP) {
        for (int i = 0; i < BENCH_COUNT; i++) {
            bench(size);
        }
    }
    puts("correct");
    for (int i = 1; i <= MAX_INDEX; i++)
        fprintf(bench_fd, "%d %ld\n", i * STEP, time_res[i - 1] / BENCH_COUNT);
    fclose(bench_fd);
    return 0;
}
