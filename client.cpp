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
#include "functions.h"
using namespace std;

using json = nlohmann::json;

// functiile do_<ceva> preiau input de la tastatura si apeleaza metodele
// ce fac apel la server primesc mesajul inapoi de la acestea si afiseaza
// raspunsul sau eroarea.
void do_register(int sockfd){
    string username;
    string password;
    cout << "username=";
    cin >> username;
    cout << "password=";
    cin >> password;
    registerr(sockfd, username, password);
}

string do_login(int sockfd,string connection_sid){
    if(!connection_sid.empty()){
        cout << "You are already logged in\n";
        return connection_sid;
    }
    string username;
    string password;
    cout << "username=";
    cin >> username;
    cout << "password=";
    cin >> password;
    connection_sid = login(sockfd, username, password);
    if(connection_sid =="Credentials are not good!"){
        cout << "Credentials are not good!\n";
        connection_sid.clear();


    }else{
        cout << "You are logged in with the user: " << username << "\n";
    }
    return connection_sid;
}

string do_enter(int sockfd, string connect_sid, string tokken){
    string empty;
    if (connect_sid.empty())
    {
        cout << "you are not logged in\n";
        return empty;
    }
    if(!tokken.empty()){
        cout << "you already are in the library\n";
        return empty;
    }
   return enter_library(sockfd, connect_sid);
}

void do_get_books(int sockfd, string connect_sid, string tokk){
    if(connect_sid.empty()){
        cout << "you are not logged in\n";
        return;
    }
    if(tokk.empty()){
        cout << "you are not in the library\n";
        return;
    }

    string msg = get_books(sockfd, connect_sid, tokk);
   if(msg == "[]"){
       cout << "There is no book in the library\n";
   }else{
        cout << "books = "<< msg << "\n";
   }
}

void do_get_book(int sockfd, string connect_sid, string tokk){
    if(connect_sid.empty()){
        cout << "you are not logged in\n";
        return;
    }
    if(tokk.empty()){
        cout << "you are not in the library\n";
        return;
    }
    int id;
    cout << "id=";
    cin >> id;
    string book = get_book(sockfd, connect_sid, tokk, id);
    if(book == "No book was found!"){
        cout << "The book with that id does not exist" << "\n";
    }
    cout << "Book specifications = " << book << "\n";
}
void do_delete_book(int sockfd, string connect_sid, string tokk){
    if(connect_sid.empty()){
        cout << "you are not logged in\n";
        return;
    }
    if(tokk.empty()){
        cout << "you are not in the library\n";
        return;
    }
    int id;
    cout << "id=";
    cin >> id;
    delete_book(sockfd, connect_sid, tokk, id);
    cout << "The book was deleted\n";
}

void do_add_book(int sockfd, string connect_sid, string tokk){
    if(connect_sid.empty()){
        cout << "you are not logged in\n";
        return;
    }
    if(tokk.empty()){
        cout << "you are not in the library\n";
        return;
    }
    string title;
    cout << "title=";
    cin >> title;
    string author;
    cout << "author=";
    cin >> author;
    string genre;
    cout << "genre=";
    cin >> genre;
    string publisher;
    cout << "publisher=";
    cin >> publisher;
    int page_count;
    cout << "page_count=";
    cin >> page_count;
    add_book(sockfd, connect_sid, tokk, title, author, genre, page_count, publisher);
    cout << "The book " << title << " was added to the library" << "\n";
}

int main(){
    char ip[14] = "34.118.48.238";
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
    string tokk;
    string connection_sid;
    string input;
    // while ce astepta input de la tastatura.
    bool runs = true;
    while (runs){
        cin >> input;
        int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
        if (input == "register")
        {
            do_register(sockfd);
            close_connection(sockfd);
        }
        else if (input == "exit")
        {
            runs = 0;
            close_connection(sockfd);
        }
        else if (input == "login")
        {
            connection_sid = do_login(sockfd, connection_sid);
            close_connection(sockfd);
        }
        else if (input == "enter_library")
        {
            tokk = do_enter(sockfd, connection_sid, tokk);
            close_connection(sockfd);
        }
        else if (input == "get_books")
        {
            do_get_books(sockfd, connection_sid, tokk);
            close_connection(sockfd);

        }
        else if (input == "get_book")
        {
            do_get_book(sockfd, connection_sid, tokk);
            close_connection(sockfd);
        }
        else if (input == "logout")
        {
            logout(sockfd, connection_sid);
            connection_sid.clear();
            tokk.clear();
            close_connection(sockfd);
            cout << "You are logged out now\n";
        }
        else if (input == "delete_book")
        {
            do_delete_book(sockfd, connection_sid, tokk);
            close_connection(sockfd);
        }
        else if (input == "add_book")
        {
            do_add_book(sockfd, connection_sid, tokk);
            close_connection(sockfd);
        }else{
            cout << "bad input\n";
        }
    }
    
    close_connection(sockfd);
    return 0;
}