#pragma once
#ifndef __OTSU__
#define __OTSU__

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

class Otsu {
public:
	Otsu(Mat& img);
	void Thresholding(Mat& img);

private:
	long double histogram[256];
	long double mean;
	vector<long double> q1, mean1, mean2, variance;

	void calculateHistogram(Mat& img);
	void calculateMean();
	long double calculateVariance(long double q1, long double mean1, long double mean2);
	int calculateParameters();
	int recursiveParameters(int t);
	void nonRecursiveParameters(int t);
	void reconstructThresholdImage(uchar threshold, Mat& img);
	void calculateOpenCVThreshold(Mat& img);
};
#endif // !__OTSU__

