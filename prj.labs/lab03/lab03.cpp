#include <opencv2/opencv.hpp>
#include <cmath>


cv::Mat make_grayscale_img(const cv::Mat& img) {
	cv::Mat result;
	cv::cvtColor(img, result, cv::COLOR_BGRA2GRAY, 0);
	return result;
}

cv::Mat make_lup() {
	cv::Mat lup_table(1, 256, CV_8U);
	for (size_t i = 0; i < 256; ++i) {
		double res = i < 128 ? 2 * i : i / 2;
		res = res > 255 ? 255 : res;
		lup_table.at<uchar>(0, i) = static_cast<uchar>(res);
	}
	return lup_table;
}

cv::Mat visualize_lup(const cv::Mat& lup_table);

int main() {
	const std::string image_path = "./data/cross_0256x0256.png";
	cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
	if (img.empty()) {
		std::cout << "Could not read the image: " << image_path << std::endl;
		return 1;
	}
	cv::imwrite("./lab03_rgb.png", img);
	auto grayscale_img = make_grayscale_img(img);
	cv::imwrite("./lab03_gre.png", grayscale_img);
	auto lup_table = make_lup();
	auto plot_img = visualize_lup(lup_table);
	cv::imwrite("./lab03_viz_func.png", plot_img);
	cv::LUT(grayscale_img, lup_table, grayscale_img);
	cv::imwrite("./lab03_gre_res.png", grayscale_img);
	cv::LUT(img, lup_table, img);
	cv::imwrite("./lab03_rgb_res.png", img);
	return 0;
}


cv::Mat visualize_lup(const cv::Mat& lup_table) {
	const int plot_size = 512;
	const int img_size = 512;
	int bottom = img_size / 2 - plot_size / 2;
	int top = img_size / 2 + plot_size / 2 - 1;
	int axes_thickness = 2;
	cv::Mat result(img_size, img_size, CV_8UC3);
	result = cv::Scalar(255, 255, 255);
	cv::line(result,
		cv::Point2i(bottom, bottom),
		cv::Point2i(bottom, top),
		cv::Scalar(0, 0, 255),
		axes_thickness);
	cv::line(result,
		cv::Point2i(bottom, top),
		cv::Point2i(top, top),
		cv::Scalar(0, 0, 255),
		axes_thickness);
	//auto prev_point = cv::Point2i(bottom, 
	//	bottom + static_cast<int>((lup_table.at<uchar>(0) / 256.0) * plot_size));
	for (int i = 0; i < 256; ++i) {
		int x = bottom + static_cast<int>((i / 256.0) * plot_size);
		int y = top - static_cast<int>((lup_table.at<uchar>(i) / 256.0) * plot_size);
		//auto cur_point = cv::Point2i(x, y);
		//cv::line(result, prev_point, cur_point, cv::Scalar(0, 0, 0));
		//prev_point = cur_point;
		result.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
	}
	return result;
}