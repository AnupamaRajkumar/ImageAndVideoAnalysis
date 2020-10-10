// Thresholding.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>

#include "Otsu.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	

	/*import image*/
	if (argc < 2) {
		cout << "Usage: ./main path_to_original_image" << endl;
		cout << "Press enter to exit" << endl;
		cin.get();
		return -1;
	}

	std::cout << "Load input image" << endl;
	Mat img = imread(argv[1]);

	/*convert image to grayscale if not already grayscale*/
	if (img.channels() > 1) {
		cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
	}
	//imshow("Input", img);
	Otsu otsu(img);
	otsu.Thresholding(img);

	waitKey(0);
	return 1;
}

