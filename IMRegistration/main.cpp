#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>
#include <cstring>        // for strcat()
#include <io.h>
#include <direct.h>
#include "Entropy.h"
#include "De_Extraction.h"

using namespace std;
using namespace cv;


Mat org, img, dst;
Point point[10] = { (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1), (-1, -1) };//总计在图中标记10个浮动点
double alpha = 1.1; /*duibidu */
int beta = -70;  /*liangdu*/
// 亮度对比度公式：g = alpha*f+beta ，g为处理后的像素值，f为原像素值

//打开图片的路径
char path[100] = "PHTO\\*";
char openpath[100];
_finddata_t findData;

void on_mouse(int event, int x, int y, int flags, void *ustc)//flags代表鼠标拖拽事件
{
	static Point pre_pt = Point(-1, -1);
	static Point cur_pt = Point(-1, -1);
	static int idx = 0;
	static int mark = 0;
	if (event == CV_EVENT_LBUTTONDOWN)//按下左键
	{
		org.copyTo(img);
		pre_pt = Point(x, y);
		cur_pt = Point(x, y);
		if (idx<10){
			point[idx] = Point(x, y);
			idx++;
		}
	}
	else if (event == CV_EVENT_MOUSEMOVE)//鼠标移动
	{
		if (idx < 10 && idx>0){//鼠标还没有
			org.copyTo(img);
			cur_pt = Point(x, y);
			for (int i = 0; i < idx - 1; i++){
				line(img, point[i], point[i + 1], Scalar(0, 255, 0), 1, 8, 0);
			}
			line(img, point[idx - 1], cur_pt, Scalar(0, 255, 0), 1, 8, 0);
			imshow("img", img);
		}
	}
	else if (event == CV_EVENT_LBUTTONUP)//放开左键
	{
		org.copyTo(img);
		pre_pt = Point(x, y);
		cur_pt = Point(x, y);
		for (int i = 0; i < idx - 1; i++){
			line(img, point[i], point[i + 1], Scalar(0, 255, 0), 1, 8, 0);
		}
		if (idx >9){//已经有10个点了
			line(img, point[9], point[0], Scalar(0, 255, 0), 1, 8, 0);
		}
		imshow("img", img);
	}
	if (idx > 9){
		if (0 == mark){
			mark = 1;
		}
	}
}



int main(){
	//输入图像
	int mark = 0;//标记是否是第一张图片
	intptr_t handle;

	//使用互信息进行配准代码
	//在path目录下新建mutual_info文件夹
	strcpy_s(openpath, path);
	openpath[strlen(openpath) - 1] = '\0';
	strcat_s(openpath, "mutual_info\\");
	_mkdir(openpath);

	handle = _findfirst(path, &findData);    // 查找目录中的第一个文件
	if (handle == -1){
		cout << "打开第一个文件失败!\n";
		return -1;
	}

	Point left_up, right_down;
	Rect rect;
	Mat image_roi;
	double image_roi_entropy;
	do{
		if (findData.attrib & _A_SUBDIR){//是否是子目录包括"."和".."
			cout << findData.name << "\t<dir>\n";
		}
		else{
			strcpy_s(openpath, path);
			openpath[strlen(openpath) - 1] = '\0';
			strcat_s(openpath, findData.name);
			if (0 == mark){
				mark = 1;//已经遍历第一张图片
				cout << openpath << endl;
				Mat srcImage = imread(openpath);//输入图像地址
				//imshow("srcImage", srcImage); //显示原图

				//建立用于存放对比度和亮度变化后的图像
				Mat ThresholdImage = Mat::zeros(srcImage.size(), srcImage.type());

				//遍历所有像素点，改变亮度和对比度
				for (int y = 0; y < srcImage.rows; y++)
				{
					for (int x = 0; x < srcImage.cols; x++)
					{
						for (int c = 0; c < 3; c++)
						{
							ThresholdImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(srcImage.at<Vec3b>(y, x)[c]) + beta);
						}
					}
				}

				Mat grayImage;
				//cvtColor(ThresholdImage, grayImage, COLOR_BGR2GRAY);   //转化灰度图像
				GaussianBlur(ThresholdImage, grayImage, Size(3, 3), 1);  //高斯滤波， 去噪点和平滑操作

				//定义锐化核
				Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);

				Mat dstImage;
				//锐化，提高边缘细节
				filter2D(grayImage, dstImage, grayImage.depth(), kernel);

				//建立用于存放对比度和亮度变化后的图像
				dstImage.copyTo(org);
				namedWindow("img");
				setMouseCallback("img", on_mouse, 0);
				imshow("img", org);

				//等待画图完整
				while (1)
				{
					int key;
					key = waitKey(20);
					if (char(key) == 27)
					{
						break;
					}
				}
				
				left_up = point[0];
				right_down = point[0];
				for (int i = 1; i < 10; i++){
					if (point[i].x < left_up.x){
						left_up.x = point[i].x;
					}
					if (point[i].y < left_up.y){
						left_up.y = point[i].y;
					}
					if (point[i].x>right_down.x){
						right_down.x = point[i].x;
					}
					if (point[i].y>right_down.y){
						right_down.y = point[i].y;
					}
				}
				rect = Rect(left_up.x, left_up.y, right_down.x - left_up.x, right_down.y - left_up.y);
				dstImage(rect).copyTo(image_roi);

				image_roi_entropy = Entropy(image_roi);

				Mat tmp_image_roi;
				//在原图上进行感兴趣区域
				srcImage(rect).copyTo(tmp_image_roi);
				////找出ROI区域后，对ROI区域（矩形）进行对比度增强，通过灰度拉伸来实现
				int POINT_MAX[3] = { tmp_image_roi.at<Vec3b>(0, 0)[0], tmp_image_roi.at<Vec3b>(0, 0)[1], tmp_image_roi.at<Vec3b>(0, 0)[2] };
				int POINT_MIN[3] = { tmp_image_roi.at<Vec3b>(0, 0)[0], tmp_image_roi.at<Vec3b>(0, 0)[1], tmp_image_roi.at<Vec3b>(0, 0)[2] };
				for (int y = 0; y < tmp_image_roi.rows; y++){
					for (int x = 0; x < tmp_image_roi.cols; x++){
						for (int c = 0; c < 3; c++){
							if (POINT_MAX[c] < tmp_image_roi.at<Vec3b>(y, x)[c]){
								POINT_MAX[c] = tmp_image_roi.at<Vec3b>(y, x)[c];
							}
							if (POINT_MIN[c]>tmp_image_roi.at<Vec3b>(y, x)[c]){
								POINT_MIN[c] = tmp_image_roi.at<Vec3b>(y, x)[c];
							}
						}
					}
				}
				//cout << "POINT_MIN=" << POINT_MIN[0] << "  " << POINT_MIN[1] << "  " << POINT_MIN[2] << endl;
				//cout << "POINT_MAX=" << POINT_MAX[0] << "  " << POINT_MAX[1] << "  " << POINT_MAX[2] << endl;
				dstImage.copyTo(org);
				for (int y = 0; y < srcImage.rows; y++){
					for (int x = 0; x < srcImage.cols; x++){
						for (int c = 0; c < 3; c++){
							//大图中只对感兴趣区域进行拉伸
							org.at<Vec3b>(y, x)[c] = 255 * (srcImage.at<Vec3b>(y, x)[c] - POINT_MIN[c]) / (POINT_MAX[c] - POINT_MIN[c]);
							if (org.at<Vec3b>(y, x)[c]>255){
								org.at<Vec3b>(y, x)[c] = 255;
							}
							else if (org.at<Vec3b>(y, x)[c] < 0){
								org.at<Vec3b>(y, x)[c] = 0;
							}
						}
					}
				}

				//遍历所有像素点，改变亮度和对比度
				for (int y = 0; y < org.rows; y++)
				{
					for (int x = 0; x < org.cols; x++)
					{
						for (int c = 0; c < 3; c++)
						{
							ThresholdImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(org.at<Vec3b>(y, x)[c]) + beta);
						}
					}
				}

				//cvtColor(ThresholdImage, grayImage, COLOR_BGR2GRAY);   //转化灰度图像
				GaussianBlur(ThresholdImage, grayImage, Size(3, 3), 1);  //高斯滤波， 去噪点和平滑操作
				filter2D(grayImage, org, grayImage.depth(), kernel);//锐化，提高边缘细节

				De_extraction(org, dstImage, rect, point);

				//变换成PHTO\\mutual_info\\A008轮辐-18.bmp
				strcpy_s(openpath, path);
				openpath[strlen(openpath) - 1] = '\0';
				strcat_s(openpath, "mutual_info\\");
				strcat_s(openpath, findData.name);

				imwrite(openpath, dstImage);
			}
			else{
				cout << openpath << endl;
				Mat IMG = imread(openpath);//输入图像地址

				IMG.copyTo(org);
				Mat ThresholdImage = Mat::zeros(org.size(), org.type());
				//遍历所有像素点，改变亮度和对比度
				for (int y = 0; y < org.rows; y++){
					for (int x = 0; x < org.cols; x++){
						for (int c = 0; c < 3; c++){
							ThresholdImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(org.at<Vec3b>(y, x)[c]) + beta);
						}
					}
				}
				Mat grayImage;
				//cvtColor(ThresholdImage, grayImage, COLOR_BGR2GRAY); //转化灰度图像
				GaussianBlur(ThresholdImage, grayImage, Size(3, 3), 1);  //高斯滤波， 去噪点和平滑操作
				Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);//定义锐化核
				Mat dstIMG;
				filter2D(grayImage, dstIMG, grayImage.depth(), kernel);//锐化，提高边缘细节
				//cout << image_roi.rows << "   " << image_roi.cols << endl;

				dstIMG.copyTo(org);
				for (int i = 0; i < 9; i++){
					line(org, point[i], point[i + 1], Scalar(0, 255, 0), 1, 8, 0);
				}
				line(org, point[9], point[0], Scalar(0, 255, 0), 1, 8, 0);

				//变换成IMG\\Hough\\A009轮心-*.bmp
				strcpy_s(openpath, path);
				openpath[strlen(openpath) - 1] = '\0';
				strcat_s(openpath, "mutual_info\\");
				strcat_s(openpath, findData.name);
				imwrite(openpath, org);
				imshow("org", org);
				cvWaitKey(0);
				cout << "开始配准" << endl;
				

				double *MI = new double[(dstIMG.rows - image_roi.rows + 1)*(dstIMG.cols - image_roi.cols + 1)];
				for (int j = 0; j <= (dstIMG.rows - image_roi.rows); j++){
					for (int i = 0; i <= (dstIMG.cols - image_roi.cols); i++){
						rect = Rect(i, j, image_roi.cols, image_roi.rows);
						Mat ROI = dstIMG(rect);
						MI[j*(dstIMG.cols - image_roi.cols) + i] = ComEntropy(image_roi, ROI, image_roi_entropy);
					}
				}

				//找出互信息最大的坐标，说明其最匹配
				double MAX = MI[0];
				Point PMAX = (0, 0);
				for (int j = 0; j <= (dstIMG.rows - image_roi.rows); j++){
					for (int i = 0; i <= (dstIMG.cols - image_roi.cols); i++){
						if (MAX < MI[j*(dstIMG.cols - image_roi.cols) + i]){
							MAX = MI[j*(dstIMG.cols - image_roi.cols) + i];
							PMAX.x = i;
							PMAX.y = j;
						}
					}
				}

				Point dis = PMAX - left_up;

				Point tmp_point[10];
				for (int i = 0; i < 10; i++){
					tmp_point[i] = point[i] + dis;
				}
				Rect tmp_rect = Rect(left_up.x + dis.x, left_up.y + dis.y, right_down.x - left_up.x, right_down.y - left_up.y);
				Mat tmp_image_roi;
				//在原图上进行感兴趣区域
				IMG(tmp_rect).copyTo(tmp_image_roi);
				////找出ROI区域后，对ROI区域（矩形）进行对比度增强，通过灰度拉伸来实现
				int POINT_MAX[3] = { tmp_image_roi.at<Vec3b>(0, 0)[0], tmp_image_roi.at<Vec3b>(0, 0)[1], tmp_image_roi.at<Vec3b>(0, 0)[2] };
				int POINT_MIN[3] = { tmp_image_roi.at<Vec3b>(0, 0)[0], tmp_image_roi.at<Vec3b>(0, 0)[1], tmp_image_roi.at<Vec3b>(0, 0)[2] };
				for (int y = 0; y < tmp_image_roi.rows; y++){
					for (int x = 0; x < tmp_image_roi.cols; x++){
						for (int c = 0; c < 3; c++){
							if (POINT_MAX[c] < tmp_image_roi.at<Vec3b>(y, x)[c]){
								POINT_MAX[c] = tmp_image_roi.at<Vec3b>(y, x)[c];
							}
							if (POINT_MIN[c]>tmp_image_roi.at<Vec3b>(y, x)[c]){
								POINT_MIN[c] = tmp_image_roi.at<Vec3b>(y, x)[c];
							}
						}
					}
				}
				//cout << "POINT_MIN=" << POINT_MIN[0] << "  " << POINT_MIN[1] << "  " << POINT_MIN[2] << endl;
				//cout << "POINT_MAX=" << POINT_MAX[0] << "  " << POINT_MAX[1] << "  " << POINT_MAX[2] << endl;

				dstIMG.copyTo(org);
				for (int y = 0; y < IMG.rows; y++){
					for (int x = 0; x < IMG.cols; x++){
						for (int c = 0; c < 3; c++){
							//大图中只对感兴趣区域进行拉伸
							org.at<Vec3b>(y, x)[c] = 255 * (IMG.at<Vec3b>(y, x)[c] - POINT_MIN[c]) / (POINT_MAX[c] - POINT_MIN[c]);
							if (org.at<Vec3b>(y, x)[c]>255){
								org.at<Vec3b>(y, x)[c] = 255;
							}
							else if (org.at<Vec3b>(y, x)[c] < 0){
								org.at<Vec3b>(y, x)[c] = 0;
							}
						}
					}
				}

				//遍历所有像素点，改变亮度和对比度
				for (int y = 0; y < org.rows; y++)
				{
					for (int x = 0; x < org.cols; x++)
					{
						for (int c = 0; c < 3; c++)
						{
							ThresholdImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(org.at<Vec3b>(y, x)[c]) + beta);
						}
					}
				}
				//cvtColor(ThresholdImage, grayImage, COLOR_BGR2GRAY);   //转化灰度图像
				GaussianBlur(ThresholdImage, grayImage, Size(3, 3), 1);  //高斯滤波， 去噪点和平滑操作

				//锐化，提高边缘细节
				filter2D(grayImage, org, grayImage.depth(), kernel);

				De_extraction(org, dstIMG, tmp_rect, tmp_point);

				//变换成PHTO\\mutual_info\\A009轮心-*.bmp
				strcpy_s(openpath, path);
				openpath[strlen(openpath) - 1] = '\0';
				strcat_s(openpath, "mutual_info\\");
				strcat_s(openpath, findData.name);
				openpath[strlen(openpath) - 4] = '\0';
				strcat_s(openpath, "_配准图.bmp");
				imwrite(openpath, dstIMG);
				cout << "配准完成，开始下一幅" << endl;

			}
		}
	} while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件

	_findclose(handle);    // 关闭搜索句柄

	system("pause");
	return 0;
}
