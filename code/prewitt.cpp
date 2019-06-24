#include <opencv2/opencv.hpp>
#include <string>
#include <tgmath.h> 
#include <chrono>

#include "pathconfig.h"

using namespace cv;
using namespace std;

void displayImages ( pair<string, Mat> input, pair<string, Mat> output )
{
    namedWindow ( input.first,  CV_WINDOW_AUTOSIZE );
    namedWindow ( output.first,  CV_WINDOW_AUTOSIZE );

    imshow ( input.first, input.second );
    imshow ( output.first, output.second );
}

Mat Prewitt ( Mat kernelX, Mat kernelY, Mat& input, int size )
{
    Mat output = Mat::zeros ( input.size(), input.type() );

    double pixelX, pixelY;
    double sumX = 0, sumY = 0;
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


                     pixelX = input.at<uchar> ( Point ( xk+tmpX-1, yk+tmpY-1 ) ) * kernelX.at<float> ( Point ( xk, yk ) );
                     sumX = sumX + pixelX;
			
		     pixelY = input.at<uchar> ( Point ( xk+tmpX-1, yk+tmpY-1 ) ) * kernelY.at<float> ( Point ( xk, yk ) );
		     sumY = sumY + pixelY;

                }
            }

            //magnituda gradijenta
            output.at<uchar> ( Point (x, y) )  = sqrt(ceil((sumX*sumX) + (sumY*sumY)));
            sumX = 0;
	    sumY = 0;
           
    
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
   
  
  //Derivacijske maske za Prewitt detektor ruba
  int kernelsize = 3;
  Mat kernelX = (Mat_<float>(3,3) << -1, 0 , 1, -1, 0, 1, -1, 0, 1);
  Mat kernelY = (Mat_<float>(3,3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
  kernelX = MakeKernel(kernelsize, kernelX);
  kernelY = MakeKernel(kernelsize, kernelY);
  
  
  //Vrijeme izvršavanja funkcije za detekciju rubova
  auto start = chrono::steady_clock::now();
  
  Mat output = Prewitt (kernelX, kernelY, input, kernelsize);
  
  auto end = chrono::steady_clock::now();
  auto diff = end- start;
  cout << chrono::duration<double, milli>(diff).count() << " ms" << endl;
   
    
//     try {
// 	imwrite( "../../output/sova_input.png", input );
//     }
//     catch (runtime_error& ex) {
//         fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
//         return 1;
//     }
  
  displayImages( make_pair("input", input), make_pair ("prewitt filter", output));
  

  waitKey(0);

  
  return 0;
}
