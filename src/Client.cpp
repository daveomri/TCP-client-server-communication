//============================================================================
// Name        : Client.cpp
// Author      : Vikturek
// Version     :
// Copyright   : Your copyright notice
// Description : TCP client - demo
//============================================================================

#include <iostream>
#include <time.h>
#include <string>
using namespace std;


#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // htons(), htonl()
#include <strings.h> // bzero()
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

#include <wait.h> // waitpid()


#define TIMEOUT 10


int main(int argc, char **argv) {
	if(argc < 3){
		cerr << "Usage: client address port" << endl;
		return -1;
	}

	int port = atoi(argv[2]);

	// Vytvoreni koncoveho bodu spojeni
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0){
		perror("Nemohu vytvorit socket!");
		return -1;
	}

	socklen_t sockAddrSize;
	struct sockaddr_in serverAddr;
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	struct hostent *host;
	host = gethostbyname(argv[1]); // <netdb.h>
	memcpy(&serverAddr.sin_addr, host->h_addr,
	host->h_length); // <string.h>

	// Pripojeni ke vzdalenemu serveru
	if(connect(s, (struct sockaddr *) &serverAddr, sockAddrSize) < 0){
		perror("Nemohu spojit!");
		close(s);
		return -1;
	}


	struct timeval timeout;
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
	//fd_set sockets;
	int retval;
	

	#define BUFFER_SIZE 10240
	char buffer[BUFFER_SIZE];
	int i = 0;
	string name1 = "Umpa";
	string name2 = "_Lumpa";
	string name3 = "\a\b";
	while(true){
		cout << s << endl;
		cout << "> ";
		if (i==0){
			if(send(s, name1.c_str(), (name1).length(), 0) < 0){
				perror("Nemohu odeslat data!");
				close(s);
				return -3;
			}
			usleep(2000);
			if(send(s, name2.c_str(), (name2).length(), 0) < 0){
				perror("Nemohu odeslat data!");
				close(s);
				return -3;
			}
			usleep(3000);
			if(send(s, name3.c_str(), (name3).length(), 0) < 0){
				perror("Nemohu odeslat data!");
				close(s);
				return -3;
			}
			i = 1;
			int bytesRead = recv(s, buffer, BUFFER_SIZE, 0);
			buffer[bytesRead] = '\0';
			cout << buffer << endl;
		}

		if (i == 2){
			if (string("FULL POWER") != buffer && string("RECHARGING") != buffer){
			cout << "waiting and so on" <<endl;
			int bytesRead = recv(s, buffer, BUFFER_SIZE, 0);
			buffer[bytesRead] = '\0';
			cout << buffer << endl;
			}
		}
		

		

		
		cin.getline (buffer, BUFFER_SIZE);
		cout << "here"<<endl;
		string messs = buffer;
		messs+="\a\b";
		if(send(s, messs.c_str(), messs.length(), 0) < 0){
			perror("Nemohu odeslat data!");
			close(s);
			return -3;
		}
		// Kdyz poslu "konec", ukoncim spojim se serverem
		if(string("konec") == buffer){
			break;
		}

		if (i == 1){
			if (string("FULL POWER") != buffer && string("RECHARGING") != buffer ){
			int bytesRead = recv(s, buffer, BUFFER_SIZE, 0);
			buffer[bytesRead] = '\0';
			cout << buffer << endl;
			}
		}
		i = 2;
		cout << "there"<<endl;
		
		//retval = select(s + 1, &sockets, NULL, NULL, &timeout);
		
		

		//if(retval < 0){
		//	perror("Chyba v select()");
		//	close(s);
		//	return -1;
		//}
		

	}


	close(s);
	return 0;
}
