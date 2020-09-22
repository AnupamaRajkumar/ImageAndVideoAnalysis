
#include<iostream>
#include<opencv2/opencv.hpp>
#include<math.h>
#include "EdgeDetector.h"

using namespace std;
using namespace cv;

/*************************************************************************
Function name : getBorderedImage
Description : This function provides border handling during
			 convolution operation
Input : input source image, center row number, center column number
Output : Image with border handling
Author : Anupama Rajkumar
**************************************************************************/
Mat EdgeDetection::getBorderedImage(Mat &src, int rowMiddle, int colMiddle)
{
	// Create bordered matrix.
	Mat borderedSrc = cv::Mat::ones(src.rows + (rowMiddle * 2),
		src.cols + (colMiddle * 2),
		src.type());
	// Copy the original matrix.
	for (unsigned i = 0; i < src.rows; i++) {
		for (unsigned j = 0; j < src.cols; j++) {
			borderedSrc.at<char>(i + rowMiddle, j + colMiddle) = src.at<char>(i, j);
		}
	}
	return borderedSrc.clone();
}

/*************************************************************************
Function name : spatialConvolution
Description : 2D convolution 
Input : input source image, kernel
Output : Convolved image
Author : Anupama Rajkumar
**************************************************************************/
Mat EdgeDetection::spatialConvolution(Mat& src, Mat& kernel)
{
	Mat result = cv::Mat::zeros(src.size(), CV_32FC1);
	Mat tempKernel = cv::Mat::zeros(kernel.size(), kernel.type());
	int colMiddle, rowMiddle;
	int row, col, i, j;
	double sum = 0;
	colMiddle = ((kernel.cols - 1) / 2);
	rowMiddle = ((kernel.rows - 1) / 2);

	/*flip columns*/
	for (row = 0; row < kernel.rows; row++) {
		for (col = 0; col < kernel.cols; col++) {
			if ((col != colMiddle) && (col < colMiddle))
			{
				tempKernel.at<float>(row, col) = kernel.at<float>(row, kernel.cols - 1 - col);
				tempKernel.at<float>(row, kernel.cols - 1 - col) = kernel.at<float>(row, col);
			}
			else if (col == colMiddle)
			{
				tempKernel.at<float>(row, col) = kernel.at<float>(row, col);
			}
			else
			{

			}
		}
	}

	/*flip rows*/
	for (col = 0; col < kernel.cols; col++) {
		for (row = 0; row < kernel.rows; row++) {
			if ((row != rowMiddle) && (row < rowMiddle))
			{
				tempKernel.at<float>(row, col) = tempKernel.at<float>(kernel.rows - 1 - row, col);
				tempKernel.at<float>(kernel.rows - 1 - row, col) = tempKernel.at<float>(row, col);
			}
			else if (row == rowMiddle)
			{
				tempKernel.at<float>(row, col) = tempKernel.at<float>(row, col);
			}
			else
			{

			}
		}
	}

	// 1. Border handling.
	int border_size = kernel.rows / 2;
	Mat bordered_src = getBorderedImage(src, border_size, border_size);


	// Go through the image
	for (unsigned i = border_size; i < src.rows + border_size; i++)
	{
		for (unsigned j = border_size; j < src.cols + border_size; j++)
		{
			// Convolve
			sum = 0.0f;
			for (unsigned r = i - border_size; r <= i + border_size; r++)
				for (unsigned c = j - border_size; c <= j + border_size; c++)
					sum += bordered_src.at<char>(r, c) * tempKernel.at<float>(r - i + border_size, c - j + border_size);
			result.at<float>(i - border_size, j - border_size) = sum;
		}
	}

	return result;
}


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
		kernel1.at<float>(0, 0) = 1.;
		kernel1.at<float>(0, 1) = 1.;
		kernel1.at<float>(0, 2) = 1.;
		kernel1.at<float>(1, 0) = 0;
		kernel1.at<float>(1, 1) = 0;
		kernel1.at<float>(1, 2) = 0;
		kernel1.at<float>(2, 0) = -1.;
		kernel1.at<float>(2, 1) = -1.;
		kernel1.at<float>(2, 2) = -1.;

	}
	else {

		kernel2.at<float>(0, 0) = 1.;
		kernel2.at<float>(0, 1) = 0;
		kernel2.at<float>(0, 2) = -1.;
		kernel2.at<float>(1, 0) = 1.;
		kernel2.at<float>(1, 1) = 0;
		kernel2.at<float>(1, 2) = -1.;
		kernel2.at<float>(2, 0) = 1.;
		kernel2.at<float>(2, 1) = 0;
		kernel2.at<float>(2, 2) = -1.;
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
	gradX = spatialConvolution(src, kernel1);

	/*generate gradient matrix in Y direction*/
	generatePrewittKernel(1);
	gradY = spatialConvolution(src, kernel2);

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
	
	/*converting gradient to 8U*/
	Mat gradXclone, gradYclone;
	gradXclone = gradX.clone();
	gradYclone = gradY.clone();
	gradXclone.convertTo(gradXclone, CV_8UC1);
	gradYclone.convertTo(gradYclone, CV_8UC1);

	Mat grad = cv::Mat::zeros(gradXclone.size(), gradXclone.type());
	//addWeighted(gradX, 0.5, gradY, 0.5, 0, grad);
	for (int r = 0; r < gradX.rows; r++) {
		for (int c = 0; c < gradY.cols; c++) {
			grad.at<char>(r, c) = sqrt(pow(gradXclone.at<char>(r, c), 2) + pow(gradYclone.at<char>(r, c), 2));
		}
	}

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
			gradDirn.at<float>(r, c) = atan2(gradX.at<float>(r, c), gradY.at<float>(r, c)) * 180 / PI;
			if (gradDirn.at<float>(r, c) < 0) {
				gradDirn.at<float>(r, c) += 180;
			}
		}
	}

	/*check for each pixel in the image, store the neigbouring pixel in each of 8 directions */
	/*if the intensity if neighboring pixel is greater than the current pixel, discard the current pixel, else keep it */
	for (int r = 1; r < grad.rows-1; r++) {
		for (int c = 1; c < grad.cols-1; c++) {
			char intensity1, intensity2;
			intensity1 = intensity2 = 255;
			/*angle 0*/
			if ((0 <= gradDirn.at<float>(r, c) < 22.5) || (157.5 <= gradDirn.at<float>(r, c)) <= 180) {
				intensity1 = grad.at<char>(r, c + 1);
				intensity2 = grad.at<char>(r, c - 1);
			}
			/*angle 45*/
			else if (22.5 <= gradDirn.at<float>(r, c) < 67.5) {
				intensity1 = grad.at<char>(r + 1, c - 1);
				intensity2 = grad.at<char>(r - 1, c + 1);
			}
			/*angle 90*/
			else if (67.5 <= gradDirn.at<float>(r, c) < 112.5) {
				intensity1 = grad.at<char>(r + 1, c);
				intensity2 = grad.at<char>(r - 1, c);
			}
			else if (112.5 <= gradDirn.at<float>(r, c) < 157.5) {
				intensity1 = grad.at<char>(r - 1, c - 1);
				intensity2 = grad.at<char>(r + 1, c + 1);
			}

			if ((grad.at<char>(r, c) >= intensity1) && (grad.at<char>(r, c) >= intensity2)) {
				NMS.at<char>(r, c) = grad.at<char>(r, c);
			}
			else {
				NMS.at<char>(r, c) = 0;
			}
		}
	}

	return NMS.clone();
}