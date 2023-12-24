#ifndef _REGISTRATION_GEOMETRIC_H
#define _REGISTRATION_GEOMETRIC_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>

cv::Point geometricTrans_r(cv::Mat &srcImg, cv::Mat &dst, cv::Mat &tmp, float r, cv::Point local, int width, int height);

#endif