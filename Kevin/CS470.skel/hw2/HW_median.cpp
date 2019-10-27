#include <algorithm>
#include <vector>
#include "IP.h"
using namespace IP;
using std::vector;
void copyRowToCircBuffer(int row, short* buf[], ChannelPtr<uchar> start, int padding, int w, int h);
void printCircBuffer(short* buf[], int maxw);
int vectormid(vector<int> findmid);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size sz x sz to I1.
// Clamp sz to 9.
// Output is in I2.
//
void
HW_median(ImagePtr I1, int sz, ImagePtr I2)
{
     // copy image header (width, height) of input image I4 to output image I2
     IP_copyImageHeader(I1, I2);

     // init vars for width, height, and total number of pixels
     int w = I1->width();
     int h = I1->height();
     int total = w * h;

     // declarations for image channel pointers and datatype
     ChannelPtr<uchar> in, out, start;
     int type;
     if (sz % 2 == 0) sz++; // Makes Sure sz Is Odd
     int padding = sz / 2; // Get Padding By Dividing sz By 2
     int paddingMax = padding + w + padding; // Max Padding
     //Our 2 buffers used for padding
     short *buffer[2]; //array of pointers
     buffer[0] = new short[paddingMax];
     buffer[1] = new short[paddingMax];

     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {	// get input  pointer for channel ch
          IP_getChannel(I2, ch, out, type);		// get output pointer for channel ch
          start = in;
          copyRowToCircBuffer(0, buffer, start, padding, w, h);
          for (int y = 0; y < h; y++) {
               copyRowToCircBuffer(y + 1, buffer, start, padding, w, h);
               //printf("start: %d\n", *start);
               //printCircBuffer(buffer, paddingMax);
               short *in1 = buffer[y % 2] + padding; //current row
               short *in2 = buffer[(y + 1) % 2] + padding; // row below
               short *in3 = buffer[(y - 1) % 2] + padding; // row above
               for (int x = 0; x < h; x++) {
                    vector<int> findmid;
                    for (int i = -padding; i < padding + 1; i++) {
                         findmid.push_back(in1[i]);
                         findmid.push_back(in2[i]);
                         findmid.push_back(in3[i]);
                    }
                    int mid = vectormid(findmid);
                    findmid.clear();
                    //printf("in1: %d, in2: %d, in3: %d, mid: %d \n", *in1, *in2, *in3, mid);
                    *out++ = mid;
                    in1++;
                    in2++;
                    in3++;
               }
          }
     }
}

void copyRowToCircBuffer(int row, short* buf[], ChannelPtr<uchar> start, int padding, int w, int h) {
     if (row >= h) return; //Invalid row to copy [0,h-1]
     ChannelPtr<uchar> original = start; //saving original location
     short *ptr = buf[row % 2]; //pointer to correct buffer
     start = (start + w * row); //making start point to the right row
     for (int i = padding; i < w + padding; i++) {
          ptr[i] = *start++;
     }
     start = original; //preserving start
}

void printCircBuffer(short* buf[], int maxw) {
     short *ptr1 = buf[0];
     short *ptr2 = buf[1];
     int i;
     for (i = 0; i < maxw; i++) {
          std::cout << ptr1[i] << " ";
     }
     std::cout << std::endl;
     for (i = 0; i < maxw; i++) {
          std::cout << ptr2[i] << " ";
     }
     std::cout << std::endl;
     std::cout << std::endl;
}

int vectormid(vector<int> findmid) {
     sort(findmid.begin(), findmid.end());
     for (int i = 0; i < findmid.size(); i++) {
          //printf("%d ", findmid[i]);
     }
     //printf("\n");
     return findmid[findmid.size() / 2];
}
