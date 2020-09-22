/* Assignment1.cpp : This file contains the 'main' function. Program execution begins and ends there.
Problem Statement : Prewitt Edge Detector : Gradient filter and nonmaxima-suppression(NMS)
Output images : 1. Gradient Magnitude, 2. Final result after NMS
*/

#include <iostream>
#include <opencv2/opencv.hpp>

#include "EdgeDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	EdgeDetection ed;
	/*import image*/
	if (argc < 2) {
		cout << "Usage: ./main path_to_original_image" << endl;
		cout << "Press enter to exit" << endl;
		cin.get();
		return -1;
	}

	std::cout << "Load input image" << endl;
	Mat img = imread(argv[1], IMREAD_COLOR);
	
	/*filtering to smooth out noise*/
	int kSize = 3;
	Mat kernel = Mat(kSize, kSize, img.type());
	Mat filtImg = Mat::zeros(img.size(), img.type());
	GaussianBlur(img, filtImg, kernel.size(), 0, 0);

	/*convert image to grayscale if not already grayscale*/
	if (filtImg.channels() > 1) {
		cv::cvtColor(filtImg, filtImg, cv::COLOR_BGR2GRAY);
	}
	
	ed.prewittFiltering(filtImg);

	cout << "Filtering completed";

	waitKey(0);
	return 0;
}


