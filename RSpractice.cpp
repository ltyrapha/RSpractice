#include "RSpractice.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>	
#include <math.h>
#include <algorithm>
#include <Qimage>
#include <QLabel>
#include <QBitmap>
#include <QPainter>
#include <QImageReader>
#include <QPixmap>
#include <fstream>
#include <QFont>
#include <complex.h>
#include <cmath>
#include "gdal_priv.h"
#include "armadillo"

using namespace arma;


RSpractice::RSpractice(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	originalData = NULL;
	resultData = NULL;
	setPalette(QPalette(Qt::white));
	setFixedSize(1200, 840);          //�̶����ڳߴ�
	setWindowTitle(QStringLiteral("Image Processing Software"));  //���ô��ڱ���
	//setStyleSheet("color: rgb(198,236,203);");
	
	//���������С����ʽ
	QFont font;
	font.setPointSize(12);
	font.setFamily("Microsoft YaHei");

	//���ô���ΪԲ��
	/*QBitmap bmp(this->size());
	bmp.fill();
	QPainter p(&bmp);
	p.setPen(Qt::NoPen);
	p.setBrush(Qt::black);
	p.drawRoundedRect(bmp.rect(), 5, 5);
	setMask(bmp);*/
	
	//���ý�����
	pProgressBar->move(200, 140);
	pProgressBar->setFixedSize(800, 30);
	pProgressBar->setStyleSheet("border:0px solid Black;color:white;");
	pProgressBar->setFormat(QStringLiteral("������ȣ�%p%"));
	pProgressBar->setAlignment(Qt::AlignVCenter);

	//���������ļ�·����ʾ
	lineEdit_input->move(135, 80);
	lineEdit_input->setFixedSize(330, 30);
	lineEdit_input->setEnabled(false);
	QLabel* input_show = new QLabel(this);
	QPushButton* input_select = new QPushButton(this);
	input_show->move(35, 80);
	input_show->setText(QStringLiteral("����·����"));
	input_show->setFont(font);
	input_select->move(475, 80);
	input_select->setText(QStringLiteral("��"));
	input_select->setStyleSheet("border:1px solid rgb(173,173,173);border-radius:5px;background-color:rgb(230,231,232);");
	//��������ļ�·����ʾ
	lineEdit_output->move(720, 80);
	lineEdit_output->setFixedSize(330, 30);
	lineEdit_output->setEnabled(false);
	QLabel* output_show = new QLabel(this);
	QPushButton* output_select = new QPushButton(this);
	output_show->move(620, 80);
	output_show->setText(QStringLiteral("���·����"));
	output_show->setFont(font);
	output_select->move(1060, 80);
	output_select->setText(QStringLiteral("����"));
	output_select->setStyleSheet("border:1px solid rgb(173,173,173);border-radius:5px;background-color:rgb(230,231,232);");
	//ͼ�񴰿�
	originallabel->move(20, 180);
	originallabel->resize(570, 580);
	originallabel->setStyleSheet("border:3px dashed rgb(173,173,173);");
	input_name->move(260, 780);
	input_name->setFont(font);
	resultlabel->move(610, 180);
	resultlabel->resize(570, 580);
	resultlabel->setStyleSheet("border:3px dashed rgb(173,173,173);");
	output_name->move(870, 780);
	output_name->setFont(font);
	output_name->setFixedWidth(300);

    //���ò˵���
	QMenuBar* myMenu = menuBar();
	setMenuBar(myMenu);
	QMenu* fileMenu = myMenu->addMenu(QStringLiteral("�ļ�"));
	QAction* fileBtn = fileMenu->addAction(QStringLiteral("���ļ�"));
	QMenu* enhanceBtn = myMenu->addMenu(QStringLiteral("ͼ����ǿ"));
	QMenu* greyTransformMenu = enhanceBtn->addMenu(QStringLiteral("�Ҷȱ任"));
	QAction* maxminBtn = greyTransformMenu->addAction(QStringLiteral("�����С����"));
	QAction* linear2Btn = greyTransformMenu->addAction(QStringLiteral("2%��������"));
	QAction* logBtn = greyTransformMenu->addAction(QStringLiteral("�����任"));
	QAction* expBtn = greyTransformMenu->addAction(QStringLiteral("ָ���任"));
	QAction* reverseBtn = greyTransformMenu->addAction(QStringLiteral("ͼ��ת"));
	QMenu* histogramTransMenu = enhanceBtn->addMenu(QStringLiteral("ֱ��ͼ����"));
	QAction* equalizationBtn = histogramTransMenu->addAction(QStringLiteral("ֱ��ͼ���⻯"));
	QMenu* sharpenMenu = enhanceBtn->addMenu(QStringLiteral("�ռ�����"));
	QAction* verticalBtn = sharpenMenu->addAction(QStringLiteral("��ֱ��"));
	QAction* horizonBtn = sharpenMenu->addAction(QStringLiteral("ˮƽ��"));
	QAction* laplaceBtn = sharpenMenu->addAction(QStringLiteral("������˹����"));
	QAction* robertsBtn = sharpenMenu->addAction(QStringLiteral("����΢������"));
	QMenu* smoothMenu = enhanceBtn->addMenu(QStringLiteral("�ռ���ƽ��"));
	QAction* mean3Btn = smoothMenu->addAction(QStringLiteral("3��3��ֵ�˲�"));
	QAction* mean5Btn = smoothMenu->addAction(QStringLiteral("5��5��ֵ�˲�"));
	QAction* median3Btn = smoothMenu->addAction(QStringLiteral("3��3��ֵ�˲�"));
	QAction* median5Btn = smoothMenu->addAction(QStringLiteral("5��5��ֵ�˲�"));
	QMenu* frequencytMenu = enhanceBtn->addMenu(QStringLiteral("Ƶ������ǿ"));
	QAction* fftBtn = frequencytMenu->addAction(QStringLiteral("FFTƵ��ͼ"));
	QAction* idealhighBtn = frequencytMenu->addAction(QStringLiteral("�����ͨ�˲�"));
	QAction* ideallowBtn = frequencytMenu->addAction(QStringLiteral("�����ͨ�˲�"));
	QMenu* radiativeMenu = myMenu->addMenu(QStringLiteral("�������У��"));
	QAction* dosBtn = radiativeMenu->addAction(QStringLiteral("��Ŀ�귨"));
	QMenu* extractMenu = myMenu->addMenu(QStringLiteral("������ȡ"));
	QAction* vegetationBtn = extractMenu->addAction(QStringLiteral("ֲ����ȡ"));
	QAction* waterBtn = extractMenu->addAction(QStringLiteral("ˮ����ȡ"));
	QAction* buildingsBtn = extractMenu->addAction(QStringLiteral("������ȡ"));
	
	//�ź���۵�����
	connect(fileBtn, SIGNAL(triggered()), this, SLOT(on_inputSelect_clicked()));
	connect(input_select, SIGNAL(clicked()), this, SLOT(on_inputSelect_clicked()));
	connect(output_select, SIGNAL(clicked()), this, SLOT(on_outputSelect_clicked()));
	connect(laplaceBtn, SIGNAL(triggered()), this, SLOT(on_laplace_sharpen()));
	connect(robertsBtn, SIGNAL(triggered()), this, SLOT(on_roberts_sharpen()));
	connect(equalizationBtn, SIGNAL(triggered()), this, SLOT(on_equalization_histogram()));
	connect(verticalBtn, SIGNAL(triggered()), this, SLOT(on_vertical_sharpen()));
	connect(horizonBtn, SIGNAL(triggered()), this, SLOT(on_horizon_sharpen()));
	connect(mean3Btn, SIGNAL(triggered()), this, SLOT(on_mean3_smooth()));
	connect(mean5Btn, SIGNAL(triggered()), this, SLOT(on_mean5_smooth()));
	connect(median3Btn, SIGNAL(triggered()), this, SLOT(on_median3_smooth()));
	connect(median5Btn, SIGNAL(triggered()), this, SLOT(on_median5_smooth()));
	connect(maxminBtn, SIGNAL(triggered()), this, SLOT(on_maxmin_greyTrans()));
	connect(linear2Btn, SIGNAL(triggered()), this, SLOT(on_linear2_greyTrans()));
	connect(logBtn, SIGNAL(triggered()), this, SLOT(on_log_greyTrans()));
	connect(expBtn, SIGNAL(triggered()), this, SLOT(on_exp_greyTrans()));
	connect(reverseBtn, SIGNAL(triggered()), this, SLOT(on_reverse_greyTrans()));
	connect(dosBtn, SIGNAL(triggered()), this, SLOT(on_dos_radiation()));
	connect(vegetationBtn, SIGNAL(triggered()), this, SLOT(on_vegetation_extract()));
	connect(waterBtn, SIGNAL(triggered()), this, SLOT(on_water_extract()));
	connect(buildingsBtn, SIGNAL(triggered()), this, SLOT(on_buildings_extract()));
	connect(fftBtn, SIGNAL(triggered()), this, SLOT(on_fft_freq()));
	connect(idealhighBtn, SIGNAL(triggered()), this, SLOT(on_high_freq()));
	connect(ideallowBtn, SIGNAL(triggered()), this, SLOT(on_low_freq()));
}

// **************��ȡң��Ӱ�������·��************************ //
void RSpractice::on_inputSelect_clicked() {
	this->input_filePath = QFileDialog::getOpenFileName \
		(this, QStringLiteral("ѡ������ң��Ӱ��"), "../../../", tr("All files(*);;""TIFF(*.tif *.tiff);;JPEG(*.jpg *.jpeg)"));
	lineEdit_input->setText(this->input_filePath);
	
	//��ȡ����
	read();
}

// **************��ȡң��Ӱ������·��************************ //
void RSpractice::on_outputSelect_clicked() {
	QString output_filepath = QFileDialog::getSaveFileName \
		(this, QStringLiteral("ѡ��ң��Ӱ�����·����"), "../../../", tr("TIFF(*.tif *.tiff);;" "All files(*)"));
	if (output_filepath == NULL)
		return;

	output(output_filepath);
}

// **************��ȡӰ�����ص����鲢��ʾ��ͼ��************************ //
void RSpractice::read() {
	if (originalData != NULL)
		delete[] originalData;
	// ��Ӱ�񣬻�ȡ���ݼ�
	GDALDataset  *poDataset;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");    // ����GDAL֧������·��
	qDebug() << this->input_filePath.toStdString().c_str();
	poDataset = (GDALDataset *)GDALOpen(this->input_filePath.toStdString().c_str(), GA_ReadOnly);
	if (poDataset == NULL) {
		QMessageBox::warning(this, "Warning", "Invaild input file name.");
		return;
	}
	// ��ȡӰ��ߴ���Ϣ
	rowHeightY = poDataset->GetRasterYSize();
	colWidthX = poDataset->GetRasterXSize();
	bandNum = poDataset->GetRasterCount();
	long bandSize = rowHeightY*colWidthX;
	//���ý�������ȡֵ��Χ
	pProgressBar->setRange(0, bandNum);
	// ��ȡӰ���������ݵ�����
	float *pInputData = new float[rowHeightY * colWidthX * bandNum];
	int *panBandMap = new int[bandNum];
	for (int i = 0; i < bandNum; ++i)
		panBandMap[i] = i + 1;
	//ֻ�����ӣ�0��0����ʼ,�洢��pInputData
	if (poDataset->RasterIO(GF_Read, 0, 0, colWidthX, rowHeightY, \
		pInputData, colWidthX, rowHeightY, \
		GDT_Float32, bandNum, panBandMap, 0, 0, 0) != CE_None)
		return;
	// �ر����ݼ�
	GDALClose((GDALDatasetH)poDataset);
	// Ӱ����������洢������--������
	long int value = 0;		//
	float *pProcessData = new float[rowHeightY * colWidthX * bandNum];
	for (int b = 0; b < bandNum; ++b)
		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				pProcessData[b*bandSize + j*rowHeightY + i] = pInputData[b*bandSize + i*colWidthX + j];
				value++;
				pProgressBar->setValue(value);		//������������ʾ
			}
	originalData = pProcessData;
	pProgressBar->reset();		//�ý���������
	
	//ͼ����ʾ����
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	originallabel->update();
	QImage image(colWidthX, rowHeightY, QImage::Format_RGB32);
	if (bandNum >= 3) {
		//���������ڵ���3
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++)
				image.setPixel(QPoint(j, i), qRgb(myFcube(i, j, 0), myFcube(i, j, 1), myFcube(i, j, 2)));
	}
	else if (bandNum == 1) {
		//������Ϊ1
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++)
				image.setPixel(QPoint(j, i), qRgb(myFcube(i, j, 0), myFcube(i, j, 0), myFcube(i, j, 0)));
	}
	else {
		QMessageBox::warning(this, "Warning", "The file cannot show.");
		return;
	}
	//�����ݺ�ȣ�ƽ�����޾��״������
	QImage resultImg = image.scaled(originallabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	originallabel->setPixmap(QPixmap::fromImage(resultImg));
	originallabel->show();
	input_name->setText(QStringLiteral("ԭͼ��"));

	//�����ڴ�
	delete[] pInputData;
	delete[] panBandMap;
}

// **************����ң��Ӱ���ļ�************************ //
void RSpractice::output(QString filepath) {
	
	if (this->resultData == NULL) {
		QMessageBox::warning(this, "Warning", QStringLiteral("��ѡ����Ӧͼ���������"));
		return;
	}
	lineEdit_output->setText(filepath);
	
	unsigned char *pOutputData = this->resultData;

	// �������ݼ�
	const char *pszFormat = "GTiff";
	GDALDriver *poDriver;
	char **papszMetadata;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	GDALDataset *poDstDS;
	char **papszOptions = NULL;
	poDstDS = poDriver->Create(filepath.toStdString().c_str(), \
		colWidthX, rowHeightY, bandNum, GDT_Byte, papszOptions);
	// д������
	int *panBandMap = new int[bandNum];
	for (int i = 0; i < bandNum; ++i)
		panBandMap[i] = i + 1;
	if (poDstDS->RasterIO(GF_Write, 0, 0, colWidthX, rowHeightY, pOutputData, \
		colWidthX, rowHeightY, GDT_Byte, bandNum, \
		panBandMap, 0, 0, 0) != CE_None) {
		QMessageBox::warning(this, "Warning", "Write raster failed.");
		return;
	}
	// �ر����ݼ�
	GDALClose((GDALDatasetH)poDstDS);

	// �����ڴ�
	delete[] panBandMap;
	delete[] pOutputData;
	this->resultData = NULL;

	// ������ʾ
	QMessageBox::information(this, "Information", QStringLiteral("ͼ�񱣴�ɹ���"));
}

// **************��ʾͼ��************************ //
void RSpractice::display(float *data){
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(data, rowHeightY, colWidthX, bandNum);

	//�����ص���ʾͼ��
	resultlabel->update();
	QImage image(colWidthX,rowHeightY,QImage::Format_RGB32);
	if (bandNum >= 3) {
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++)
				image.setPixel(QPoint(j, i), qRgb(myFcube(i, j, 0), myFcube(i, j, 1), myFcube(i, j, 2) ));
	}
	else if (bandNum == 1) {
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++)
				image.setPixel(QPoint(j, i), qRgb(myFcube(i, j, 0), myFcube(i, j, 0), myFcube(i, j, 0)));
	}
	else {
		QMessageBox::warning(this, "Warning", "The file cannot show.");
		return;
	}
	QImage resultImg = image.scaled(resultlabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	resultlabel->setPixmap(QPixmap::fromImage(resultImg));
	resultlabel->show();

	pProgressBar->reset();		//�ý���������
}

// **************�洢����************************ //
void RSpractice::storageData(unsigned char* data) {
	//����һ���ڴ棬ʹ��������ȷ�����·���������ʱ������û��ɾ��
	if (this->resultData != NULL) {
		delete[] resultData;
		resultData = NULL;
	}
	// Ӱ����������洢������--������
	unsigned char * pResData = data;
	long bandSize = rowHeightY*colWidthX;
	unsigned char *pOutputData = new unsigned char[rowHeightY * colWidthX * bandNum];
	for (int b = 0; b < bandNum; ++b)
		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j)
				pOutputData[b*bandSize + i*colWidthX + j] = \
				pResData[b*bandSize + j*rowHeightY + i];
	this->resultData = pOutputData;
}

// **************�ռ�����-��ֱ������************************ //
void RSpractice::on_vertical_sharpen() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//��ֱ������
	long int value = 0;
	//�߽粿�ֲ�����
	int border = 1;
	for (int i = border; i < rowHeightY - border; i++)
		for (int j = border; j < colWidthX - border; j++)
			for (int b = 0; b < bandNum; b++) {
				//��ֱ����ģ��
				float sum = myFcube(i + 1, j - 1, b) + 2 * myFcube(i, j - 1, b) + myFcube(i - 1, j - 1, b)
					- myFcube(i + 1, j + 1, b) - 2 * myFcube(i, j + 1, b) - myFcube(i - 1, j + 1, b);
				//���Ϊ����ȡ��������255��ȡ255����֤��0~255֮��
				if (sum < 0) sum = abs(sum);
				if (sum > 255) sum = 255;
				resFcube(i, j, b) = sum;
				value++;
				pProgressBar->setValue(value);
			}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("��ֱ��"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�ռ�����-ˮƽ������************************ //
void RSpractice::on_horizon_sharpen() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//ˮƽ������
	long int value = 0;
	//�߽粿�ֲ�����
	int border = 1;
	for (int i = border; i < rowHeightY - border; i++)
		for (int j = border; j < colWidthX - border; j++)
			for (int b = 0; b < bandNum; b++) {
				//ˮƽ����ģ��
				float sum = myFcube(i + 1, j - 1, b) + 2 * myFcube(i + 1, j, b) + myFcube(i + 1, j + 1, b)
					- myFcube(i - 1, j - 1, b) - 2 * myFcube(i - 1, j, b) - myFcube(i - 1, j + 1, b);
				//���Ϊ����ȡ��������255��ȡ255����֤��0~255֮��
				if (sum < 0) sum = abs(sum);
				if (sum > 255) sum = 255;
				resFcube(i, j, b) = sum;
				value++;
				pProgressBar->setValue(value);
			}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("ˮƽ��"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�ռ���ƽ��-3*3��ֵ�˲�************************ //
void RSpractice::on_mean3_smooth() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//3*3��ֵ�˲�����
	long int value = 0;
	int mean[9] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// �����һȦ�����ز�������
		for (int i = 1; i < colWidthX - 1; i++)
		{
			for (int j = 1; j < rowHeightY - 1; j++)
			{
				mean[0] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j - 1];
				mean[1] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j];
				mean[2] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j + 1];

				mean[3] = (int)myFcube[k*bandSize + i*rowHeightY + j - 1];
				mean[4] = (int)myFcube[k*bandSize + i*rowHeightY + j];
				mean[5] = (int)myFcube[k*bandSize + i*rowHeightY + j + 1];

				mean[6] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j - 1];
				mean[7] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j];
				mean[8] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j + 1];

				//���ֵ
				double sum = 0;
				for (int k = 0; k < 9; k++)
					sum += mean[k];
				sum = sum / 9;
				resFcube[k*bandSize + i*rowHeightY + j] = (unsigned char)sum;
				value++;
				pProgressBar->setValue(value);
			}
		}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("3��3��ֵ�˲�  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�ռ���ƽ��-5*5��ֵ�˲�************************ //
void RSpractice::on_mean5_smooth() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//5*5��ֵ�˲�����
	long int value = 0;
	int mean[25] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// �������Ȧ�����ز�������
		for (int i = 2; i < colWidthX - 2; i++)
		{
			for (int j = 2; j < rowHeightY - 2; j++)
			{
				mean[0] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j - 2];
				mean[1] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j - 1];
				mean[2] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j];
				mean[3] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j + 1];
				mean[4] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j + 2];

				mean[5] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j - 2];
				mean[6] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j - 1];
				mean[7] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j];
				mean[8] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j + 1];
				mean[9] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j + 2];

				mean[10] = (int)myFcube[k*bandSize + i*rowHeightY + j - 2];
				mean[11] = (int)myFcube[k*bandSize + i*rowHeightY + j - 1];
				mean[12] = (int)myFcube[k*bandSize + i*rowHeightY + j];
				mean[13] = (int)myFcube[k*bandSize + i*rowHeightY + j + 1];
				mean[14] = (int)myFcube[k*bandSize + i*rowHeightY + j + 2];

				mean[15] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j - 2];
				mean[16] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j - 1];
				mean[17] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j];
				mean[18] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j + 1];
				mean[19] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j + 2];

				mean[20] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j - 2];
				mean[21] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j - 1];
				mean[22] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j];
				mean[23] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j + 1];
				mean[24] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j + 2];

				//���ֵ
				double sum = 0;
				for (int k = 0; k < 25; k++)
					sum += mean[k];
				sum = sum / 25;
				resFcube[k*bandSize + i*rowHeightY + j] = (unsigned char)sum;
				value++;
				pProgressBar->setValue(value);
			}
		}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("5��5��ֵ�˲�  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�ռ���ƽ��-3*3��ֵ�˲�************************ //
void RSpractice::on_median3_smooth() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//3*3��ֵ�˲�����
	long int value = 0;
	int median[9] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// �����һȦ�����ز�������
		for (int i = 1; i < colWidthX - 1; i++)
		{
			for (int j = 1; j < rowHeightY - 1; j++)
			{
				median[0] = (int)myFcube[k *bandSize + (i - 1)*rowHeightY + j - 1];
				median[1] = (int)myFcube[k *bandSize + (i - 1)*rowHeightY + j];
				median[2] = (int)myFcube[k *bandSize + (i - 1)*rowHeightY + j + 1];

				median[3] = (int)myFcube[k *bandSize + i*rowHeightY + j - 1];
				median[4] = (int)myFcube[k *bandSize + i*rowHeightY + j];
				median[5] = (int)myFcube[k *bandSize + i*rowHeightY + j + 1];

				median[6] = (int)myFcube[k *bandSize + (i + 1)*rowHeightY + j - 1];
				median[7] = (int)myFcube[k *bandSize + (i + 1)*rowHeightY + j];
				median[8] = (int)myFcube[k *bandSize + (i + 1)*rowHeightY + j + 1];

				//��������ֵ
				std::sort(median, median + 9);
				resFcube[k *bandSize + i *rowHeightY + j] = (unsigned char)median[4];
				value++;
				pProgressBar->setValue(value);
			}
		}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("3��3��ֵ�˲�  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�ռ���ƽ��-5*5��ֵ�˲�************************ //
void RSpractice::on_median5_smooth() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//3*3��ֵ�˲�����
	long int value = 0;
	int median[25] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// �������Ȧ�����ز�������
		for (int i = 2; i < colWidthX - 2; i++)
		{
			for (int j = 2; j < rowHeightY - 2; j++)
			{
				median[0] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j - 2];
				median[1] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j - 1];
				median[2] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j];
				median[3] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j + 1];
				median[4] = (int)myFcube[k*bandSize + (i - 2)*rowHeightY + j + 2];

				median[5] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j - 2];
				median[6] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j - 1];
				median[7] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j];
				median[8] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j + 1];
				median[9] = (int)myFcube[k*bandSize + (i - 1)*rowHeightY + j + 2];

				median[10] = (int)myFcube[k*bandSize + i*rowHeightY + j - 2];
				median[11] = (int)myFcube[k*bandSize + i*rowHeightY + j - 1];
				median[12] = (int)myFcube[k*bandSize + i*rowHeightY + j];
				median[13] = (int)myFcube[k*bandSize + i*rowHeightY + j + 1];
				median[14] = (int)myFcube[k*bandSize + i*rowHeightY + j + 2];

				median[15] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j - 2];
				median[16] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j - 1];
				median[17] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j];
				median[18] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j + 1];
				median[19] = (int)myFcube[k*bandSize + (i + 1)*rowHeightY + j + 2];

				median[20] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j - 2];
				median[21] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j - 1];
				median[22] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j];
				median[23] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j + 1];
				median[24] = (int)myFcube[k*bandSize + (i + 2)*rowHeightY + j + 2];

				//��������ֵ
				std::sort(median, median + 25);
				resFcube[k *bandSize + i *rowHeightY + j] = (unsigned char)median[12];
				value++;
				pProgressBar->setValue(value);
			}
		}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("5��5��ֵ�˲�  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************Ƶ������ǿ-FFT************************ //
void RSpractice::on_fft_freq() {
	//���ж�ά���ٸ���Ҷ�任����ʾƵ��ͼ
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//FFT����
	int value = 0;
	for (int b = 0; b < bandNum; b++) {
		Mat<double> mat(rowHeightY, colWidthX);
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				mat(i, j) = (double)(myFcube(i, j, b));
				//ʹ�任��ĸ���ҶƵ�����Ļ�
				mat(i, j) = pow(-1, i + j) * mat(i, j);
			}
		//��ά���ٸ���Ҷ�任
		cx_mat pfft = fft2(mat);

		//����任��ķ�ֵ
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//ȡ����ֵ
				double temp = abs(pfft(i, j));
				//��ֵԶ����255��Ϊ�����ڴ���ֱ����ʾ���������e����������
				resFcube(i, j, b) = 5 * logf(temp);
				value++;
				pProgressBar->setValue(value);
			}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("FFTƵ��ͼ"));
	//����ת��Ϊuint8����ͼ�����
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************Ƶ������ǿ-�����ͨ�˲���************************ //
void RSpractice::on_high_freq() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//����Ƶ��
	Mat<double> D(rowHeightY, colWidthX);
	//���ļ���
	float centerx = colWidthX / 2.0;
	float centery = rowHeightY / 2.0;
	for (int u = 0; u < rowHeightY; u++)
		for (int v = 0; v < colWidthX; v++)
			//D��u��v����ʽ����
			D(u, v) = sqrt(pow(double(u - centery), 2) + pow(v - centerx, 2));

	//���ý�ֹƵ��Ϊ10
	double D0 = 10;

	//���д���
	int value = 0;
	for (int b = 0; b < bandNum; b++) {
		Mat<double> mat(rowHeightY, colWidthX);
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				double k = i + j;
				mat(i, j) = (double)(myFcube(i, j, b));
				//ʹ�任��ĸ���ҶƵ�����Ļ�
				mat(i, j) = pow(-1, i + j) * mat(i, j);
			}

		//��ά���ٸ���Ҷ�任
		cx_mat resfft = fft2(mat);

		//�����ͨ�˲�
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//�������ڽ�ֹƵ��D0��ֵ
				if (D(i, j) > D0)
					continue;
				else
					resfft(i, j) = 0;
			}

		//��ά���ٸ���Ҷ���任
		cx_mat pifft = ifft2(resfft);

		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//ȡ��ֵ
				float temp = abs(pifft(i, j));
				resFcube(i, j, b) = temp;
				value++;
				pProgressBar->setValue(value);
			}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("�����ͨ�˲���"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************Ƶ������ǿ-�����ͨ�˲���************************ //
void RSpractice::on_low_freq() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//����Ƶ��
	Mat<double> D(rowHeightY, colWidthX);
	//���ļ���
	float centerx = colWidthX / 2.0;
	float centery = rowHeightY / 2.0;
	for (int u = 0; u < rowHeightY; u++)
		for (int v = 0; v < colWidthX; v++)
			//D��u��v����ʽ����
			D(u, v) = sqrt(pow(double(u - centery), 2) + pow(v - centerx, 2));

	//���ý�ֹƵ��Ϊ50
	double D0 = 50;

	//���д���
	int value = 0;
	for (int b = 0; b < bandNum; b++) {
		Mat<double> mat(rowHeightY, colWidthX);
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				double k = i + j;
				mat(i, j) = (double)(myFcube(i, j, b));
				//ʹ�任��ĸ���ҶƵ�����Ļ�
				mat(i, j) = pow(-1, i + j) * mat(i, j);
			}

		//��ά���ٸ���Ҷ�任
		cx_mat resfft = fft2(mat);

		//�����ͨ�˲�
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//����С�ڵ��ڽ�ֹƵ��D0��ֵ
				if (D(i, j) <= D0)
					continue;
				else
					resfft(i, j) = 0;
			}

		//��ά���ٸ���Ҷ���任
		cx_mat pifft = ifft2(resfft);

		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//ȡ��ֵ
				float temp = abs(pifft(i, j));
				resFcube(i, j, b) = temp;
				value++;
				pProgressBar->setValue(value);
			}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("�����ͨ�˲���"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�������У��-��Ŀ�귨************************ //
void RSpractice::on_dos_radiation() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//��Ŀ�귨ȥ������
	long int value = 0;
	long bandSize = rowHeightY*colWidthX;
	for (int b = 0; b < bandNum; ++b)
	{
		//matΪ��b������
		Mat<float> mat = resFcube.slice(b);
		//���������Сֵ
		fcolvec B = min(mat, 1);
		//����������ֵ��С��ֵ
		float pmin = min(B);

		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				//������Сֵ
				resFcube[b*bandSize + i*colWidthX + j] = resFcube[b*bandSize + i*colWidthX + j] - pmin;
				value++;
				pProgressBar->setValue(value);
			}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("��Ŀ�귨ȥ����"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************ֱ��ͼ���⻯************************ //
void RSpractice::on_equalization_histogram() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	
	fcube resFcube = myFcube / 2 + 10;

	int gray[256] = { 0 };//�����洢ÿ���Ҷ�ֵ�����ظ���
	double gray_prob[256] = { 0 };//�����洢ÿ���Ҷ�ֵ��Ƶ��
	double gray_distribution[256] = { 0 };//�洢�ۻ��ֲ�
	int gray_equal[256] = { 0 };//�洢���⻯��ĻҶ�ֵ
	int gray_sum = bandNum*rowHeightY*colWidthX;

	//ͳ��ÿ���Ҷ�ֵ�����ظ���
	for (int b = 0; b < bandNum; ++b) {
		for (int i = 0; i < rowHeightY; ++i) {
			for (int j = 0; j < colWidthX; ++j) {
				int value = myFcube(i, j, b);
				gray[value]++;
			}
		}
	}

	//����Ҷ�Ƶ��
	for (int i = 0; i < 256; i++)
	{
		gray_prob[i] = ((double)gray[i] / gray_sum);
	}

	//�����ۼƷֲ�
	gray_distribution[0] = gray_prob[0];
	for (int i = 1; i < 256; i++)
	{
		gray_distribution[i] = gray_distribution[i - 1] + gray_prob[i];
	}

	//������⻯��ĻҶ�ֵ����������
	for (int i = 0; i < 256; i++)
	{
		gray_equal[i] = (uchar)(255 * gray_distribution[i] + 0.5);
	}

	//���⻯��ÿ�����ػҶ�ֵ
	for (int b = 0; b < bandNum; ++b) {
		for (int i = 0; i < rowHeightY; ++i) {
			for (int j = 0; j < colWidthX; ++j) {
				int value = myFcube(i, j, b);
				resFcube(i, j, b) = gray_equal[value];
			}
		}
	}
	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("ֱ��ͼ���⻯"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�ռ�����-������˹����************************ //
void RSpractice::on_laplace_sharpen() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	//������˹���ӿռ����񻯣�����һȦ���ز�����
	long int value = 0;
	int border = 1;
	for(int i=border;i < rowHeightY-border;i++)
		for (int j = border; j < colWidthX - border; j++)
			for (int b = 0; b < bandNum; b++) {
				float sum = 4 * myFcube(i, j, b) - myFcube(i + 1, j, b) - myFcube(i - 1, j, b)
						- myFcube(i, j + 1, b) - myFcube(i, j - 1, b);
				if (sum < 0) sum = 0;
				if (sum > 255) sum = 255;
				resFcube(i, j, b) = sum;
				value++;
				pProgressBar->setValue(value);
			}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("������˹"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�ռ�����-����΢������************************ //
void RSpractice::on_roberts_sharpen() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	
	//����΢���㷨���пռ����񻯣����һ�к����һ�����ز�����
	long int value = 0;
	int border = 1;
	for (int i = 0; i < rowHeightY - border; i++)
		for (int j = 0; j < colWidthX - border; j++)
			for (int b = 0; b < bandNum; b++) {
				float sum = abs(myFcube(i + 1, j + 1, b) - myFcube(i, j, b)) 
					+ abs(myFcube(i + 1, j, b) - myFcube(i, j + 1, b));
				if (sum < 0) sum = 0;
				if (sum > 255) sum = 255;
				resFcube(i, j, b) = sum;
				value++;
				pProgressBar->setValue(value);
			}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("����΢��"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�Ҷȱ任-2%��������************************ //
void RSpractice::on_linear2_greyTrans() {
	//����2%���죬��ͼ��Ҷ�2%~98%�Ĳ������쵽0~255
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//�Ҷȱ任
	long int value = 0;
	float upper98 = 0, low2 = 0,sum = 0;
	long bandSize = rowHeightY*colWidthX;
	for (int b = 0; b < bandNum; b++) {
		fvec vec_sort = sort(vectorise(myFcube.slice(b)));
		low2 = vec_sort(0.02 * bandSize); 
		if (low2 < 0) low2 = 0;
		upper98 = vec_sort(0.98 * bandSize);
		
		for (int i = 0; i < rowHeightY ; i++)
			for (int j = 0; j < colWidthX; j++) {
				sum = myFcube(i, j, b);
				if (sum < low2)
					resFcube(i, j, b) = 0;
				else if (low2 <= sum && sum <= upper98) {
					resFcube(i, j, b) = 255 * (sum - low2) / (upper98 - low2); 
				}
				else
					resFcube(i, j, b) = 255;
				value++;
				pProgressBar->setValue(value);
			}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("2%��������"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�Ҷȱ任-�����С����************************ //
void RSpractice::on_maxmin_greyTrans() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//�Ҷȱ任
	long int value = 0;
	for (int b = 0; b < bandNum; ++b)
	{
		float max = myFcube.slice(b).max();
		float min = myFcube.slice(b).min();
		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				resFcube[i, j, b] = 255 * (myFcube[i, j, b] - min) / (max - min);
				value++;
				pProgressBar->setValue(value);
			}
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("�����С����"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�Ҷȱ任-�����任************************ //
void RSpractice::on_log_greyTrans() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	long int value = 0;
	float sum = 0;
	for (int b = 0; b < bandNum; ++b)
	{
		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				sum = myFcube(i, j, b);
				resFcube(i, j, b) = 30 * log(sum + 1);
				value++;
				pProgressBar->setValue(value);
			}
	}
	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("�����任"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�Ҷȱ任-ͼ��ת************************ //
void RSpractice::on_reverse_greyTrans() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	long int value = 0;
	float sum = 0;
	for (int b = 0; b < bandNum; ++b)
	{
		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				sum = myFcube(i, j, b);
				resFcube(i, j, b) = 255 - sum;
				value++;
				pProgressBar->setValue(value);
			}
	}
	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("ͼ��ת"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************�Ҷȱ任-ָ���任************************ //
void RSpractice::on_exp_greyTrans() {
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	long int value = 0;
	float sum = 0;
	long bandSize = rowHeightY*colWidthX;
	for (int b = 0; b < bandNum; ++b)
	{
		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				sum = double(myFcube(i, j, b) / 255);
				sum = int(pow(sum, 2) * 255);
				if (sum >= 0 && sum <= 255)
					resFcube(i, j, b) = sum;
				else if (sum > 255)
					resFcube(i, j, b) = 255;
				value++;
				pProgressBar->setValue(value);
			}
	}
	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("ָ���任"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************������ȡ-ֲ��************************ //
void RSpractice::on_vegetation_extract() {
	//����NDVIָ������ֵ������ȡֲ��
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	//��ȡ����
	long int value = 0;
	long bandSize = rowHeightY*colWidthX;
	if (bandNum < 5)
	{
		QMessageBox::warning(this, "Warning", "check your image format!");
		return;
	}

	else
	{
		QMessageBox::information(this, "Information", "your image format is right!");
		Mat <float> mat_nir = resFcube.slice(4);
		Mat <float> mat_red = resFcube.slice(3);
		double temp = 0;
		float count, sum;
		for (int k = 0; k < bandNum; k++)
		{
			for (int i = 0; i < colWidthX; i++)
			{
				for (int j = 0; j < rowHeightY; j++)
				{
					resFcube[k*bandSize + i*rowHeightY + j] = (float)((mat_nir[i*rowHeightY + j] - mat_red[i*rowHeightY + j]) / (mat_nir[i*rowHeightY + j] + mat_red[i*rowHeightY + j]));
					if (resFcube[k*bandSize + i*rowHeightY + j] < temp)
						resFcube[k*bandSize + i*rowHeightY + j] = 0;
					if (resFcube[k*bandSize + i*rowHeightY + j] > temp)
					{
						resFcube[k*bandSize + i*rowHeightY + j] = 255;
						count += 1;
					}
					value++;
					pProgressBar->setValue(value);
				}
			}
		}
		sum = float(count / (rowHeightY * colWidthX * bandNum));
		cout << sum << endl;
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("ֲ����ȡ"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************������ȡ-ˮ��************************ //
void RSpractice::on_water_extract() {
	//����NDWIָ������ֵ������ȡˮ��
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	long int value = 0;
	long bandSize = rowHeightY*colWidthX;
	if (bandNum < 5)
	{
		QMessageBox::warning(this, "Warning", "check your image format!");
		return;
	}
	else
	{
		QMessageBox::information(this, "Information", "your image format is right!");
		Mat <float> mat_nir = resFcube.slice(4);
		Mat <float> mat_green = resFcube.slice(2);
		double temp = 0;
		float count, sum;
		for (int k = 0; k < bandNum; k++)
		{
			for (int i = 0; i < colWidthX; i++)
			{
				for (int j = 0; j < rowHeightY; j++)
				{
					resFcube[k*bandSize + i*rowHeightY + j] = (float)((mat_green[i*rowHeightY + j] - mat_nir[i*rowHeightY + j]) / (mat_nir[i*rowHeightY + j] + mat_green[i*rowHeightY + j]));
					if (resFcube[k*bandSize + i*rowHeightY + j] < temp)
						resFcube[k*bandSize + i*rowHeightY + j] = 0;
					if (resFcube[k*bandSize + i*rowHeightY + j] > temp)
					{
						resFcube[k*bandSize + i*rowHeightY + j] = 255;
						count += 1;
					}
					value++;
					pProgressBar->setValue(value);
				}
			}
		}
		sum = float(count / (rowHeightY * colWidthX * bandNum));
		cout << sum << endl;
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("ˮ����ȡ"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************������ȡ-����********************** //
void RSpractice::on_buildings_extract() {
	//����NDBIָ������ֵ������ȡ����
	if (originalData == NULL)
		return;
	// �����������ʼ��Ϊammadillo�еĶ�ά�������ݽṹ
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	long int value = 0;
	long bandSize = rowHeightY*colWidthX;
	if (bandNum < 5)
	{
		QMessageBox::warning(this, "Warning", "check your image format!");
		return;
	}
	else
	{
		QMessageBox::information(this, "Information", "your image format is right!");
		Mat <float> mat_nir = resFcube.slice(4);
		Mat <float> mat_mir = resFcube.slice(5);
		double temp = 0;
		float count, sum;
		for (int k = 0; k < bandNum; k++)
		{
			for (int i = 0; i < colWidthX; i++)
			{
				for (int j = 0; j < rowHeightY; j++)
				{
					resFcube[k*bandSize + i*rowHeightY + j] = (float)((mat_mir[i*rowHeightY + j] - mat_nir[i*rowHeightY + j]) / (mat_nir[i*rowHeightY + j] + mat_mir[i*rowHeightY + j]));
					if (resFcube[k*bandSize + i*rowHeightY + j] < temp)
						resFcube[k*bandSize + i*rowHeightY + j] = 0;
					if (resFcube[k*bandSize + i*rowHeightY + j] > temp)
					{
						resFcube[k*bandSize + i*rowHeightY + j] = 255;
						count += 1;
					}
					value++;
					pProgressBar->setValue(value);
				}
			}
		}
		sum = float(count / (rowHeightY * colWidthX * bandNum));
		cout << sum << endl;
	}

	//��ʾͼ��
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("������ȡ"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

