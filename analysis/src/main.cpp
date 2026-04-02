#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <filesystem>
#include <random>


class Manager{
public:
    Manager(int count_link) {
        if(!std::filesystem::exists("build") ||
            !std::filesystem::is_directory("build")) {
                std::filesystem::create_directories("build");
            }
        create_test_file(count_link);
        std::cout << "Папка build и тестовые данные созданы\n";
        std::cout << "==========================\n";
    }

    void run(int count_thread, int count_start) const {
        build();
        std::cout << "Идут запуски программ\n";
        std::cout << "==========================\n";
        std::string command(
            "./loader ../__links__.txt ../directory_for_files ");
        command += std::to_string(count_thread);
        for (int i = 0; i < count_start; ++i){
            std::string command_cmd = command;
            command_cmd += " > \"../../log/" + 
                std::to_string(i + 1) + ".log\" 2>&1";

            auto start_time = std::chrono::high_resolution_clock::now();

            int system_code = std::system(command_cmd.c_str());
            if(system_code != 0){
                std::cout << "Выполнения " + std::to_string(i + 1) + 
                    " запуска привело к ошибке, код - " << 
                        std::to_string(system_code) << std::endl;
                std::cout << "==========================\n";
                continue;
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = end_time - start_time;
            std::chrono::duration<double> seconds = duration;

            std::cout << "Время выполнения " + std::to_string(i + 1) + 
                " запуска: " << std::fixed << std::setprecision(1) 
                    << seconds.count() << " сек" << std::endl;
            std::cout << "==================================" << std::endl;
        }
        std::cout << "Суммарно было скачано " << 
            std::to_string(count_files_in_directory("../directory_for_files")) 
                << " файла" << std::endl;
        std::cout << "==========================\n";
    }

    ~Manager(){
        std::filesystem::path parent = 
            std::filesystem::current_path().parent_path();
        std::filesystem::current_path(parent);
        std::filesystem::remove_all("build");
        std::filesystem::remove_all("directory_for_files");
        std::filesystem::remove("__links__.txt");
        std::cout << "Папка build и тестовые данные удалены" << "\n";
        std::cout << "==========================\n";
    }

private:
    size_t count_files_in_directory(const std::string& path) const {
        size_t count = 0;
        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (std::filesystem::is_regular_file(entry.status())) {
                    ++count;
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
        return count;
    }

    std::vector<std::string> file_read() const {
        std::ifstream file("../variants.txt");
        std::vector<std::string> urls;
        if(!file.is_open()){
            throw std::runtime_error("Файл невозможно открыть!");
        }

        std::string line;
        while (std::getline(file, line)) {
            if(line.empty()) continue;
            urls.push_back(line);
        }
        file.close();
        return urls;
    }

    void create_test_file(int count_link) const {
        std::vector<std::string> variant_links = file_read();
        std::ofstream file("__links__.txt");
        if (!file.is_open()) {
            throw std::runtime_error(
                "Ошибка: не удалось создать/открыть файл\n");
        }
        for(int i = 0; i < count_link; ++i){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, variant_links.size() - 1);
            std::string random_object = variant_links[dis(gen)];
            file << random_object << "\n";
        }
        file.close();
    }

    void build() const {
        std::filesystem::path build_dir = 
            std::filesystem::current_path() / "build";
        std::filesystem::current_path(build_dir);
        try{
            std::system("cmake ../../../solution");
            std::system("make");
        } catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
        std::cout << "==========================\n";
        std::cout << "Бинарный файл успешно создан\n";
        std::cout << "==========================\n";
    }
};





int main(int argc, char const *argv[])
{
    if(argc != 4){
        throw std::runtime_error("Неверное количество аргументов!");
    }

    int count_start = std::stoi(argv[1]);
    int count_thread = std::stoi(argv[2]);
    int count_link = std::stoi(argv[3]);

    std::cout << "===== Запуск тестов =====\n";
    std::cout << "Количество запусков = " << count_start  << "\n";
    std::cout << "Количество потоков = " << count_thread << "\n";
    std::cout << "Количество ссылок = " << count_link   << "\n";
    std::cout << "==========================\n";

    Manager(count_link).run(count_thread, count_start);

    return 0;
}
