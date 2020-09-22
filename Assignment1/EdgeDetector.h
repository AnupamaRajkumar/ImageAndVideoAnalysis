#pragma once
#ifndef __EDGE_DETECTOR__
#define __EDGE_DETECTOR__

#include <iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define PI 3.14159265

class EdgeDetection {
public:
	Mat spatialConvolution(Mat& src, Mat& kernel);
	Mat getBorderedImage(Mat &src, int rowMiddle, int colMiddle);
	void prewittFiltering(Mat &src);
	Mat getEdgeMagnitude();
	Mat nonMaximalSuppression(Mat& grad);
private:
	Mat kernel1, kernel2, gradX, gradY;
	void generatePrewittKernel(int direction);
};


#endif

