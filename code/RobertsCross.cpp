#include <opencv2/opencv.hpp>
#include <string>
#include <tgmath.h> 
#include <fstream>

#include "pathconfig.h"
#include <chrono>

using namespace cv;
using namespace std;

void displayImages ( pair<string, Mat> input, pair<string, Mat> output )
{
    namedWindow ( input.first, CV_WINDOW_AUTOSIZE );
    namedWindow ( output.first, CV_WINDOW_AUTOSIZE );

    imshow ( input.first, input.second );
    imshow ( output.first, output.second );
}

Mat RobertsCross ( Mat Gx, Mat Gy, Mat& input, int size )
{
    Mat output = Mat::zeros ( input.size(), input.type() );
    
    double pixelX, pixelY;
    double Gx_sum = 0, Gy_sum = 0, G; 
    double tmpX, tmpY;
  
    //iteracija po slici
    for ( int y = 0; y < input.rows; ++y )
    {
        for ( int x = 0; x < input.cols; ++x )
        {

            tmpX = x; tmpY = y;
	    
	    //iteracija po kernelu
            for ( int yk = 0; yk <size; ++yk )
            {
                for ( int xk = 0; xk < size; ++xk )
                {

		     //rubni uvjeti
                     if ( x == 0 ) tmpX =  x + 1;
                     if ( y == 0 ) tmpY =  y + 1;
                     if ( x == ( input.rows-1 ) ) tmpX = x - 1;
                     if ( y == ( input.cols-1 ) ) tmpY = y - 1;


                     pixelX = input.at<uchar> ( Point ( xk+tmpX-1, yk+tmpY-1 ) ) * Gx.at<float> ( Point ( xk, yk ) );
                     Gx_sum = Gx_sum + pixelX;
			
		     pixelY = input.at<uchar> ( Point ( xk+tmpX-1, yk+tmpY-1 ) ) * Gy.at<float> ( Point ( xk, yk ) );
		     Gy_sum = Gy_sum + pixelY;
                }
            }

            //magnituda gradijenta
            G = abs(Gx_sum) + abs(Gy_sum);
	    output.at<uchar> (Point (x, y))  = G;
   
	     Gx_sum = 0;
	     Gy_sum = 0;
    
	}

    }

    return output;

}



Mat MakeKernel (int size, Mat& kernel) 
{

  double sum = 0;

  for ( int y = 0; y < size; ++y) 
  {
    for ( int x = 0; x < size; ++x) 
    {
	 if ( kernel.at<float> (Point (y, x)) >= 0) {sum = sum + kernel.at<float> (Point (y, x));}
	 else {sum = sum + (kernel.at<float> (Point (y, x)) * -1);}
     } 
  }
  
  
  for (int y = 0; y < size; ++y) 
  {
    for (int x = 0; x < size; ++x) 
    {
	  kernel.at<float>(Point (y, x)) = kernel.at<float>(Point (y, x)) / sum;
    }
    
  }

  return kernel;
  
}

int main( int argc, char** argv )
{
  //Učitavanje slike
  string imgName = "sova.png";
  Mat input = imread(filePath (imgName), CV_LOAD_IMAGE_GRAYSCALE);
  
  //Gaussov filter
  Mat Gauss;
  GaussianBlur(input, Gauss, cv::Size(5,5), 1, 0, BORDER_DEFAULT );
   
   
  //Derivacijske maske Robert's cross detektora rubova
  int kernelsize = 2;
  Mat KernelX = (Mat_<float>(2,2) << 1, 0, 0, -1);
  Mat KernelY = (Mat_<float>(2,2) << 0, 1, -1, 0);
  KernelX = MakeKernel (kernelsize, KernelX);
  KernelY = MakeKernel (kernelsize, KernelY);
  
  //Vrijeme izvršavanja funkcije za detekciju rubova
  auto start = chrono::steady_clock::now();
  Mat robertscross = RobertsCross (KernelX, KernelY, input, kernelsize);
  auto end = chrono::steady_clock::now();
  auto diff = end- start;
  cout << chrono::duration<double, milli>(diff).count() << " ms" << endl;
   
//  try {
// 	imwrite( "../../output/rcross_output.png", robert);
// 	
//     }
//     catch (runtime_error& ex) {
//         fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
//         return 1;
//     }
//   
   
   
  displayImages( make_pair("input", input), make_pair ("Robert's cross filter", robertscross));

  waitKey(0);

  
  return 0;
}
