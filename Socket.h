#ifndef __SOCKET_H__
#define __SOCKET_H__
#include "Util.h"
#include "SocketException.h"

using namespace std;

const int MAXCONNECTIONS = 5;
const int MAXRECV = 1024;

class Socket
{
public:
	Socket(const string name); // Name gibt an ob Cleint oder Server z.B.
	virtual ~Socket();

	// TCP Socket erstellen
	bool create();

	// UDP Socket erstellen
	bool UDP_create();
	bool bind(const int port);
	bool listen();
	bool accept(Socket& new_socket);

	bool connect(const string host, const int port);

	bool send(const string s) const;

	int recv(string& s) const;

	bool UDP_send();
	bool UDP_recv();

	bool close() const;

	void cleanup() const;

	bool is_valid() const {
		return m_sock != -1;
	};

	const Socket& operator << (const string& s) const;
	const Socket& operator >> (string& s) const;

private:
	int m_sock;
	sockaddr_in m_addr;
	string m_name;
};

#endif