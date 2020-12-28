#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/highgui.hpp>   // for imshow
#include <opencv2/imgcodecs.hpp> // for imread
using namespace cv;
using namespace std;

void load_data(string& path, vector<Mat>& images) {
    vector<string> file_names;
    cv::glob(path, file_names);
    if(file_names.size()==0) exit;
    size_t s = file_names.size();
    images.resize(s);
    cout << "Start loading" << endl;
    for (size_t i = 0; i < s; ++i) {
        images[i] = imread(file_names[i]);
    }
    cout << "End loading" << endl;
}

int main() {
    string img_folder_path = "*.png";
    vector<Mat> images;
    load_data(img_folder_path, images);

    Mat result;
    for (int g = 0; g < images.size(); g++) {
        // imshow("test", img);
        // waitKey(0);
        Mat img = images[g].clone();

        Mat src;
        src=img.clone();
        // 中值滤波
        Mat mediandst, dst;
        medianBlur(src, mediandst, 3);

        cvtColor(mediandst, mediandst, COLOR_BGR2GRAY);		//转为灰度图像

        //霍夫圆检测
        vector<Vec3f>pcircles;
        HoughCircles(mediandst, pcircles, HOUGH_GRADIENT, 1, 100, 100, 30, 50, 400);
        src.copyTo(dst);
        Mat roi_img;
        for (size_t i = 0; i < pcircles.size(); i++) {
            Vec3f cc = pcircles[i];
            //cc[2]圓半徑
            circle(dst, Point(cc[0], cc[1]), cc[2], Scalar(0, 0, 255), 2, LINE_AA);
            //圓心
            circle(dst, Point(cc[0], cc[1]), 2, Scalar(125, 25, 255), 2, LINE_AA);

            //cut circle
            
            Rect rect(cc[0]-cc[2], cc[1]- cc[2], cc[2]*2, cc[2]*2);

            src(rect).copyTo(roi_img);

            // imshow("roi_img", roi_img);
            // waitKey(0);
            break;
        }
        if(g==0)
        {
            Mat out(roi_img.size(), roi_img.type(), Scalar(255,255,255));
            result = out.clone();
            // imshow("fuck", result);
            // waitKey(0);
        }
        for (int x = 0; x < roi_img.rows; x++)
        {
            for (int y = 0; y < roi_img.cols; y++)
            {
                if(!(roi_img.data[(x * roi_img.cols * 3) + (y * 3) + 0] >200&&
                     roi_img.data[(x * roi_img.cols * 3) + (y * 3) + 1] >200&&
                     roi_img.data[(x * roi_img.cols * 3) + (y * 3) + 2] >200 ))//not white
                {
                    if (x >= result.cols || y >= result.rows)continue;
                    if(result.data[(x * result.cols * 3) + (y * 3) + 0] <50&&
                         result.data[(x * result.cols * 3) + (y * 3) + 1] <50&&
                         result.data[(x * result.cols * 3) + (y * 3) + 2] >200)//is red
                    {
                         
                        result.data[(x * result.cols * 3) + (y * 3) + 0] = 26;
                        result.data[(x * result.cols * 3) + (y * 3) + 1] = 26;
                        result.data[(x * result.cols * 3) + (y * 3) + 2] = 139;
                    }
                    else
                    {
                        result.data[(x * result.cols * 3) + (y * 3) + 0] = roi_img.data[(x * roi_img.cols * 3) + (y * 3) + 0];
                        result.data[(x * result.cols * 3) + (y * 3) + 1] = roi_img.data[(x * roi_img.cols * 3) + (y * 3) + 1];
                        result.data[(x * result.cols * 3) + (y * 3) + 2] = roi_img.data[(x * roi_img.cols * 3) + (y * 3) + 2];
                    }
                }
                
                
            }
        }
        imwrite("result.png", result);
        // imshow("fuc2k", result);
        // waitKey(0);
        // imshow("mediandst", mediandst);
        // imshow("dst", dst);
        // waitKey(0);
    }
}