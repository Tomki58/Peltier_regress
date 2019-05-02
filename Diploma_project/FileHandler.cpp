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
	bool first = true;

	while (std::getline(file, s))
	{
		std::stringstream linestream(s);
		std::string index, result;
		std::getline(linestream, index, ';');
		if (first && index.at(0) < 0)
		{
			first = false;
			index = index.substr(3, index.length() - 3);
		}
		std::getline(linestream, result);
		for (auto& it : index)
			if (isalpha(it))
				throw std::runtime_error("Wrong Data Format");
		for (auto& it : result)
			if (isalpha(it))
				throw std::runtime_error("Wrong Data Format");
		for (auto& it : result)
			if (it == ',') it = '.';
		std::pair<int, double> pair(std::atoi(index.c_str()), std::stod(result));
		results.push_back(pair);
	}
	if (results.size() > 20)
		throw std::runtime_error("Invalid Data Number");
	else
		return results;
}

