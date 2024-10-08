#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
int main()
{
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket == INVALID_SOCKET)
  {
    std::cerr << "Socket creation failed" << std::endl;
    return 1;
  }

  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(54000);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (connect(clientSocket, (sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
  {
    std::cerr << "Failed to connect to server" << std::endl;
    return 1;
  }
  std::string message;
  char buffer[1024];
  int bytesReceived;
  while (true)
  {
    std::cout << "Enter message (or 'exit' to quit): ";
    std::getline(std::cin, message);
    if (message == "exit")
    {
      break;
    }

    send(clientSocket, message.c_str(), message.size(), 0);

    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0)
    {
      buffer[bytesReceived] = '\0';
      std::cout << "Server response: " << buffer << std::endl;
    }
  }

  closesocket(clientSocket);
  WSACleanup();
  return 0;
}
