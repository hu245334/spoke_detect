#include "De_Extraction.h"
#include "RemoveSmallRegion.h"
using namespace std;
using namespace cv;

void De_imageblur(Mat& src, Mat& dst, Size size, int threshold){
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



//MsrcIMG是经过灰度拉伸的感兴趣区域，srcImage是经过提高锐化后的图像
void De_extraction(cv::Mat& srcImage, cv::Mat& dstImage, Rect rect, Point *point){
	//对ROI(矩形)图像进行中值滤波，此处可以先进行灰度变换，从而优化结果
	Mat edges;
	Canny(srcImage, edges, 32, 200, 3);
	//imshow("边缘曲线", edges);
	Mat thre_IMG1;
	edges.copyTo(thre_IMG1);
	//找出缺陷的相对位置，暂时默认在左1/3，中间1/2
	int width = rect.width;
	int height = rect.height;
	//小ROI区域的左上角以及右下角
	Point small_left_up = Point(rect.x, rect.y );
	Point small_right_down = Point(int(rect.x + width / 3), rect.y + height);
	for (int y = 0; y < thre_IMG1.rows; y++){
		for (int x = 0; x < thre_IMG1.cols; x++){
			//清除其他区域的边缘
			if (x>=small_left_up.x&&x<=small_right_down.x&&y>=small_left_up.y&&y<=small_right_down.y){
			}
			else{
				thre_IMG1.at<uchar>(y, x) = 0;
			}
		}
	}
	//imshow("清除其它区域后的边缘", thre_IMG1);
	Mat ele = getStructuringElement(MORPH_CROSS, Size(3, 3));
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	dilate(thre_IMG1, thre_IMG1, ele);//膨胀
	//imshow("清除并膨胀后的图像", thre_IMG1);

	Mat fill_edges;
	thre_IMG1.copyTo(fill_edges);
	Size m_Size = thre_IMG1.size();
	Mat temimage = Mat::zeros(m_Size.height + 2, m_Size.width + 2, thre_IMG1.type());//延展图像
	thre_IMG1.copyTo(temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
	floodFill(temimage, Point(0, 0), Scalar(255));
	Mat cutImg;//裁剪延展的图像
	temimage(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
	fill_edges = thre_IMG1 | (~cutImg);
	//imshow("填充图像", fill_edges);

	//五次腐蚀，再四次膨胀，得到最终的圈
	erode(fill_edges, fill_edges, ele);//腐蚀1
	erode(fill_edges, fill_edges, ele);//腐蚀2
	erode(fill_edges, fill_edges, ele);//腐蚀3
	erode(fill_edges, fill_edges, ele);//腐蚀4
	erode(fill_edges, fill_edges, ele);//腐蚀5
	erode(fill_edges, fill_edges, ele);//腐蚀6
	erode(fill_edges, fill_edges, ele);//腐蚀7
	erode(fill_edges, fill_edges, ele);//腐蚀8
	erode(fill_edges, fill_edges, ele);//腐蚀9
	erode(fill_edges, fill_edges, ele);//腐蚀10
	dilate(fill_edges, fill_edges, ele);//膨胀
	dilate(fill_edges, fill_edges, ele);//膨胀
	dilate(fill_edges, fill_edges, ele);//膨胀
	dilate(fill_edges, fill_edges, ele);//膨胀
	dilate(fill_edges, fill_edges, ele);//膨胀
	dilate(fill_edges, fill_edges, ele);//膨胀
	dilate(fill_edges, fill_edges, ele);//膨胀
	dilate(fill_edges, fill_edges, ele);//膨胀
	//imshow("处理后的图像", fill_edges);
	De_imageblur(fill_edges, fill_edges, Size(3, 3), 128);
	//imshow("边缘光滑后的图像", fill_edges);
	Canny(fill_edges, thre_IMG1, 15, 40, 3);
	//imshow("边缘图像", thre_IMG1);
	
	for (int i = 0; i < 9; i++){
		line(dstImage, point[i], point[i + 1], Scalar(0, 255, 0), 1, 8, 0);
	}
	line(dstImage, point[9], point[0], Scalar(0, 255, 0), 1, 8, 0);


	//画红色缺陷检测线,这个线需要在圆里面
	for (int y = 0; y < edges.rows; y++){
		for (int x = 0; x < edges.cols; x++){
			//当这个点是边缘线
			if (thre_IMG1.at<uchar>(y, x) == 255){
				dstImage.at<Vec3b>(y, x)[0] = 0;
				dstImage.at<Vec3b>(y, x)[1] = 0;
				dstImage.at<Vec3b>(y, x)[2] = 255;
			}
		}
	}
	imshow("目标图", dstImage);

	waitKey(0);
}