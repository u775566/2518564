#pragma once
#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <filesystem>
#include <memory>

/*
1. Информация о группе студентов из N человек, где запись о студенте содержит следующие данные:
1). Фамилия, имя, отчество студента.
2). Число, месяц, год рождения.
3). Год поступления в институт.
4). Факультет (институт).
5). Кафедра.
6). Группа.
7). Номер зачетной книжки.
8). Пол.
9). Названия предметов и оценки по каждому предмету в каждой сессии, максимально 9 сессий и 10 предметов в каждом семестре, которые могут быть разные.
Все данные должны быть форматными: даты, числа и т.д.
*/

class Student
{
	// для хранения текущего года
	static char CURR_YEAR[5];

	// параметры сессии
	static constexpr size_t SESS_COUNT = 9;
	static constexpr size_t SUBJS_COUNT = 10;
	// парметры таблиц вывода
	static constexpr size_t  INFO_TBL_WIDTH = 153;
	static constexpr size_t  EDIT_TBL_WIDTH = 159;
	static constexpr size_t  SESS_TBL_WIDTH = 87;
	
	// рабочий бинарный файл
	static constexpr char F_NAME[] = "students.bin";
	
	// семантические идентификаторы пола 
	enum Gender { male, female };

	// описание дня рождения
	struct Birthday {
		unsigned year : 11;
		unsigned month : 4;
		unsigned day : 5;
	};

	// описание фио
	struct FIO {
		char name[24];
		char patronimic[24];
		char surname[24];
	};

	// описание дисциплины
	struct Subject {
		char subject[48]{};	// название
		char grade[21]{};		// оценка
	};

	// описание сессии
	struct Session {
		Subject disciplines[SUBJS_COUNT]{}; // массив дисциплин
		double average{};										// средний балл
		unsigned active : 1;								// актвна/неактивна
	};
	
	// метод для вычисление среднего балла
	void calcAverage(size_t n);

	// полная информация о студенте для хранения в файле
	struct StudentInfo {

		FIO mFio{};
		Birthday mBirthday{};
		uint16_t mAdmissionY{};
		Gender mGender{};
		Session mSessions[SESS_COUNT]{};

		char mFaculty[8]{};
		char mDepartment[8]{};
		char mGroupID[16]{};
		char mGradeBookID[8]{};

	} mInfo;

public:

	// конструктор
	Student();
	
	// инициализация объекта
	bool init();
	// сортировка файла в соответствии с заданием
	void sortFile();
	// редактирование записей в файле
	void edit();
	// очистить информацию о студенте (mInfo)
	void clearInfo();

	// даёт доступ к mInfo снаружи
	// две версии - константная и не константная ссылки
	const StudentInfo& info() const;
	StudentInfo& info();

	// возвращает размер mInfo в байтах
	size_t infoSize() const;

	// возвращает имя файла снаружи
	static const char* fname();

	// методы вывода в консоль
	void printHeader() const;
	void print() const;
	void printFio() const;
	void printBirthday() const;
	void printSession(size_t n = std::numeric_limits<size_t>::max()) const;
	void printGender() const;
	void printAdmissionYear() const;
	void printFaculty() const;
	void printDepartment() const;
	void printGroupID() const;
	void printGradeBookID() const;

	// нормализация строки - первая буква заглавная, остальные прописные
	void normalizeUpperLower(std::string& str);

	// убирает пробельные символы в начале и конце строки
	void trimStrView(std::string_view& sv) const;
	
	// проверяет строку на пустоту, например, если были только пробелы, то считается пустой 
	bool isEmptyInputString(std::string_view& sv) const;

	// конвертирует строку из char в wchar_t для тестирования кириллицы 
	template<size_t N>
	void convertToWideStr(std::string_view sv, wchar_t(&ws)[N]) const;

	// проверяет строку на наличие определённых символов (буквы, цифры, дефис)
	// для номера группы
	template<size_t N>
	bool isAlphaNumDash(wchar_t(&ws)[N]) const;

	// проверяет строку на наличие определённых символов (буквы, цифры)
	// для номера зачётки
	template<size_t N>
	bool isAlphaNum(wchar_t(&ws)[N]) const;

	// проверяет слово из wchar_t на наличие определённых символов (только буквы)
	// для ФИО, дисциплин, названия факультета (кириллица)
	template<size_t N>
	bool isAlphabetic(wchar_t(&ws)[N]) const;

	// проверяет строку на наличие только букв, предварительно преобразовывая её
	// к wchar_t
	template<size_t N>
	bool checkAlphabetic(std::string& str, wchar_t(&ws)[N]) const;

	// тестовые методы для проверки формата названия факультета, кафедры и остальных данных студента
	template<size_t N>
	bool checkFaculty(std::string& str, wchar_t(&ws)[N]);

	template<size_t N>
	bool checkDepartment(std::string_view sv, wchar_t(&ws)[N]);

	template<size_t N>
	bool checkGroup(std::string_view sv, wchar_t(&ws)[N]);

	template<size_t N>
	bool checkGradeBook(std::string_view sv, wchar_t(&ws)[N]);

	template<size_t N>
	bool checkGender(std::string_view sv, wchar_t(&ws)[N]);

	template<size_t N>
	bool checkDisciplineMark(std::string& str, wchar_t(&ws)[N]);

	template<size_t N>
	bool checkDisciplineName(std::string& str, wchar_t(&ws)[N]);

	bool checkBirthday(std::string_view sv);

	bool checkAdmissionYear(std::string_view sv);
};

