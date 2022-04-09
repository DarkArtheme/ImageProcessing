#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

void split_into_frames() {
    int video_total = 7;
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
        cv::imwrite("./output/grayscale_frame" + std::to_string(c) + ".png", frame);
        cv::GaussianBlur(frame, frame, cv::Size(15, 15), 0);
        cv::threshold(frame, frame, threshold_value, max_binary_value, cv::THRESH_BINARY);
        cv::imwrite("./output/bin_frame" + std::to_string(c) + ".png", frame);
        c++;
    }
    return frames;
}

std::vector<cv::Mat>& morph_images(std::vector<cv::Mat>& frames) {
    int c = 1;
//    auto kernel = cv::Size(7, 7);
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(35, 10));
    for (auto& frame : frames) {
//        cv::morphologyEx(frame, frame, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, kernel));
//        cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, kernel));
//        cv::dilate(frame, frame, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
//        cv::erode(frame, frame, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
        cv::morphologyEx(frame, frame, cv::MORPH_CLOSE, kernel);
        cv::morphologyEx(frame, frame, cv::MORPH_OPEN, kernel);
        cv::dilate(frame, frame, kernel);
        cv::imwrite("./output/morph_frame" + std::to_string(c) + ".png", frame);
        c++;
    }
    return frames;
}

std::vector<std::array<cv::Point, 4>> read_mask_points(std::string path) {
    std::ifstream input(path);
    std::vector<std::array<cv::Point, 4>> result;
    auto jf = json::parse(input);
    for (auto& elem : jf) {
        std::array<cv::Point, 4> points;
        points[0] = cv::Point(elem["bottom-left"].get<std::array<int, 2>>()[0], 
                              elem["bottom-left"].get<std::array<int, 2>>()[1]);
        points[1] = cv::Point(elem["top-left"].get<std::array<int, 2>>()[0], 
                              elem["top-left"].get<std::array<int, 2>>()[1]);
        points[2] = cv::Point(elem["top-right"].get<std::array<int, 2>>()[0], 
                              elem["top-right"].get<std::array<int, 2>>()[1]);
        points[3] = cv::Point(elem["bottom-right"].get<std::array<int, 2>>()[0], 
                              elem["bottom-right"].get<std::array<int, 2>>()[1]);
        result.push_back(points);
    }
    return result;
}

std::vector<cv::Mat> draw_masks(const std::vector<std::array<cv::Point, 4>>& points,
                                const std::vector<cv::Mat>& images) {
    size_t i = 0;
    std::vector<cv::Mat> result;
    for (const auto& vertexes : points) {
        cv::Point pts[1][4];
        pts[0][0] = vertexes[0];
        pts[0][1] = vertexes[1];
        pts[0][2] = vertexes[2];
        pts[0][3] = vertexes[3];
        const cv::Point* ppt[1] = { pts[0] };
        int npt[] = { 4 };
        auto img = images[i].clone();
        img = 0;
        cv::fillPoly(img, ppt, npt, 1, cv::Scalar(255, 255, 255), cv::LINE_8);
        cv::imwrite("./output/mask" + std::to_string(i + 1) + ".png", img);
        result.push_back(img);
        ++i;
    }
    return result;
}

bool intersect(const cv::Rect& rect1, const cv::Rect& rect2) {
    return rect1.contains(cv::Point(rect2.x, rect2.y)) || 
        rect1.contains(cv::Point(rect2.x + rect2.width - 1, rect2.y + rect2.height - 1));
}

void improve_mask(cv::Mat& mask, const cv::Mat& img) {
    for (int i = 0; i < mask.rows; i++) {
        int last_updated_ind = 0;
        for (int j = 0; j < mask.cols; j++) {
            if (img.at<uchar>(i, j) == 255) break;
            else {
                last_updated_ind++;
                mask.at<uchar>(i, j) = 0;
            }
        }
        for (int k = mask.cols - 1; k > last_updated_ind; k--) {
            if (img.at<uchar>(i, k) == 255) break;
            else {
                mask.at<uchar>(i, k) = 0;
            }
        }
    }
}


std::vector<cv::Mat>& get_mask(std::vector<cv::Mat>& frames) {
    int c = 1;
    for (auto& frame : frames) {
        cv::Mat labels, stats, centroids;
        cv::Mat selected_img_part(frame.rows, frame.cols, frame.type(), cv::Scalar(0, 0, 0));
        cv::Mat mask(frame.rows, frame.cols, frame.type(), 255);
        int max_comp_ind = 0, max_comp_stat = 0;

        cv::connectedComponentsWithStats(frame, labels, stats, centroids);

        for (int i = 1; i < stats.rows; i++) {
            int square = stats.at<int>(i, 4);
            if (square > max_comp_stat) {
                max_comp_ind = i;
                max_comp_stat = square;
            }
        }

        cv::Rect2i max_component = { stats.at<int>(max_comp_ind, 0), 
                                    stats.at<int>(max_comp_ind, 1), 
                                    stats.at<int>(max_comp_ind, 2), 
                                    stats.at<int>(max_comp_ind, 3) };

        for (int i = max_component.x; i < max_component.x + max_component.width; i++) {
            for (int j = max_component.y; j < max_component.y + max_component.height; j++) {
                selected_img_part.at<uchar>(j, i) = frame.at<uchar>(j, i);
                mask.at<uchar>(j, i) = 255;
            }
        }

        for (int i = 1; i < stats.rows; i++) {
            cv::Rect2i component = { stats.at<int>(i, 0), stats.at<int>(i, 1), stats.at<int>(i, 2), stats.at<int>(i, 3) };
            if (i != max_comp_ind && intersect(max_component, component)) {
                cv::rectangle(selected_img_part, component, 255, cv::FILLED);
            }
        }

        improve_mask(mask, selected_img_part);
        cv::imwrite("./output/improved_morph" + std::to_string(c) + ".png", frame);
        c++;
    }
    return frames;
}

std::vector<cv::Mat> concatenate_masks(const std::vector<cv::Mat>& orig_frames, 
                                       const std::vector<cv::Mat>& mask_frames, 
                                       const std::vector<cv::Mat>& drawn_frames) {
    auto result(orig_frames);
    for (size_t i = 0; i < orig_frames.size(); ++i) {
        cv::Mat rgb_image_channels[3];

        cv::split(orig_frames[i], rgb_image_channels);
        cv::max(rgb_image_channels[2], mask_frames[i], rgb_image_channels[2]);
        cv::max(rgb_image_channels[1], drawn_frames[i], rgb_image_channels[1]);
        cv::merge(rgb_image_channels, 3, result[i]);
        cv::imwrite("./output/conc_frame" + std::to_string(i + 1) + ".png", result[i]);
    }
    return result;
}

std::vector<int> calc_intersect_masks(const std::vector<cv::Mat>& mask_frames, 
                                      const std::vector<cv::Mat>& drawn_frames) {
    int right_value = 255;
    auto result = std::vector<int>(mask_frames.size());
    for (size_t ind = 0; ind < mask_frames.size(); ind++) {
        for (int i = 0; i < mask_frames[ind].rows; i++) {
            for (int j = 0; j < mask_frames[ind].cols; j++) {
                if (mask_frames[ind].at<uchar>(i, j) == right_value && 
                    mask_frames[ind].at<uchar>(i, j) == drawn_frames[ind].at<uchar>(i, j)) {
                    result[ind]++;
                }
            }
        }
    }
    return result;
}

std::vector<int> calc_union_masks(const std::vector<cv::Mat>& mask_frames, 
                                  const std::vector<cv::Mat>& drawn_frames) {
    int right_value = 255;
    auto result = std::vector<int>(mask_frames.size());
    for (size_t ind = 0; ind < mask_frames.size(); ind++) {
        for (int i = 0; i < mask_frames[ind].rows; i++) {
            for (int j = 0; j < mask_frames[ind].cols; j++) {
                if (mask_frames[ind].at<uchar>(i, j) == right_value || 
                    drawn_frames[ind].at<uchar>(i, j) == right_value) {
                    result[ind]++;
                }
            }
        }
    }
    return result;
}

std::vector<double> calc_accuracy(const std::vector<cv::Mat>& mask_frames,
                                  const std::vector<cv::Mat>& drawn_masks) {
    std::ofstream out("./output/accuracy.txt", std::ios_base::out);
    auto result = std::vector<double>(mask_frames.size());
    auto int_masks = calc_intersect_masks(mask_frames, drawn_masks);
    auto union_masks = calc_union_masks(mask_frames, drawn_masks);
    for (size_t i = 0; i < mask_frames.size(); i++) {
        result[i] = int_masks[i] * 1.0 / union_masks[i];
        out << i + 1 << ") " << result[i] << std::endl;
    }
    return result;
}

int main() {
    const std::string output_path = "./output/random_frame.png";
    split_into_frames();
    auto frames = read_images(21);
    auto orig_frames(frames);
    binarize_frames(frames);
    morph_images(frames);
    get_mask(frames);
    auto r = read_mask_points("./data/masks.json");
    auto drawn_masks = draw_masks(r, frames);
    concatenate_masks(orig_frames, frames, drawn_masks);
    calc_accuracy(frames, drawn_masks);
    return 0;
}
