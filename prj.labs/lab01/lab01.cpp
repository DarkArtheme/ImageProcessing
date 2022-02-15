#include <opencv2/opencv.hpp>
#include <cmath>
#include <chrono>
#include <iostream>

using std::chrono::steady_clock;
const int X_SIZE = 768, Y_SIZE = 60;

cv::Mat draw_i1() {
	cv::Mat img(Y_SIZE, X_SIZE, CV_8UC1);
	img = 0;
	for (double i = 0; i < X_SIZE; i += 3) {
		cv::Rect2d rc = { i, 0, i + 3, Y_SIZE };
		int thickness = -1;
		cv::rectangle(img, rc, { i / 3 }, thickness);
	}
	return img;
}

cv::Mat draw_g1() {
	auto img = draw_i1();
	img.convertTo(img, CV_64F, 1.0 / 255);
	cv::pow(img, 2.3, img);
	img.convertTo(img, CV_8UC1, 255.0);
	return img;
}

cv::Mat draw_g2() {
	auto img = draw_i1();
	img.convertTo(img, CV_64F, 1.0 / 255);
	for (int i = 0; i < Y_SIZE; ++i) {
		for (int j = 0; j < X_SIZE; ++j) {
			img.at<double>(i, j) = std::pow(img.at<double>(i, j), 2.3);
		}
	}
	img.convertTo(img, CV_8UC1, 255.0);
	return img;
}

cv::Mat& draw_canvas(cv::Mat& img) {
	cv::Rect2d rc = { 0, 0, X_SIZE, Y_SIZE };
	cv::rectangle(img, rc, { 255 }, 1);
	return img;
}

void print_time(const steady_clock::time_point& start, const steady_clock::time_point& finish, 
				const std::string& method_name) {
	auto time = finish - start;
	std::cout << "Took "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(time).count()
		<< " ms to run '" << method_name << "'" << std::endl;
}

int main() {
	auto I_1 = draw_i1();
	auto start = std::chrono::high_resolution_clock::now();
	auto G_1 = draw_g1();
	auto finish = std::chrono::high_resolution_clock::now();
	print_time(start, finish, "draw_g1()");
	start = std::chrono::high_resolution_clock::now();
	auto G_2 = draw_g2();
	finish = std::chrono::high_resolution_clock::now();
	print_time(start, finish, "draw_g2()");
	// save result
	auto result = draw_canvas(I_1);
	result.push_back(draw_canvas(G_1));
	result.push_back(draw_canvas(G_2));
	cv::Rect2d rc = { 0, 0, X_SIZE, Y_SIZE * 3 };
	cv::rectangle(result, rc, { 0 }, 1);
	cv::imwrite("lab01.png", result);
	return 0;
}