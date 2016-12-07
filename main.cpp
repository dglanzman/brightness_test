#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <unistd.h>
#include <stdio.h>
#include <pigpio.h>

using namespace std;
using namespace cv;

int start_h, end_h, start_v, end_v, spacing;

int mean(Mat frame) {
    long sum = 0;
    int samples = 0;
    for (int i = start_v * frame.cols + start_h;
            i < end_v * frame.cols + end_h;) {
        samples++;
        sum += frame.data[3*i] + frame.data[3*i+1] + frame.data[3*i+2];
        i = i % frame.cols + spacing > end_h ?
            i + spacing + (frame.cols - end_h) + start_h +
            (spacing-1) * frame.cols : i + spacing;
    }
    return sum / (samples * 3);
}

int main() {
    if (gpioInitialize() < 0) {
        printf("PWM initialization failed, quitting\n");
        return -1;
    }
    VideoCapture camera;
    for (;;) {
        printf("opening camera\n");
        camera.open(0); // open the default camera
        if (camera.isOpened()) break;
        sleep(5);
    }
    printf("opened camera\n");
    for (;;) {
        static int first_run = 1;
        Mat frame;
        camera >> frame;
        if (frame.empty()) continue;
        else if (first_run) {
            first_run = 0;
            start_h = frame.cols / 3;
            end_h = 2 * frame.cols / 3;
            start_v = frame.rows / 3;
            end_v = 2 * frame.rows / 3;
            spacing = 30;
        }
        printf("Got frame\n");
        int brightness = mean(frame);
        gpioPWM(12, brightness);
        printf("Brightness: %d\n", brightness);
    }
}
