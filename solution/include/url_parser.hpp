#ifndef URL_PARSER_HPP
#define URL_PARSER_HPP

#include <string>

class UrlParser{
    static const int port_http = 80;
    static const int port_https = 443;
public:
    std::string protocol;
    std::string host;
    int port;
    std::string path;

    UrlParser(const std::string& url){
        size_t end_protocol = url.find("://");
        if (end_protocol == 5) {
            protocol = "https";
            port = port_https;
        } else {
            protocol = "http";
            port = port_http;
        }

        size_t begin_host = end_protocol + 3;
        size_t end_host = url.find(":", begin_host);

        if (end_host != std::string::npos){
            size_t begin_port = end_host + 1;
            size_t end_port = url.find("/", begin_port);

            std::string new_port;
            if (end_port != std::string::npos) {
                new_port = url.substr(begin_port, 
                    end_port - begin_port);
            } else {
                new_port = url.substr(begin_port);
            }
            port = std::stoi(new_port);

        } else {
            end_host = url.find("/", begin_host);
            if (end_host == std::string::npos) {
                end_host = url.size();
            }
        }
        host = url.substr(begin_host, end_host - begin_host);

        size_t path_begin = url.find("/", end_host);
        if (path_begin != std::string::npos) {
            path = url.substr(path_begin);
        } else {
            path = "/";
        }
    }

    std::string get_filename() const {
        size_t last_slash = path.find_last_of("/");
        std::string filename;
        if (last_slash != std::string::npos && 
            last_slash < path.size() - 1) {
                filename = path.substr(last_slash + 1);
        }
        return filename;
    }

};

#endif