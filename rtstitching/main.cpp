#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/stitching.hpp>

using namespace std;
using namespace cv;

bool try_use_gpu = false;
vector<Mat> imgs;
string result_name = "/Users/hehao/work/rtcontroller/bin/picture12/result.jpg";

void printUsage();
int parseCmdArgs(int argc, char** argv);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    parseCmdArgs(argc, argv);
    {
        vector<Mat> part1( imgs.begin()+20, imgs.end() );
        //part1.insert(part1.end(), imgs.begin(), imgs.begin()+8);
        Mat pano;
        Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
        Stitcher::Status status = stitcher.stitch(part1, pano);
        if (status != Stitcher::OK)
        {
            cout << "Can't stitch images, error code = " << int(status) << endl;
            return -1;
        }
        QString name = QString("/Users/hehao/work/rtcontroller/bin/picture12/result.jpg");
        imwrite(name.toStdString(), pano);
        return 0;
    }

    for(int i=0; i<3; ++i) {
        vector<Mat> part1( imgs.begin()+9*i, imgs.begin()+9*(i+1) );
        Mat pano;
        Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
        Stitcher::Status status = stitcher.stitch(part1, pano);

        if (status != Stitcher::OK)
        {
            cout << "Can't stitch images, error code = " << int(status) << endl;
            return -1;
        }
        QString name = QString("/Users/hehao/work/rtcontroller/bin/picture12/result%1.jpg").arg(i);
        imwrite(name.toStdString(), pano);

    }

    MainWindow w;
    w.show();

    return a.exec();
}

int parseCmdArgs(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    int i;
    for(i=1; i<=30; ++i) {
        QString p=QString("%1").arg(i);
        QString name=QString("/Users/hehao/work/rtcontroller/bin/picture12/%1.BMP").arg(p, 3, '0');
        Mat img = imread(name.toStdString());
        if(!img.empty()) {
            qDebug()<<name;
            imgs.push_back(img);
        }
    }

    return 0;
}
