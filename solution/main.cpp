#include <iostream>
#include <string>
#include <fstream>



// Логирование (в stdout с временем до мс):

// Запуск программы

// Входные параметры

// Начало загрузки файла

// Окончание загрузки файла

// Ошибки HTTP (не 200)

// Завершение программы




int file_read(const std::string& file_with_urls){
    std::ifstream file(file_with_urls);
    
    if(!file.is_open()){
        std::cerr << "Файл невозможно открыть" << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    
    file.close();
    return 0;
}




int main(int argc, char const *argv[])
{
    if(argc != 4){
        std::cerr << "Неверное количество аргументов!" << "\n";
        std::cerr << "Использование: " << argv[0] << " <file_with_urls> "
            "<output_directory> <concurrent_downloads>" << std::endl;
        return 1;
    }
    std::string file_with_urls = argv[1];
    std::string output_directory = argv[2];
    std::string concurrent_downloads = argv[3];

    std::cout << "==================================" << std::endl;
    std::cout << "Программа запущена" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Файл с URL: " << file_with_urls << std::endl;
    std::cout << "Папка сохранения: " << output_directory << std::endl;
    std::cout << "Количество файлов загружаемых одновременно: " 
        << concurrent_downloads << std::endl;
    std::cout << "==================================" << std::endl;


    file_read(file_with_urls);


    return 0;
}
