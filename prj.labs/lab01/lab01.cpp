#include <opencv2/opencv.hpp>

int main() {
  cv::Mat img(180, 768, CV_8UC1);
  // draw dummy image
  img = 0;
  cv::Rect2d rc = {0, 0, 768, 60 };
  cv::rectangle(img, rc, { 100 }, 1);
  rc.y += rc.height;
  cv::rectangle(img, rc, { 250 }, 1);
  rc.y += rc.height;
  cv::rectangle(img, rc, { 150 }, 1);
  // save result
  cv::imwrite("lab01.png", img);
}
