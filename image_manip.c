#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "image_manip.h"
#include "ppm_io.h"

/* HELPER for grayscale:
 * convert a RGB pixel to a single grayscale intensity;
 * uses NTSC standard conversion
 */
unsigned char pixel_to_gray(const Pixel *p) {
  return (unsigned char)( (0.3 * (double)p->r) +
                          (0.59 * (double)p->g) +
                          (0.11 * (double)p->b) );
}

/* ______grayscale______
 * convert an image to grayscale (NOTE: pixels are still
 * RGB, but the three values will be equal)
 */
void grayscale(Image *im) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - grayscale given a bad image pointer\n");
    return;
  }

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      // find grayscale intensity
      unsigned char grayLevel = pixel_to_gray(&(im->data[(r*im->cols)+c]));

      // set all chanels to it
      im->data[(r*im->cols)+c].r = grayLevel;
      im->data[(r*im->cols)+c].g = grayLevel;
      im->data[(r*im->cols)+c].b = grayLevel;

    }
  }
}

/* ______swap______
 * swap color channels of an image
 */
void swap(Image *im){
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - swap given a bad image pointer\n");
    return;
  }

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      // create temp variables for rgb and effectively swap them  
      Pixel curr = im->data[(r*(im->cols))+c];
      unsigned char oldRed = curr.r;
      unsigned char oldGreen= curr.g;
      unsigned char oldBlue = curr.b;

      im->data[(r*(im->cols))+c].r = oldGreen;
      im->data[(r*(im->cols))+c].g = oldBlue;
      im->data[(r*(im->cols))+c].b = oldRed;
    }
  }
}
 

/* ______invert______
 * invert the intensity of each channel
 */

 void invert(Image *im)
 {
    if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - invert given a bad image pointer\n");
    return;
  }

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      // set to inverse
      im->data[(r*im->cols)+c].r = 255 - im->data[(r*im->cols)+c].r;
      im->data[(r*im->cols)+c].g = 255 - im->data[(r*im->cols)+c].g;
      im->data[(r*im->cols)+c].b = 255 - im->data[(r*im->cols)+c].b;
    }
  }
 }

/* ______zoom_out______
 * "zoom out" an image, by taking a 2x2 square of pixels and averaging
 * each of the three color channels to make a single pixel. If an odd
 * number of rows in original image, we lose info about the bottom row.
 * If an odd number of columns in original image, we lose info about the
 * rightmost column.
 */
Image *zoomout(Image *im){
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - zoomout given a bad image pointer\n");
    return im;
  }

  // calculate space to properly allocate
  // if odd # of rows or cols, the last row/col will be disregarded
  // ex: 13 rows / 2 = 6 rows
  int rows = im->rows / 2;
  int cols = im->cols / 2;

  // allocate space for new image
  Image *newIm = malloc(sizeof(Image));
  newIm->rows=rows;
  newIm->cols=cols;
  newIm->data = malloc(sizeof(Pixel) * rows * cols);

  // traverse through 2x2 pixels and create new pixel
  // elimate odd row and column
  if(im->rows % 2 != 0){
    im->rows= (im->rows)-1;
  }
   if(im->cols % 2 != 0){
    im->cols = (im->cols)-1;
  }

  // counter for the index of the 2x2 pixel
  int count=0;
  for(int r=0;r<im->rows;r++){
    for(int c=0;c<im->cols;c++){
      unsigned char red = im->data[(r*im->cols)+c].r;
      unsigned char red2 = im->data[(r*im->cols)+c+1].r;
      unsigned char red3 = im->data[((r+1)*im->cols)+c].r;
      unsigned char red4 = im->data[((r+1)*im->cols)+c+1].r;

      unsigned char green = im->data[(r*im->cols)+c].g;
      unsigned char green2 = im->data[(r*im->cols)+c+1].g;
      unsigned char green3 = im->data[((r+1)*im->cols)+c].g;
      unsigned char green4 = im->data[((r+1)*im->cols)+c+1].g;

      unsigned char blue = im->data[(r*im->cols)+c].b;
      unsigned char blue2 = im->data[(r*im->cols)+c+1].b;
      unsigned char blue3 = im->data[((r+1)*im->cols)+c].b;
      unsigned char blue4 = im->data[((r+1)*im->cols)+c+1].b;

      // average values
      newIm->data[count].r=(red+red2+red3+red4)/4;
      newIm->data[count].g=(green+green2+green3+green4)/4;
      newIm->data[count].b=(blue+blue2+blue3+blue4)/4;
      count+=1;

      // increment to skip an additional column
      c+=1;
    }
    r+=1;
  }

  // free original image before returning new one
  free_image(&im);
  return newIm;
}

/* _______rotate-right________
 * rotate the input image clockwise 90 degrees
 */

Image *rotateright(Image *im){
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - rotateright given a bad image pointer\n");
    return im;
  }

  // realloc approporiate space
  int rows = im->rows;
  int cols = im-> cols;
  Image *newIm = malloc(sizeof(Image));
  newIm->rows=cols;
  newIm->cols=rows;
  newIm->data = malloc(sizeof(Pixel) * rows * cols);

  // transposing a right-rotated version of our image to our new image
  // reverse first column of old = first row of new
  int count=0;
  for (int c = 0; c < cols; c++){
    for (int r = rows-1; r>=0; r--) {
      // assign new pixels
      newIm->data[count].r = im->data[(r*im->cols)+c].r;
      newIm->data[count].g = im->data[(r*im->cols)+c].g;
      newIm->data[count].b = im->data[(r*im->cols)+c].b;
      count+=1;
    }
  }
  //freeing original image
  free_image(&im);
  //returning new image
  return newIm;
}



/* ________Swirl effect_________
 * Create a whirlpool effect!
 */

 // note: take in double parameters to prevent integer division
Image *swirl(Image *im, double cx, double cy, double s) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - swirl given a bad image pointer\n");
    return im;
  }

  if(cx<-1 || cy<-1 || s<0){
    fprintf(stderr, "Error:image_manip - swirl given a negative center coordinates or a negative scale\n");
    return im;
  }

  // allocate new image
  Image *newIm=malloc(sizeof(Image));
  newIm->cols=im->cols;
  newIm->rows=im->rows;
  newIm->data=malloc(sizeof(Pixel)*im->rows*im->cols);

  // perform swirl
  // if conditions for -1 special condition
  if(cx==-1){
    cx = im->cols/2.0;
  }
  if(cy==-1){
    cy = im->rows/2.0;
  }

  for(int r=0;r<im->rows;r++){
    for(int c=0;c<im->cols;c++){
      double a = (sqrt((c-cx)*(c-cx)+(r-cy)*(r-cy)))/s;

      // the coordinates for the swirl image
      int sC = (c-cx)*cos(a)-(r-cy)*sin(a)+cx;
      int sR = (c-cx)*sin(a)+(r-cy)*cos(a)+cy;
      
      // if pixel is not outside the range, add approporiate pixel
      if(!(sR>im->rows-1 || sC>im->cols-1 || sC<0 || sR<0)){
        newIm->data[(r*im->cols)+c].r = im->data[(sR*im->cols)+sC].r;
        newIm->data[(r*im->cols)+c].g = im->data[(sR*im->cols)+sC].g;
        newIm->data[(r*im->cols)+c].b = im->data[(sR*im->cols)+sC].b;
      }
      else{
        newIm->data[(r*im->cols)+c].r=0;
        newIm->data[(r*im->cols)+c].g=0;
        newIm->data[(r*im->cols)+c].b=0;
      }
    }
  }

  free_image(&im);
  return newIm;
}

/* _______edges________
 * apply edge detection as a grayscale conversion
 * followed by an intensity gradient computation and
 * thresholding
 */
Image *edgeDetection(Image *im, int threshold) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - edge_detection given a bad image pointer\n");
    return im;
  }
  grayscale(im);
  Image* newIm = make_image(im->rows, im->cols);

  for(int c=1;c<im->cols-1;c++){
    for(int r=1;r<im->rows-1;r++){
      double x = 1.0 * (im->data[(r*im->cols)+c+1].r - im->data[(r*im->cols)+c-1].r)/2;
      double y = 1.0 * (im->data[((r+1)*im->cols)+c].r - im->data[((r-1)*im->cols)+c].r)/2;
      double magnitude = sqrt(x*x+y*y);
      
      if (magnitude > threshold) {
        newIm->data[(r*im->cols)+c].r = 0;
        newIm->data[(r*im->cols)+c].g = 0;
        newIm->data[(r*im->cols)+c].b = 0;
      }
      else {
        newIm->data[(r*im->cols)+c].r = 255;
        newIm->data[(r*im->cols)+c].g = 255;
        newIm->data[(r*im->cols)+c].b = 255;
      }
    }
  }

  // manually set boundaries to avoid memory errors
  for(int i=0;i<im->cols;i++){
    newIm->data[i].r = im->data[i].r;
    newIm->data[i].g = im->data[i].g;
    newIm->data[i].b = im->data[i].b;
  }
  for(int i=0;i<im->cols;i++){
    newIm->data[(im->rows-1)*im->cols+i].r = im->data[(im->rows-1)*im->cols+i].r;
    newIm->data[(im->rows-1)*im->cols+i].g = im->data[(im->rows-1)*im->cols+i].g;
    newIm->data[(im->rows-1)*im->cols+i].b = im->data[(im->rows-1)*im->cols+i].b;
  }
  for(int i=0;i<im->rows;i++){
    newIm->data[i*(im->cols)].r = im->data[i*(im->cols)].r;
    newIm->data[i*(im->cols)].g = im->data[i*(im->cols)].g;
    newIm->data[i*(im->cols)].b = im->data[i*(im->cols)].b;
  }
  for(int i=0;i<im->rows;i++){
    newIm->data[i*(im->cols)+(im->cols-1)].r = im->data[i*(im->cols)+(im->cols-1)].r;
    newIm->data[i*(im->cols)+(im->cols-1)].g = im->data[i*(im->cols)+(im->cols-1)].g;
    newIm->data[i*(im->cols)+(im->cols-1)].b = im->data[i*(im->cols)+(im->cols-1)].b;
  }

  free_image(&im);
  return newIm;
}
 