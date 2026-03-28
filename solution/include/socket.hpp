#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdexcept>
#include <iostream>
#include <mutex>

#include "url_parser.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    
    typedef int SOCKET;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define closesocket close
#endif


extern std::mutex mutex_cout;


class SocketDefine{
public:
    SocketDefine() {
        #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
    }
    
    ~SocketDefine() {
        #ifdef _WIN32
            WSACleanup();
        #endif
    }
};


class SSLDefine {
private:
    static bool initialized;
public:
    static SSL_CTX* context;

    SSLDefine() {
        if (!initialized) {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_all_algorithms();
            context = SSL_CTX_new(TLS_client_method());
            if (!context) {
                throw std::runtime_error("Не могу создать SSL контекст");
            }
            SSL_CTX_set_verify(context, SSL_VERIFY_NONE, nullptr);
            initialized = true;
        }
    }

    ~SSLDefine() {
        if (initialized && context) {
            SSL_CTX_free(context);
        }
    }

};


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


class SSL_HTTP : public SocketHTTP {
public:
    SSL* ssl;

    SSL_HTTP(const UrlParser& url) : SocketHTTP(), ssl(nullptr) {
        ssl = SSL_new(SSLDefine::context);
        if (!ssl) {
            throw std::runtime_error("Не могу создать SSL объект");
        }
        
        SSL_set_tlsext_host_name(ssl, url.host.c_str());
        SSL_set_fd(ssl, sock);
    }
    
    void ssl_connect() {
        int ret = SSL_connect(ssl);
        if (ret != 1) {
            int err = SSL_get_error(ssl, ret);
            {
                std::lock_guard<std::mutex> lock(mutex_cout);
                std::cerr << "SSL_get_error = " << err << std::endl;
            }
            
            if (err == SSL_ERROR_SYSCALL) {
                throw std::runtime_error("Системная ошибка");
            }
            
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("SSL рукопожатие не удалось");
        }
    }
    
    ~SSL_HTTP() {
        if (ssl) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
    }
};

#endif
