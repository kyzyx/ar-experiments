#include <opencv2\highgui\highgui.hpp>
#include <iostream>

using namespace std;

int testmain( int argc, char** argv )
{   
    cvNamedWindow( "KillCam", CV_WINDOW_AUTOSIZE );
    cvWaitKey(0);
    CvCapture* capture = cvCreateCameraCapture(2);
    assert(capture != NULL);
    IplImage* frame;

    while(1){
        frame = cvQueryFrame( capture ); 
        if( !frame ) break;
        cvShowImage( "KillCam", frame );
        char c = cvWaitKey(33);
        if( c == 30 ) break;
    }
    cvReleaseCapture( &capture );
    cvDestroyWindow( "KillCam" );
	return 0;
}