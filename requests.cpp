#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string>
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include<iostream>
#include "helpers.h"
#include "requests.h"

using namespace std;

// creaza get_requst-ul
char * compute_get_request(char *host, char *url, char *query_params,
                            char *cookie, char * token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

// scrie numele metodei, URL si tipul protocolului
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // adauga host-ul
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
// adauga Authorization Bearer-ul
    if(token != NULL){
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

// adauga cookies daca exista
    if (cookie != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Cookie: ");
        strcat(line, cookie);
        compute_message(message, line);
    }
    // adauga new line de final
    compute_message(message, "");
    return message;
}

// creaza post_requst-ul
char *compute_post_request(char *host, char *url, char* content_type, string value, char *cookie, char * token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
// scrie numele metodei, URL si tipul protocolului
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // adauga host-ul
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

// adauga Authorization Bearer-ul
   if(token != NULL){
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // adauga headerele necesare
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", (int)value.size());
    compute_message(message, line);


// adauga cokkies daca exista    
    if (cookie != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Cookie: ");
        strcat(line, cookie);
        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // adauga mesajul in sine
    char * val = &value[0];
    strcat(message, val);
    free(line);
    return message;
}

// creaza delete_requst-ul
char *compute_delete_request(char *host, char *url, char* content_type, char *cookie, char * token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
// scrie numele metodei, URL si tipul protocolului
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    
    // adauga host-ul
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

// adauga headerele necesare
   if(token != NULL){
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
// adauga cookies daca exista
    if (cookie != NULL) {
        memset(line, 0, LINELEN);
        strcat(line, "Cookie: ");
        strcat(line, cookie);
        compute_message(message, line);
    }

    // adauga mesajul in sine
    compute_message(message, "");

    free(line);
    return message;
}