#include <iostream>
#include <string>
#include <winsock2.h> // Use appropriate headers for your platform
#include <Ws2tcpip.h> // Use appropriate headers for your platform

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 

int main() {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Change to server port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change to server IP

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }

    std::cout << "Connected to server. Type '/exit' to quit." << std::endl;

    std::string message;
    do {
        std::getline(std::cin, message);
        send(clientSocket, message.c_str(), message.size() + 1, 0);

    } while (message != "/exit");

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
