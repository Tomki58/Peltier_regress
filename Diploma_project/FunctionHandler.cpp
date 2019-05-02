#include "FunctionHandler.h"




FunctionHandler::FunctionHandler(std::initializer_list<Matrix_Plan> list, std::vector<std::string> values): matrices{list},
Qmax{ std::atof(values.at(2).c_str()) + 1.682*std::atof(values.at(3).c_str()) }, deltaT{ std::atof(values.at(4).c_str()) }, Th{ std::atof(values.at(6).c_str()) }
{}

void FunctionHandler::initialize_graphs(std::unique_ptr<TApplication>& App)
{

	// Установка данных для инициализации
	double_t x_current[155];
	double_t y_current[155], y_voltage[155], y_consumption[155];

	ZeroMemory(x_current, 155);
	ZeroMemory(y_current, 155);
	ZeroMemory(y_voltage, 155);
	ZeroMemory(y_consumption, 155);

	// Подсчет значений функции
	for (int i = 0; i < Qmax; i++)
	{
		x_current[i] = i;
		y_current[i] = matrices.at(0).get_value(x_current[i], deltaT, Th);
		y_voltage[i] = matrices.at(1).get_value(x_current[i], deltaT, Th);
		y_consumption[i] = y_current[i] * y_voltage[i];
	}

	TCanvas *t11 = new TCanvas();
	t11->Divide(3, 1);
	t11->cd(1);

	

	// Установка среды для отображения
	TGraph *t1 = new TGraph(Qmax, x_current, y_current);
	t1->SetTitle("Current");
	t1->Draw();
	
	t11->cd(2);
	TGraph *t2 = new TGraph(Qmax, x_current, y_voltage);
	t2->SetTitle("Voltage");
	t2->Draw();

	t11->cd(3);
	TGraph *t3 = new TGraph(Qmax, x_current, y_consumption);
	t3->SetTitle("Power Consumption");
	t3->Draw();

	App->Run();
}

FunctionHandler::~FunctionHandler()
{
	matrices.clear();
}
