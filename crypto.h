#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <time.h>
#include <fstream>

void Crypt()
{
	srand(time(nullptr));
	char* pass = new char[64] {};
	for (int i = 0; i < 64; ++i) {
		switch (rand() % 3) {
		case 0:
			pass[i] = rand() % 10 + '0';
			break;
		case 1:
			pass[i] = rand() % 26 + 'A';
			break;
		case 2:
			pass[i] = rand() % 26 + 'a';
		}
	}

	std::string command = "openssl\\bin\\openssl.exe enc -aes-256-cbc -salt -in	students.bin -out students.bin.enc -pass pass:";
	command += pass;
	system(command.c_str());

	if (remove("students.bin") != 0)
		std::cout << "[ERROR] - deleting file" << std::endl;

	std::ofstream file{ "key.txt", std::ios::binary };
	file.write(pass, 65);
	file.close();

	command = "openssl\\bin\\openssl.exe rsautl -encrypt -inkey rsa.public -pubin -in key.txt -out key.txt.enc";
	system(command.c_str());
	if (remove("key.txt") != 0)
		std::cout << "[ERROR] - deleting file" << std::endl;

}

void Decrypt()
{
	std::string command = "openssl\\bin\\openssl.exe rsautl -decrypt -inkey rsa.private -in key.txt.enc -out key.txt";
	system(command.c_str());

	if (remove("key.txt.enc") != 0)
		std::cout << "[ERROR] - deleting file" << std::endl;

	char* pass = new char[64] {};

	std::ifstream file{ "key.txt", std::ios::binary };
	file.read(pass, 65);
	file.close();

	if (remove("key.txt") != 0)
		std::cout << "[ERROR] - deleting file" << std::endl;

	command = "openssl\\bin\\openssl.exe enc -aes-256-cbc -d -in students.bin.enc -out students.bin -pass pass:";
	command += pass;
	system(command.c_str());

	if (remove("students.bin.enc") != 0)
		std::cout << "[ERROR] - deleting file" << std::endl;
}
