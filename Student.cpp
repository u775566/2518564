#define _CRT_SECURE_NO_WARNINGS

#include "Student.h"
#include <cwctype>

char Student::CURR_YEAR[5]{};
namespace fs = std::filesystem;

Student::Student()
{
	// текущий год по системным часам
	std::memmove(CURR_YEAR, __DATE__ + 7, 4);
}

template<size_t N>
void Student::convertToWideStr(std::string_view sv, wchar_t(&ws)[N]) const
{
	if (sv.empty())
		return;

	// конвертирование строки char в wchar_t
	// нужно для корректного тестирования русских букв
	std::wmemset(ws, L'\0', N);
	std::mbstowcs(ws, sv.data(), std::min(N, sv.length()));

	/*std::wcout
		<< "wide string: '" << ws << "'"
		<< '(' << n << ')' << '\n';*/
}

void Student::clearInfo()
{
	// обнуление памяти
	memset(&mInfo, 0, sizeof mInfo);
}

void Student::calcAverage(size_t n)
{
	using namespace std::string_view_literals;
	auto& session = mInfo.mSessions[n];
	session.average = 0;

	size_t k = 0;
	// вычисление среднего балла с преобразованием оценок в числа
	for (size_t i = 0; i < SUBJS_COUNT; i++)
	{
		size_t len = std::strlen(session.disciplines[i].subject);
		std::string_view grade{ session.disciplines[i].grade };
		if (len > 0)
		{
			++k;
			if (grade == "Отлично"sv || grade == "5"sv)
				session.average += 5;
			else if (grade == "Хорошо"sv || grade == "4"sv)
				session.average += 4;
			if (grade == "Удовлетворительно"sv || grade == "3"sv)
				session.average += 3;
			if (grade == "Неудовлетворительно"sv || grade == "2"sv)
				session.average += 2;
			if (grade == "Зачтено"sv)
				session.average += .5;
			if (grade == "Не зачтено"sv)
				session.average -= .5;
		}
	}
	if (k != 0)
		session.average /= k;
}

void Student::normalizeUpperLower(std::string& str)
{
	// первая буква в строке заглавная, остальные прописные
	for (size_t i = 0; i < str.length(); i++)
		str[i] = i == 0 ? std::toupper(str[i]) : std::tolower(str[i]);
};

bool Student::isEmptyInputString(std::string_view& sv) const
{
	// удаляются пробелы в начале и в конце строки
	trimStrView(sv);

	// после этого что-нибудь осталось?
	if (sv.empty())
	{
		std::cerr << "Введена пустая строка\n";
		return true;
	}

	return false;
}

// аргумент передаётся по ссылке!
void Student::trimStrView(std::string_view& sv) const
{
	if (sv.length() == 0)
		return;

	// отбросить пробелы в начале строки (если есть)
	sv.remove_prefix(std::min(sv.find_first_not_of(" \t"), sv.length()));

	// если что-то осталось отбросить пробелы в конце строки (если есть)
	if (!sv.empty())
		sv.remove_suffix(sv.size() - sv.find_last_not_of(" \t") - 1);
}

template<size_t N>
bool Student::checkAlphabetic(std::string& str, wchar_t(&ws)[N]) const
{
	// проверить на пустоту
	std::string_view sv{ str };
	if (isEmptyInputString(sv))
		return false;

	// конвертировать в wchar_t строку
	convertToWideStr(sv, ws);

	// проверить что содержит только буквы
	if (!isAlphabetic(ws))
		return false;

	// заменить входную строку (удалены пробелы в начале и в конце)
	str = sv;
	return true;
}

template<size_t N>
bool Student::isAlphaNumDash(wchar_t(&ws)[N]) const
{
	// перебор элементов wchar_t строки и проверка что содержит только буквы, цифры и дефис
	for (auto ch = ws; *ch != '\0'; ++ch)
	{
		if (!iswalnum(*ch) && *ch != L'-')
		{
			std::cerr << "Недопустимый символ:\n";
			std::wcout << "'" << *ch << "'" << std::endl;
			return false;
		}
	}

	return true;
}

template<size_t N>
bool Student::isAlphaNum(wchar_t(&ws)[N]) const
{
	// перебор элементов wchar_t строки и проверка что содержит только буквы и цифры
	for (size_t i = 0; i < std::wcslen(ws); ++i)
		if (!iswalnum(ws[i]))
		{
			std::cerr << "Недопустимый символ в слове!\n";
			std::wcout << "'" << ws[i] << "'";
			return false;
		}

	return true;
}

// перебор элементов wchar_t строки и проверка что содержит только буквы
template<size_t N>
bool Student::isAlphabetic(wchar_t(&ws)[N]) const
{
	for (size_t i = 0; i < std::wcslen(ws); ++i)
		if (!iswalpha(ws[i]))
		{
			std::cerr << "Недопустимый символ в слове!\n";
			std::wcout << "'" << ws[i] << "'";
			return false;
		}

	return true;
}

[[maybe_unused]] bool Student::checkBirthday(std::string_view sv)
{
	std::tm t{};
	std::stringstream ss;

	if (isEmptyInputString(sv))
		return false;

	// проверка правильного формата ввода даты рождения
	ss << sv;
	ss >> std::get_time(&t, "%d.%m.%Y");
	if (ss.fail())
	{
		std::cerr << "Некорректная дата рождения\n";
		return false;
	}

	mInfo.mBirthday.year = static_cast<unsigned>(t.tm_year += 1900);
	mInfo.mBirthday.month = static_cast<unsigned>(++t.tm_mon);
	mInfo.mBirthday.day = static_cast<unsigned>(t.tm_mday);

	return true;
};

[[maybe_unused]] bool Student::checkAdmissionYear(std::string_view sv)
{
	if (isEmptyInputString(sv))
		return false;

	if (sv.length() != 4)
	{
		std::cerr << "Некорректный ввод года рождения 1\n";
		return false;
	}

	// проверка строки на содержание только цифр
	for (auto d : sv)
		if (!std::isdigit(d))
		{
			std::cerr << "Некорректный ввод года рождения 2\n";
			return false;
		}

	// проверка года рождения студента - не позже текущего года и не раньше, чем 35 лет назад
	auto y = static_cast<uint16_t>(std::atoi(sv.data()));
	if (y > std::atoi(CURR_YEAR) || y < std::atoi(CURR_YEAR) - 35)
	{
		std::cerr << "Некорректный ввод года рождения 3\n";
		return false;
	}

	mInfo.mAdmissionY = y;
	return true;
}

template<size_t N>
[[maybe_unused]] bool Student::checkFaculty(std::string& str, wchar_t(&ws)[N])
{
	if (!checkAlphabetic(str, ws))
	{
		//std::cerr << "Недопустимые символы в названии факультета\n";
		return false;
	}

	// проверка на длину названия факультета (7 букв, судя по примеру данных)
	if (str.length() >= std::size(mInfo.mFaculty))
	{
		std::cerr << "Слишком длинное название факультета\n";
		return false;
	}

	// перевод в верхний регистр
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// инициализация
	memset(mInfo.mFaculty, '\0', sizeof mInfo.mFaculty);
	memcpy(mInfo.mFaculty, str.data(), str.length());

	return true;
}

template<size_t N>
[[maybe_unused]] bool Student::checkDepartment(std::string_view sv, wchar_t(&ws)[N])
{
	if (isEmptyInputString(sv))
		return false;

	convertToWideStr(sv, ws);

	if (!isAlphaNumDash(ws))
	{
		std::cerr << "Недопустимые символы в названии кафедры\n";
		return false;
	}

	// проверка на длину названия кафедры (7 букв, судя по примеру данных)
	if (std::wcslen(ws) >= std::size(mInfo.mDepartment))
	{
		std::cerr << "Слишком длинное название кафедры\n";
		return false;
	}

	// перевод в верхний регистр
	std::string str(sv.cbegin(), sv.cend());
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// инициализация
	memset(mInfo.mDepartment, '\0', sizeof mInfo.mDepartment);
	memcpy(mInfo.mDepartment, str.data(), str.length());

	return true;
}

template<size_t N>
[[maybe_unused]] bool Student::checkGroup(std::string_view sv, wchar_t(&ws)[N])
{
	if (isEmptyInputString(sv))
		return false;

	convertToWideStr(sv, ws);

	if (!isAlphaNumDash(ws))
	{
		std::cerr << "Недопустимые символы в номере группы\n";
		return false;
	}

	// проверка на длину номера группы
	if (std::wcslen(ws) >= std::size(mInfo.mGroupID))
	{
		std::cerr << "Слишком много символов в номере группы\n";
		return false;
	}

	// перевод в верхний регистр
	std::string str(sv.cbegin(), sv.cend());
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// инициализация
	memset(mInfo.mGroupID, '\0', sizeof mInfo.mGroupID);
	memcpy(mInfo.mGroupID, str.data(), str.length());

	return true;
}

template<size_t N>
[[maybe_unused]] bool Student::checkGradeBook(std::string_view sv, wchar_t(&ws)[N])
{
	if (isEmptyInputString(sv))
		return false;

	convertToWideStr(sv, ws);

	if (!isAlphaNum(ws))
	{
		std::cerr << "Недопустимые символы в номере зачётки\n";
		return false;
	}

	// проверка на длину номера зачётки
	if (std::wcslen(ws) > std::size(mInfo.mGradeBookID) - 1)
	{
		std::cerr << "Слишком много символов в номере зачётки\n";
		return false;
	}

	// перевод в верхний регистр
	std::string str(sv.cbegin(), sv.cend());
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// инициализация
	memset(mInfo.mGradeBookID, '\0', sizeof mInfo.mGradeBookID);
	memcpy(mInfo.mGradeBookID, str.data(), str.length());

	return true;
}

template<size_t N>
[[maybe_unused]] bool Student::checkGender(std::string_view sv, wchar_t(&ws)[N])
{
	if (isEmptyInputString(sv))
		return false;

	if (sv.length() > 1)
	{
		std::cerr << "Слишком много букв\n";
		return false;
	}

	convertToWideStr(sv, ws);

	// инициализация пола
	switch (ws[0])
	{
	case L'м':
	case L'М':
		mInfo.mGender = male;
		return true;

	case L'ж':
	case L'Ж':
		mInfo.mGender = female;
		return true;

	default:
		return false;
		break;
	}
}

template<size_t N>
bool Student::checkDisciplineMark(std::string& str, wchar_t(&ws)[N])
{
	std::string_view sv{ str };
	if (isEmptyInputString(sv))
		return false;

	// форматирование входной строки: 1-ая буква заглавная, остальный прописные
	normalizeUpperLower(str);

	sv = str;
	convertToWideStr(sv, ws);

	// проверка значения
	if (
		(std::wcscmp(ws, L"Отлично") == 0)
		|| (std::wcscmp(ws, L"Хорошо") == 0)
		|| (std::wcscmp(ws, L"Удовлетворительно") == 0)
		|| (std::wcscmp(ws, L"Неудовлетворительно") == 0)
		|| (std::wcscmp(ws, L"Зачтено") == 0)
		|| (std::wcscmp(ws, L"Не зачтено") == 0)
		|| (std::wcscmp(ws, L"2") == 0)
		|| (std::wcscmp(ws, L"3") == 0)
		|| (std::wcscmp(ws, L"4") == 0)
		|| (std::wcscmp(ws, L"5") == 0)
		)
		return true;

	return false;
}

template<size_t N>
bool Student::checkDisciplineName(std::string& str, wchar_t(&ws)[N])
{
	std::string_view sv{ str };
	if (isEmptyInputString(sv))
		return false;

	// форматирование входной строки: 1-ая буква заглавная, остальный прописные
	normalizeUpperLower(str);

	convertToWideStr(sv, ws);

	//проверка на недопустимые символы в названии дисциплины;
	for (size_t i = 0; i < std::wcslen(ws); i++)
	{
		if (
			(!std::iswalpha(ws[i]))
			&& (!std::iswblank(ws[i]))
			&& (ws[i] != '(')
			&& (ws[i] != ')')
			&& (ws[i] != ',')
			&& (ws[i] != '.')
			)
		{
			//std::cerr << "Недопустимые символы в названии дисциплины\n";
			return false;
		}
	}

	str = sv;
	return true;
}

bool Student::init()
{
	std::string in;
	wchar_t wstr[48]{};
	memset(&mInfo, 0, sizeof mInfo);

	size_t w = std::strlen("Название факультета (института): ");

	std::cout.setf(std::ios::left, std::ios::adjustfield);

	// ввод и проверка фамилии
	std::cout << std::setw(w) << "Фамилия: ";
	getline(std::cin, in);
	if (!checkAlphabetic(in, wstr))
		return false;

	// инициализация
	normalizeUpperLower(in);
	memcpy(mInfo.mFio.surname, in.c_str(), in.length());

	// ввод и проверка имени
	std::cout << std::setw(w) << "Имя: ";
	getline(std::cin, in);
	if (!checkAlphabetic(in, wstr))
		return false;

	// инициализация
	normalizeUpperLower(in);
	memcpy(mInfo.mFio.name, in.c_str(), in.length());

	// ввод и проверка отчества
	std::cout << std::setw(w) << "Отчество: ";
	getline(std::cin, in);
	if (!checkAlphabetic(in, wstr))
		return false;

	// инициализация
	normalizeUpperLower(in);
	memcpy(mInfo.mFio.patronimic, in.c_str(), in.length());

	//printFio();

	std::cout << std::setw(w) << "Дата рождения (дд.мм.ггг): ";
	getline(std::cin, in);
	if (!checkBirthday(in))
		return false;

	//printBirthday();

	std::cout << std::setw(w) << "Пол (м/ж): ";
	getline(std::cin, in);
	if (!checkGender(in, wstr))
		return false;

	//printGender();

	std::cout << std::setw(w) << "Год поступления: ";
	getline(std::cin, in);
	if (!checkAdmissionYear(in))
		return false;

	//printAdmissionYear();

	std::cout << std::setw(w) << "Название факультета (института): ";
	getline(std::cin, in);
	if (!checkFaculty(in, wstr))
		return false;

	//printFaculty();

	std::cout << std::setw(w) << "Название кафедры: ";
	getline(std::cin, in);
	if (!checkDepartment(in, wstr))
		return false;

	//printDepartment();

	std::cout << std::setw(w) << "Номер группы: ";
	getline(std::cin, in);
	if (!checkGroup(in, wstr))
		return false;

	//printGroupID();

	std::cout << std::setw(w) << "Номер зачётки: ";
	getline(std::cin, in);
	if (!checkGradeBook(in, wstr))
		return false;

	//printGradeBookID();

	// ввод данных сессий
	for (size_t i = 0; i < SESS_COUNT; i++)
	{
		auto& session = mInfo.mSessions[i];
		std::cout << "\tСессия #" << i + 1 << "\n";

		// ввод данных по дисциплинам i-ой сессии
		for (size_t j = 0; j < SUBJS_COUNT; )
		{
			auto& discipline = session.disciplines[j];

			std::cout << "Дисциплина: ";
			getline(std::cin, in);

			if (!checkDisciplineName(in, wstr))
				std::cerr << "Ошибка в название дисциплины\n";
			else
			{

				memset(discipline.subject, '\0', sizeof discipline.subject);
				memcpy(discipline.subject, in.data(), in.length());

				std::cout << "Оценка (зачтено/не зачтено): ";
				getline(std::cin, in);

				if (!checkDisciplineMark(in, wstr))
					std::cerr << "Ошибка ввода оценки\n";
				else
				{
					memset(discipline.grade, '\0', sizeof discipline.grade);
					memcpy(discipline.grade, in.data(), in.length());
					++j;

					if (!session.active)
						session.active = 1;
				}
			}

			std::cout
				<< "\tСледующая дисциплина:      нажмите 1\n"
				<< "\tЗакончить ввод  дисциплин: нажмите ENTER\n"
				<< "\t>> ";
			getline(std::cin, in);
			if (in != "1") break;
		}
		// вычисление среднего балла i-ой сессии
		calcAverage(i);

		std::cout
			<< "\tСледующая сессия:      нажмите 1\n"
			<< "\tЗакончить ввод сессий: нажмите ENTER\n"
			<< "\t>> ";
		getline(std::cin, in);
		if (in != "1") break;
	}

	return true;
}

// вывод заголовка информационной таблицы
void Student::printHeader() const
{
	for (size_t i = 0; i < INFO_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl
		<< '|'
		<< "      Фамилия Имя Отчество      " << '|'
		<< "   Дата рождения   " << '|'
		<< "    Пол    " << '|'
		<< "   Год поступления   " << '|'
		<< "   Факультет   " << '|'
		<< "   Кафедра   " << '|'
		<< "    Группа    " << '|'
		<< "   Номер зачётки   "
		<< '|' << std::endl;
	for (size_t i = 0; i < INFO_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl;
}

// вывод данных информационной таблицы
void Student::print() const
{
	std::cout.setf(std::ios::left, std::ios::adjustfield);
	printHeader();
	std::cout << "|  "; printFio();
	std::cout << "  |  "; printBirthday();
	std::cout << "  |  "; printGender();
	std::cout << "  |  "; printAdmissionYear();
	std::cout << "  |  "; printFaculty();
	std::cout << "  |  "; printDepartment();
	std::cout << "  |  "; printGroupID();
	std::cout << "  |  "; printGradeBookID();
	std::cout << "  |" << std::endl;
	for (size_t i = 0; i < INFO_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl;
}

// вывод фио
void Student::printFio() const
{
	char fio[72]{};
	std::strcat(fio, mInfo.mFio.surname);
	std::strcat(fio, " ");
	std::strcat(fio, mInfo.mFio.name);
	std::strcat(fio, " ");
	std::strcat(fio, mInfo.mFio.patronimic);
	std::cout << std::setw(28) << fio;
}

// вывод даты рождения
void Student::printBirthday() const
{
	char date[11]{};
	if (mInfo.mBirthday.day > 9)
		std::strcat(date, std::to_string(mInfo.mBirthday.day).c_str());
	else
	{
		std::strcat(date, "0");
		std::strcat(date, std::to_string(mInfo.mBirthday.day).c_str());
	}
	std::strcat(date, ".");

	if (mInfo.mBirthday.month > 9)
		std::strcat(date, std::to_string(mInfo.mBirthday.month).c_str());
	else
	{
		std::strcat(date, "0");
		std::strcat(date, std::to_string(mInfo.mBirthday.month).c_str());
	}
	std::strcat(date, ".");
	std::strcat(date, std::to_string(mInfo.mBirthday.year).c_str());

	std::cout << std::setw(15) << date;
}

// вывод пола
void Student::printGender() const
{
	if (mInfo.mGender == male)
		std::cout << "Мужской";
	else if (mInfo.mGender == female)
		std::cout << "Женский";
	else
		std::cout << "Неопределённый";
}

const Student::StudentInfo& Student::info() const
{
	return mInfo;
}

Student::StudentInfo& Student::info()
{
	return mInfo;
}

// сортировка файла
void Student::sortFile()
{
	std::string input;
	size_t num = 2;

	std::cout
		<< "Номер сессии по умолчанию 2\n"
		<< "Чтобы оставить значение по умолчанию нажмите ENTER\n\n";

	std::cout << "Ввести номер сессии (1-9 или ENTER): ";
	std::getline(std::cin, input);

	// проверка ввода номера сессии
	if (input.length() > 1)
	{
		std::cerr << "Введён неверный номер\n";
		return;
	}
	if (input.length() == 1 && std::isdigit(input[0]))
		num = static_cast<size_t>(input[0]) - '0';

	//std::cout << num << std::endl;
	
	// ввод интервала года рождения
	std::cout << "Введите интервал года рождения (гггг-гггг): ";
	std::getline(std::cin, input);

	// проверка корректности ввола
	if (input.length() == 0)
	{
		std::cout << "Введена пустая строка\n";
		return;
	}

	if (input.length() != 9)
	{
		std::cout << "Интервал введён неверно\n";
		return;
	}

	// парсинг строки с интервалом г.р.
	std::string year1, year2;
	year1 = input.substr(0, 4);
	for (const auto& ch : year1)
		if (!std::isdigit(ch))
		{
			std::cerr
				<< "Неверно введён год\n"
				<< "Сортировка остановлена\n";
			return;
		}

	year2 = input.substr(5);
	for (const auto& ch : year1)
		if (!std::isdigit(ch))
		{
			std::cerr
				<< "Неверно введён год\n"
				<< "Сортировка остановлена\n";
			return;
		}

	// если все проверки пройдены
	std::cout << year1 << '-' << year2 << std::endl;

	// открыть бинарный файл для записи/чтения
	std::fstream inout{ F_NAME, std::ios::binary | std::ios::in | std::ios::out };

	// если не открылся
	if (!inout.is_open())
	{
		std::cerr
			<< "Не удалось открыть файл\n"
			<< "Сортировка остановлена\n";
		return;
	}

	// вычисления кол-ва записей в файле
	size_t listLength = fs::file_size(F_NAME) / sizeof mInfo;
	std::cout << "LIST LENGTH " << listLength << std::endl;

	// если файл пуст
	if (listLength == 0)
	{
		std::cerr << "Файл пуст. Сортировка остановлена.\n";
		return;
	}

	// указатель на динамический массив для записей
	std::unique_ptr<StudentInfo[]> list{ new StudentInfo[listLength]{} };

	// чтение файла в массив list
	inout.seekg(0, std::ios::beg);
	inout.read(reinterpret_cast<char*>(list.get()), sizeof(StudentInfo) * listLength);

	// сортировка выбором с учётом с учётом интервала года рождения
	for (size_t i = 0; i < listLength - 1; i++)
	{
		if (list[i].mBirthday.year < std::stoul(year1) || list[i].mBirthday.year > std::stoul(year2))
			continue;

		size_t max = i;
		for (size_t j = i + 1; j < listLength; j++)
		{
			if (list[j].mBirthday.year < std::stoul(year1) || list[j].mBirthday.year > std::stoul(year2))
				continue;

			if (list[j].mSessions[num - 1].average > list[max].mSessions[num - 1].average)
				max = j;
		}

		if (max != i)
		{
			StudentInfo buf;
			memcpy(&buf, &list[i], sizeof(StudentInfo));
			memcpy(&list[i], &list[max], sizeof(StudentInfo));
			memcpy(&list[max], &buf, sizeof(StudentInfo));
			//std::swap(list[i], list[max]);
		}
	}

	// запись отсортированного массива в файл
	inout.seekp(0, std::ios::beg);
	if (inout.write(reinterpret_cast<const char*>(list.get()), sizeof(StudentInfo) * listLength))
	{
		std::cout << "ЗАПИСЬ ФАЙЛА ПОСЛЕ СОРТИРОВКИ УСПЕШНО\n";
		system("pause");
		std::cout << "ЧТЕНИЕ ФАЙЛА ПОСЛЕ СОРТИРОВКИ\n";

		// если запись прошла без ошибок, чтение файла после сортировки и вывод в консоль
		inout.seekg(0, std::ios::beg);
		for (size_t i = 0; i < listLength; i++)
		{
			inout.read(reinterpret_cast<char*>(&mInfo), sizeof mInfo);
			if (inout.good())
			{
				print();
				printSession();
			}
			else break;
		}
	}
	else
		std::cout << "ОШИБКА ЗАПИСИ В ФАЙЛ ПОСЛЕ СОРТИРОВКИ\n";

	inout.close();

}

void Student::printAdmissionYear() const
{
	std::cout << std::setw(17) << mInfo.mAdmissionY;
}

size_t Student::infoSize() const
{
	return sizeof mInfo;
}

const char* Student::fname()
{
	return F_NAME;
}

void Student::printFaculty() const
{
	std::cout << std::setw(11) << mInfo.mFaculty;
}

void Student::printDepartment() const
{
	std::cout << std::setw(9) << mInfo.mDepartment;
}

// редактирование записив файле
void Student::edit()
{
	// открыть бинарный файл для ввода/вывода
	std::fstream inoutbin{ F_NAME, std::ios::binary | std::ios::in | std::ios::out };

	// проверка
	if (!inoutbin.is_open())
	{
		std::cerr << "Ошибка при открытии файла для чтения\n";
		return;
	}

	// вычисление кол-ва записей в файле
	size_t length = fs::file_size(F_NAME) / sizeof mInfo;

	// вывод заголовка списка редактирования в консоль
	std::cout.setf(std::ios::left, std::ios::adjustfield);
	for (size_t i = 0; i < EDIT_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl
		<< '|'
		<< "  №  " << '|'
		<< "      Фамилия Имя Отчество      " << '|'
		<< "   Дата рождения   " << '|'
		<< "    Пол    " << '|'
		<< "   Год поступления   " << '|'
		<< "   Факультет   " << '|'
		<< "   Кафедра   " << '|'
		<< "    Группа    " << '|'
		<< "   Номер зачётки   "
		<< '|' << std::endl;
	for (size_t i = 0; i < EDIT_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl;

	// вывод пронумерованного списка студентов
	for (size_t i = 0; i < length; i++)
	{
		inoutbin.read(reinterpret_cast<char*>(&mInfo), sizeof mInfo);
		if (!inoutbin.good())
			break;

		std::cout << "| " << std::setw(3) << i + 1;
		std::cout << " |  "; printFio();
		std::cout << "  |  "; printBirthday();
		std::cout << "  |  "; printGender();
		std::cout << "  |  "; printAdmissionYear();
		std::cout << "  |  "; printFaculty();
		std::cout << "  |  "; printDepartment();
		std::cout << "  |  "; printGroupID();
		std::cout << "  |  "; printGradeBookID();
		std::cout << "  |" << std::endl;
		for (size_t i = 0; i < EDIT_TBL_WIDTH; i++)	std::cout << '-';
		std::cout << std::endl;
	}

	// ввод номера записи для редактирования
	std::string recNum;
	std::cout << "Введите номер записи для редактирования: ";
	std::getline(std::cin, recNum);
	
	// проверка, что не пусто
	if (recNum.length() == 0)
	{
		std::cout << "Номер не введён\n";
		std::memset(&mInfo, 0, sizeof mInfo);
		return;
	}

	// проверка что введено число
	bool num = true;
	for (const auto& ch : recNum)
		if (!std::isdigit(ch))
		{
			num = false;
			return;
		}

	// конвертирование строки в число
	size_t n = std::stoul(recNum);

	// чтение требуемой записи в mInfo
	inoutbin.seekg((n - 1) * sizeof mInfo, std::ios::beg);
	inoutbin.read(reinterpret_cast<char*>(&mInfo), sizeof mInfo);


	/******************** запросы на редактирование данных, проверка т изменение, если нужно ****************/
	std::string in;
	wchar_t wstr[48]{};
	size_t w = std::strlen("Название факультета (института): ");
	std::cout << "Чтобы оставить текщее значение нажмите ENTER\n";

	std::cout << std::setw(w) << "Фамилия: ";
	getline(std::cin, in);
	if (checkAlphabetic(in, wstr))
	{
		normalizeUpperLower(in);
		memset(mInfo.mFio.surname, '\0', sizeof mInfo.mFio.surname);
		memcpy(mInfo.mFio.surname, in.c_str(), in.length());
	}

	std::cout << std::setw(w) << "Имя: ";
	getline(std::cin, in);
	if (checkAlphabetic(in, wstr))
	{
		normalizeUpperLower(in);
		memset(mInfo.mFio.name, '\0', sizeof mInfo.mFio.name);
		memcpy(mInfo.mFio.name, in.c_str(), in.length());
	}

	std::cout << std::setw(w) << "Отчество: ";
	getline(std::cin, in);
	if (checkAlphabetic(in, wstr))
	{
		normalizeUpperLower(in);
		memset(mInfo.mFio.patronimic, '\0', sizeof mInfo.mFio.patronimic);
		memcpy(mInfo.mFio.patronimic, in.c_str(), in.length());
	}

	//printFio();

	std::cout << std::setw(w) << "Дата рождения (дд.мм.ггг): ";
	getline(std::cin, in);
	checkBirthday(in);

	//printBirthday();

	std::cout << std::setw(w) << "Пол (м/ж): ";
	getline(std::cin, in);
	checkGender(in, wstr);

	//printGender();

	std::cout << std::setw(w) << "Год поступления: ";
	getline(std::cin, in);
	checkAdmissionYear(in);

	//printAdmissionYear();

	std::cout << std::setw(w) << "Название факультета (института): ";
	getline(std::cin, in);
	checkFaculty(in, wstr);

	//printFaculty();

	std::cout << std::setw(w) << "Название кафедры: ";
	getline(std::cin, in);
	checkDepartment(in, wstr);

	//printDepartment();

	std::cout << std::setw(w) << "Номер группы: ";
	getline(std::cin, in);
	checkGroup(in, wstr);

	//printGroupID();

	std::cout << std::setw(w) << "Номер зачётки: ";
	getline(std::cin, in);
	checkGradeBook(in, wstr);

	//printGradeBookID();

	for (size_t i = 0; i < SESS_COUNT; i++)
	{
		std::cout << "\tСессия #" << i + 1 << "\n";
		auto& session = mInfo.mSessions[i];

		for (size_t j = 0; j < SUBJS_COUNT; )
		{
			auto& discipline = session.disciplines[j];
			std::cout << "Дисциплина: ";
			getline(std::cin, in);

			if (checkDisciplineName(in, wstr))
			{
				memset(discipline.subject, '\0', sizeof discipline.subject);
				memcpy(discipline.subject, in.data(), in.length());

				std::cout << "Оценку (зачёт): ";
				getline(std::cin, in);

				if (checkDisciplineMark(in, wstr))
				{
					memset(discipline.grade, '\0', sizeof discipline.grade);
					memcpy(discipline.grade, in.data(), in.length());
					++j;

					if (!session.active)
						session.active = 1;
				}
				else
					std::cerr << "Ошибочное значение оценки\n";
			}
			else
				std::cerr << "Ошибка в названии дисциплины\n";

			std::cout
				<< "\tСледующая дисциплина:     нажмите 1\n"
				<< "\tЗакончить ввод дисциплин: нажмите ENTER\n"
				<< "\t>> ";
			getline(std::cin, in);
			if (in != "1") break;
		}
		calcAverage(i);

		std::cout
			<< "\tСледующая сессия:      нажмите 1\n"
			<< "\tЗакончить ввод сессий: нажмите ENTER\n"
			<< "\t>> ";
		getline(std::cin, in);
		if (in != "1") break;
	}

	// вывод в консоль отредактированных данных
	print();
	printSession();

	inoutbin.seekp((n - 1) * sizeof mInfo, std::ios::beg);
	inoutbin.write(reinterpret_cast<const char*>(&mInfo), sizeof mInfo);
}

void Student::printGroupID() const
{
	std::cout << std::setw(10) << mInfo.mGroupID;
}

void Student::printGradeBookID() const
{
	std::cout << std::setw(15) << mInfo.mGradeBookID;
}

// вывод данных сессии номер n или всех, которые активны
void Student::printSession(size_t n) const
{
	// вывод заголовка таблицы
	for (size_t i = 0; i < SESS_TBL_WIDTH; i++) std::cout << '-';
	std::cout << std::endl;
	std::cout << '|'
		<< " Семестр " << '|'
		<< "                   Дисциплина                     " << '|'
		<< "        Оценка          " << '|'
		<< std::endl;

	for (size_t i = 0; i < SESS_TBL_WIDTH; i++) std::cout << '-';
	std::cout << std::endl;

	// вывод сессии с номером n 
	if (n != std::numeric_limits<size_t>::max())
	{
		if (n < 1 || n > 9)
		{
			std::cerr << "Несуществующий номер сессии!\n";
			return;
		}

		if (!mInfo.mSessions[n - 1].active)
		{
			std::cerr << "Сессия ещё не активирована!\n";
			return;
		}

		std::cout << "| " << std::setw(5) << n;

		const auto& session = mInfo.mSessions[n - 1];
		std::string_view sv;
		for (size_t i = 0; i < SUBJS_COUNT; ++i)
		{
			const auto& discipline = session.disciplines[i];
			if (std::strlen(discipline.subject) != 0)
			{
				if (i > 0)
					std::cout << "| " << std::setw(5) << ' ';

				std::cout
					<< "   | " << std::setw(47) << discipline.subject
					<< "  |   ";

				sv = discipline.grade;
				if (sv == "Неудовлетворительно")
					std::cout << std::setw(17) << 2;
				else if (sv == "Удовлетворительно")
					std::cout << std::setw(17) << 3;
				else if (sv == "Хорошо")
					std::cout << std::setw(17) << 4;
				else if (sv == "Отлично")
					std::cout << std::setw(17) << 5;
				else
					std::cout << std::setw(17) << sv;
				std::cout << "    |" << std::endl;
			}
		}
		for (size_t i = 0; i < SESS_TBL_WIDTH; i++)	std::cout << '-';
		std::cout << std::endl;
		std::cout
			<< "| "
			<< std::setw(57) << "Средний балл" << "  |   "
			<< std::setw(17) << session.average << "    |" << std::endl;
		for (size_t i = 0; i < SESS_TBL_WIDTH; i++)	std::cout << '-';
		std::cout << std::endl;
	}	
	// вывод всех "активных" сессий
	else
	{
		std::string_view sv;
		
		// перебор сессий
		for (size_t i = 0; i < SESS_COUNT; i++)
		{
			const auto& session = mInfo.mSessions[i];
			
			// если сессия инициализирована
			if (session.active)
			{
				std::cout << "| " << std::setw(5) << i + 1;
				
				// перебор дисциплин
				for (size_t i = 0; i < SUBJS_COUNT; ++i)
				{
					const auto& discipline = session.disciplines[i];
					
					//  если есть название
					if (std::strlen(discipline.subject) != 0)
					{
						if (i > 0)
							std::cout << "| " << std::setw(5) << ' ';

						std::cout
							<< "   | " << std::setw(47) << discipline.subject
							<< "  |   ";
						
						// вывод оценок в числовом формате
						sv = discipline.grade;
						if (sv == "Неудовлетворительно")
							std::cout << std::setw(17) << 2;
						else if (sv == "Удовлетворительно")
							std::cout << std::setw(17) << 3;
						else if (sv == "Хорошо")
							std::cout << std::setw(17) << 4;
						else if (sv == "Отлично")
							std::cout << std::setw(17) << 5;
						else
							std::cout << std::setw(17) << sv;
						std::cout << "    |" << std::endl;
					}
				}

				// вывод среднего балла
				for (size_t i = 0; i < SESS_TBL_WIDTH; i++)	std::cout << '-';
				std::cout << std::endl;
				std::cout
					<< "| "
					<< std::setw(57) << "Средний балл" << "  |   "
					<< std::setw(17) << session.average << "    |" << std::endl;
				for (size_t i = 0; i < SESS_TBL_WIDTH; i++)	std::cout << '-';
				std::cout << std::endl;
			}
		}
	}
}


