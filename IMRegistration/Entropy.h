#ifndef _ENTROPY_H
#define _ENTROPY_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>

double Entropy(cv::Mat img);
double ComEntropy(cv::Mat img1, cv::Mat img2, double img1_entropy);

#endif