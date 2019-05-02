#include "Libs.h"
#include "FileHandler.h"

using FILEPATH = std::string;

#pragma once

class Matrix_Plan
{
private:
	int lines, columns;													// размерность матрицы
	
	int core_points;													// полный факторный эксперимент
	int star_points;													// "звездные" точки
	int zero_points;													// точки в центре плана
	double alpha;														// константа альфа
	double k;															// количество переменных
	double lambda;														// лямбда
	double A;															// константа А
	double c;															// константа С

	std::vector<double> values;											// матрица эксперимента
	std::vector<double> exp_results;									// данные эксперимента
	std::vector<std::pair<double, bool>> coeffs;						// значения коэффициентов
	std::vector<double> disp;											// значения дисперсий
	std::vector<double> trust_intrv;									// абсолютные значения доверительных интервалов
	std::vector<std::string> options;

	static double fisher_table[20][11];

	// локальные функции
	std::vector<double> new_func_values();
public:
	explicit Matrix_Plan(unsigned int var_count, std::vector<std::string> options);						// конструктор инициалзации
	~Matrix_Plan();
	double& operator()(size_t line, size_t col);						// значение по индексу
	void enter_results();												// ввод экспериментальных данных
	bool read_from_file(FileHandler&);
	void print();														// вывод матрицы на печать
	void count();														// подсчет коэффициентов
	bool assessment();													// оценка модели
	// Дебажные методы
	double get_value(double x, double deltaT, double Th);
	static void fill_table(FILEPATH);
};
