#pragma once
#include "Libs.h"

// Класс для ввода/вывода данных из файла

class FileHandler
{
public:
	FileHandler(std::string);
	bool is_open() { return file.is_open(); }
	std::vector<std::pair<int, double>> get_pair();
	~FileHandler();
private:
	std::fstream file;
};

