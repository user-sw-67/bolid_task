#include <iostream>
#include <string>





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
    return 0;
}
