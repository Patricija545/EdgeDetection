#include <opencv2/opencv.hpp>
#include <string>
#include <tgmath.h> 

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


int main( int argc, char** argv )
{
  //učitavanje slike
  string imgName = "laplacian1.jpg";
  Mat input = imread(filePath (imgName), CV_LOAD_IMAGE_GRAYSCALE);

  //Gaussov filter
  Mat Gauss;
  GaussianBlur(input, Gauss, cv::Size(5,5), 2, 0, BORDER_DEFAULT );
  
  //Derivacijske matrice za Laplasijan detektor ruba
  // POZITIVAN - detektira vanjske rubove
   Mat poz1 = (Mat_<float>(3,3) <<
   0, 1, 0,
   1, -4, 1,
   0, 1, 0);
   
   Mat poz2 = (Mat_<float>(3,3) <<
   1, 1, 1,
   1, -8, 1,
   1, 1, 1);
   
   // NEGATIVAN - detektira unutrašnje rubove
   Mat neg1 = (Mat_<float>(3,3) <<
   0, -1, 0,
   -1, 4, -1,
   0, -1, 0);
   
   Mat neg2 = (Mat_<float>(3,3) <<
   -1, -1, -1,
   -1, 8, -1,
   -1, -1, -1);
  
  //filter2D
  Mat vanjskirubovi, unutarnjirubovi; 
  int ddepth;
  double delta;
  Point anchor;
  
  anchor = Point (-1, -1);
  delta = 0;
  ddepth = -1;
  filter2D(input, vanjskirubovi, ddepth , poz1, anchor, delta, BORDER_DEFAULT );
  filter2D(input, unutarnjirubovi, ddepth , neg1, anchor, delta, BORDER_DEFAULT );
  
  
  namedWindow ( "Izvorna slika",  CV_WINDOW_AUTOSIZE );
  imshow ( "Izvorna slika", input );
  displayImages( make_pair("Pozitivan Laplasijan", vanjskirubovi), make_pair ("Negativan Laplasijan", unutarnjirubovi));

  //spremanje novih slika
//    try {
// 	imwrite( "../../output/pozLaplasijan.png", vanjskirubovi);
//	imwrite( "../../output/negLaplasijan.png", unutarnjirubovi);
//     }
//     catch (runtime_error& ex) {
//         fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
//         return 1;
//     }

  waitKey(0);

  
  return 0;
}
