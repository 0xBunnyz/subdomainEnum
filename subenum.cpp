#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

void error(const char *msg)
{
    std::cerr << msg << ": " << strerror(errno) << std::endl;
    exit(0);
}

bool Connect(const std::string &host, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    struct hostent *server = gethostbyname(host.c_str());
    if (server == NULL) {
        std::cerr << "ERROR, no such host: " << host << std::endl;
        return false;
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting to " << host << std::endl;
        return false;
    }

    close(sockfd);
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " domain_name" << std::endl;
        exit(0);
    }

    std::string domain_name = argv[1];
    std::string subdomain;
    std::string host;
    std::ifstream file("words.txt");
    if (!file) {
        std::cerr << "ERROR opening file" << std::endl;
        exit(0);
    }

    while (file >> subdomain) {
        host = subdomain + "." + domain_name;
        if (Connect(host, 80)) {
            std::cout << "Connected to " << host << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    file.close();
    return 0;
}