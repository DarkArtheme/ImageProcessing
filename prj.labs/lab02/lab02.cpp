#include <opencv2/opencv.hpp>

int main() {
	const std::string image_path = "./data/cross_0256x0256.png";
	cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
	if (img.empty()) {
		std::cout << "Could not read the image: " << image_path << std::endl;
		return 1;
	}
	std::cout << "Image loaded" << std::endl;
	cv::waitKey(0);
	return 0;	
}