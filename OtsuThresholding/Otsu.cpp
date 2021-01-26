#include "Otsu.h"


/****************************************************
Constructor for Otsu Thresholding to initialise variables
and calculate mean and histogram of the input image
*****************************************************/
Otsu::Otsu(Mat& img) {
	for (int l = 0; l < 256; l++) {
		this->histogram[l] = 0.;
	}

	this->mean = 0.;

	this->q1.reserve(256);
	for (int l = 0; l < 256; l++) {
		this->q1.push_back(0.);
	}

	this->mean1.reserve(256);
	for (int l = 0; l < 256; l++) {
		this->mean1.push_back(0.);
	}

	this->mean2.reserve(256);
	for (int l = 0; l < 256; l++) {
		this->mean2.push_back(0.);
	}

	this->variance.reserve(256);
	for (int l = 0; l < 256; l++) {
		this->variance.push_back(0.);
	}

	this->calculateHistogram(img);
	this->calculateMean();
	cout << "Mean:" << this->mean << endl;
}

/****************************************************
Function to calculate the histogram of the input image
*****************************************************/
void Otsu::calculateHistogram(Mat& img) {

	int cnt = 0;
	for (int row = 0; row < img.rows; row++) {
		for (int col = 0; col < img.cols; col++) {
			this->histogram[(int)img.at<uchar>(row, col)]++;
			cnt++;
		}
	}
	
	for (int l = 0; l < 256; l++) {
		cout << this->histogram[l] << ",";
	}
	cout << "\nTotal number of pixels:" << cnt << endl;

	/*Normalise the histogram*/
	for (int l = 0; l < 256; l++) {
		this->histogram[l] /= cnt;
	}

	cout << endl;
	double sum = 0.;
	for (int l = 0; l < 256; l++) {
		sum += this->histogram[l];
	}
	cout << "Normalised histogram sum:" << sum << endl;
}

/****************************************************
Function to calculate mean of the input image
*****************************************************/
void Otsu::calculateMean() {
	for (int l = 0; l < 256; l++) {
		this->mean += (l * this->histogram[l]);
	}
}

/****************************************************
Function to calculate between-class variance
*****************************************************/
long double Otsu::calculateVariance(long double q1, long double mean1, long double mean2) {
	long double variance = 0.;
	variance = q1 * (1 - q1) * (mean1 - mean2) * (mean1 - mean2);
	return variance;
}

/****************************************************
Function to calculate the first parameters ignoring the
initial zero values
*****************************************************/
int Otsu::calculateParameters() {
	/*Ignore initial zeros*/
	int t = 0;
	while (t < 256) {
		if (this->histogram[t] == 0) {
			t++;
		}
		else
			break;
	}
	cout << "first non-zero index:" << t << endl;
	/*calculate first q1*/
	long double q = 0.;
	q += this->histogram[t];
	this->q1.at(t) = q;

	/*calculate first mean*/
	long double m1 = 0.;
	m1 += (t*this->histogram[t]);
	m1 /= q;
	this->mean1.at(t) = m1;

	/*calculate second mean*/
	long double m2 = 0.;
	for (int i = t; i < 256; i++) {
		m2 += (i*this->histogram[i]);
	}
	m2 /= (1 - q);
	this->mean2.at(t) = m2;

	/*calculate variance*/
	long double v = this->calculateVariance(q, m1, m2);
	this->variance.at(t) = v;
	return t;
}

/****************************************************
Function to calculate the remaining parameters to 
determine the between-class variance in a recursive 
way
*****************************************************/
int Otsu::recursiveParameters(int t) {
	if (t < 256) {
		long double q = 0.;
		q = this->q1.at(t - 1) + this->histogram[t];
		this->q1.at(t) = q;

		long double m1 = 0.;
		m1 = ((this->q1.at(t - 1)*this->mean1.at(t - 1)) + (t*this->histogram[t])) / q;
		this->mean1.at(t) = m1;

		long double m2 = 0.;
		m2 = (this->mean - q * m1) / (1 - q);
		this->mean2.at(t) = m2;

		long double v = this->calculateVariance(q, m1, m2);
		this->variance.at(t) = v;
	}
	else {
		return 1;
	}

}

/****************************************************
Function to calculate the remaining parameters to
determine the between-class variance in a non-recursive
way
*****************************************************/
void Otsu::nonRecursiveParameters(int t) {
	long double q = 0.;
	for (int i = 0; i <= t; i++) {
		q += this->histogram[i];
	}
	this->q1.at(t) = q;

	/*calculate first mean*/
	long double m1 = 0.;
	for (int i = 0; i <= t; i++) {
		m1 += (i*this->histogram[i]);
	}
	m1 /= q;
	this->mean1.at(t) = m1;

	/*calculate second mean*/
	long double m2 = 0.;
	for (int i = t+1; i < 256; i++) {
		m2 += (i*this->histogram[i]);
	}
	m2 /= (1 - q);
	this->mean2.at(t) = m2;

	/*calculate variance*/
	long double v = this->calculateVariance(q, m1, m2);
	this->variance.at(t) = v;
}

/****************************************************
Function to reconstruct the thresholded image after 
the threshold value has been determined
*****************************************************/
void Otsu::reconstructThresholdImage(uchar threshold, Mat& img) {
	Mat threshImg = img.clone();
	for (int row = 0; row < img.rows; row++) {
		for (int col = 0; col < img.cols; col++) {
			if (img.at<uchar>(row, col) > threshold) {
				threshImg.at<uchar>(row, col) = 255;
			}
			else {
				threshImg.at<uchar>(row, col) = 0;
			}
		}
	}

	imshow("Thresholded", threshImg);
	imwrite("Threshold.png", threshImg);
}

/****************************************************
Function to calculate threshold value the thresholded 
image after using OpenCV function for verification
*****************************************************/

void Otsu::calculateOpenCVThreshold(Mat& img) {
	Mat imgThres;
	long double thresh = threshold(img, imgThres, 0., 255., THRESH_OTSU);
	cout << "Opencv threshold value:" << thresh << endl;
	imwrite("OpenCvThreshold.png", imgThres);
}

/****************************************************
Function to calculate Otsu threshold value and 
reconstruct thresholded image
1. Calculate the histogram and mean of the input image
2. Recursively/non-recursively compute parameters q,
mean1 and mean2
3. Calculate the between-class variance
4. Select threshold as the max of the calculate between-
class variance
*****************************************************/
void Otsu::Thresholding(Mat& img) {
	int t;
	t = this->calculateParameters();
	for (int i = (t+1); i < 256; i++) {
		recursiveParameters(i);
		//nonRecursiveParameters(i);
	}
	long double maxVal = 0.;
	uchar maxIdx = 0;
	/*max value of variance*/
	maxVal = *max_element(this->variance.begin(), this->variance.end());
	/*index of the max value*/
	maxIdx = std::max_element(this->variance.begin(), this->variance.end()) - this->variance.begin();;
	cout << "Max value:" << maxVal << endl;
	cout << "Max value idx ie. the threshold:" << (int)maxIdx << endl;

	/*reconstructing thresholded image*/
	reconstructThresholdImage(maxIdx, img);

	/*comparing the calculated threshold with opencv threshold*/
	calculateOpenCVThreshold(img);
}