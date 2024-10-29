#include "csapp.h"
#include <stdio.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define TINY_SERVER_HOST "3.36.72.78"
#define TINY_SERVER_PORT "8080"

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

void doit(int connfd);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);
void read_requesthdrs(rio_t *rp);

int main(int argc, char **argv) {
  printf("%s", user_agent_hdr);
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen); // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);  // line:netp:tiny:doit
    Close(connfd); // line:netp:tiny:close
  }
  return 0;
}

void doit(int fd) {

  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char tiny_req[MAXLINE]; // 클라이언트의 요청을 기반으로 Tiny 서버로 전송할
                          // 요청 메시지
  rio_t rio, server_rio; // rio 는 클라이언트로부터 데이터를 읽는 데 사용,
                         // server_rio는 Tiny 서버에서 데이터를 읽어올 때 사용
  int serverfd;

  // 클라이언트로부터 요청 읽기
  Rio_readinitb(&rio, fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  sscanf(buf, "%s %s %s", method, uri, version);

  if (strcmp(method, "GET")) {
    clienterror(fd, method, "501", "Not implemented",
                "Tiny does not implement this method");
    return;
  }

  // 서버에 보낼 GET request와 request header
  int offset = 0;
  // GET request
  offset += snprintf(tiny_req + offset, sizeof(tiny_req) - offset,
                     "GET %s HTTP/1.0\r\n", uri);
  // GET request header
  offset += snprintf(tiny_req + offset, sizeof(tiny_req) - offset, "%s",
                     user_agent_hdr);
  offset += snprintf(tiny_req + offset, sizeof(tiny_req) - offset,
                     "Connection: close\r\n");
  offset += snprintf(tiny_req + offset, sizeof(tiny_req) - offset,
                     "Proxy-Connection: close\r\n\r\n");

  // Tiny 서버와 연결
  serverfd = Open_clientfd(TINY_SERVER_HOST, TINY_SERVER_PORT);
  if (serverfd < 0) {
    printf("Connection to Tiny server failed\n");
    return;
  }

  // Tiny 서버로 요청 전송
  Rio_writen(serverfd, tiny_req, strlen(tiny_req));

  // Tiny 서버로부터 응답 읽어서 클라이언트에 전송
  Rio_readinitb(&server_rio, serverfd);

  size_t n;
  while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0) {
    printf("Proxy received %zu bytes from Tiny server\n", n);
    Rio_writen(fd, buf, n);
  }

  Close(serverfd);
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg) {
  char buf[MAXLINE], body[MAXBUF];

  /* Build the HTTP response body */
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body,
          "%s<body bgcolor="
          "ffffff"
          ">\r\n",
          body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* Print the HTTP response */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rp) {
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  while (strcmp(buf, "\r\n")) {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}
