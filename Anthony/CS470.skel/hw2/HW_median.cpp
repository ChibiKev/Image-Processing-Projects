#include <algorithm>
#include <vector>
#include "IP.h"
using namespace IP;
using std::vector;

int FindMid(vector<int> Storage);
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
     short** buffer = new short*[sz]; // Setting 2D Buffer
     for (int i = 0; i < sz; i++) { // Setting Up Height Buffer
          buffer[i] = new short[paddingMax];
     }
     // Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
     // The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
     // It is ignored here since we assume that our input images consist exclusively of uchars.
     // IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

     // visit all image channels and evaluate output image
     for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {    // get input  pointer for channel ch
          IP_getChannel(I2, ch, out, type);        // get output pointer for channel ch
          // Beginning Set Up. From Top.
          for (int i = 0; i < padding; i++) { // Padding of Rows. Outside of Initial.
               start = in;
               for (int j = 0; j < padding; j++) buffer[i][j] = *start; // Outside Buffer Should Be Replicated
               for (int j = padding; j < padding + w; j++) buffer[i][j] = *start++; // On Top (Still Outside) Buffer Should Be Replicated
               for (int j = padding + w; j < paddingMax; j++) buffer[i][j] = start[-1]; // Outside Buffer Should Be Replicated
          }
          for (int i = padding; i < sz; i++) { // Padding of Columns. Inside of Initial. Mainly Left and Right. Only Get What We Need, To Size.
               for (int j = 0; j < padding; j++) buffer[i][j] = *in; // Left Padding Is Set To Have The First Value Of Original Image
               for (int j = padding; j < padding + w; j++) buffer[i][j] = *in++; // No Padding, Has Original Values
               for (int j = padding + w; j < paddingMax; j++) buffer[i][j] = in[-1]; // Right Padding Is Set To Have the Last Value of Original Image
          }
          vector<int> Storage; // Initize of Storage Used To Store Value To Find Median.
          for (int row = 0; row < h; row++) { // Start At The First Index of Original.
               for (int i = 0; i < sz; i++) { // Storing Values Into The Vector. Start From X
                    for (int j = 0; j < sz; j++) { // Storing Value Into The Vector. Start From Y
                         Storage.push_back(buffer[j][i]); // Top To Bottom. Left To Right
                    }
               }
               for (int column = 0; column < w; column++) { // Goes Through Pixel By Pixel, Left To Right.
                    *out++ = FindMid(Storage); // Stores Median Into Output
                    if (column < w - 1) { // Only Update Value Up Until The Last. Don't Update The Last.
                         for (int i = 0; i < sz; i++) { // Add New Column Values
                              Storage.push_back(buffer[i][column + sz]);
                         }
                         Storage.erase(Storage.begin(), Storage.begin() + sz); // Remove Old Column Values
                    }
               }
               Storage.clear(); // Clear The Vector. Next Row. Don't Need Anything Saved.
               if (row < h - padding) { // Circular Buffer, Replace Old Buffer With New Buffer
                    for (int j = 0; j < padding; j++) buffer[(row + sz - 1) % sz][j] = *in; // Left Padding Is Set To Have The First Value Of Original Image
                    for (int j = padding; j < padding + w; j++) buffer[(row + sz - 1) % sz][j] = *in++; // No Padding, Has Original Values
                    for (int j = padding + w; j < paddingMax; j++) buffer[(row + sz - 1) % sz][j] = in[-1]; // Right Padding Is Set To Have the Last Value of Original Image
               }
               else { // For The Bottom Padding. Last Rows.
                    IP_getChannel(I1, ch, start, type); // Reinitize Start, Start Is At First Index.
                    start += (h-1) * w; // Copy The Last Row, which index value is (Height - 1) * Width
                    for (int j = 0; j < padding; j++) buffer[(row + sz - 1) % sz][j] = *start; // Left Padding Is Set To Have The First Value Of Original Image
                    for (int j = padding; j < padding + w; j++) buffer[(row + sz - 1) % sz][j] = *start++;  // No Padding, Has Original Values
                    for (int j = padding + w; j < paddingMax; j++) buffer[(row + sz - 1) % sz][j] = start[-1]; // Right Padding Is Set To Have the Last Value of Original Image
               }
          }
     }
     for (int i = 0; i < sz; i++) // Delete Buffer
     {
          delete[] buffer[i];
     }
     delete[] buffer; // Delete Final Buffer
}

int FindMid(vector<int> Storage) // Used So Storage Doesn't Get Modified
{
     sort(Storage.begin(), Storage.end()); // Sort The Storage
     return Storage[Storage.size() / 2]; // Return MidPoint
}
