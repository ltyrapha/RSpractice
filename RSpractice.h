#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RSpractice.h"
#include <QProgressbar>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class RSpractice : public QMainWindow
{
	Q_OBJECT

public:
	RSpractice(QWidget *parent = Q_NULLPTR);	

private slots:
	void on_inputSelect_clicked();
	void on_outputSelect_clicked();
	void on_laplace_sharpen();
	void on_roberts_sharpen();
	void on_equalization_histogram();
	void on_vertical_sharpen();
	void on_horizon_sharpen();
	void on_mean3_smooth();
	void on_mean5_smooth();
	void on_median3_smooth();
	void on_median5_smooth();
	void on_maxmin_greyTrans();
	void on_linear2_greyTrans();
	void on_log_greyTrans();
	void on_reverse_greyTrans();
	void on_exp_greyTrans();
	void on_dos_radiation();
	void on_vegetation_extract();
	void on_water_extract();
	void on_buildings_extract();
	void on_fft_freq();
	void on_high_freq();
	void on_low_freq();

private:
	Ui::RSpracticeClass ui;
	QString input_filePath;
	float *originalData;
	unsigned char *resultData;
	void read();
	void output(QString filepath);
	void display(float *data);
	void storageData(unsigned char* data);
	//遥感影像的信息
	int rowHeightY;
	int colWidthX;
	int bandNum;
	//界面控件
	QProgressBar* pProgressBar = new QProgressBar(this);
	QLineEdit* lineEdit_input = new QLineEdit(this);
	QLineEdit* lineEdit_output = new QLineEdit(this);
	QLabel* resultlabel = new QLabel(this);
	QLabel* originallabel = new QLabel(this);
	QLabel* input_name = new QLabel(this);
	QLabel* output_name = new QLabel(this);
};
