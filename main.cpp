// StudentSearchEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <iostream>
#include <iomanip>

#include <clocale>
#include "Student.h"
#include "crypto.h"


int main()
{
	namespace fs = std::filesystem;

#ifdef _WIN32
	system("chcp 1251");
	std::setlocale(LC_CTYPE, "Rus");
#endif //  _WIN32

	// создание файла
	std::string input;
	std::cout
		<< "Для создания файла нажмите ENTER\n"
		<< "Для выхода из программы нажмите 1\n>> ";
	std::getline(std::cin, input);

	if (input.length() == 0)
	{
		// если нажали ENTER
		std::cout << "Кол-во студентов в группе: ";
		std::getline(std::cin, input);

		// проверка ввода
		bool number = true;
		for (const auto& ch : input)
			if (!std::isdigit(ch))
			{
				std::cerr << "Введено не число\n";
				number = false;
				break;
			}

		// если введено число
		if (number)
		{
			size_t N{};
			N = std::stoul(input);

			// очистить файл если он не пуст
			if (!fs::is_empty(Student::fname()))
				fs::resize_file(Student::fname(), 0);

			// открыть бинарный файл для ввода/вывода
			std::fstream binfile{ Student::fname(), std::ios::binary | std::ios::in | std::ios::out };
			Student stud;

			// флаг шифрования
			bool encrypted = false;

			// инициализация N записей студентов
			for (size_t i = 0; i < N; ++i)
			{
				// инициализация
				if (stud.init())
				{
					// вывод данных текущего студента в консоль
					stud.print();
					stud.printSession();
					system("pause");

					// вывод текущей записи в файл
					if (!binfile.write(reinterpret_cast<const char*>(&stud.info()), stud.infoSize()))
					{
						std::cerr << "Ошибка записи в файл.\n";
						return EXIT_FAILURE;
					}
				}
			}
			// очистить буфер mInfo
			stud.clearInfo();

			std::cout << "ФАЙЛ ЗАПИСАН\n";
			system("pause");

			// вывод данных из файла в порядке ввода информации
			binfile.seekg(0, std::ios::beg);
			while (binfile.good())
			{
				// непосредственно чтение и вывод в консоль
				if (binfile.read(reinterpret_cast<char*>(&stud.info()), stud.infoSize()))
				{
					stud.print();
					stud.printSession();
				}
				else break;
			}
			// закрыть файл, очистить буфер mInfo
			binfile.close();
			stud.clearInfo();

			// основное меню
			while (true)
			{
				std::cout
					<< std::endl
					<< "1.Отсортировать список в соответствии с заданием\n"
					<< "2.Редактировать список\n"
					<< "3.Зашифровать список\n"
					<< "4.Выход\n"
					<< ">> ";
				std::getline(std::cin, input);

				// проверка ввода
				if (input.length() == 0)
				{
					std::cerr << "Введена пустая строка\n";
					break;
				}

				if (input.length() != 1)
				{
					std::cerr << "Некорректный ввод\n";
					break;
				}

				number = std::isdigit(input[0]) ? true : false;

				// если введено однозначное число
				if (number)
				{
					// пункт меню 4 - завершение программы
					if (input[0] == '4')
					{
						std::cout << "Работа программы завершена\n";
						break;
					}

					// если файл зашифрован, то расшифровать
					if (encrypted)
						Decrypt();


					// обработка соответствующего пункта меню
					switch (input[0])
					{
					case '1':
						stud.sortFile();
						break;
					case '2':
						stud.edit();
						break;
					case '3':
						Crypt();
						encrypted = true;
						break;
					default:
						std::cerr << "В меню нет такого пункта\n";
						break;
					}
				}
				else
				{
					std::cerr << "Введено не число\n";
					number = false;
					break;
				}
			}	// while(true)
		}	// if(number)
	}
	else
		std::cout << "Работа программы завершена\n";
}