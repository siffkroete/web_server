#include "Socket.h";

#define PORT 1500


int server_1(const int port) {
	try {
		cout << endl << "Server: start!" << endl;
		Socket sock1("Server");
		sock1.create();
		sock1.bind(port);
		sock1.listen();
		bool r;
		while (true) {
			Sleep(1); // Soll ja nicht durchdrehen..
			Socket sock2("Server socket 2");
			r = sock1.accept(sock2); // Das ist eine blockierende Funktion und frisst beim Blockieren hoffentlich nicht zu viel cpu Zeit.
			try {
				while (true) {
					Sleep(1); // Soll ja nicht durchdrehen..
					string s;
					sock2 >> s; // Server bekommt Daten
					cout << endl << "Nachricht von Client: " << s << endl;
					sock2 << "Server hat Daten erhalten!"; // Server antwortet
				}
			}
			catch (SocketException& e) {
				cout << endl << e.what();
				sock2.close();
			}

		}

		cout << endl << "Server: Stopp!" << endl;
		return 0;
	}
	catch (exception& e) {
		cout << endl << e.what();
	}
	catch (...) {
		cout << endl << "Unbekannter Fehler!";
	}
	return 1;
}

int main(int argc, char** argv)
{
	return server_1(PORT);
}