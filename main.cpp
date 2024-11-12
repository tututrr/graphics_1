#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>

using namespace cv;
using namespace std;

void saveImage(Mat* image, string name) {
    imwrite("/Users/hisoka/Downloads/graphics1/result/" + name, *image);
}

Mat loadImage(string name) {
    Mat image = imread("/Users/hisoka/Downloads/graphics1/source/" + name);
    if (image.empty()) {
        cout << "Can not load image " << name << endl;
    }
    return image;
}

void showImage(Mat* image) {
    imshow("Display window", *image);
}

Mat createAlphaChannel(const Mat& image, float alphaValue) {
    Mat alpha(image.size(), CV_8UC1, Scalar(alphaValue * 255.0f));
    return alpha;
}

Mat createGrayscale(const Mat& image) {
    Mat gray_image;
    cvtColor(image, gray_image, COLOR_BGR2GRAY);

    return gray_image;
}

void grayInCircle() {
    Mat image = loadImage("cat.png");

    Mat gray_image = createGrayscale(image);

    Mat mask(image.size(), CV_8UC1, Scalar(0));

    Point center(image.cols / 2, image.rows / 2);
    int radius = min(image.cols, image.rows) / 2;

    circle(mask, center, radius, Scalar(255), -1);

    Mat result(image.size(), CV_8UC4);

    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            if (mask.at<uchar>(y, x) == 255) {
                uchar color = gray_image.at<uchar>(y, x);
                result.at<Vec4b>(y, x) = Vec4b(color, color, color, 255);
            }
            else {
                result.at<Vec4b>(y, x) = Vec4b(0, 0, 0, 0);
            }
        }
    }

    saveImage(&result, "cat_gray.png");
}

const string BlendMode[9] = {
    "normal", 
    "multiply", 
    "screen", 
    "darken", 
    "lighten", 
    "difference",
    "color_dodge",
    "color_burn",
    "soft_light",
};

float D(float x) {
    if (x <= 0.25f) {
        return ((16 * x - 12) * x + 4) * x;
    }
    return sqrt(x);
}


float B(float Cb, float Cs, const std::string& mode) {
    int modeIndex = -1;

    for (int i = 0; i < 9; ++i) {
        if (BlendMode[i] == mode) {
            modeIndex = i;
            break;
        }
    }

    switch (modeIndex) {
        case 0: // normal
            return Cs;
        case 1: // multiply
            return Cs * Cb;
        case 2: // screen
            return (1 - (1 - Cs) * (1 - Cb));
        case 3: // darken
            return std::min(Cs, Cb);
        case 4: // lighten
            return std::max(Cs, Cb);
        case 5: // difference
            return std::abs(Cs - Cb);
        case 6: // color_dodge
            if (Cs == 1.0f) {
                return 1.0f;
            }
            return std::min(1.0f, Cb / (1.0f - Cs));
        case 7: // color_burn
            if (Cs == 0) {
                return 0;
            }
            return (1 - std::min(1.0f, (1.0f - Cb) / Cs));
        case 8: // soft_light
            if (Cs <= 0.5f) {
                return (Cb - (1 - 2 * Cs) * Cb * (1 - Cb));
            }
            return (Cb + (2 * Cs - 1) * (D(Cb) - Cb));
        default:
            return 0; // If mode is not recognized
    }
}

void blending(string mode) {
    Mat cat_1  = loadImage("cat_1.png");
    Mat cat_2  = loadImage("cat_2.png");
    Mat cat_3    = loadImage("cat_3.png");
    Mat cat_4 = loadImage("cat_4.png");

    Mat cat_3Gray = createGrayscale(cat_3);
    Mat cat_4Gray = createGrayscale(cat_4);

    Mat result = Mat::zeros(cat_1.size(), CV_8UC3);

    for (int y = 0; y < cat_1.rows; y++) {
        for (int x = 0; x < cat_1.cols; x++) {
            Vec3b CsP = cat_1.at<Vec3b>(y, x);
            Vec3b CbP = cat_2.at<Vec3b>(y, x);
            float alphaS = cat_3Gray.at<uchar>(y, x)    / 255.0f;
            float alphaB = cat_4Gray.at<uchar>(y, x) / 255.0f;

            for (int c = 0; c < 3; c++) {
                float Cs = CsP[c] / 255.0f;
                float Cb = CbP[c] / 255.0f;
                result.at<Vec3b>(y, x)[c] = ((1 - alphaS) * alphaB * Cb + (1 - alphaB) * alphaS * Cs + alphaS * alphaB * B(Cb, Cs, mode)) * 255.0f;
            }
        }
    }

    saveImage(&result, "cat_1_" + mode + ".png");
}

void mirror() {
    Mat cat_1 = loadImage("cat_1.png");

    for (int x = 0; x < cat_1.cols / 2; x++) {
        for (int y = 0; y < cat_1.rows; y++) {
            Vec3b leftPixel = cat_1.at<Vec3b>(y, x);
            Vec3b rightPixel = cat_1.at<Vec3b>(y, cat_1.cols - x - 1);
            Vec3b copy = rightPixel;

            cat_1.at<Vec3b>(y, cat_1.cols - x - 1) = leftPixel;
            cat_1.at<Vec3b>(y, x) = copy;
        }
    }

    saveImage(&cat_1, "cat_1_mirror.png");
}

void transpon() {
    Mat cat_1 = loadImage("cat_1.png");

    Mat result = Mat::zeros(cat_1.cols, cat_1.rows, CV_8UC3);

    for (int y = 0; y < cat_1.rows; y++) {
        for (int x = 0; x < cat_1.cols; x++) {
            result.at<Vec3b>(x, y) = cat_1.at<Vec3b>(y, x);
        }
    }

    saveImage(&result, "cat_1_transpon.png");
}

int main() {
    grayInCircle();
    
    for (int i = 0; i < sizeof(BlendMode); i++) {
        blending(BlendMode[i]);
    }

    mirror();
    transpon();
    waitKey(0);
    return 0;
}


