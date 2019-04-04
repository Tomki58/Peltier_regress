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
public:
	FunctionHandler(std::initializer_list<Matrix_Plan> list);
	void initialize_graphs(std::unique_ptr<TApplication>&);
	~FunctionHandler();
};

