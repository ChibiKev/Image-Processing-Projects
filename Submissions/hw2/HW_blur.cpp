#include "IP.h"
using namespace IP;
ImagePtr RowBlur(ImagePtr I1, int filterW);
ImagePtr ColumnBlur(ImagePtr I3,int filterH);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
     ImagePtr I3, I4;
     if (filterW % 2 == 0) filterW++; // Makes sure filterW is odd
     if (filterH % 2 == 0) filterH++; // Makes sure filterW is odd
     I3 = RowBlur(I1, filterW);
     I4 = ColumnBlur(I3, filterH);
     // copy image header (width, height) of input image I4 to output image I2
     IP_copyImageHeader(I4, I2);

     // init vars for width, height, and total number of pixels
     int w = I4->width();
     int h = I4->height();
     int total = w * h;

     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> in, out;
     int type;

     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I4, ch, in, type); ch++) {	// get input  pointer for channel ch
          IP_getChannel(I2, ch, out, type);		// get output pointer for channel ch
          for (int i = 0; i < total; i++) {
               *out++ = *in++;
          }
     }
}

ImagePtr RowBlur(ImagePtr I1, int filterW) {
     ImagePtr I3;
     // copy image header (width, height) of input image I1 to output image I3
     IP_copyImageHeader(I1, I3);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> in, out;
     int type;

     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {	// get input  pointer for channel ch
          IP_getChannel(I3, ch, out, type);		// get output pointer for channel ch
          int padding = filterW / 2; // Padding Needed (Left and Right of Current)
          int paddingMaxWidth = padding + w + padding; // Max Width Needed For Buffer
          short* buffer = new short[paddingMaxWidth]; // Set Up Buffer Size
          for (int i = 0; i < h; i++) { // Blur Row By Row
               for (int start = 0; start < padding; start++) { // Left Padding Is Set To Have The First Value Of Original Image
                    buffer[start] = *in;
               }
               for (int middle = padding; middle < w + padding; middle++) { // No Padding, Has Original Values
                    buffer[middle] = *in++;
               }
               for (int end = w + padding; end < paddingMaxWidth; end++) { // Right Padding Is Set To Have the Last Value of Original Image
                    buffer[end] = in[-1];
               }
               int sum = 0; // Sum = 0
               for (int k = 0; k < filterW; k++) { // Summing Each Value
                    sum += buffer[k];
               }
               for (int k = padding; k < w + padding; k++) { // Getting Output
                    *out++ = sum / filterW; // Get The Unweighted Average
                    sum += buffer[k + padding + 1] - buffer[k - padding]; // Subtracted The Unnessarry So Next Sum Will Work.
                    // Limited Excursion Reduce Size Of Output
               }
          }
          delete[] buffer; // Delete Buffer To Save Memory
     }
     return I3;
}

ImagePtr ColumnBlur(ImagePtr I3, int filterH) {
     ImagePtr I4;
     // copy image header (width, height) of input image I3 to output image I4
     IP_copyImageHeader(I3, I4);

     // init vars for width, height, and total number of pixels
     int w = I3->width();
     int h = I3->height();
     int total = w * h;

     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> in, out;
     int type;

     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I3, ch, in, type); ch++) {	// get input  pointer for channel ch
          IP_getChannel(I4, ch, out, type);		// get output pointer for channel ch
          int padding = filterH / 2; // Padding Needed (Up and Down of Current)
          int paddingMaxHeight = padding + h + padding; // Max Height Needed For Buffer
          short* buffer = new short[paddingMaxHeight]; // Set Up Buffer Size
          ChannelPtr<uchar> temp;
          ChannelPtr<uchar> temp2;
          for (int i = 0; i < w; i++) { // Blur Column By Column
               temp = in++; // Store Current In Temp
               temp2 = out++; // Store Output In Temp2 So Out Doesn't Get Modified When Adding Width 
               for (int start = 0; start < padding; start++) { // Top Padding Is Set To Have The First Value Of Original Image
                    buffer[start] = *temp;
               }
               for (int middle = padding; middle < h + padding; middle++) { // No Padding, Has Original Values
                    buffer[middle] = *temp;
                    if (middle < padding + h - 1) { // Only Update Value Up Until The Last. Don't Update The Last.
                         temp = temp + w;
                    }
               }
               for (int end = h + padding; end < paddingMaxHeight; end++) { // Bottom Padding Is Set To Have the Last Value of Original Image
                    buffer[end] = *temp;
               }
               int sum = 0; // Sum = 0
               for (int k = 0; k < filterH; k++) { // Summing Each Value
                    sum += buffer[k];
               }
               for (int k = padding; k < h + padding; k++) { // Getting Output
                    *temp2 = sum / filterH; // Get The Unweighted Average
                    sum += buffer[k + 1 + padding] - buffer[k - padding]; // Subtracted The Unnessarry So Next Sum Will Work.
                    temp2 += w; // Goes Onto Next Line/Row
                    // Limited Excursion Reduce Size Of Output
               }
          }
          delete[] buffer; // Delete Buffer To Save Memory
     }
     return I4;
}