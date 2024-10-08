#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
void ClientHandler(SOCKET clientSocket)
{
  char buffer[1024];
  int bytesReceived;

  while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
  {
    buffer[bytesReceived] = '\0';
    std::cout << "Received message from client: " << buffer << std::endl;

    std::string response = "Server received: " + std::string(buffer);
    send(clientSocket, response.c_str(), response.size(), 0);
  }

  closesocket(clientSocket);
}
int main()
{
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket == INVALID_SOCKET)
  {
    std::cerr << "Socket creation failed" << std::endl;
    return 1;
  }

  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(54000);
  bind(serverSocket, (sockaddr *) &serverAddr, sizeof(serverAddr));

  listen(serverSocket, SOMAXCONN);
  std::cout << "Server is listening on port 54000..." << std::endl;
  std::vector< std::thread > clientThreads;

  while (true)
  {
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr *) &clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET)
    {
      std::cerr << "Failed to accept connection" << std::endl;
      continue;
    }

    clientThreads.push_back(std::thread(ClientHandler, clientSocket));
  }

  closesocket(serverSocket);
  WSACleanup();

  for (auto &t: clientThreads)
  {
    if (t.joinable())
    {
      t.join();
    }
  }
  return 0;
}
