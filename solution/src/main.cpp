#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <filesystem>
#include <stdexcept>
#include <thread>
#include <atomic>
#include <mutex>

#include "socket.hpp"
#include "url_parser.hpp"
#include "client.hpp"
#include "time.hpp"
#include "multithreading.hpp"


bool SSLDefine::initialized = false;
SSL_CTX* SSLDefine::context = nullptr;
const int UrlParser::port_http = 80;
const int UrlParser::port_https = 443;
std::mutex mutex_cout;
std::mutex UniqueNameFile::name_mutex;


std::vector<UrlParser> file_read(const std::string& file_name) {
    std::ifstream file(file_name);
    std::vector<UrlParser> urls;

    if(!file.is_open()){
        throw std::runtime_error("Файл невозможно открыть!");
    }

    std::string line;
    while (std::getline(file, line)) {
        if(line.empty()) continue;

        if ((line.find("https://") == 0) || (line.find("http://") == 0))
            urls.push_back(UrlParser(line));
    }

    file.close();
    return urls;
}


int main(int argc, char const *argv[])
{   
    std::cout << "==================================" << std::endl;
    std::cout << "Программа запущена " << current_time() << std::endl;
    std::cout << "==================================" << std::endl;

    if(argc != 4){
        throw std::runtime_error("Неверное количество аргументов!");
    }

    std::string file_with_urls = argv[1];
    std::string output_directory = argv[2];
    int concurrent_downloads = std::stoi(argv[3]);
    
    if(!std::filesystem::exists(output_directory) ||
        !std::filesystem::is_directory(output_directory)) {
            std::filesystem::create_directories(output_directory);
        }

    std::cout << "Файл с URL: " << file_with_urls << std::endl;
    std::cout << "Папка сохранения: " << output_directory << std::endl;
    std::cout << "Количество файлов загружаемых одновременно: " 
        << concurrent_downloads << std::endl;
    std::cout << "==================================" << std::endl;

    SocketDefine socket_def;
    SSLDefine ssl_def;

    std::vector<UrlParser> urls = file_read(file_with_urls);

    MultithreadingWorker multithreading_worker(
        urls, output_directory, concurrent_downloads);
    
    multithreading_worker.run();
    
    std::cout << "Программа завершена " << current_time() << std::endl;
    std::cout << "==================================" << std::endl;

    return 0;
}
