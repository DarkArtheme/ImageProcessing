#include <opencv2/opencv.hpp>


float geometric_mean(float x, float y) {
	float t = sqrt(x * x * 1.0L + y * y * 1.0L);
	if (t < 0) {
		t = (t + 255.0f) / 2;
	}
	return t;
}


int main() {
	const int BASE = 150;
	const int COL = 255;
	cv::Mat image(2 * BASE, 3 * BASE, CV_32FC1);
	for (int i = 0; i < 3; i++) {
		int k = (i + 1) % 3;
		cv::Rect2d r = cv::Rect2d(BASE * i, 0.0, BASE * (i + 1), BASE);
		cv::rectangle(image, r, COL * i / 2.0, -1);
		cv::circle(image, { BASE / 2 + BASE * i, BASE / 2 }, 64,
			COL* k / 2.0, -1);

		r = cv::Rect2d(BASE * i, BASE, BASE * (i + 1), 2 * BASE);
		cv::rectangle(image, r, COL * k / 2.0, -1);
		cv::circle(image, { BASE / 2 + BASE * i, BASE / 2 + BASE }, 64, 
			COL* i / 2.0, -1);
	}
	cv::imwrite("./output/drawn_image.png", image);

	cv::Mat I1;
	cv::filter2D(image, I1, -1, 
		cv::Mat(3, 3, CV_32FC1, new float[9]{ 1, 0, -1, 2, 0, -2, 1, 0, -1 }));

	cv::imwrite("./output/I1.png", I1);

	cv::Mat I2;
	cv::filter2D(image, I2, -1, 
		cv::Mat(3, 3, CV_32FC1, new float[9] { 1, 2, 1, 0, 0, 0, -1, -2, -1 }));
	cv::imwrite("./output/I2.png", I2);

	auto I_Result = I1.clone();
	for (int i = 0; i < I_Result.rows; i++) {
		for (int j = 0; j < I_Result.cols; j++) {
			I_Result.at<float>(i, j) = geometric_mean(I1.at<float>(i, j), I2.at<float>(i, j));
		}
	}
	cv::imwrite("./output/I_Result.png", I_Result);
}