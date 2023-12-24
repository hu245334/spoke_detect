#include"Registration_Hough.h"
using namespace std;
using namespace cv;

#define TO_BE_BLACK 40 

//使用Hough方法求得src图像中半径为r的圆的圆心，并返回圆心坐标
Point houghTrans_r(IplImage *src, IplImage *dst, IplImage *tmp, float r, int xstep, int ystep)
{
	int width = src->width;
	int height = src->height;

	int channel = src->nChannels;
	int xmax = width%xstep ? width / xstep + 1 : width / xstep;
	int ymax = height%ystep ? height / ystep + 1 : height / ystep;

	int i, j, x, y;
	int **para = new int*[width];
	for (int i = 0; i < width; i++){
		para[i] = new int[height];

	}

	//赋值
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			para[i][j] = 0;
		}
	}

	//i,j are in the pixel space  
	//x,y are in the parameter space  
	for (j = 0; j<height; j++)
	{
		uchar* pin = (uchar*)(src->imageData + j*src->widthStep);
		for (i = 0; i<width; i++)
		{
			//pixel is black  
			if (pin[channel*i] < TO_BE_BLACK)
			{
				float temp;

				//calculate every probable y-cord based on x-cord  
				for (x = 0; x<xmax; x++)
				{
					temp = r*r - (i - x*xstep)*(i - x*xstep);
					temp = sqrt(temp);

					y = j - (int)temp;
					if (y >= 0 && y<height){
						para[x][y / ystep]++;
					}

					y = j + (int)temp;
					if (y >= 0 && y<height){
						para[x][y / ystep]++;
					}
				}
			}
		}
	}
	//find circle in parameter space  
	int paramax = 0;
	Point find = (-1, -1);
	for (y = 0; y<ymax; y++)
	{
		for (x = 0; x<xmax; x++)
		{
			if (para[x][y] > paramax)
			{
				paramax = para[x][y];
				find.x = x;
				find.y = y;
			}
		}
	}

	//draw the parameter space image  
	int ii, jj;
	for (y = 0; y<ymax; y++)
	{
		uchar* pout = (uchar*)(tmp->imageData + y*tmp->widthStep);
		for (x = 0; x<xmax; x++)
		{
			pout[channel*x] = para[x][y] * 255 / paramax;
			pout[channel*x + 1] = para[x][y] * 255 / paramax;
			pout[channel*x + 2] = para[x][y] * 255 / paramax;
		}
	}

	//draw the found circle  
	if (find.x >= 0 && find.y >= 0)
	{
		for (j = 0; j<height; j++)
		{
			uchar* pin = (uchar*)(src->imageData + j*src->widthStep);
			uchar* pout = (uchar*)(dst->imageData + j*dst->widthStep);
			for (i = 0; i<width; i++)
			{
				pout[3 * i] = pin[3 * i];
				pout[3 * i + 1] = pin[3 * i + 1];
				pout[3 * i + 2] = pin[3 * i + 2];
			}
		}
		cvCircle(dst, cvPoint(find.x*xstep + xstep / 2.0, find.y*ystep + ystep / 2.0), r, cvScalar(0, 255, 0), 1, 8, 0);
	}

	//cvShowImage("Src", src);
	cvShowImage("Temp", tmp);
	cvShowImage("Result", dst);

	cvWaitKey(0);
	cvReleaseImage(&tmp);

	//cvDestroyWindow("Src");
	cvDestroyWindow("Temp");

	return find;
}