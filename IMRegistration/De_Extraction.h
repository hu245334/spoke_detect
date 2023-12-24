#ifndef _DE_EXTRACTION_H
#define _DE_EXTRACTION_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>

void De_extraction(cv::Mat& srcImage, cv::Mat& dstImage, cv::Rect rect, cv::Point *point);

#endif