/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "csapp.h"

int main(void) {
  char *buf, *p, *method;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1 = 0, n2 = 0;

  if ((buf = getenv("QUERY_STRING")) != NULL) {
    p = strchr(buf, '&'); // &가 나오는 포인터 찾기
    *p = '\0';
    strcpy(arg1, buf);
    strcpy(arg2, p + 1);
    n1 = atoi(arg1);
    n2 = atoi(arg2);
  }

  method = getenv("REQUEST_METHOD");
  /* Make the response body */
  // int offset = 0;
  // offset += snprintf(content + offset, sizeof(content) - offset,
  //                    "QUERY_STRING=%d?%d\r\n<p>", n1, n2);
  // offset += snprintf(content + offset, sizeof(content) - offset,
  //                    "Welcome to add.com ");
  // offset += snprintf(content + offset, sizeof(content) - offset,
  //                    "THE Internet addition portal.\r\n<p>");
  // offset += snprintf(content + offset, sizeof(content) - offset,
  //                    "The answer is %d + %d = %d\r\n<p>", n1, n2, n1 + n2);
  // offset += snprintf(content + offset, sizeof(content) - offset,
  //                    "Thanks for visiting\r\n");

  sprintf(content, "%d", n1 + n2);

  /* Generate the HTTP response */
  printf("Content-type: text/html\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Connection: close\r\n\r\n"); // 헤더와 본문을 구분하기 위한 빈 줄

  if (strcmp(method, "GET") == 0)
    printf("%s", content);
  fflush(stdout);

  exit(0);
}
/* $end adder */
