#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>



// Логирование (в stdout с временем до мс):

// Запуск программы

// Входные параметры

// Начало загрузки файла

// Окончание загрузки файла

// Ошибки HTTP (не 200)

// Завершение программы



std::string current_time(){
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    return ss.str();
}


std::vector<std::string> file_read(const std::string& file_with_urls){
    std::ifstream file(file_with_urls);
    std::vector<std::string> urls;

    if(!file.is_open()){
        std::cerr << "Файл невозможно открыть! " << current_time() << std::endl;
        return urls;
    }

    std::string line;
    while (std::getline(file, line)) {
        if(line.empty()) continue;

        if ((line.find("https://") == 0) || (line.find("http://") == 0))
            urls.push_back(line);   
    }
    
    file.close();
    return urls;
}


int main(int argc, char const *argv[])
{
    if(argc != 4){
        std::cerr << "Неверное количество аргументов! " 
            << current_time() << std::endl;
        std::cerr << "Использование: " << argv[0] << " <file_with_urls> "
            "<output_directory> <concurrent_downloads>" << std::endl;
        return 1;
    }
    std::string file_with_urls = argv[1];
    std::string output_directory = argv[2];
    std::string concurrent_downloads = argv[3];

    std::cout << "==================================" << std::endl;
    std::cout << "Программа запущена " << current_time() << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Файл с URL: " << file_with_urls << std::endl;
    std::cout << "Папка сохранения: " << output_directory << std::endl;
    std::cout << "Количество файлов загружаемых одновременно: " 
        << concurrent_downloads << std::endl;
    std::cout << "==================================" << std::endl;


    std::vector<std::string> urls = file_read(file_with_urls);
    for (auto it = urls.begin(); it != urls.end(); ++it){
        std::cout << *it << std::endl;
    }
    

    return 0;
}
