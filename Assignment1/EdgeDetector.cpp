
#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/core.hpp>
#include<math.h>
#include "EdgeDetector.h"

using namespace std;
using namespace cv;


/*************************************************************************
Function name : generatePrewittKernel
Description : Prewitt filter kernel generator
Input : direction x or y flag
Author : Anupama Rajkumar
**************************************************************************/

void EdgeDetection::generatePrewittKernel(int direction) {

	kernel1 = cv::Mat::zeros(3, 3, CV_32FC1);
	kernel2 = cv::Mat::zeros(3, 3, CV_32FC1);

	if (direction == 0) {		/*X direction*/
		kernel1.at<float>(0, 0) = 1./3.;
		kernel1.at<float>(0, 1) = 1./3.;
		kernel1.at<float>(0, 2) = 1./3.;
		kernel1.at<float>(1, 0) = 0.;
		kernel1.at<float>(1, 1) = 0.;
		kernel1.at<float>(1, 2) = 0.;
		kernel1.at<float>(2, 0) = -1./3.;
		kernel1.at<float>(2, 1) = -1./3.;
		kernel1.at<float>(2, 2) = -1./3.;

	}
	else {

		kernel2.at<float>(0, 0) = 1./3.;
		kernel2.at<float>(0, 1) = 0.;
		kernel2.at<float>(0, 2) = -1./3.;
		kernel2.at<float>(1, 0) = 1./3.;
		kernel2.at<float>(1, 1) = 0.;
		kernel2.at<float>(1, 2) = -1./3.;
		kernel2.at<float>(2, 0) = 1./3.;
		kernel2.at<float>(2, 1) = 0.;
		kernel2.at<float>(2, 2) = -1./3.;
	}
}

/*************************************************************************
Function name : prewittFiltering
Description : Prewitt filtering function
Input : direction x or y flag
Author : Anupama Rajkumar
**************************************************************************/

void EdgeDetection::prewittFiltering(Mat& src) {

	/*generate gradient matrix in X direction*/
	generatePrewittKernel(0);
	filter2D(src, gradX, CV_32FC1, kernel1);

	/*generate gradient matrix in Y direction*/
	generatePrewittKernel(1);
	filter2D(src, gradY, CV_32FC1, kernel2);

	imwrite("XPrewittEdge.png", gradX);
	imwrite("YPrewittEdge.png", gradY);

	/*determining the edge magnitude from the X and Y gradient matrices*/
	Mat grad = getEdgeMagnitude();
	imwrite("EdgeMagnitude.png", grad);

	/*suppressing non maximal points in the edge magnitude*/
	Mat NMS = nonMaximalSuppression(grad);
	imwrite("NonMaximalSuppression.png", NMS);

}

/*************************************************************************
Function name : getEdgeMagnitude 
Description : Calculating the edge magnitude from X and Y magnitude
Author : Anupama Rajkumar
**************************************************************************/
Mat EdgeDetection::getEdgeMagnitude() {

	Mat grad = cv::Mat::zeros(gradX.size(), gradX.type());
	magnitude(gradX, gradY, grad);
	return grad.clone();
}


/*************************************************************************
Function name : nonMaximalSuppression
Description : Calculating the edge magnitude from X and Y magnitude
Input : 
Author : Anupama Rajkumar
**************************************************************************/
Mat EdgeDetection::nonMaximalSuppression(Mat& grad)
{
	/*calculate gradient direction*/
	Mat gradDirn = cv::Mat::zeros(grad.size(), CV_32FC1);
	Mat NMS = cv::Mat::zeros(grad.size(), grad.type());
	
	for (int r = 0; r < gradDirn.rows; r++) {
		for (int c = 0; c < gradDirn.cols; c++) {
			gradDirn.at<float>(r, c) = atan2f(gradY.at<float>(r, c), gradX.at<float>(r, c));
			gradDirn.at<float>(r, c) = (gradDirn.at<float>(r, c)*180.0) / PI;
			//cout << gradDirn.at<float>(r, c) << endl;
			if (gradDirn.at<float>(r, c) < 0) {
				gradDirn.at<float>(r, c) += 180.0;
			}
		}
	}

	/*check for each pixel in the image, store the neigbouring pixel in each of 8 directions */
	/*if the intensity if neighboring pixel is greater than the current pixel, discard the current pixel, else keep it */
	for (int r = 1; r < gradDirn.rows-1; r++) {
		for (int c = 1; c < gradDirn.cols-1; c++) {
			float intensityRt, intensityLt;
			intensityRt = intensityLt = 255.;
			float dirn = gradDirn.at<float>(r, c);
			if ((dirn == 0.0) || (dirn == 180.0)) {
				intensityRt = grad.at<float>(r, c + 1);
				intensityLt = grad.at<float>(r, c - 1);
			}
			else if ((0.0 < dirn) && (dirn < 90.0)) {
				intensityRt = grad.at<float>(r + 1, c + 1);
				intensityLt = grad.at<float>(r - 1, c - 1);
			}
			else if ((dirn == 90.0)) {
				intensityRt = grad.at<float>(r + 1, c);
				intensityLt = grad.at<float>(r - 1, c);
			}
			else if ((90.0 < dirn) && (dirn < 180.0)) {
				intensityRt = grad.at<float>(r + 1, c - 1);
				intensityLt = grad.at<float>(r - 1, c + 1);
			}

			if ((grad.at<float>(r, c) > intensityRt) && (grad.at<float>(r, c) > intensityLt)) {
				NMS.at<float>(r, c) = grad.at<float>(r, c);
			}
			else {
				NMS.at<float>(r, c) = 0.;
			}
		}
	}
	return NMS.clone();
}
