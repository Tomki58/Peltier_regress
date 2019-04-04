#include "Libs.h"
#include "FileHandler.h"

#pragma once

class Matrix_Plan
{
private:
	int lines, columns;													// ����������� �������
	
	int core_points;													// ������ ��������� �����������
	int star_points;													// "��������" �����
	int zero_points;													// ����� � ������ �����
	double alpha;														// ��������� �����
	double k;															// ���������� ����������
	double lambda;														// ������
	double A;															// ��������� �
	double c;															// ��������� �

	std::vector<double> values;											// ������� ������������
	std::vector<double> exp_results;									// ������ ������������
	std::vector<std::pair<double, bool>> coeffs;						// �������� �������������
	std::vector<double> disp;											// �������� ���������
	std::vector<double> trust_intrv;									// ���������� �������� ������������� ����������

	// ��������� �������
	std::vector<double> new_func_values();
public:
	explicit Matrix_Plan(unsigned int var_count);						// ����������� ������������
	~Matrix_Plan();
	double& operator()(size_t line, size_t col);						// �������� �� �������
	void enter_results();												// ���� ����������������� ������
	bool read_from_file(FileHandler&);
	void print();														// ����� ������� �� ������
	void count();														// ������� �������������
	void assessment();													// ������ ������
	// �������� ������
	double get_value(double x, double deltaT, double Th);
};
