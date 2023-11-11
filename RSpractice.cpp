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
	setFixedSize(1200, 840);          //固定窗口尺寸
	setWindowTitle(QStringLiteral("Image Processing Software"));  //设置窗口标题
	//setStyleSheet("color: rgb(198,236,203);");
	
	//设置字体大小和样式
	QFont font;
	font.setPointSize(12);
	font.setFamily("Microsoft YaHei");

	//设置窗口为圆角
	/*QBitmap bmp(this->size());
	bmp.fill();
	QPainter p(&bmp);
	p.setPen(Qt::NoPen);
	p.setBrush(Qt::black);
	p.drawRoundedRect(bmp.rect(), 5, 5);
	setMask(bmp);*/
	
	//设置进度条
	pProgressBar->move(200, 140);
	pProgressBar->setFixedSize(800, 30);
	pProgressBar->setStyleSheet("border:0px solid Black;color:white;");
	pProgressBar->setFormat(QStringLiteral("处理进度：%p%"));
	pProgressBar->setAlignment(Qt::AlignVCenter);

	//设置输入文件路径显示
	lineEdit_input->move(135, 80);
	lineEdit_input->setFixedSize(330, 30);
	lineEdit_input->setEnabled(false);
	QLabel* input_show = new QLabel(this);
	QPushButton* input_select = new QPushButton(this);
	input_show->move(35, 80);
	input_show->setText(QStringLiteral("输入路径："));
	input_show->setFont(font);
	input_select->move(475, 80);
	input_select->setText(QStringLiteral("打开"));
	input_select->setStyleSheet("border:1px solid rgb(173,173,173);border-radius:5px;background-color:rgb(230,231,232);");
	//设置输出文件路径显示
	lineEdit_output->move(720, 80);
	lineEdit_output->setFixedSize(330, 30);
	lineEdit_output->setEnabled(false);
	QLabel* output_show = new QLabel(this);
	QPushButton* output_select = new QPushButton(this);
	output_show->move(620, 80);
	output_show->setText(QStringLiteral("输出路径："));
	output_show->setFont(font);
	output_select->move(1060, 80);
	output_select->setText(QStringLiteral("导出"));
	output_select->setStyleSheet("border:1px solid rgb(173,173,173);border-radius:5px;background-color:rgb(230,231,232);");
	//图像窗口
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

    //设置菜单栏
	QMenuBar* myMenu = menuBar();
	setMenuBar(myMenu);
	QMenu* fileMenu = myMenu->addMenu(QStringLiteral("文件"));
	QAction* fileBtn = fileMenu->addAction(QStringLiteral("打开文件"));
	QMenu* enhanceBtn = myMenu->addMenu(QStringLiteral("图像增强"));
	QMenu* greyTransformMenu = enhanceBtn->addMenu(QStringLiteral("灰度变换"));
	QAction* maxminBtn = greyTransformMenu->addAction(QStringLiteral("最大最小拉伸"));
	QAction* linear2Btn = greyTransformMenu->addAction(QStringLiteral("2%线性拉伸"));
	QAction* logBtn = greyTransformMenu->addAction(QStringLiteral("对数变换"));
	QAction* expBtn = greyTransformMenu->addAction(QStringLiteral("指数变换"));
	QAction* reverseBtn = greyTransformMenu->addAction(QStringLiteral("图像反转"));
	QMenu* histogramTransMenu = enhanceBtn->addMenu(QStringLiteral("直方图处理"));
	QAction* equalizationBtn = histogramTransMenu->addAction(QStringLiteral("直方图均衡化"));
	QMenu* sharpenMenu = enhanceBtn->addMenu(QStringLiteral("空间域锐化"));
	QAction* verticalBtn = sharpenMenu->addAction(QStringLiteral("垂直锐化"));
	QAction* horizonBtn = sharpenMenu->addAction(QStringLiteral("水平锐化"));
	QAction* laplaceBtn = sharpenMenu->addAction(QStringLiteral("拉普拉斯算子"));
	QAction* robertsBtn = sharpenMenu->addAction(QStringLiteral("交叉微分算子"));
	QMenu* smoothMenu = enhanceBtn->addMenu(QStringLiteral("空间域平滑"));
	QAction* mean3Btn = smoothMenu->addAction(QStringLiteral("3×3均值滤波"));
	QAction* mean5Btn = smoothMenu->addAction(QStringLiteral("5×5均值滤波"));
	QAction* median3Btn = smoothMenu->addAction(QStringLiteral("3×3中值滤波"));
	QAction* median5Btn = smoothMenu->addAction(QStringLiteral("5×5中值滤波"));
	QMenu* frequencytMenu = enhanceBtn->addMenu(QStringLiteral("频率域增强"));
	QAction* fftBtn = frequencytMenu->addAction(QStringLiteral("FFT频谱图"));
	QAction* idealhighBtn = frequencytMenu->addAction(QStringLiteral("理想高通滤波"));
	QAction* ideallowBtn = frequencytMenu->addAction(QStringLiteral("理想低通滤波"));
	QMenu* radiativeMenu = myMenu->addMenu(QStringLiteral("云雾辐射校正"));
	QAction* dosBtn = radiativeMenu->addAction(QStringLiteral("暗目标法"));
	QMenu* extractMenu = myMenu->addMenu(QStringLiteral("地物提取"));
	QAction* vegetationBtn = extractMenu->addAction(QStringLiteral("植被提取"));
	QAction* waterBtn = extractMenu->addAction(QStringLiteral("水体提取"));
	QAction* buildingsBtn = extractMenu->addAction(QStringLiteral("建筑提取"));
	
	//信号与槽的连接
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

// **************获取遥感影像的输入路径************************ //
void RSpractice::on_inputSelect_clicked() {
	this->input_filePath = QFileDialog::getOpenFileName \
		(this, QStringLiteral("选择输入遥感影像"), "../../../", tr("All files(*);;""TIFF(*.tif *.tiff);;JPEG(*.jpg *.jpeg)"));
	lineEdit_input->setText(this->input_filePath);
	
	//读取数据
	read();
}

// **************获取遥感影像的输出路径************************ //
void RSpractice::on_outputSelect_clicked() {
	QString output_filepath = QFileDialog::getSaveFileName \
		(this, QStringLiteral("选择遥感影像输出路径："), "../../../", tr("TIFF(*.tif *.tiff);;" "All files(*)"));
	if (output_filepath == NULL)
		return;

	output(output_filepath);
}

// **************读取影像像素到数组并显示其图像************************ //
void RSpractice::read() {
	if (originalData != NULL)
		delete[] originalData;
	// 打开影像，获取数据集
	GDALDataset  *poDataset;
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");    // 设置GDAL支持中文路径
	qDebug() << this->input_filePath.toStdString().c_str();
	poDataset = (GDALDataset *)GDALOpen(this->input_filePath.toStdString().c_str(), GA_ReadOnly);
	if (poDataset == NULL) {
		QMessageBox::warning(this, "Warning", "Invaild input file name.");
		return;
	}
	// 获取影像尺寸信息
	rowHeightY = poDataset->GetRasterYSize();
	colWidthX = poDataset->GetRasterXSize();
	bandNum = poDataset->GetRasterCount();
	long bandSize = rowHeightY*colWidthX;
	//设置进度条的取值范围
	pProgressBar->setRange(0, bandNum);
	// 读取影像像素数据到数组
	float *pInputData = new float[rowHeightY * colWidthX * bandNum];
	int *panBandMap = new int[bandNum];
	for (int i = 0; i < bandNum; ++i)
		panBandMap[i] = i + 1;
	//只读，从（0，0）开始,存储于pInputData
	if (poDataset->RasterIO(GF_Read, 0, 0, colWidthX, rowHeightY, \
		pInputData, colWidthX, rowHeightY, \
		GDT_Float32, bandNum, panBandMap, 0, 0, 0) != CE_None)
		return;
	// 关闭数据集
	GDALClose((GDALDatasetH)poDataset);
	// 影像像素数组存储行主序--列主序
	long int value = 0;		//
	float *pProcessData = new float[rowHeightY * colWidthX * bandNum];
	for (int b = 0; b < bandNum; ++b)
		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				pProcessData[b*bandSize + j*rowHeightY + i] = pInputData[b*bandSize + i*colWidthX + j];
				value++;
				pProgressBar->setValue(value);		//进度条进度显示
			}
	originalData = pProcessData;
	pProgressBar->reset();		//让进度条重置
	
	//图像显示部分
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	originallabel->update();
	QImage image(colWidthX, rowHeightY, QImage::Format_RGB32);
	if (bandNum >= 3) {
		//波段数大于等于3
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++)
				image.setPixel(QPoint(j, i), qRgb(myFcube(i, j, 0), myFcube(i, j, 1), myFcube(i, j, 2)));
	}
	else if (bandNum == 1) {
		//波段数为1
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++)
				image.setPixel(QPoint(j, i), qRgb(myFcube(i, j, 0), myFcube(i, j, 0), myFcube(i, j, 0)));
	}
	else {
		QMessageBox::warning(this, "Warning", "The file cannot show.");
		return;
	}
	//保持纵横比，平滑（无锯齿状）处理
	QImage resultImg = image.scaled(originallabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	originallabel->setPixmap(QPixmap::fromImage(resultImg));
	originallabel->show();
	input_name->setText(QStringLiteral("原图像"));

	//清理内存
	delete[] pInputData;
	delete[] panBandMap;
}

// **************导出遥感影像文件************************ //
void RSpractice::output(QString filepath) {
	
	if (this->resultData == NULL) {
		QMessageBox::warning(this, "Warning", QStringLiteral("请选择相应图像处理操作！"));
		return;
	}
	lineEdit_output->setText(filepath);
	
	unsigned char *pOutputData = this->resultData;

	// 创建数据集
	const char *pszFormat = "GTiff";
	GDALDriver *poDriver;
	char **papszMetadata;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	GDALDataset *poDstDS;
	char **papszOptions = NULL;
	poDstDS = poDriver->Create(filepath.toStdString().c_str(), \
		colWidthX, rowHeightY, bandNum, GDT_Byte, papszOptions);
	// 写出数据
	int *panBandMap = new int[bandNum];
	for (int i = 0; i < bandNum; ++i)
		panBandMap[i] = i + 1;
	if (poDstDS->RasterIO(GF_Write, 0, 0, colWidthX, rowHeightY, pOutputData, \
		colWidthX, rowHeightY, GDT_Byte, bandNum, \
		panBandMap, 0, 0, 0) != CE_None) {
		QMessageBox::warning(this, "Warning", "Write raster failed.");
		return;
	}
	// 关闭数据集
	GDALClose((GDALDatasetH)poDstDS);

	// 清理内存
	delete[] panBandMap;
	delete[] pOutputData;
	this->resultData = NULL;

	// 处理提示
	QMessageBox::information(this, "Information", QStringLiteral("图像保存成功！"));
}

// **************显示图像************************ //
void RSpractice::display(float *data){
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(data, rowHeightY, colWidthX, bandNum);

	//逐像素点显示图像
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

	pProgressBar->reset();		//让进度条重置
}

// **************存储数据************************ //
void RSpractice::storageData(unsigned char* data) {
	//开辟一块内存，使其数据在确认输出路径进行输出时其数据没被删除
	if (this->resultData != NULL) {
		delete[] resultData;
		resultData = NULL;
	}
	// 影像像素数组存储列主序--行主序
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

// **************空间域锐化-垂直方向锐化************************ //
void RSpractice::on_vertical_sharpen() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//垂直方向锐化
	long int value = 0;
	//边界部分不处理
	int border = 1;
	for (int i = border; i < rowHeightY - border; i++)
		for (int j = border; j < colWidthX - border; j++)
			for (int b = 0; b < bandNum; b++) {
				//垂直方向模板
				float sum = myFcube(i + 1, j - 1, b) + 2 * myFcube(i, j - 1, b) + myFcube(i - 1, j - 1, b)
					- myFcube(i + 1, j + 1, b) - 2 * myFcube(i, j + 1, b) - myFcube(i - 1, j + 1, b);
				//结果为负的取正，超过255的取255，保证在0~255之间
				if (sum < 0) sum = abs(sum);
				if (sum > 255) sum = 255;
				resFcube(i, j, b) = sum;
				value++;
				pProgressBar->setValue(value);
			}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("垂直锐化"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************空间域锐化-水平方向锐化************************ //
void RSpractice::on_horizon_sharpen() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//水平方向锐化
	long int value = 0;
	//边界部分不处理
	int border = 1;
	for (int i = border; i < rowHeightY - border; i++)
		for (int j = border; j < colWidthX - border; j++)
			for (int b = 0; b < bandNum; b++) {
				//水平方向模板
				float sum = myFcube(i + 1, j - 1, b) + 2 * myFcube(i + 1, j, b) + myFcube(i + 1, j + 1, b)
					- myFcube(i - 1, j - 1, b) - 2 * myFcube(i - 1, j, b) - myFcube(i - 1, j + 1, b);
				//结果为负的取正，超过255的取255，保证在0~255之间
				if (sum < 0) sum = abs(sum);
				if (sum > 255) sum = 255;
				resFcube(i, j, b) = sum;
				value++;
				pProgressBar->setValue(value);
			}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("水平锐化"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************空间域平滑-3*3均值滤波************************ //
void RSpractice::on_mean3_smooth() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//3*3均值滤波处理
	long int value = 0;
	int mean[9] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// 最外层一圈的像素不做处理
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

				//求均值
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("3×3均值滤波  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************空间域平滑-5*5均值滤波************************ //
void RSpractice::on_mean5_smooth() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//5*5均值滤波处理
	long int value = 0;
	int mean[25] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// 最外层两圈的像素不做处理
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

				//求均值
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("5×5均值滤波  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************空间域平滑-3*3中值滤波************************ //
void RSpractice::on_median3_smooth() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//3*3中值滤波处理
	long int value = 0;
	int median[9] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// 最外层一圈的像素不做处理
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

				//排序并求中值
				std::sort(median, median + 9);
				resFcube[k *bandSize + i *rowHeightY + j] = (unsigned char)median[4];
				value++;
				pProgressBar->setValue(value);
			}
		}
	}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("3×3中值滤波  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************空间域平滑-5*5中值滤波************************ //
void RSpractice::on_median5_smooth() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//3*3中值滤波处理
	long int value = 0;
	int median[25] = { 0 };
	long bandSize = rowHeightY*colWidthX;
	for (int k = 0; k < bandNum; k++)
	{
		// 最外层两圈的像素不做处理
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

				//排序并求中值
				std::sort(median, median + 25);
				resFcube[k *bandSize + i *rowHeightY + j] = (unsigned char)median[12];
				value++;
				pProgressBar->setValue(value);
			}
		}
	}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("5×5中值滤波  "));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************频率域增强-FFT************************ //
void RSpractice::on_fft_freq() {
	//进行二维快速傅里叶变换，显示频谱图
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//FFT处理
	int value = 0;
	for (int b = 0; b < bandNum; b++) {
		Mat<double> mat(rowHeightY, colWidthX);
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				mat(i, j) = (double)(myFcube(i, j, b));
				//使变换后的傅里叶频谱中心化
				mat(i, j) = pow(-1, i + j) * mat(i, j);
			}
		//二维快速傅里叶变换
		cx_mat pfft = fft2(mat);

		//计算变换后的幅值
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//取绝对值
				double temp = abs(pfft(i, j));
				//幅值远大于255，为便于在窗口直接显示结果，计算e对数做拉伸
				resFcube(i, j, b) = 5 * logf(temp);
				value++;
				pProgressBar->setValue(value);
			}
	}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("FFT频谱图"));
	//数据转化为uint8便于图像输出
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************频率域增强-理想高通滤波器************************ //
void RSpractice::on_high_freq() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//计算频率
	Mat<double> D(rowHeightY, colWidthX);
	//中心计算
	float centerx = colWidthX / 2.0;
	float centery = rowHeightY / 2.0;
	for (int u = 0; u < rowHeightY; u++)
		for (int v = 0; v < colWidthX; v++)
			//D（u，v）公式计算
			D(u, v) = sqrt(pow(double(u - centery), 2) + pow(v - centerx, 2));

	//设置截止频率为10
	double D0 = 10;

	//进行处理
	int value = 0;
	for (int b = 0; b < bandNum; b++) {
		Mat<double> mat(rowHeightY, colWidthX);
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				double k = i + j;
				mat(i, j) = (double)(myFcube(i, j, b));
				//使变换后的傅里叶频谱中心化
				mat(i, j) = pow(-1, i + j) * mat(i, j);
			}

		//二维快速傅里叶变换
		cx_mat resfft = fft2(mat);

		//理想高通滤波
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//保留大于截止频率D0的值
				if (D(i, j) > D0)
					continue;
				else
					resfft(i, j) = 0;
			}

		//二维快速傅里叶反变换
		cx_mat pifft = ifft2(resfft);

		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//取正值
				float temp = abs(pifft(i, j));
				resFcube(i, j, b) = temp;
				value++;
				pProgressBar->setValue(value);
			}
	}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("理想高通滤波器"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************频率域增强-理想低通滤波器************************ //
void RSpractice::on_low_freq() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//计算频率
	Mat<double> D(rowHeightY, colWidthX);
	//中心计算
	float centerx = colWidthX / 2.0;
	float centery = rowHeightY / 2.0;
	for (int u = 0; u < rowHeightY; u++)
		for (int v = 0; v < colWidthX; v++)
			//D（u，v）公式计算
			D(u, v) = sqrt(pow(double(u - centery), 2) + pow(v - centerx, 2));

	//设置截止频率为50
	double D0 = 50;

	//进行处理
	int value = 0;
	for (int b = 0; b < bandNum; b++) {
		Mat<double> mat(rowHeightY, colWidthX);
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				double k = i + j;
				mat(i, j) = (double)(myFcube(i, j, b));
				//使变换后的傅里叶频谱中心化
				mat(i, j) = pow(-1, i + j) * mat(i, j);
			}

		//二维快速傅里叶变换
		cx_mat resfft = fft2(mat);

		//理想低通滤波
		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//保留小于等于截止频率D0的值
				if (D(i, j) <= D0)
					continue;
				else
					resfft(i, j) = 0;
			}

		//二维快速傅里叶反变换
		cx_mat pifft = ifft2(resfft);

		for (int i = 0; i < rowHeightY; i++)
			for (int j = 0; j < colWidthX; j++) {
				//取正值
				float temp = abs(pifft(i, j));
				resFcube(i, j, b) = temp;
				value++;
				pProgressBar->setValue(value);
			}
	}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("理想低通滤波器"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************云雾辐射校正-暗目标法************************ //
void RSpractice::on_dos_radiation() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//暗目标法去云雾处理
	long int value = 0;
	long bandSize = rowHeightY*colWidthX;
	for (int b = 0; b < bandNum; ++b)
	{
		//mat为第b个波段
		Mat<float> mat = resFcube.slice(b);
		//矩阵各行最小值
		fcolvec B = min(mat, 1);
		//波段中像素值最小的值
		float pmin = min(B);

		for (int i = 0; i < rowHeightY; ++i)
			for (int j = 0; j < colWidthX; ++j) {
				//减掉最小值
				resFcube[b*bandSize + i*colWidthX + j] = resFcube[b*bandSize + i*colWidthX + j] - pmin;
				value++;
				pProgressBar->setValue(value);
			}
	}

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("暗目标法去云雾"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************直方图均衡化************************ //
void RSpractice::on_equalization_histogram() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	
	fcube resFcube = myFcube / 2 + 10;

	int gray[256] = { 0 };//用来存储每个灰度值的像素个数
	double gray_prob[256] = { 0 };//用来存储每个灰度值的频率
	double gray_distribution[256] = { 0 };//存储累积分布
	int gray_equal[256] = { 0 };//存储均衡化后的灰度值
	int gray_sum = bandNum*rowHeightY*colWidthX;

	//统计每个灰度值的像素个数
	for (int b = 0; b < bandNum; ++b) {
		for (int i = 0; i < rowHeightY; ++i) {
			for (int j = 0; j < colWidthX; ++j) {
				int value = myFcube(i, j, b);
				gray[value]++;
			}
		}
	}

	//计算灰度频率
	for (int i = 0; i < 256; i++)
	{
		gray_prob[i] = ((double)gray[i] / gray_sum);
	}

	//计算累计分布
	gray_distribution[0] = gray_prob[0];
	for (int i = 1; i < 256; i++)
	{
		gray_distribution[i] = gray_distribution[i - 1] + gray_prob[i];
	}

	//计算均衡化后的灰度值，四舍五入
	for (int i = 0; i < 256; i++)
	{
		gray_equal[i] = (uchar)(255 * gray_distribution[i] + 0.5);
	}

	//均衡化后每个像素灰度值
	for (int b = 0; b < bandNum; ++b) {
		for (int i = 0; i < rowHeightY; ++i) {
			for (int j = 0; j < colWidthX; ++j) {
				int value = myFcube(i, j, b);
				resFcube(i, j, b) = gray_equal[value];
			}
		}
	}
	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("直方图均衡化"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************空间域锐化-拉普拉斯算子************************ //
void RSpractice::on_laplace_sharpen() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	//拉普拉斯算子空间域锐化，最外一圈像素不处理
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("拉普拉斯"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************空间域锐化-交叉微分算子************************ //
void RSpractice::on_roberts_sharpen() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	
	//交叉微分算法进行空间域锐化，最后一行和最后一列像素不处理
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("交叉微分"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************灰度变换-2%线性拉伸************************ //
void RSpractice::on_linear2_greyTrans() {
	//线性2%拉伸，将图像灰度2%~98%的部分拉伸到0~255
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//灰度变换
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("2%线性拉伸"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************灰度变换-最大最小拉伸************************ //
void RSpractice::on_maxmin_greyTrans() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;

	//灰度变换
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("最大最小拉伸"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************灰度变换-对数变换************************ //
void RSpractice::on_log_greyTrans() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
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
	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("对数变换"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************灰度变换-图像反转************************ //
void RSpractice::on_reverse_greyTrans() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
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
	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("图像反转"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************灰度变换-指数变换************************ //
void RSpractice::on_exp_greyTrans() {
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
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
	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("指数变换"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************地物提取-植被************************ //
void RSpractice::on_vegetation_extract() {
	//基于NDVI指数，二值化后提取植被
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
	fcube myFcube(originalData, rowHeightY, colWidthX, bandNum);
	fcube resFcube = myFcube;
	//提取处理
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("植被提取"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************地物提取-水体************************ //
void RSpractice::on_water_extract() {
	//基于NDWI指数，二值化后提取水体
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("水体提取"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

// **************地物提取-建筑********************** //
void RSpractice::on_buildings_extract() {
	//基于NDBI指数，二值化后提取建筑
	if (originalData == NULL)
		return;
	// 将像素数组初始化为ammadillo中的多维矩阵数据结构
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

	//显示图像
	display(resFcube.memptr());
	output_name->setText(QStringLiteral("建筑提取"));
	Cube<unsigned char>mss = conv_to<Cube<unsigned char>>::from(resFcube);
	storageData(mss.memptr());
}

