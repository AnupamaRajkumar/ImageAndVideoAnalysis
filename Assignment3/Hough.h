#pragma once

#ifndef __HOUGH__
#define __HOUGH__

#include <iostream>
#include <map>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class houghTrm {
public:
	houghTrm(Mat& img, int rmin, int rmax);
	void DetectCircles();
private:
	Mat img;
	int rmin, rmax;
	Mat EdgeDetector();
	void DrawCircles(map<list<int>, int>& accumulator);
	void HoughTransform(Mat& edgeImg, map<list<int>, int>& accumulator);

};
#endif // !__HOUGH__

