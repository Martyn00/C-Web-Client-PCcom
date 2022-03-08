#include<iostream>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "single_include/nlohmann/json.hpp"
using namespace std;

using json = nlohmann::json;
char ip[14] = "34.118.48.238";
char json_specifier[20] = "application/json";

// primeste raspunsul de lserver login si returneaza connection_sid
string get_connect_sid(string response)
{
    int pos = response.find("connect.sid");
    string cutted = response.substr(pos, response.size() - 1);
    int final_pos = cutted.find(";");
    string cookie = cutted.substr(0, final_pos + 1);
    return cookie;
}

// creaza mesajul de post il trimite catre server il primeste
// si verifica daca acesta nu a dat vreo eroare
// la final returneaza connection_sid.
string login(int sockfd, string username, string password){
    char url_login[25] = "/api/v1/tema/auth/login";
    json j;
    j["password"] = password;
    j["username"] = username;
    char *message = compute_post_request(ip, url_login, json_specifier, j.dump(), NULL, NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    string res(response);
    try{
        return get_connect_sid(res);
    }catch(...){
        string json_string(response);
        int pos = json_string.find("{");
        int final_pos = json_string.find("}");
        string s = json_string.substr(pos, final_pos - pos + 1);
        json j = json::parse(s);
        return j["error"];
    }
}


// creaza mesajul de post il trimite catre server il primeste
// si verifica daca acesta nu a dat vreo eroare
string registerr(int sockfd, string username, string password){
    char url_register[30] = "/api/v1/tema/auth/register";
    json j;
    j["password"] = password;
    j["username"] = username;
    char *message = compute_post_request(ip, url_register, json_specifier, j.dump(), NULL, NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    string res(response);
    try{
        string json_string(response);
        int pos = json_string.find("{");
        int final_pos = json_string.find("}");
        string s = json_string.substr(pos, final_pos - pos + 1);
        json j = json::parse(s);
        cout <<  j["error"] << "\n";
    }catch(...){
        cout << "You are registered with the user" << username << "\n";
    }
    return res;
}

// primeste raspunsul de la enter_library il parseaza si intoarce token-ul
string get_tok(char * rezz){
    string res(rezz);
    int pos = res.find("{");
    int final_pos = res.find("}");
    string s = res.substr(pos, final_pos - pos + 1);
    json j = json::parse(s);
    if(!j["error"].is_null()){
        cout << "Bad account";
        return nullptr;
    }else {
        string tokk = j["token"];
        cout << "You entered the library" << "\n";
        return tokk;
    }
}

// preia raspunsul de la get_book/get_books si intoarce
// lista de obiecte json
string get_json_list(char * rezz){
    string res(rezz);
    int pos = res.find("[");
    int final_pos = res.find("]");
    string s = res.substr(pos, final_pos - pos + 1);
    return s;
}

// creaza mesajul de get il trimite catre server, primeste raspunsul
// si verifica daca acesta nu a dat vreo eroare
// la final returneaza token-ul de acces.
string enter_library(int sockfd, string connection_sid){
    char url_access[30] = "/api/v1/tema/library/access";
    char *message = compute_get_request(ip, url_access, NULL, &connection_sid[0], NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    char *rezz = strdup(response);
    return get_tok(rezz);
}

// creaza mesajul de get il trimite catre server, primeste raspunsul
// si returneaza lista carti
string get_books(int sockfd, string connection_sid, string tokenn){
    char url_get_books[30] = "/api/v1/tema/library/books";
    char *message = compute_get_request(ip, url_get_books, NULL, &connection_sid[0], &tokenn[0]);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    return get_json_list(response);
}

// creaza mesajul de get il trimite catre server, primeste raspunsul
// si returneaza o carte in cazul in care exista, daca nu returneaza 
// mesajul eroarii care urmeaza a fi afisat.
string get_book(int sockfd, string connection_sid, string tokenn, int bookid){
    char url[100];
    memset(url, 0, 100);
    sprintf(url, "/api/v1/tema/library/books/%d", bookid);
    char *message = compute_get_request(ip, url, NULL, &connection_sid[0], &tokenn[0]);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    try{
        string book = get_json_list(response);
        return book.substr(1, book.size() - 1);

    }catch(...){
        string json_string(response);
        int pos = json_string.find("{");
        int final_pos = json_string.find("}");
        string s = json_string.substr(pos, final_pos - pos + 1);
        json j = json::parse(s);
        return j["error"];
    }
}


// creaza mesajul de post il trimite catre server, primeste raspunsul
// si il returneaza.
void add_book(int sockfd, string connection_sid, string tokenn, string title, string author, string genre, int page_cnt, string publisher){
    char url_get_books[30] = "/api/v1/tema/library/books";
    json j;
    j["title"] = title;
    j["author"] = author;
    j["genre"] = genre;
    j["page_count"] = page_cnt;
    j["publisher"] = publisher;
    string s = j.dump();
    char * message = compute_post_request(ip, url_get_books, json_specifier, s, &connection_sid[0], &tokenn[0]);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
}

// verifica daca clientul nu este deja logat,
// creaza mesajul de get, il trimite 
void logout(int sockfd, string connection_sid){
    char url_logout[30] = "/api/v1/tema/auth/logout";
    if (connection_sid.empty())
    {
        cout << "you are not logged in\n";
        return;
    }
    char * message = compute_get_request(ip, url_logout, NULL, &connection_sid[0], NULL);
    send_to_server(sockfd, message);
    receive_from_server(sockfd);
}

// creaza mesajul de delete il trimite catre server, primeste raspunsul
// si il returneaza.
void delete_book(int sockfd, string connection_sid, string tokenn, int bookid){
    char url[100];
    memset(url, 0, 100);
    sprintf(url, "/api/v1/tema/library/books/%d", bookid);
    char *message = compute_delete_request(ip, url, json_specifier, &connection_sid[0], &tokenn[0]);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
}