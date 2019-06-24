#include <opencv2/opencv.hpp>
#include <string>
#include <tgmath.h>  
#include <iostream>
#include <fstream>

#include "pathconfig.h"

using namespace cv;
using namespace std;

void displayImages ( pair<string, Mat> input, pair<string, Mat> output )
{
    namedWindow ( input.first, CV_WINDOW_AUTOSIZE );
    namedWindow ( output.first, CV_WINDOW_AUTOSIZE );

    imshow ( input.first, input.second );
    imshow ( output.first, output.second );
}


int main ( int argc, char** argv )
{ 
    //učitavanje slike
    string imgName = "laplacian1.jpg";
    Mat input = imread ( filePath ( imgName ), CV_LOAD_IMAGE_GRAYSCALE );

    //derivacijska matrica za LoG detektor ruba
    Mat kernel = (Mat_<float>(5,5) <<
    0, 0, 1, 0, 0,
    0, 1, 2, 1, 0,
    1, 2, -16, 2, 1,
    0, 1, 2, 1, 0,
    0, 0, 1, 0, 0);
    
    Mat output = Mat::zeros(input.size(), input.type());
  
    int ddepth;
    double delta;
    Point anchor;
    
    anchor = Point (-1, -1);
    delta = 0;
    ddepth = -1;
    filter2D(input, output, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
    
    displayImages ( make_pair ( imgName, input ), make_pair ( "output", output ) );
 
    //spremanje novih slika
//    try {
// 	imwrite( "../../output/LoG.png", output);
//    }
//     catch (runtime_error& ex) {
//         fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
//         return 1;
//     }


    waitKey ( 0 );

    return 0;
}


