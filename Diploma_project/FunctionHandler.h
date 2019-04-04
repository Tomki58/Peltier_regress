#include "Libs.h"
#include "Matrix.h"

// FunctionHandler - ����� ��� ��������� ������������, ���������� � �������
// ������������ ������������, ������������ �������� ���������� ROOT
// ��������� ���������� �� MainWindow ������ TCanvas, ������� ����������� 
// �� ��� ������
// ������ ����� - ����������� ����������� ��� ����
// ������ - ����������� ����������� ����������

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

