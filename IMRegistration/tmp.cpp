//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>    
//#include <opencv2/highgui/highgui.hpp>
//#include <cstring>
//#include "Entropy.h"
//
//using namespace std;
//using namespace cv;
//
//Mat org, img, dst;
//Mat srcImage, grayImage, dstImage;
//
//int alpha = 1.1; /*duibidu */
//int beta = -70;  /*liangdu*/
//// 亮度对比度公式：g = alpha*f+beta ，g为处理后的像素值，f为原像素值
//
//Point point[10] = { (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1) };//总计在图中标记10个浮动点
//Point local[2] = { (-1, -1), (-1, -1) };
//int width, height;
//
//void on_mouse(int event, int x, int y, int flags, void *ustc)//flags代表鼠标拖拽事件
//{
//	static Point pre_pt = Point(-1, -1);
//	static Point cur_pt = Point(-1, -1);
//	static int idx = 0;
//	static int mark = 0;
//	if (event == CV_EVENT_LBUTTONDOWN)//按下左键
//	{
//		org.copyTo(img);
//		pre_pt = Point(x, y);
//		cur_pt = Point(x, y);
//		if (idx<10){
//			point[idx] = Point(x, y);
//			idx++;
//		}
//	}
//	else if (event == CV_EVENT_MOUSEMOVE)//鼠标移动
//	{
//		if (idx < 10 && idx>0){//鼠标还没有
//			org.copyTo(img);
//			cur_pt = Point(x, y);
//			for (int i = 0; i < idx - 1; i++){
//				line(img, point[i], point[i + 1], Scalar(0, 255, 0), 1, 8, 0);
//			}
//			line(img, point[idx - 1], cur_pt, Scalar(0, 255, 0), 1, 8, 0);
//			imshow("img", img);
//		}
//	}
//	else if (event == CV_EVENT_LBUTTONUP)//放开左键
//	{
//		org.copyTo(img);
//		pre_pt = Point(x, y);
//		cur_pt = Point(x, y);
//		for (int i = 0; i < idx - 1; i++){
//			line(img, point[i], point[i + 1], Scalar(0, 255, 0), 1, 8, 0);
//		}
//		if (idx >9){//已经有10个点了
//			line(img, point[9], point[0], Scalar(0, 255, 0), 1, 8, 0);
//
//			//求出10个点包围的范围的最大最小值
//			Point min, max;
//			for (int i = 0; i < 10; i++){
//				if (0 == i){
//					max.x = min.x = point[i].x;
//					max.y = min.y = point[i].y;
//				}
//				else
//				{
//					if (max.x < point[i].x){
//						max.x = point[i].x;
//					}
//					if (min.x > point[i].x){
//						min.x = point[i].x;
//					}
//					if (max.y < point[i].y){
//						max.y = point[i].y;
//					}
//					if (min.y > point[i].y){
//						min.y = point[i].y;
//					}
//				}
//			}
//
//			//将范围分别向外扩充一点
//			if (min.x - 3 > 0){
//				min.x = min.x - 3;
//			}
//			else{
//				min.x = 0;
//			}
//			if (min.y - 3 > 0){
//				min.y = min.y - 3;
//			}
//			else{
//				min.y = 0;
//			}
//			if (max.x + 3 < org.cols){
//				max.x = max.x + 3;
//			}
//			else{
//				max.x = org.cols - 1;
//			}
//			if (max.y + 3 < org.rows){
//				max.y = max.y + 3;
//			}
//			else{
//				max.y = org.rows - 1;
//			}
//
//			width = abs(max.x - min.x);
//			height = abs(max.y - min.y);
//			if (width == 0 || height == 0)
//			{
//				return;
//			}
//			dst = org(Rect(min.x, min.y, width, height));
//			local[0].x = min.x;
//			local[0].y = min.y;
//		}
//		imshow("img", img);
//	}
//	if (idx > 9){
//		if (0 == mark){
//			org.copyTo(img);
//			for (int i = 0; i < idx - 1; i++){
//				line(img, point[i], point[i + 1], Scalar(0, 255, 0), 1, 8, 0);
//			}
//			line(img, point[9], point[0], Scalar(0, 255, 0), 1, 8, 0);
//
//
//			mark = 1;
//		}
//	}
//}
//
//int main()
//{
//	//输入图像
//	srcImage = imread("PHTO\\Hough\\A008轮辐-18.bmp");//输入图像地址
//
//	//建立用于存放对比度和亮度变化后的图像
//	Mat ThresholdImage = Mat::zeros(srcImage.size(), srcImage.type());
//
//	//遍历所有像素点，改变亮度和对比度
//	for (int y = 0; y < srcImage.rows; y++)
//	{
//		for (int x = 0; x < srcImage.cols; x++)
//		{
//			for (int c = 0; c < 3; c++)
//			{
//				ThresholdImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(srcImage.at<Vec3b>(y, x)[c]) + beta);
//			}
//		}
//	}
//
//	cvtColor(ThresholdImage, grayImage, COLOR_BGR2GRAY); //转化灰度图像
//	GaussianBlur(grayImage, grayImage, Size(3, 3), 1);  //高斯滤波， 去噪点和平滑操作
//
//	//定义锐化核
//	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
//
//	//锐化，提高边缘细节
//	filter2D(grayImage, dstImage, grayImage.depth(), kernel);
//
//
//
//	//建立用于存放对比度和亮度变化后的图像
//	dstImage.copyTo(org);
//	org.copyTo(img);
//	namedWindow("img");
//	setMouseCallback("img", on_mouse, 0);
//	imshow("img", org);
//	//等待画图完整
//	while (1)
//	{
//		int key;
//		key = waitKey(20);
//		if (char(key) == 27)
//		{
//			break;
//		}
//	}
//
//	Point left_up, right_down;
//	left_up = point[0];
//	right_down = point[0];
//	for (int i = 1; i < 10; i++){
//		if (point[i].x < left_up.x){
//			left_up.x = point[i].x;
//		}
//		if (point[i].y < left_up.y){
//			left_up.y = point[i].y;
//		}
//		if (point[i].x>right_down.x){
//			right_down.x = point[i].x;
//		}
//		if (point[i].y>right_down.y){
//			right_down.y = point[i].y;
//		}
//	}
//	Rect rect(left_up.x,left_up.y,right_down.x-left_up.x, right_down.y-left_up.y);
//	Mat image_roi = dstImage(rect);
//
//
//
//	Mat IMG = imread("PHTO\\Hough\\A008轮辐-2.bmp");//输入图像地址
//	ThresholdImage = Mat::zeros(IMG.size(), IMG.type());
//	//遍历所有像素点，改变亮度和对比度
//	for (int y = 0; y < IMG.rows; y++){
//		for (int x = 0; x < IMG.cols; x++){
//			for (int c = 0; c < 3; c++){
//				ThresholdImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(IMG.at<Vec3b>(y, x)[c]) + beta);
//			}
//		}
//	}
//	cvtColor(ThresholdImage, grayImage, COLOR_BGR2GRAY); //转化灰度图像
//	GaussianBlur(grayImage, grayImage, Size(3, 3), 1);  //高斯滤波， 去噪点和平滑操作
//	kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
//	Mat dstIMG;
//	filter2D(grayImage, dstIMG, grayImage.depth(), kernel);
//
//
//	double *MI = new double[(dstIMG.rows - image_roi.rows + 1)*(dstIMG.cols - image_roi.cols + 1)];
//	for (int j = 0; j <= (dstIMG.rows - image_roi.rows); j++){
//		for (int i = 0; i <= (dstIMG.cols - image_roi.cols); i++){
//			rect=Rect(i, j, image_roi.cols, image_roi.rows);
//			Mat ROI = dstIMG(rect);
//			MI[j*(dstIMG.cols - image_roi.cols) + i] = ComEntropy(image_roi, ROI);
//		}
//	}
//
//
//	//找出互信息最大的坐标，说明其最匹配
//	double MAX = MI[0];
//	Point PMAX = (0, 0);
//	for (int j = 0; j <= (dstIMG.rows - image_roi.rows); j++){
//		for (int i = 0; i <= (dstIMG.cols - image_roi.cols); i++){
//			if (MAX < MI[j*(dstIMG.cols - image_roi.cols) + i]){
//				MAX = MI[j*(dstIMG.cols - image_roi.cols) + i];
//				PMAX.x = i;
//				PMAX.y = j;
//			}
//		}
//	}
//
//	Point dis=PMAX-left_up;
//	
//	for (int i = 0; i < 9; i++){
//		line(IMG, point[i] + dis, point[i + 1] + dis, Scalar(0, 255, 0), 1, 8, 0);
//	}
//	line(IMG, point[9] + dis, point[0] + dis, Scalar(0, 255, 0), 1, 8, 0);
//	imshow();
//	rect = Rect(PMAX.x, PMAX.y, image_roi.cols, image_roi.rows);
//	Mat ROI = dstIMG(rect);
//
//	imshow("最匹配", ROI);
//
//
//
//	// ESC->EXIT
//	while (1)
//	{
//		int key;
//		key = waitKey(20);
//		if (char(key) == 27)
//		{
//			break;
//		}
//	}
//
//	return 0;
//}
//
//
//
//
//
//
