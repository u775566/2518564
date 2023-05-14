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
1. ���������� � ������ ��������� �� N �������, ��� ������ � �������� �������� ��������� ������:
1). �������, ���, �������� ��������.
2). �����, �����, ��� ��������.
3). ��� ����������� � ��������.
4). ��������� (��������).
5). �������.
6). ������.
7). ����� �������� ������.
8). ���.
9). �������� ��������� � ������ �� ������� �������� � ������ ������, ����������� 9 ������ � 10 ��������� � ������ ��������, ������� ����� ���� ������.
��� ������ ������ ���� ����������: ����, ����� � �.�.
*/

class Student
{
	// ��� �������� �������� ����
	static char CURR_YEAR[5];

	// ��������� ������
	static constexpr size_t SESS_COUNT = 9;
	static constexpr size_t SUBJS_COUNT = 10;
	// �������� ������ ������
	static constexpr size_t  INFO_TBL_WIDTH = 153;
	static constexpr size_t  EDIT_TBL_WIDTH = 159;
	static constexpr size_t  SESS_TBL_WIDTH = 87;
	
	// ������� �������� ����
	static constexpr char F_NAME[] = "students.bin";
	
	// ������������� �������������� ���� 
	enum Gender { male, female };

	// �������� ��� ��������
	struct Birthday {
		unsigned year : 11;
		unsigned month : 4;
		unsigned day : 5;
	};

	// �������� ���
	struct FIO {
		char name[24];
		char patronimic[24];
		char surname[24];
	};

	// �������� ����������
	struct Subject {
		char subject[48]{};	// ��������
		char grade[21]{};		// ������
	};

	// �������� ������
	struct Session {
		Subject disciplines[SUBJS_COUNT]{}; // ������ ���������
		double average{};										// ������� ����
		unsigned active : 1;								// ������/���������
	};
	
	// ����� ��� ���������� �������� �����
	void calcAverage(size_t n);

	// ������ ���������� � �������� ��� �������� � �����
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

	// �����������
	Student();
	
	// ������������� �������
	bool init();
	// ���������� ����� � ������������ � ��������
	void sortFile();
	// �������������� ������� � �����
	void edit();
	// �������� ���������� � �������� (mInfo)
	void clearInfo();

	// ��� ������ � mInfo �������
	// ��� ������ - ����������� � �� ����������� ������
	const StudentInfo& info() const;
	StudentInfo& info();

	// ���������� ������ mInfo � ������
	size_t infoSize() const;

	// ���������� ��� ����� �������
	static const char* fname();

	// ������ ������ � �������
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

	// ������������ ������ - ������ ����� ���������, ��������� ���������
	void normalizeUpperLower(std::string& str);

	// ������� ���������� ������� � ������ � ����� ������
	void trimStrView(std::string_view& sv) const;
	
	// ��������� ������ �� �������, ��������, ���� ���� ������ �������, �� ��������� ������ 
	bool isEmptyInputString(std::string_view& sv) const;

	// ������������ ������ �� char � wchar_t ��� ������������ ��������� 
	template<size_t N>
	void convertToWideStr(std::string_view sv, wchar_t(&ws)[N]) const;

	// ��������� ������ �� ������� ����������� �������� (�����, �����, �����)
	// ��� ������ ������
	template<size_t N>
	bool isAlphaNumDash(wchar_t(&ws)[N]) const;

	// ��������� ������ �� ������� ����������� �������� (�����, �����)
	// ��� ������ �������
	template<size_t N>
	bool isAlphaNum(wchar_t(&ws)[N]) const;

	// ��������� ����� �� wchar_t �� ������� ����������� �������� (������ �����)
	// ��� ���, ���������, �������� ���������� (���������)
	template<size_t N>
	bool isAlphabetic(wchar_t(&ws)[N]) const;

	// ��������� ������ �� ������� ������ ����, �������������� �������������� �
	// � wchar_t
	template<size_t N>
	bool checkAlphabetic(std::string& str, wchar_t(&ws)[N]) const;

	// �������� ������ ��� �������� ������� �������� ����������, ������� � ��������� ������ ��������
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

