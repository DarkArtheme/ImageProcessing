#include <opencv2/opencv.hpp>
#include <iostream>
#include <format>

void split_into_frames() {
    int video_total = 5;
	const std::string video_path_prefix = "./data/banknote";
    for (int video_num = 0; video_num < video_total; ++video_num) {
		cv::VideoCapture capture(video_path_prefix + std::to_string(video_num + 1) + ".mp4");
		if (!capture.isOpened()) {
			std::cerr << "Failed to open the video device, video file or image sequence!\n";
            exit(1);
		}
		std::vector<cv::Mat> result;
		cv::Mat frame;
		result.reserve(500);
		for (;;) {
			capture >> frame;
			if (frame.empty()) {
				break;
			}
			result.push_back(frame.clone());
		}
		auto n = result.size();
        int img_total = 3;
		for (size_t i = 0; i < img_total; ++i) {
            int img_num = img_total * video_num + int(i) + 1;
            size_t idx = (i + 2) * n / 5;
            cv::imwrite("./output/frame" + std::to_string(img_num) + ".png", result[idx]);
		}
    }
}

std::vector<cv::Mat> read_images(int n) {
    std::vector<cv::Mat> result;
    for (int i = 0; i < n; ++i) {
        auto img = cv::imread("./output/frame" + std::to_string(i + 1) + ".png", cv::IMREAD_COLOR);
        result.push_back(img.clone());
    }
    return result;
}

std::vector<cv::Mat>& binarize_frames(std::vector<cv::Mat>& frames) {
    // watch https://docs.opencv.org/4.x/db/d8e/tutorial_threshold.html
    int threshold_value = 140;  // below this value is black and above this value is white
    int threshold_type = cv::THRESH_BINARY | cv::THRESH_OTSU;     // binary type (divided into black & white)
    int max_binary_value = 255;
    int c = 1;
    for (auto& frame : frames) {
        cv::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY, 0);
        cv::GaussianBlur(frame, frame, cv::Size(63, 63), 0);
        cv::threshold(frame, frame, threshold_value, max_binary_value, cv::THRESH_BINARY);
        cv::imwrite("./output/bin_frame" + std::to_string(c) + ".png", frame);
        c++;
    }
    return frames;
}

std::vector<cv::Mat>& morph_images(std::vector<cv::Mat>& frames) {
    int c = 1;
    auto kernel = cv::Size(7, 7);
    for (auto& frame : frames) {
        cv::morphologyEx(frame, frame, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, kernel));
        cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, kernel));
        cv::dilate(frame, frame, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
        cv::erode(frame, frame, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
        cv::imwrite("./output/morph_frame" + std::to_string(c) + ".png", frame);
        c++;
    }
    return frames;
}

cv::Rect2i detect_banknote(const cv::Mat& image) {
    cv::Mat labels, stats, centroids;
    cv::connectedComponentsWithStats(image, labels, stats, centroids);
    int cmp_num = 1;
    auto x = stats.at<int>(cmp_num, 0), y = stats.at<int>(cmp_num, 1);
    auto width = stats.at<int>(cmp_num, 2), height = stats.at<int>(cmp_num, 3);
    cv::Rect2i rect = { x, y, width, height };
    
    return rect;
}

void print_image(const std::string& output_path, const cv::Mat& image) {
    cv::imwrite(output_path, image);
    std::cout << "Successfully written to '" << output_path << "'" << std::endl;
}

int main() {
    const std::string output_path = "./output/random_frame.png";
    size_t frame_idx = 144;
    //split_into_frames();
    auto frames = read_images(15);
    auto image = frames[0];
    print_image(output_path, image);
    binarize_frames(frames);
    morph_images(frames);
    auto binarized_image = frames[0];
    auto rect = detect_banknote(binarized_image);
    cv::rectangle(image, rect, cv::Scalar(0, 0, 255), 4);
    return 0;
}
