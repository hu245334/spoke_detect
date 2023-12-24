#ifndef _REGISTRATION_HOUGH_H
#define _REGISTRATION_HOUGH_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>

cv::Point houghTrans_r(IplImage *src, IplImage *dst, IplImage *tmp, float r, int xstep, int ystep);

#endif