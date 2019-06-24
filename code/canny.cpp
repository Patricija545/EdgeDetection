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

Mat SobelFilter ( Mat Gx, Mat Gy, Mat& input, int size, Mat direction)
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
	    //alternative: G = sqrt(ceil((Gx_sum*Gx_sum) + (Gy_sum*Gy_sum)));
            output.at<uchar> ( Point (x, y) )= G;
	    
	    //smjer piksela
	    direction.at<float>(Point (x, y)) = atan (Gy_sum / Gx_sum);
            Gx_sum = 0;
	    Gy_sum = 0;
    
	}
	
    }
    
    return output;

}


int find_angle (float angle) {
  if (angle > 0 && angle <=33.75) return 0;
  else if (angle > 33.75 && angle <=78.75) return 45;
  else if (angle > 78.75 && angle <=123.75) return 90;
  else return 135;
}


void Non_maximum_suppression (Mat &output, Mat& direction) {
  int theta;
  uchar piksel;
   
    for (int x = 0; x < output.rows; ++x)  {
	for (int y = 0; y < output.cols; ++y) {
	      
	  piksel = output.at<uchar>(Point (x, y)); 
		     
	  
              if (x!=0 && y!=0 && x!=output.rows && y!=output.cols) {
		theta = find_angle (direction.at<float>(Point (x, y)));
		
		if (theta == 0) {
		    if (!(piksel > output.at<uchar>(Point (x-1, y)) && piksel > output.at<uchar>(Point (x+1, y)))) 
		    {
		      output.at<uchar>(Point (x, y)) = 0;
		    }
		}
		
		else if (theta == 45) {
		    if (!(piksel > output.at<uchar>(Point (x, y-1)) && piksel > output.at<uchar>(Point (x, y+1)))) 
		    {
		      output.at<uchar>(Point (x, y)) = 0;
		    }
		}
		
		else if (theta == 90) {
		    if (!(piksel > output.at<uchar>(Point (x-1, y+1)) && piksel > output.at<uchar>(Point (x+1, y-1)))) 
		    {
		      output.at<uchar>(Point (x, y)) = 0;
		    }
		}
		
		else {
		   if (!(piksel > output.at<uchar>(Point (x-1, y-1)) && piksel > output.at<uchar>(Point (x+1, y+1)))) 
		    {
		      output.at<uchar>(Point (x, y)) = 0;
		    }
		}
		
		
	      }
		
		  
	}
    }
  
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


Mat Double_threshold ( Mat& output, Mat& strength_of_pixel, int low, int high )
{   
    for ( int y = 0; y < output.rows; ++y )
    {
        for ( int x = 0; x < output.cols; ++x )
        {
	  
		//0-not edge, 1-weak, 2-strong
		if (output.at<uchar>(Point (x, y)) > high)  
		{
		    strength_of_pixel.at<uchar>(Point (x, y)) = 2;
		    output.at<uchar>(Point (x, y)) = 255;
		}
		
		else if (output.at<uchar>(Point(x,y)) < high && output.at<uchar>(Point (x, y)) > low) 
		{
		  strength_of_pixel.at<uchar>(Point (x, y)) = 1;
		  output.at<uchar>(Point (x, y)) = 100;
		}
		
		else {
		  strength_of_pixel.at<uchar>(Point (x, y)) = 0;
		  output.at<uchar>(Point(x, y)) = 0;
		}
		
	}
    }
    
    
    return output;

}


Mat Hysteresis ( Mat& output, Mat& strength_of_pixel)
{  
    uchar strength;
   
    for (int x = 0; x < output.rows; ++x) 
    {
      for (int y = 0; y < output.cols; ++y) 
      {
	
	strength = strength_of_pixel.at<uchar>(Point (x,y));
	
	
	  if (strength == 1) 
	  {
	    for (int i = -1; i <= 1 ; i++) 
	    {
	      for (int j = -1; j <= 1 ; j++) 
	      {
		
		      if (strength_of_pixel.at<uchar>(Point (x+i,y+j)) == 2)
		      {
			output.at<uchar>(Point (x, y)) = 255;
		      }
		      
		      else output.at<uchar>(Point (x, y)) = 0;
	      }
	    }

	    
	  }
	
      }
    }
    
    
    return output;

}


int main( int argc, char** argv )
{
  //Učitavanje slike
  string imgName = "slika3.png";
  Mat input = imread(filePath (imgName), CV_LOAD_IMAGE_GRAYSCALE);

  //1. Gaussian filter
  Mat Gauss;
  GaussianBlur(input, Gauss, cv::Size(5,5), 1, 0, BORDER_DEFAULT );
  
  //2. Sobel detektor ruba  
  int kernelsize = 3;
  Mat kernelX = (Mat_<float>(3,3) << -1, 0 , 1, -2, 0, 2, -1, 0, 1);
  Mat kernelY = (Mat_<float>(3,3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
  kernelX = MakeKernel(kernelsize, kernelX);
  kernelY = MakeKernel(kernelsize, kernelY);
  
  Mat direction = Mat::zeros ( input.size(), DataType<float>::type );
  Mat sobelMat = SobelFilter (kernelX, kernelY, input, kernelsize, direction);
   
  
  // 3. Odstranjivanje piksela koji nisu lokalno maksimalni
  Non_maximum_suppression(sobelMat, direction);
  Mat strength_of_pixel = Mat::zeros (input.size(), input.type());
  
  //4. Dupli prag
  Double_threshold(sobelMat, strength_of_pixel, 5, 20);
   
  //5. Praćenje ruba putem histereze
  Hysteresis (sobelMat, strength_of_pixel);
  
//   try {
// 	imwrite( "../../output/canny5_output.png", sobelMat );
//     }
//     catch (runtime_error& ex) {
//         fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
//         return 1;
//     }

  displayImages( make_pair("input", input), make_pair ("Canny Edge Detection", sobelMat));
    

  waitKey(0);

  
  return 0;
}