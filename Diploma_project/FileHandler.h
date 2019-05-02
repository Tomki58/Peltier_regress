#pragma once
#include "Libs.h"

// ����� ��� �����/������ ������ �� �����

class FileHandler
{
public:
	FileHandler(std::string);
	bool is_open() { return file.is_open(); }
	std::vector<std::pair<int, double>> get_pair();
	~FileHandler() { file.close(); }
private:
	std::fstream file;
};