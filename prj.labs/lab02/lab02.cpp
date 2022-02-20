#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include <string>
#include <vector>

enum class Color {
	BLUE,
	GREEN,
	RED
};

cv::Mat merge_channels(const std::vector<cv::Mat>& bgr_channels, const Color color);

cv::Mat divide_image(const cv::Mat& img) {
	std::vector<cv::Mat> bgr_channels(3);
	cv::split(img, bgr_channels);
	auto blue_img = merge_channels(bgr_channels, Color::BLUE);
	auto green_img = merge_channels(bgr_channels, Color::GREEN);
	auto red_img = merge_channels(bgr_channels, Color::RED);
	cv::Mat up_row, down_row, result;
	cv::hconcat(img, red_img, up_row);
	cv::hconcat(green_img, blue_img, down_row);
	cv::vconcat(up_row, down_row, result);
	return result;
}


int main() {
	const std::string image_path = "./data/cross_0256x0256.png";
	const std::string compressed_image_path = "./cross_0256x0256_025.jpg";
	cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
	if (img.empty()) {
		std::cout << "Could not read the image: " << image_path << std::endl;
		return 1;
	}
	std::vector<int> compression_params = { cv::ImwriteFlags::IMWRITE_JPEG_QUALITY, 25 };
	cv::imwrite(compressed_image_path, img, compression_params);
	cv::Mat cmpr_img = cv::imread(compressed_image_path, cv::IMREAD_COLOR);

	auto img_chnl = divide_image(img);
	auto cmpr_img_chnl = divide_image(cmpr_img);
	cv::imwrite("./cross_0256x0256_png_channels.png", img_chnl);
	cv::imwrite("./cross_0256x0256_jpg_channels.png", cmpr_img_chnl);
	cv::waitKey(0);
	return 0;	
}

cv::Mat merge_channels(const std::vector<cv::Mat>& bgr_channels, const Color color) {
	cv::Mat result, zero_mat;
	zero_mat = cv::Mat::zeros(cv::Size(bgr_channels[0].cols, bgr_channels[0].rows), CV_8UC1);
	std::vector<cv::Mat> channels;
	switch (color) {
		case Color::BLUE: {
			channels.push_back(bgr_channels[0]);
			channels.push_back(zero_mat);
			channels.push_back(zero_mat);
			cv::merge(channels, result);
			break;
		}
		case Color::GREEN: {
			channels.push_back(zero_mat);
			channels.push_back(bgr_channels[1]);
			channels.push_back(zero_mat);
			cv::merge(channels, result);
			break;
		}
		case Color::RED: {
			channels.push_back(zero_mat);
			channels.push_back(zero_mat);
			channels.push_back(bgr_channels[2]);
			cv::merge(channels, result);
			break;
		}
	}
	return result;
}