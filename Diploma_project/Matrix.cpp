#include "Matrix.h"

// TODO:
// сохранение матрицы в файл + сохранение коэффициентов в файл

Matrix_Plan::Matrix_Plan(unsigned int var_count) 
{
	// определение размерности матрицы
	core_points = pow(2, var_count);
	alpha = 1.682;
	columns = 8 + var_count;
	star_points = 6;
	zero_points = 6;
	lines = core_points + star_points + zero_points;

	this->values.resize(lines * columns, 0.0);
	this->exp_results.resize(lines, 0.0);

	// инициализация значений внутри матрицы
	for (int i{ 0 }; i < lines; i++) (*this)(i, 0) = 1.0; // инициализация X0 единицами
	int j = 0;
	for (int i{ 1 }; i <= var_count; i++, j = 0)
		while (j < core_points)
		{
			int gap = static_cast<int>(pow(2, i - 1));
			for (int k = 0; k < gap && j < core_points; k++, j++)
				(*this)(j, i) = 1.0;
			j += gap;
		}

		// Инициализация звездных точек
		(*this)(8, 1) = alpha;											
		(*this)(9, 1) = -alpha;											
		(*this)(10, 2) = alpha;											
		(*this)(11, 2) = -alpha;										
		(*this)(12, 3) = alpha;											
		(*this)(13, 3) = -alpha;	

		for (int line{ 1 }; line < pow(2, var_count); line++)
			for (int col{ 1 }; col < pow(2, var_count); col++)
				if ((*this)(line, col) == 0) (*this)(line, col) = -1.0;

		// инициализация промежуточных столбцов X12,X13...
		for (auto i{ 0 }; i < core_points + star_points; i++)
		{
			(*this)(i, 4) = (*this)(i, 1) * (*this)(i, 2);							//	X12
			(*this)(i, 5) = (*this)(i, 1) * (*this)(i, 3);							//	X13
			(*this)(i, 6) = (*this)(i, 2) * (*this)(i, 3);							//	X23
			(*this)(i, 7) = (*this)(i, 1) * (*this)(i, 2) * (*this)(i, 3);			//	X123
			(*this)(i, 8) = pow((*this)(i, 1), 2);									//  X1^2
			(*this)(i, 9) = pow((*this)(i, 2), 2);									//  X2^2
			(*this)(i, 10) = pow((*this)(i, 3), 2);									//  X^3
		}

	// дозаполнение матрицы
	for (auto& i : values) if (i == -0) i = 0;
}

Matrix_Plan::~Matrix_Plan()
{
	lines = 0;
	columns = 0;
	values.clear();
	exp_results.clear();
	coeffs.clear();
}

// Получение значения по индексу
double& Matrix_Plan::operator()(size_t line, size_t col)
{
	if (line >= lines || col >= columns)
		throw std::out_of_range("Выход за пределы матрицы");
	else
		return values.at(line * columns + col);
}

// Ввод экспериментальных данных
void Matrix_Plan::enter_results()
{
	double data;
	std::cout << "Enter Experimental Data:\n";

	for (int i{ 0 }; i < lines; i++)
	{
		std::cout << "Enter " << i << " value\n>";
		std::cin >> data;
		std::cout << std::endl;
		exp_results.at(i) = data;
	}
}

bool Matrix_Plan::read_from_file(FileHandler& f)
{
	for (auto& x : f.get_pair())
	{
		exp_results.resize(20);
		exp_results.at(x.first) = x.second;
	}
	return true;
}

// Вывод матрицы на печать
void Matrix_Plan::print()
{
	std::cout << "\t\t\tCoefficients\t\t\t\t\tData\n";
	for (auto i = 0; i < lines; i++)
	{
		for (auto j = 0; j < columns; j++)
			std::cout << (*this)(i, j) << '\t';
		std::cout << "\t\t" <<(this)->exp_results.at(i);
		std::cout << '\n';
	}
}

// подсчет значений коэффициентов
void Matrix_Plan::count()
{
	std::vector<double> sums(columns, 0.0);													//	Промежуточные суммы
	(*this).coeffs.resize(columns, { 0.0, true });

	k = log2(core_points);	
	lambda = (k * (lines)) / ((k + 2) * (lines - zero_points));
	A = pow(2 * lambda * ((k + 2)*lambda - k), -1);
	
	// Нахождение значения константы C
	double sum = 0.0;
	for (auto j = 0; j < lines; j++)
		sum += pow((*this)(j, 1), 2);
	c = lines / sum;

	// Поиск промежуточных сумм для вычисления коэффициентов
	for (auto i{ 0 }; i < columns; i++)
		for (auto j{ 0 }; j < lines; j++)
			sums.at(i) += (*this)(j, i) * exp_results.at(j);

	sums.push_back(std::accumulate(exp_results.begin(), exp_results.end(), 0.0));			// Сумма экспериментальных данных Yi

	// Подсчет коэффициентов b
	for (auto i{ 0 }; i < columns; i++)
		if (i == 0)
			coeffs.at(i).first = A / lines * (2 * pow(lambda, 2) * (k + 2) * sums.at(11) -
				2 * lambda * c * (sums.at(8) + sums.at(9) + sums.at(10)));
		else if (i > 0 && i < 4)
			coeffs.at(i).first = c / lines * sums.at(i);
		else if (i >= 4 && i < 7)
			coeffs.at(i).first = pow(c, 2) / (lines * lambda) * (sums.at(i));
		else if (i >= 8 && i < columns)
			coeffs.at(i).first = (A / lines) * (pow(c, 2) * ((k + 2) * lambda - k) * sums.at(i)
				+ pow(c, 2) * (1 - lambda) * (sums.at(8) + sums.at(9) + sums.at(10))
				- 2 * lambda * c * sums.at(11));
}

void Matrix_Plan::assessment()
{
	auto sum_zero_points = std::accumulate(exp_results.begin() + 14, exp_results.end(), 0.0);
	auto average = sum_zero_points / zero_points;

	auto Se = 0.0;
	for (int i = 14 ; i < lines; i++)
		Se += pow(exp_results.at(i) - average, 2);
	auto Sy = Se / (zero_points - 1);									// степени свободы в знаменателе

	// расчет дисперсий коэффициентов
	for (auto i : { 0, 1, 4, 8 })
	{
		double S = 0.0 ;
		switch (i)
		{
		case 0:
			S = 2 * A * pow(lambda, 2) * (k + 2) * Sy / lines;
			break;
		case 1:
			S = c * Sy / lines;
			break;
		case 4:
			S = pow(c, 2) * Sy / (lambda * lines);
			break;
		case 8:
			S = A * pow(c, 2) * ((k + 1) * lambda - (k - 1))
				* Sy / lines;
			break;
		}
		disp.push_back(S);
	}

	// исследование статистической значимости коэффициентов
	for (auto& S : disp)
	{
		double t = 2.57;
		trust_intrv.push_back(t * sqrt(S));
	}

	for (int i = 0; i < coeffs.size(); i++)
	{
		if (i == 0)
			coeffs.at(i).second = (abs(coeffs.at(i).first) < abs(trust_intrv.at(0))) ? false : true;
		if (i > 0 && i < 4)
			coeffs.at(i).second = (abs(coeffs.at(i).first) < abs(trust_intrv.at(1))) ? false : true;
		if (i >= 4 && i < 8)
			coeffs.at(i).second = (abs(coeffs.at(i).first) < abs(trust_intrv.at(2))) ? false : true;
		else 
			coeffs.at(i).second = (abs(coeffs.at(i).first) < abs(trust_intrv.at(3))) ? false : true;
	}

	std::vector<double> results = new_func_values();

	int k_ind = std::count_if(coeffs.begin(), coeffs.end(), [](std::pair<double, bool> pr) {return pr.second; });

	// нахождение значения Sr
	double Sr = 0.0;
	for (int i = 0; i < 20; i++)
		Sr += pow((exp_results.at(i) - results.at(i)), 2);

	// Подсчет Sад
	double Sad = (Sr - Se) / (lines - k_ind - (zero_points - 1));		// степени свободы в числителе

	double Fp = Sad / Sy;

	// сравнение со значениями из таблицы критериев Фишера
	// суждение об адекватности модели
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Дебажные методы
////////////////////////////////////////////////////////////////////////////////////////////////

// поменять в соответствии со статистической пригодностью коэффициентов
double Matrix_Plan::get_value(double x, double deltaT, double Th)
{
	double result = 0.0;

	if (coeffs.at(0).second)
		result += coeffs.at(0).first;
	if (coeffs.at(1).second)
		result += coeffs.at(1).first * x;
	if (coeffs.at(2).second)
		result += coeffs.at(2).first * deltaT;
	if (coeffs.at(3).second)
		result += coeffs.at(3).first * Th;
	if (coeffs.at(8).second)
		result += coeffs.at(8).first * pow(x, 2);
	if (coeffs.at(9).second)
		result += coeffs.at(9).first * pow(deltaT, 2);
	if (coeffs.at(10).second)
		result += coeffs.at(10).first * pow(Th, 2);

	return result;
}

std::vector<double> Matrix_Plan::new_func_values()
{
	std::vector<double> new_results;			// контейнер с результатами новой функции

	for (int i = 0; i < lines; i++)
	{
		double value = 0.0;
		for (int j = 0; j < coeffs.size(); j++)
			if (coeffs.at(j).second)
				value += coeffs.at(j).first * (*this)(i, j);

		new_results.push_back(value);
	}

	return new_results;
}