#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdexcept>
#include <fstream>
#include <iostream>

#include "socket.hpp"
#include "url_parser.hpp"
#include "time.hpp"



class SocketHTTP{
public:
    SOCKET sock;

    SocketHTTP() : sock(socket(AF_INET, SOCK_STREAM, 0)) {
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
        if(connect(sock, dns_server->ai_addr, dns_server->ai_addrlen) != 0) {
            throw std::runtime_error("Не удалось подключиться к серверу");
        }
    }
};


class RequestHTTP : public ConnectHTTP {
public:
    const UrlParser& url;

    RequestHTTP(const UrlParser& url) : ConnectHTTP(url), url(url) {}

    void send_request() const {
        std::string request = 
            "GET " + url.path + " HTTP/1.1\r\n"
            "Host: " + url.host + "\r\n"
            "Connection: close\r\n"
            "\r\n";
        int res_send = send(sock, request.c_str(), request.size(), 0);
        if(res_send < 0) {
            std::runtime_error("Не могу отправить HTTP запрос");
        }
    }

    void recv_request(const std::string& dir) const {
        std::string head;
        char buffer[4096];
        while (true){
            int bytes = recv(sock, buffer, sizeof(buffer), 0);
            if (bytes <= 0) {break;}
            std::string data_str(buffer, bytes);

            size_t begin_endl = data_str.find("\r\n\r\n");
            if (begin_endl != std::string::npos) {
                head.append(data_str.substr(0, begin_endl));

                if (head.find("200 OK") == std::string::npos) {
                    std::cerr << "Сервер вернул ошибку (не 200 OK) для URL-a " 
                        << url.protocol << "://" << url.host + ":" << url.port 
                        << url.path << " " << current_time() << std::endl;
                    std::cout << "==================================" 
                        << std::endl;
                    return;
                }

                size_t begin_data = begin_endl + 4;
                std::string name_file = get_file_name(head, dir);

                std::cout << "Начало загрузки файла " + name_file 
                    << " " << current_time() << std::endl;
                std::cout << "==================================" << std::endl;

                std::ofstream file(dir + "/" + name_file, std::ios::binary);
                if (!file.is_open()) {
                    throw std::runtime_error(
                        "Не удалось открыть файл для записи");
                }
                if (begin_data < data_str.size()) {
                    file.write(data_str.c_str() + begin_data, 
                        data_str.size() - begin_data);
                }
                while (true) {
                    int bytes = recv(sock, buffer, sizeof(buffer), 0);
                    if (bytes <= 0) {break;}
                    file.write(buffer, bytes);
                }
                file.close();

                std::cout << "Окончание загрузки файла " + name_file 
                    << " " << current_time() << std::endl;
                std::cout << "==================================" << std::endl;

                break;

            } else {
                head.append(data_str);
            }
        }
    }

    std::string get_file_name(const std::string& header, 
        const std::string& dir) const {
            std::string name;
            size_t begin_cd = header.find("Content-Disposition:");

            if (begin_cd != std::string::npos){
                size_t end_line = header.find("\r\n", begin_cd);
                std::string cd_line = header.substr(begin_cd, 
                    end_line - begin_cd);

                if (cd_line.find("attachment") != std::string::npos) {
                    size_t begin_fn = cd_line.find("filename=\"");

                    if (begin_fn != std::string::npos) {
                        size_t begin_name = begin_fn + 10;
                        size_t end_name = cd_line.find("\"", begin_name);
                        name = cd_line.substr(begin_name, 
                            end_name - begin_name);
                    }
                }
            }

            if (name.empty()) {
                name = url.get_filename();
            }

            valid_name(name);
            unique_name(name, dir);

            return name;
    }


private:
    void valid_name(std::string& name) const {
        for(char& c : name){
            bool is_valid = false;
        
            if ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9')) {
                is_valid = true;
            }

            else if (c == '.' || c == '-' || c == '_') {
                is_valid = true;
            }

            #ifdef _WIN32
                if (c == '\\' || c == '/' || c == ':' || c == '*' ||
                    c == '?' || c == '"' || c == '<' || c == '>' || c == '|'
                    || c == ' ') {
                    is_valid = false;
                }
            #else
                if (c == '/' || c == ' ') {
                    is_valid = false;
                }
            #endif

            if (!is_valid) {
                c = '_';
            }
        }
    }

    void unique_name(std::string& name, const std::string& dir) const {
        std::string full_path = dir + "/" + name;
        if (!std::filesystem::exists(full_path)) {
            return;
        }

        size_t dot_pos = name.find_last_of('.');
        std::string base_name, extension;
        
        if (dot_pos != std::string::npos) {
            base_name = name.substr(0, dot_pos);
            extension = name.substr(dot_pos);
        } else {
            base_name = name;
            extension = "";
        }
        size_t counter = 1;
        while (true) {
            std::string new_name = base_name + " (" + 
                std::to_string(counter) + ")" + extension;
            std::string new_path = dir + "/" + new_name;
            
            if (!std::filesystem::exists(new_path)) {
                name = new_name;
                return;
            }
            ++counter;
        }
    }

};


#endif