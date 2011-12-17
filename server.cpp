#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include "argument.h"

using namespace std;

#define ADDRESS_LENGTH NI_MAXHOST+NI_MAXSERV+10
// A reader reads characters from stream until a whole message is found
// message is delimited by a start string and an end string
class Reader {
  public:

  enum State {
    got_nothing,
    got_start
  };

  State state_;
  char start_, end_;
  string message_;
  int (*handler_)(string);

  Reader(char start, char end, int (*f)(string)) {
    state_   = got_nothing;
    start_   = start;
    end_     = end;
    handler_ = f;
  }

  void set_state(State state) {
    state_ = state;
  }

  State state() {
    return state_;
  } 

  void read(char *s) {
    char *ptr = s;
    int length = strlen(s);
    int i = 0;
    while (i < length) {
      char c = s[i];
      switch(state()) {
        case got_nothing:
          if (c == start_) {
            set_state(got_start);
          }
        break;

        case got_start:
          if (c == end_) {
            (*handler_)(message_);
            message_ = "";
            set_state(got_nothing);
          }else 
            message_ += c;
        break;
      }
      ++i;
    }
  }


};

class RegisterMessage {
  string client_ip_;

  RegisterMessage(string client_ip) {
    client_ip_ = client_ip;
  }
};

class TaskMessage {
  string start_, end_;

  TaskMessage(string start, string end) {
    start_ = start;
    end_ = end;
  }
};


class Message {
  vector<string> message_parts_;

  public:
  Message() {
  }

  static Message fromString(string message) {
    vector<string> message_parts;
    message_parts = split(message, ",");
  };

  // TODO: refactor into utility module
  static vector<string> split(string str, string separator) {
    int position = 0;
    int last_position = 0;
    vector<string> v;

    while ( position != string::npos ) {
      last_position = position;
      position = str.find(separator, position+1);
      if (last_position == 0)
        v.push_back(str.substr(last_position, position - last_position));
      else
        v.push_back(str.substr(last_position+1, position - last_position - 1));
    }

    return v;
  }
};

class MessageHandler {

  public:
  MessageHandler() {
  }

  void handle(string data) {
    Message message = Message::fromString(data);
  }
};

int handler(string data) {
  cout << "Got message: " << data << endl;
}
// Don't even ask...
class Server {

  int get_socket(char *port) {
    struct addrinfo *result, *ptr;
    struct addrinfo hints;
    int socket_fd, success, optval;


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;  
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  

    success = getaddrinfo(NULL, port, &hints, &result);
    if ( success != 0 ) {
      fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(success));
      exit(1);
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
      socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      if (socket_fd == -1) continue;
      setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

      if (bind(socket_fd, ptr->ai_addr, ptr->ai_addrlen) == 0) break;

      close(socket_fd);
    }
    freeaddrinfo(result);

    if (ptr == NULL) perror("Could not bind socket to any address");

    if (listen(socket_fd, 50) == -1) perror("Error in listen()");

    return socket_fd;
  }

  int accept_connection(int socket_fd) {
    struct sockaddr client_address;
    int sockaddr_length, accept_fd;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    char address[ADDRESS_LENGTH];

    sockaddr_length = sizeof(struct sockaddr);
    accept_fd = accept(socket_fd, &client_address, (socklen_t*)&sockaddr_length);
    getnameinfo(&client_address, sockaddr_length, host, NI_MAXHOST, service, NI_MAXSERV,0);
    snprintf(address, ADDRESS_LENGTH, "(%s, %s)", host, service);
    fprintf(stderr, "Connection from %s\n", address);

    return accept_fd;
  }

  void spawn_handler_process(int accept_fd, int (*handler)(string data)) {
    char buffer[1024];
    int bytes_read; 
    if (!fork()) {
      Reader reader('<','>', handler);
      for (;;) {
        bytes_read = recv(accept_fd, buffer, 1024, 0);
        buffer[bytes_read] = '\0';
        reader.read(buffer);
        // printf("read %d bytes: %s", bytes_read, buffer);
        fflush(stdout);
      }
    }
  }

  public:

  Server() { }

  int lstn(char *port) {
    int accept_fd, socket_fd;

    socket_fd = get_socket(port);
    for (;;) {
      accept_fd = accept_connection(socket_fd);
      spawn_handler_process(accept_fd, handler);
      }
    }



};


int main(int argc, char** argv) {
  Server s;
  s.lstn(argv[1]);
  /*
  testing message.split
  vector<string> v = Message::split("hello, world, tesT", ",");
  for (int i=0; i < v.size(); ++i) 
    cout << v[i] << endl;
  */

}
