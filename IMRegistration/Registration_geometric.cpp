#include "Registration_geometric.h"
using namespace std;
using namespace cv;

//用几何法求解srcImg中在local（width,height）感兴趣区域中半径为r的圆的圆心，并返回圆心坐标
Point geometricTrans_r(Mat &srcImg, Mat &dst, Mat &temp, float r, Point local, int width, int height){
	Point find;
	dst = srcImg(Rect(local.x, local.y, width, height));
	cvtColor(dst, dst, CV_BGR2GRAY);//三通道的图转化为1通道的灰度图
	threshold(dst, dst, 90, 255, THRESH_BINARY);//通过阈值操作把灰度图变成二值图

	int midx = dst.cols / 2;//列
	int midy = dst.rows / 2;//行
	midx = midx + local.x + 1;
	midy = midy + local.y + 1;

	srcImg.copyTo(dst);
	cvtColor(dst, dst, CV_BGR2GRAY);//三通道的图转化为1通道的灰度图
	threshold(dst, dst, 90, 255, THRESH_BINARY);//通过阈值操作把灰度图变成二值图

	int remark[4][2];
	//左
	for (int i = midx; i >= 0; i--){
		if (dst.at<uchar>(midy, i) == 0){
			remark[0][0] = midy;//行
			remark[0][1] = i + 1;
			break;
		}
	}
	//右
	for (int i = midx; i <= dst.cols; i++){
		if (dst.at<uchar>(midy, i) == 0){
			remark[1][0] = midy;//行
			remark[1][1] = i - 1;
			break;
		}
	}
	//上
	for (int j = midy; j >= 0; j--){
		if (dst.at<uchar>(j, midx) == 0){
			remark[2][0] = j + 1;//行
			remark[2][1] = midx;
			break;
		}
	}
	//下
	for (int j = midy; j <= dst.rows; j++){
		if (dst.at<uchar>(j, midx) == 0){
			remark[3][0] = j - 1;//行
			remark[3][1] = midx;
			break;
		}
	}

	int gz = (remark[0][1] + remark[1][1]) / 2;//行
	int hz = (remark[2][0] + remark[3][0]) / 2;//列
	find.x = gz;
	find.y = hz;

	srcImg.copyTo(temp);
	IplImage tmp = IplImage(temp);
	CvArr* arr = (CvArr*)&tmp;
	cvCircle(arr, cvPoint(gz, hz), r, cvScalar(0, 255, 0), 1, 8, 0);
	imshow("dst", temp);
	cvWaitKey(0);

	return find;
}