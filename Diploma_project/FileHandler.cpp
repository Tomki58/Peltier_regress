#include "FileHandler.h"



FileHandler::FileHandler(std::string filename)
{
	file.open(filename, std::fstream::in);
}

std::vector<std::pair<int, double>> FileHandler::get_pair()
{
	if (!file.is_open())
		throw std::runtime_error("Не открыт поток вывода");

	std::vector<std::pair<int, double>> results;
	std::string s;

	while (std::getline(file, s))
	{
		std::stringstream linestream(s);
		std::string index, result;

		std::getline(linestream, index, ';');
		std::getline(linestream, result);
		for (auto& it : result)
			if (it == ',') it = '.';
		std::pair<int, double> pair(std::atoi(index.c_str()), std::stod(result));
		results.push_back(pair);
	}

	return results;
}


FileHandler::~FileHandler()
{
	file.close();
}
