#include "FunctionHandler.h"




FunctionHandler::FunctionHandler(std::initializer_list<Matrix_Plan> list): matrices{list}
{
}

void FunctionHandler::initialize_graphs(std::unique_ptr<TApplication>& App)
{

	// Установка данных для инициализации
	Int_t N = 20;
	double_t x_current[20];
	double_t y_current[20], y_voltage[20], y_consumption[20];

	// Подсчет значений функции
	for (auto i = 0; i < N; i++)
	{
		x_current[i] = i;
		y_current[i] = matrices.at(0).get_value(x_current[i], 20, 27);
		y_voltage[i] = matrices.at(1).get_value(x_current[i], 20, 27);
		y_consumption[i] = y_current[i] * y_voltage[i];
	}

	TCanvas *t11 = new TCanvas();
	t11->Divide(3, 1);
	t11->cd(1);

	

	// Установка среды для отображения
	TGraph *t1 = new TGraph(N, x_current, y_current);
	t1->Draw();
	
	t11->cd(2);
	TGraph *t2 = new TGraph(N, x_current, y_voltage);
	t2->Draw();

	t11->cd(3);
	TGraph *t3 = new TGraph(N, x_current, y_consumption);
	t3->Draw();

	App->Run();
}

FunctionHandler::~FunctionHandler()
{
}
