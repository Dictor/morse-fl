#include <stdio.h>
#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/logging/log.h>

#include "../inc/http.h"
#include "../inc/ca_certificate.h"

#define HTTP_PORT "443"

LOG_MODULE_REGISTER(http);

using namespace hangang_view;

int hangang_view::HttpInit() {
  tls_credential_add(CA_CERTIFICATE_TAG, TLS_CREDENTIAL_CA_CERTIFICATE,
                     ca_certificate, sizeof(ca_certificate));
  return 0;
}

#define CHECK(r)                 \
  {                              \
    if (r == -1) {               \
      LOG_ERR("Error: " #r "\n"); \
      return -1;                 \
    }                            \
  }

void dump_addrinfo(const struct addrinfo *ai)
{
	printf("addrinfo @%p: ai_family=%d, ai_socktype=%d, ai_protocol=%d, "
	       "sa_family=%d, sin_port=%x, addr=%s\n",
	       ai, ai->ai_family, ai->ai_socktype, ai->ai_protocol,
	       ai->ai_addr->sa_family,
	       ((struct sockaddr_in *)ai->ai_addr)->sin_port, ai->ai_addr);
}

char HttpResponse[1024];
int hangang_view::HttpGet(char* host, char* path) {
  static struct addrinfo hints;
  struct addrinfo* res;
  int st, sock;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  st = getaddrinfo(host, HTTP_PORT, &hints, &res);

  sock = socket(res->ai_family, res->ai_socktype, IPPROTO_TLS_1_2);
  CHECK(sock);
  dump_addrinfo(res);

  sec_tag_t sec_tag_opt[] = {
      CA_CERTIFICATE_TAG,
  };
  CHECK(setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST, sec_tag_opt,
                   sizeof(sec_tag_opt)));

  char tlshost[] = "google.com";
  CHECK(setsockopt(sock, SOL_TLS, TLS_HOSTNAME, tlshost, sizeof(tlshost)))

  char request[100];
  sprintf(request, "GET %s HTTP/1.0\r\nHost: %s \r\n\r\n", path, host);

  #define SSTRLEN(s) (sizeof(s) - 1)
  CHECK(connect(sock, res->ai_addr, res->ai_addrlen));
  CHECK(send(sock, request, SSTRLEN(request), 0));

  LOG_INF("Response:\n\n");

  while (1) {
    int len = recv(sock, HttpResponse, sizeof(HttpResponse) - 1, 0);

    if (len < 0) {
      LOG_ERR("Error reading response\n");
      return -1;
    }

    if (len == 0) {
      break;
    }

    HttpResponse[len] = 0;
    LOG_DBG("%s", HttpResponse);
  }

  LOG_DBG("\n");
  (void)close(sock);
}