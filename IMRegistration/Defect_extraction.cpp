#include "Defect_extraction.h"
using namespace std;
using namespace cv;


void imageblur(Mat& src, Mat& dst, Size size, int threshold){
	int height = src.rows;
	int width = src.cols;
	blur(src, dst, size);
	for (int i = 0; i < height; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			if (p[j] < threshold)
				p[j] = 0;
			else p[j] = 255;
		}
	}
}

//对MsrcIMG即选中的矩形进行缺陷提取，最后画在org上显示并返回（同时圆也画出来）
//其中ROI_rect是感兴趣区域的圆心，width是感兴趣区域宽，local主要用到里面的左上角
void Defect_extraction(cv::Mat& MsrcIMG, cv::Mat& srcImage, cv::Mat& org, CvPoint& center, int width, cv::Point local){
	//对ROI(矩形)图像进行中值滤波，此处可以先进行灰度变换，从而优化结果
	Mat MID_IMG;
	medianBlur(MsrcIMG, MID_IMG, 15);
	Mat result;
	subtract(MsrcIMG, MID_IMG, result);
	Mat thre_IMG;
	threshold(result, thre_IMG, 4, 255, THRESH_BINARY);
	//imshow("相减后的图像", thre_IMG);
	//接下来对ROI区域里面的图像进行统计，或者进行形态学的变换
	Mat ele = getStructuringElement(MORPH_CROSS, Size(3, 3));
	Mat thre_IMG1;
	thre_IMG.copyTo(thre_IMG1);

	erode(thre_IMG1, thre_IMG1, ele);//腐蚀
	//imshow("腐蚀后的图像", thre_IMG1);
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	//imshow("腐蚀膨胀后的图像", thre_IMG1);
	//遍历所有像素点，筛除掉上面腐蚀膨胀后没有的地方
	for (int y = 0; y < thre_IMG1.rows; y++){
		for (int x = 0; x < thre_IMG1.cols; x++){
			for (int c = 0; c < 3; c++){
				if (thre_IMG1.at<Vec3b>(y, x)[c] == 0){
					thre_IMG.at<Vec3b>(y, x)[c] = 0;
				}
			}
		}
	}
	thre_IMG.copyTo(thre_IMG1);
	//三次膨胀，五次腐蚀，再四次膨胀，得到最终的圈
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	erode(thre_IMG1, thre_IMG1, ele);//腐蚀
	erode(thre_IMG1, thre_IMG1, ele);//腐蚀
	erode(thre_IMG1, thre_IMG1, ele);//腐蚀
	erode(thre_IMG1, thre_IMG1, ele);//腐蚀
	erode(thre_IMG1, thre_IMG1, ele);//腐蚀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	//imshow("处理后的图像", thre_IMG1);
	imageblur(thre_IMG1, thre_IMG1, Size(3, 3), 128);
	//imshow("边缘光滑后的图像", thre_IMG1);

	Mat edges;
	Canny(thre_IMG1, edges, 15, 40, 3);
	//imshow("边缘图像", edges);

	srcImage.copyTo(org);
	IplImage psrcImg = org;
	IplImage* srcImg = &psrcImg;
	int radius = width / 2;//HJ:计算半径
	cvCircle(srcImg, center, radius, CV_RGB(0, 255, 150), 2, CV_AA, 0);
	//画红色缺陷检测线,这个线需要在圆里面
	for (int y = 0; y < edges.rows; y++){
		for (int x = 0; x < edges.cols; x++){
			//当这个点是边缘线
			if (edges.at<uchar>(y, x) == 255){
				Point True_coord((x + local.x - 1), (y + local.y - 1));//真实的坐标
				//判断这个点在圆中,即到圆心的距离小于等于半径,这个圆心应该是校准之后的圆心
				int dis = int(sqrt(double(True_coord.x - center.x)*(True_coord.x - center.x) + \
					(True_coord.y - center.y)*(True_coord.y - center.y)));//HJ:计算半径
				if (dis <= radius){
					org.at<Vec3b>(True_coord.y, True_coord.x)[0] = 0;
					org.at<Vec3b>(True_coord.y, True_coord.x)[1] = 0;
					org.at<Vec3b>(True_coord.y, True_coord.x)[2] = 255;
				}
			}
		}
	}
	imshow("目标图", org);

	waitKey(0);
}