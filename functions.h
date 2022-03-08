#ifndef _FUNCTIONS_
#define _FUNCTIONS_
#include<string>
using namespace std;

string get_connect_id(string response);

string login(int sockfd, string username, string password);

string registerr(int sockfd, string username, string password);

string get_tok(char *rezz);

string enter_library(int sockfd, string connection_sid);

string get_books(int sockfd, string connection_sid, string tokenn);

string get_book(int sockfd, string connection_sid, string tokenn, int bookid);

void add_book(int sockfd, string connection_sid, string tokenn, string title, string author, string genre, int page_cnt, string publisher);

void logout(int sockfd, string connection_sid);

void delete_book(int sockfd, string connection_sid, string tokenn, int bookid);

#endif
