#include <iostream>
#include <winsock2.h> // Libreria necesaria para la programación de sockets en Windows
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm> // Libreria necesaria para utilizar la función std::find()

// Librerias necesarias para trabajar con MySQL Connector C++
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>

#pragma comment(lib, "ws2_32.lib")

// Variable utilizadas en la creacion de sockects
std::vector<SOCKET> clientSockets;
std::mutex clientMutex;
int clientID{};

// Constantes del servidor MySQL
const std::string address{"tcp://127.0.0.1:3306"};
const std::string username{ "root" };
const std::string password{ "92110876540a" };
const std::string database{ "ejercicio" };

// Funcion encargada de la comunicacion con un cliente especifico.
void HandleClient(SOCKET clientSocket, int clientID);

// Funcion encargada de realizar una consulta a la base de datos
void sqlQuery(std::string query);

// Funcion encargada de generar una consulta para la insercion de datos en una tabla creada
std::string insertQuery(std::string& message, std::string& cliente);

int main() {
    // Consulta SQL para crear la tabla Historial
    std::string query = "CREATE TABLE Historial ("
        "ID INT AUTO_INCREMENT PRIMARY KEY,"
        "Cliente VARCHAR(255) NOT NULL,"
        "Mensaje TEXT NOT NULL,"
        "Fecha DATETIME NOT NULL"
        ")";
    sqlQuery(query);
     
    // Inicialización de la bibilioteca de sockets (Winsock v2.2)
    WSADATA initWinsock;
    if (WSAStartup(MAKEWORD(2, 2), &initWinsock) != 0) {
        std::cerr << "Error al inicializar Winsock." << std::endl;
        return 1;
    }

    // Creacion del socket del lado servidor
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket del servidor." << std::endl;
        WSACleanup(); // Libera los recursos utilizados por Winsock.
        return 1;
    }

    // Configuracion de dirección y puerto del socket del lado servidor utilizando
    // la estructura sockaddr_in. 
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Enlazamiento del socket creado con la dirección IP y puerto configurados. 
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Error al enlazar el socket del servidor." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // El socket del servidor escucha las conexiones entrantes de clientes
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error al escuchar." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Servidor iniciado. Escuchando conexiones entrantes...." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        // Verifica si el socket del cliente creado es válido
        if (clientSocket != INVALID_SOCKET) {
            clientMutex.lock();
            clientSockets.push_back(clientSocket); // Se agrega al vector clientSockets el socket del cliente conectado 
            clientMutex.unlock();

            std::cout << "Cliente conectado." << std::endl;
            clientID++;

            std::thread clientThread(HandleClient, clientSocket, clientID);
            clientThread.detach();
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

// Definicion de las funciones utilizadas en el programa
void HandleClient(SOCKET clientSocket, int clientID) {
    char buffer[4096];

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0); // Recepcion de datos del cliente a través del socket

        if (bytesReceived > 0) {
            std::string message(buffer, bytesReceived);
            std::string cliente{ "Client " + std::to_string(clientID) };
            std::string header{ cliente + ": " + message };

            sqlQuery(insertQuery(message, cliente));

            clientMutex.lock();
            // Difusion del mensaje envaido por un cliente a los demas clientes conectados
            for (auto client : clientSockets) {
                if (client != clientSocket) {
                    send(client, header.c_str(), header.size(), 0);
                }
            }
            clientMutex.unlock();
        }
        else if (bytesReceived == 0) {
            clientMutex.lock();
            // Si se encuentra el socket del cliente y se elimina del vector clientSockets
            auto it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
            if (it != clientSockets.end()) {
                clientSockets.erase(it);
            }
            clientMutex.unlock();

            std::cout << "Cliente desconectado." << std::endl;
            break;
        }
        else {
            std::cerr << "Error en la recepcion de datos." << std::endl;
            break;
        }
    }
    closesocket(clientSocket);
}

void sqlQuery(std::string query)
{
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(address, username, password);
        con->setSchema(database); 

        sql::Statement* stmt = con->createStatement();
        stmt->execute(query);

        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        std::cout << "SQLException: " << e.what() << std::endl;
    }
}

std::string insertQuery(std::string& message, std::string& cliente)
{
    std::string insert{};

    // Obtener la fecha y hora actual
    time_t now = time(0);
    struct tm tstruct;
    char fecha[80];
    localtime_s(&tstruct, &now);
    strftime(fecha, sizeof(fecha), "%Y-%m-%d %X", &tstruct);

    // Consulta SQL para insertar datos en la tabla Historial
    return insert = "INSERT INTO Historial (Cliente, Mensaje, Fecha) VALUES ('"
        + cliente + "', '"
        + message + "', '"
        + fecha + "')";
}