#include <opencv2/opencv.hpp>
#include <iostream>

std::vector<cv::Mat> split_into_frames(cv::VideoCapture& capture) {
    cv::Mat frame;
    std::vector<cv::Mat> result;
    result.reserve(500);
    for (;;) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }
        result.push_back(frame);
    }
    return result;
}

std::vector<cv::Mat>& process_frames(std::vector<cv::Mat>& frames) {

    // watch https://docs.opencv.org/4.x/db/d8e/tutorial_threshold.html
    int threshold_value = 148;  // below this value is black and above this value is white
    int threshold_type = 0;     // binary type (divided into black & white)
    int max_binary_value = 255;

    for (auto& frame : frames) {
        cv::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY, 0);
        cv::threshold(frame, frame, threshold_value, max_binary_value, threshold_type);
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
	const std::string video_path = "./data/banknote.mp4";
    const std::string output_path = "random_frame.png";
    size_t frame_idx = 144;
	cv::VideoCapture capture(video_path);
    if (!capture.isOpened()) {
        std::cerr << "Failed to open the video device, video file or image sequence!\n";
        return 1;
    }
    auto frames = split_into_frames(capture);
    auto image = frames.at(frame_idx);
    print_image(output_path, image);
    process_frames(frames);
    auto binarized_image = frames.at(frame_idx);
    auto rect = detect_banknote(binarized_image);
    cv::rectangle(image, rect, cv::Scalar(0, 0, 255), 4);
    return 0;
}
