#include "Libs.h"
#include "Matrix.h"

// FunctionHandler - класс для отрисовки зависимостей, полученных в матрице
// планирования эксперимента, использующий средства библиотеки ROOT
// Отрисовка происходит на MainWindow класса TCanvas, которое разделяется 
// на два фрейма
// Первый фрейм - отображение зависимости для тока
// Второй - отображение зависимости напряжения

#pragma once
class FunctionHandler
{
private:
	std::vector<Matrix_Plan> matrices;					// 0 - current, 1 - voltage
	double Qmax, deltaT, Th;
public:
	FunctionHandler(std::initializer_list<Matrix_Plan> list, std::vector<std::string> values);
	void initialize_graphs(std::unique_ptr<TApplication>&);
	~FunctionHandler();
};

