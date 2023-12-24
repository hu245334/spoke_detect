#ifndef _DEFECT_EXTRACTION_H
#define _DEFECT_EXTRACTION_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>    
#include <opencv2/highgui/highgui.hpp>

void Defect_extraction(cv::Mat& MsrcIMG, cv::Mat& srcImage, cv::Mat& org, CvPoint& center, int width, cv::Point local);

#endif