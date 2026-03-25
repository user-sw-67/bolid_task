#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <stdexcept>

#include "socket.hpp"
#include "url_parser.hpp"


class SocketHTTP{
public:
    SOCKET sock;

    SocketHTTP() : sock(socket(AF_INET, SOCK_STREAM, 0)) {
        std::cout << "socket" << std::endl;
        if (sock == INVALID_SOCKET) {
            throw std::runtime_error("Не могу создать сокет");
        }
    }

    ~SocketHTTP(){
        closesocket(sock);
    }

};

class DNS_HTTP : public SocketHTTP{
public:
    addrinfo* dns_server;

    DNS_HTTP(const UrlParser& url) : SocketHTTP(), dns_server(nullptr) {
        std::cout << "dns" << std::endl;
        const char* host = url.host.c_str();
        const char* port = std::to_string(url.port).c_str();
        addrinfo hints = {};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        int result_connect = getaddrinfo(host, port, &hints, &dns_server);
        if (result_connect != 0) {
            throw std::runtime_error("Не могу найти хост: " + url.host);
        }
    }

    ~DNS_HTTP() {
        if(dns_server != nullptr) {
            freeaddrinfo(dns_server);
        }
    }

};


class ConnectHTTP : public DNS_HTTP {
public:
    ConnectHTTP(const UrlParser& url) : DNS_HTTP(url) {
        std::cout << "connect" << std::endl;
        if(connect(sock, dns_server->ai_addr, dns_server->ai_addrlen) != 0) {
            throw std::runtime_error("Не удалось подключиться к серверу");
        }
    }
};


class RequestHTTP : public ConnectHTTP {
public:
    const UrlParser& url;

    RequestHTTP(const UrlParser& url) : ConnectHTTP(url), url(url) {}

    void send_request() {
        std::string request = 
            "GET " + url.path + " HTTP/1.1\r\n"
            "Host: " + url.host + "\r\n"
            "Connection: close\r\n"
            "\r\n";
        int res_send = send(sock, request.c_str(), request.size(), 0);
        if(res_send < 0) {
            std::runtime_error("Не могу отправить HTTP запрос");
        }
        std::cout << "good rq" << std::endl;
    }

    void recv_request() {
        while (true){
            char buffer[4096];
            int bytes = recv(sock, buffer, sizeof(buffer), 0);
            if (bytes <= 0) {return;}
            std::string str(buffer, bytes);
            std::cout << str << std::endl;
        }
    }
};




#endif