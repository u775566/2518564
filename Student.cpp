#define _CRT_SECURE_NO_WARNINGS

#include "Student.h"
#include <cwctype>

char Student::CURR_YEAR[5]{};
namespace fs = std::filesystem;

Student::Student()
{
	// ������� ��� �� ��������� �����
	std::memmove(CURR_YEAR, __DATE__ + 7, 4);
}

template<size_t N>
void Student::convertToWideStr(std::string_view sv, wchar_t(&ws)[N]) const
{
	if (sv.empty())
		return;

	// ��������������� ������ char � wchar_t
	// ����� ��� ����������� ������������ ������� ����
	std::wmemset(ws, L'\0', N);
	std::mbstowcs(ws, sv.data(), std::min(N, sv.length()));

	/*std::wcout
		<< "wide string: '" << ws << "'"
		<< '(' << n << ')' << '\n';*/
}

void Student::clearInfo()
{
	// ��������� ������
	memset(&mInfo, 0, sizeof mInfo);
}

void Student::calcAverage(size_t n)
{
	using namespace std::string_view_literals;
	auto& session = mInfo.mSessions[n];
	session.average = 0;

	size_t k = 0;
	// ���������� �������� ����� � ��������������� ������ � �����
	for (size_t i = 0; i < SUBJS_COUNT; i++)
	{
		size_t len = std::strlen(session.disciplines[i].subject);
		std::string_view grade{ session.disciplines[i].grade };
		if (len > 0)
		{
			++k;
			if (grade == "�������"sv || grade == "5"sv)
				session.average += 5;
			else if (grade == "������"sv || grade == "4"sv)
				session.average += 4;
			if (grade == "�����������������"sv || grade == "3"sv)
				session.average += 3;
			if (grade == "�������������������"sv || grade == "2"sv)
				session.average += 2;
			if (grade == "�������"sv)
				session.average += .5;
			if (grade == "�� �������"sv)
				session.average -= .5;
		}
	}
	if (k != 0)
		session.average /= k;
}

void Student::normalizeUpperLower(std::string& str)
{
	// ������ ����� � ������ ���������, ��������� ���������
	for (size_t i = 0; i < str.length(); i++)
		str[i] = i == 0 ? std::toupper(str[i]) : std::tolower(str[i]);
};

bool Student::isEmptyInputString(std::string_view& sv) const
{
	// ��������� ������� � ������ � � ����� ������
	trimStrView(sv);

	// ����� ����� ���-������ ��������?
	if (sv.empty())
	{
		std::cerr << "������� ������ ������\n";
		return true;
	}

	return false;
}

// �������� ��������� �� ������!
void Student::trimStrView(std::string_view& sv) const
{
	if (sv.length() == 0)
		return;

	// ��������� ������� � ������ ������ (���� ����)
	sv.remove_prefix(std::min(sv.find_first_not_of(" \t"), sv.length()));

	// ���� ���-�� �������� ��������� ������� � ����� ������ (���� ����)
	if (!sv.empty())
		sv.remove_suffix(sv.size() - sv.find_last_not_of(" \t") - 1);
}

template<size_t N>
bool Student::checkAlphabetic(std::string& str, wchar_t(&ws)[N]) const
{
	// ��������� �� �������
	std::string_view sv{ str };
	if (isEmptyInputString(sv))
		return false;

	// �������������� � wchar_t ������
	convertToWideStr(sv, ws);

	// ��������� ��� �������� ������ �����
	if (!isAlphabetic(ws))
		return false;

	// �������� ������� ������ (������� ������� � ������ � � �����)
	str = sv;
	return true;
}

template<size_t N>
bool Student::isAlphaNumDash(wchar_t(&ws)[N]) const
{
	// ������� ��������� wchar_t ������ � �������� ��� �������� ������ �����, ����� � �����
	for (auto ch = ws; *ch != '\0'; ++ch)
	{
		if (!iswalnum(*ch) && *ch != L'-')
		{
			std::cerr << "������������ ������:\n";
			std::wcout << "'" << *ch << "'" << std::endl;
			return false;
		}
	}

	return true;
}

template<size_t N>
bool Student::isAlphaNum(wchar_t(&ws)[N]) const
{
	// ������� ��������� wchar_t ������ � �������� ��� �������� ������ ����� � �����
	for (size_t i = 0; i < std::wcslen(ws); ++i)
		if (!iswalnum(ws[i]))
		{
			std::cerr << "������������ ������ � �����!\n";
			std::wcout << "'" << ws[i] << "'";
			return false;
		}

	return true;
}

// ������� ��������� wchar_t ������ � �������� ��� �������� ������ �����
template<size_t N>
bool Student::isAlphabetic(wchar_t(&ws)[N]) const
{
	for (size_t i = 0; i < std::wcslen(ws); ++i)
		if (!iswalpha(ws[i]))
		{
			std::cerr << "������������ ������ � �����!\n";
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

	// �������� ����������� ������� ����� ���� ��������
	ss << sv;
	ss >> std::get_time(&t, "%d.%m.%Y");
	if (ss.fail())
	{
		std::cerr << "������������ ���� ��������\n";
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
		std::cerr << "������������ ���� ���� �������� 1\n";
		return false;
	}

	// �������� ������ �� ���������� ������ ����
	for (auto d : sv)
		if (!std::isdigit(d))
		{
			std::cerr << "������������ ���� ���� �������� 2\n";
			return false;
		}

	// �������� ���� �������� �������� - �� ����� �������� ���� � �� ������, ��� 35 ��� �����
	auto y = static_cast<uint16_t>(std::atoi(sv.data()));
	if (y > std::atoi(CURR_YEAR) || y < std::atoi(CURR_YEAR) - 35)
	{
		std::cerr << "������������ ���� ���� �������� 3\n";
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
		//std::cerr << "������������ ������� � �������� ����������\n";
		return false;
	}

	// �������� �� ����� �������� ���������� (7 ����, ���� �� ������� ������)
	if (str.length() >= std::size(mInfo.mFaculty))
	{
		std::cerr << "������� ������� �������� ����������\n";
		return false;
	}

	// ������� � ������� �������
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// �������������
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
		std::cerr << "������������ ������� � �������� �������\n";
		return false;
	}

	// �������� �� ����� �������� ������� (7 ����, ���� �� ������� ������)
	if (std::wcslen(ws) >= std::size(mInfo.mDepartment))
	{
		std::cerr << "������� ������� �������� �������\n";
		return false;
	}

	// ������� � ������� �������
	std::string str(sv.cbegin(), sv.cend());
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// �������������
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
		std::cerr << "������������ ������� � ������ ������\n";
		return false;
	}

	// �������� �� ����� ������ ������
	if (std::wcslen(ws) >= std::size(mInfo.mGroupID))
	{
		std::cerr << "������� ����� �������� � ������ ������\n";
		return false;
	}

	// ������� � ������� �������
	std::string str(sv.cbegin(), sv.cend());
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// �������������
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
		std::cerr << "������������ ������� � ������ �������\n";
		return false;
	}

	// �������� �� ����� ������ �������
	if (std::wcslen(ws) > std::size(mInfo.mGradeBookID) - 1)
	{
		std::cerr << "������� ����� �������� � ������ �������\n";
		return false;
	}

	// ������� � ������� �������
	std::string str(sv.cbegin(), sv.cend());
	for (size_t i = 0; i < str.length(); str[i] = std::toupper(str[i]), i++);

	// �������������
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
		std::cerr << "������� ����� ����\n";
		return false;
	}

	convertToWideStr(sv, ws);

	// ������������� ����
	switch (ws[0])
	{
	case L'�':
	case L'�':
		mInfo.mGender = male;
		return true;

	case L'�':
	case L'�':
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

	// �������������� ������� ������: 1-�� ����� ���������, ��������� ���������
	normalizeUpperLower(str);

	sv = str;
	convertToWideStr(sv, ws);

	// �������� ��������
	if (
		(std::wcscmp(ws, L"�������") == 0)
		|| (std::wcscmp(ws, L"������") == 0)
		|| (std::wcscmp(ws, L"�����������������") == 0)
		|| (std::wcscmp(ws, L"�������������������") == 0)
		|| (std::wcscmp(ws, L"�������") == 0)
		|| (std::wcscmp(ws, L"�� �������") == 0)
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

	// �������������� ������� ������: 1-�� ����� ���������, ��������� ���������
	normalizeUpperLower(str);

	convertToWideStr(sv, ws);

	//�������� �� ������������ ������� � �������� ����������;
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
			//std::cerr << "������������ ������� � �������� ����������\n";
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

	size_t w = std::strlen("�������� ���������� (���������): ");

	std::cout.setf(std::ios::left, std::ios::adjustfield);

	// ���� � �������� �������
	std::cout << std::setw(w) << "�������: ";
	getline(std::cin, in);
	if (!checkAlphabetic(in, wstr))
		return false;

	// �������������
	normalizeUpperLower(in);
	memcpy(mInfo.mFio.surname, in.c_str(), in.length());

	// ���� � �������� �����
	std::cout << std::setw(w) << "���: ";
	getline(std::cin, in);
	if (!checkAlphabetic(in, wstr))
		return false;

	// �������������
	normalizeUpperLower(in);
	memcpy(mInfo.mFio.name, in.c_str(), in.length());

	// ���� � �������� ��������
	std::cout << std::setw(w) << "��������: ";
	getline(std::cin, in);
	if (!checkAlphabetic(in, wstr))
		return false;

	// �������������
	normalizeUpperLower(in);
	memcpy(mInfo.mFio.patronimic, in.c_str(), in.length());

	//printFio();

	std::cout << std::setw(w) << "���� �������� (��.��.���): ";
	getline(std::cin, in);
	if (!checkBirthday(in))
		return false;

	//printBirthday();

	std::cout << std::setw(w) << "��� (�/�): ";
	getline(std::cin, in);
	if (!checkGender(in, wstr))
		return false;

	//printGender();

	std::cout << std::setw(w) << "��� �����������: ";
	getline(std::cin, in);
	if (!checkAdmissionYear(in))
		return false;

	//printAdmissionYear();

	std::cout << std::setw(w) << "�������� ���������� (���������): ";
	getline(std::cin, in);
	if (!checkFaculty(in, wstr))
		return false;

	//printFaculty();

	std::cout << std::setw(w) << "�������� �������: ";
	getline(std::cin, in);
	if (!checkDepartment(in, wstr))
		return false;

	//printDepartment();

	std::cout << std::setw(w) << "����� ������: ";
	getline(std::cin, in);
	if (!checkGroup(in, wstr))
		return false;

	//printGroupID();

	std::cout << std::setw(w) << "����� �������: ";
	getline(std::cin, in);
	if (!checkGradeBook(in, wstr))
		return false;

	//printGradeBookID();

	// ���� ������ ������
	for (size_t i = 0; i < SESS_COUNT; i++)
	{
		auto& session = mInfo.mSessions[i];
		std::cout << "\t������ #" << i + 1 << "\n";

		// ���� ������ �� ����������� i-�� ������
		for (size_t j = 0; j < SUBJS_COUNT; )
		{
			auto& discipline = session.disciplines[j];

			std::cout << "����������: ";
			getline(std::cin, in);

			if (!checkDisciplineName(in, wstr))
				std::cerr << "������ � �������� ����������\n";
			else
			{

				memset(discipline.subject, '\0', sizeof discipline.subject);
				memcpy(discipline.subject, in.data(), in.length());

				std::cout << "������ (�������/�� �������): ";
				getline(std::cin, in);

				if (!checkDisciplineMark(in, wstr))
					std::cerr << "������ ����� ������\n";
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
				<< "\t��������� ����������:      ������� 1\n"
				<< "\t��������� ����  ���������: ������� ENTER\n"
				<< "\t>> ";
			getline(std::cin, in);
			if (in != "1") break;
		}
		// ���������� �������� ����� i-�� ������
		calcAverage(i);

		std::cout
			<< "\t��������� ������:      ������� 1\n"
			<< "\t��������� ���� ������: ������� ENTER\n"
			<< "\t>> ";
		getline(std::cin, in);
		if (in != "1") break;
	}

	return true;
}

// ����� ��������� �������������� �������
void Student::printHeader() const
{
	for (size_t i = 0; i < INFO_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl
		<< '|'
		<< "      ������� ��� ��������      " << '|'
		<< "   ���� ��������   " << '|'
		<< "    ���    " << '|'
		<< "   ��� �����������   " << '|'
		<< "   ���������   " << '|'
		<< "   �������   " << '|'
		<< "    ������    " << '|'
		<< "   ����� �������   "
		<< '|' << std::endl;
	for (size_t i = 0; i < INFO_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl;
}

// ����� ������ �������������� �������
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

// ����� ���
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

// ����� ���� ��������
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

// ����� ����
void Student::printGender() const
{
	if (mInfo.mGender == male)
		std::cout << "�������";
	else if (mInfo.mGender == female)
		std::cout << "�������";
	else
		std::cout << "�������������";
}

const Student::StudentInfo& Student::info() const
{
	return mInfo;
}

Student::StudentInfo& Student::info()
{
	return mInfo;
}

// ���������� �����
void Student::sortFile()
{
	std::string input;
	size_t num = 2;

	std::cout
		<< "����� ������ �� ��������� 2\n"
		<< "����� �������� �������� �� ��������� ������� ENTER\n\n";

	std::cout << "������ ����� ������ (1-9 ��� ENTER): ";
	std::getline(std::cin, input);

	// �������� ����� ������ ������
	if (input.length() > 1)
	{
		std::cerr << "����� �������� �����\n";
		return;
	}
	if (input.length() == 1 && std::isdigit(input[0]))
		num = static_cast<size_t>(input[0]) - '0';

	//std::cout << num << std::endl;
	
	// ���� ��������� ���� ��������
	std::cout << "������� �������� ���� �������� (����-����): ";
	std::getline(std::cin, input);

	// �������� ������������ �����
	if (input.length() == 0)
	{
		std::cout << "������� ������ ������\n";
		return;
	}

	if (input.length() != 9)
	{
		std::cout << "�������� ����� �������\n";
		return;
	}

	// ������� ������ � ���������� �.�.
	std::string year1, year2;
	year1 = input.substr(0, 4);
	for (const auto& ch : year1)
		if (!std::isdigit(ch))
		{
			std::cerr
				<< "������� ����� ���\n"
				<< "���������� �����������\n";
			return;
		}

	year2 = input.substr(5);
	for (const auto& ch : year1)
		if (!std::isdigit(ch))
		{
			std::cerr
				<< "������� ����� ���\n"
				<< "���������� �����������\n";
			return;
		}

	// ���� ��� �������� ��������
	std::cout << year1 << '-' << year2 << std::endl;

	// ������� �������� ���� ��� ������/������
	std::fstream inout{ F_NAME, std::ios::binary | std::ios::in | std::ios::out };

	// ���� �� ��������
	if (!inout.is_open())
	{
		std::cerr
			<< "�� ������� ������� ����\n"
			<< "���������� �����������\n";
		return;
	}

	// ���������� ���-�� ������� � �����
	size_t listLength = fs::file_size(F_NAME) / sizeof mInfo;
	std::cout << "LIST LENGTH " << listLength << std::endl;

	// ���� ���� ����
	if (listLength == 0)
	{
		std::cerr << "���� ����. ���������� �����������.\n";
		return;
	}

	// ��������� �� ������������ ������ ��� �������
	std::unique_ptr<StudentInfo[]> list{ new StudentInfo[listLength]{} };

	// ������ ����� � ������ list
	inout.seekg(0, std::ios::beg);
	inout.read(reinterpret_cast<char*>(list.get()), sizeof(StudentInfo) * listLength);

	// ���������� ������� � ������ � ������ ��������� ���� ��������
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

	// ������ ���������������� ������� � ����
	inout.seekp(0, std::ios::beg);
	if (inout.write(reinterpret_cast<const char*>(list.get()), sizeof(StudentInfo) * listLength))
	{
		std::cout << "������ ����� ����� ���������� �������\n";
		system("pause");
		std::cout << "������ ����� ����� ����������\n";

		// ���� ������ ������ ��� ������, ������ ����� ����� ���������� � ����� � �������
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
		std::cout << "������ ������ � ���� ����� ����������\n";

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

// �������������� ������� �����
void Student::edit()
{
	// ������� �������� ���� ��� �����/������
	std::fstream inoutbin{ F_NAME, std::ios::binary | std::ios::in | std::ios::out };

	// ��������
	if (!inoutbin.is_open())
	{
		std::cerr << "������ ��� �������� ����� ��� ������\n";
		return;
	}

	// ���������� ���-�� ������� � �����
	size_t length = fs::file_size(F_NAME) / sizeof mInfo;

	// ����� ��������� ������ �������������� � �������
	std::cout.setf(std::ios::left, std::ios::adjustfield);
	for (size_t i = 0; i < EDIT_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl
		<< '|'
		<< "  �  " << '|'
		<< "      ������� ��� ��������      " << '|'
		<< "   ���� ��������   " << '|'
		<< "    ���    " << '|'
		<< "   ��� �����������   " << '|'
		<< "   ���������   " << '|'
		<< "   �������   " << '|'
		<< "    ������    " << '|'
		<< "   ����� �������   "
		<< '|' << std::endl;
	for (size_t i = 0; i < EDIT_TBL_WIDTH; i++)	std::cout << '-';
	std::cout << std::endl;

	// ����� ���������������� ������ ���������
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

	// ���� ������ ������ ��� ��������������
	std::string recNum;
	std::cout << "������� ����� ������ ��� ��������������: ";
	std::getline(std::cin, recNum);
	
	// ��������, ��� �� �����
	if (recNum.length() == 0)
	{
		std::cout << "����� �� �����\n";
		std::memset(&mInfo, 0, sizeof mInfo);
		return;
	}

	// �������� ��� ������� �����
	bool num = true;
	for (const auto& ch : recNum)
		if (!std::isdigit(ch))
		{
			num = false;
			return;
		}

	// ��������������� ������ � �����
	size_t n = std::stoul(recNum);

	// ������ ��������� ������ � mInfo
	inoutbin.seekg((n - 1) * sizeof mInfo, std::ios::beg);
	inoutbin.read(reinterpret_cast<char*>(&mInfo), sizeof mInfo);


	/******************** ������� �� �������������� ������, �������� � ���������, ���� ����� ****************/
	std::string in;
	wchar_t wstr[48]{};
	size_t w = std::strlen("�������� ���������� (���������): ");
	std::cout << "����� �������� ������ �������� ������� ENTER\n";

	std::cout << std::setw(w) << "�������: ";
	getline(std::cin, in);
	if (checkAlphabetic(in, wstr))
	{
		normalizeUpperLower(in);
		memset(mInfo.mFio.surname, '\0', sizeof mInfo.mFio.surname);
		memcpy(mInfo.mFio.surname, in.c_str(), in.length());
	}

	std::cout << std::setw(w) << "���: ";
	getline(std::cin, in);
	if (checkAlphabetic(in, wstr))
	{
		normalizeUpperLower(in);
		memset(mInfo.mFio.name, '\0', sizeof mInfo.mFio.name);
		memcpy(mInfo.mFio.name, in.c_str(), in.length());
	}

	std::cout << std::setw(w) << "��������: ";
	getline(std::cin, in);
	if (checkAlphabetic(in, wstr))
	{
		normalizeUpperLower(in);
		memset(mInfo.mFio.patronimic, '\0', sizeof mInfo.mFio.patronimic);
		memcpy(mInfo.mFio.patronimic, in.c_str(), in.length());
	}

	//printFio();

	std::cout << std::setw(w) << "���� �������� (��.��.���): ";
	getline(std::cin, in);
	checkBirthday(in);

	//printBirthday();

	std::cout << std::setw(w) << "��� (�/�): ";
	getline(std::cin, in);
	checkGender(in, wstr);

	//printGender();

	std::cout << std::setw(w) << "��� �����������: ";
	getline(std::cin, in);
	checkAdmissionYear(in);

	//printAdmissionYear();

	std::cout << std::setw(w) << "�������� ���������� (���������): ";
	getline(std::cin, in);
	checkFaculty(in, wstr);

	//printFaculty();

	std::cout << std::setw(w) << "�������� �������: ";
	getline(std::cin, in);
	checkDepartment(in, wstr);

	//printDepartment();

	std::cout << std::setw(w) << "����� ������: ";
	getline(std::cin, in);
	checkGroup(in, wstr);

	//printGroupID();

	std::cout << std::setw(w) << "����� �������: ";
	getline(std::cin, in);
	checkGradeBook(in, wstr);

	//printGradeBookID();

	for (size_t i = 0; i < SESS_COUNT; i++)
	{
		std::cout << "\t������ #" << i + 1 << "\n";
		auto& session = mInfo.mSessions[i];

		for (size_t j = 0; j < SUBJS_COUNT; )
		{
			auto& discipline = session.disciplines[j];
			std::cout << "����������: ";
			getline(std::cin, in);

			if (checkDisciplineName(in, wstr))
			{
				memset(discipline.subject, '\0', sizeof discipline.subject);
				memcpy(discipline.subject, in.data(), in.length());

				std::cout << "������ (�����): ";
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
					std::cerr << "��������� �������� ������\n";
			}
			else
				std::cerr << "������ � �������� ����������\n";

			std::cout
				<< "\t��������� ����������:     ������� 1\n"
				<< "\t��������� ���� ���������: ������� ENTER\n"
				<< "\t>> ";
			getline(std::cin, in);
			if (in != "1") break;
		}
		calcAverage(i);

		std::cout
			<< "\t��������� ������:      ������� 1\n"
			<< "\t��������� ���� ������: ������� ENTER\n"
			<< "\t>> ";
		getline(std::cin, in);
		if (in != "1") break;
	}

	// ����� � ������� ����������������� ������
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

// ����� ������ ������ ����� n ��� ����, ������� �������
void Student::printSession(size_t n) const
{
	// ����� ��������� �������
	for (size_t i = 0; i < SESS_TBL_WIDTH; i++) std::cout << '-';
	std::cout << std::endl;
	std::cout << '|'
		<< " ������� " << '|'
		<< "                   ����������                     " << '|'
		<< "        ������          " << '|'
		<< std::endl;

	for (size_t i = 0; i < SESS_TBL_WIDTH; i++) std::cout << '-';
	std::cout << std::endl;

	// ����� ������ � ������� n 
	if (n != std::numeric_limits<size_t>::max())
	{
		if (n < 1 || n > 9)
		{
			std::cerr << "�������������� ����� ������!\n";
			return;
		}

		if (!mInfo.mSessions[n - 1].active)
		{
			std::cerr << "������ ��� �� ������������!\n";
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
				if (sv == "�������������������")
					std::cout << std::setw(17) << 2;
				else if (sv == "�����������������")
					std::cout << std::setw(17) << 3;
				else if (sv == "������")
					std::cout << std::setw(17) << 4;
				else if (sv == "�������")
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
			<< std::setw(57) << "������� ����" << "  |   "
			<< std::setw(17) << session.average << "    |" << std::endl;
		for (size_t i = 0; i < SESS_TBL_WIDTH; i++)	std::cout << '-';
		std::cout << std::endl;
	}	
	// ����� ���� "��������" ������
	else
	{
		std::string_view sv;
		
		// ������� ������
		for (size_t i = 0; i < SESS_COUNT; i++)
		{
			const auto& session = mInfo.mSessions[i];
			
			// ���� ������ ����������������
			if (session.active)
			{
				std::cout << "| " << std::setw(5) << i + 1;
				
				// ������� ���������
				for (size_t i = 0; i < SUBJS_COUNT; ++i)
				{
					const auto& discipline = session.disciplines[i];
					
					//  ���� ���� ��������
					if (std::strlen(discipline.subject) != 0)
					{
						if (i > 0)
							std::cout << "| " << std::setw(5) << ' ';

						std::cout
							<< "   | " << std::setw(47) << discipline.subject
							<< "  |   ";
						
						// ����� ������ � �������� �������
						sv = discipline.grade;
						if (sv == "�������������������")
							std::cout << std::setw(17) << 2;
						else if (sv == "�����������������")
							std::cout << std::setw(17) << 3;
						else if (sv == "������")
							std::cout << std::setw(17) << 4;
						else if (sv == "�������")
							std::cout << std::setw(17) << 5;
						else
							std::cout << std::setw(17) << sv;
						std::cout << "    |" << std::endl;
					}
				}

				// ����� �������� �����
				for (size_t i = 0; i < SESS_TBL_WIDTH; i++)	std::cout << '-';
				std::cout << std::endl;
				std::cout
					<< "| "
					<< std::setw(57) << "������� ����" << "  |   "
					<< std::setw(17) << session.average << "    |" << std::endl;
				for (size_t i = 0; i < SESS_TBL_WIDTH; i++)	std::cout << '-';
				std::cout << std::endl;
			}
		}
	}
}


