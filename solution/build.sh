g++ -std=c++17 main.cpp

if [ $? -eq 0 ]; then
    echo "Компиляция успешна!"
else
    echo "Ошибка компиляции!"
fi