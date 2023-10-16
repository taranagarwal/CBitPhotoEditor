//project.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm_io.h"
#include "image_manip.h"

// Return (exit) codes
// TODO implement all these errors
#define RC_SUCCESS            0
#define RC_MISSING_FILENAME   1
#define RC_OPEN_FAILED        2
#define RC_INVALID_PPM        3
#define RC_INVALID_OPERATION  4
#define RC_INVALID_OP_ARGS    5
#define RC_OP_ARGS_RANGE_ERR  6
#define RC_WRITE_FAILED       7
#define RC_UNSPECIFIED_ERR    8

void print_usage();

int main(int argc, char* argv[]) {

  // check for mandatory command line arguments
  if (argc < 3) {
    fprintf(stderr, "Missing input/output filenames\n");
    print_usage();
    return RC_MISSING_FILENAME;
  }

  // Read in PPM file
  FILE *input = fopen(argv[1], "r");
  if(input==NULL){
    printf("Error: File open has failed; PPM is empty\n");
    fclose(input);
    return RC_OPEN_FAILED;
  }
  Image *im = read_ppm(input);
  fclose(input);

  if(im==NULL){
    printf("Error: Given PPM file is invalid\n");
    free_image(&im);
    return RC_INVALID_PPM;
  }

  if(argc < 4){
    printf("Error: Operation function not provided\n");
    free_image(&im);
    return RC_INVALID_OPERATION;
  }

  // apply approporiate function
  if(!strcmp(argv[3], "swap")){    
    // if more than 4 arguments; main file, input image, result image, and operation = 4
    if(argc != 4){
      printf("Error: Incorrect amount of parameters for the requested function\n");
      free_image(&im);
      return RC_INVALID_OP_ARGS;
    }
    swap(im);
  }
  else if(!strcmp(argv[3], "invert")){
    if(argc != 4){
      printf("Error: Incorrect amount of parameters for the requested function\n");
      free_image(&im);
      return RC_INVALID_OP_ARGS;
    }
    invert(im);
  } 
  else if(!strcmp(argv[3], "zoom-out")){
    if(argc != 4){
      printf("Error: Incorrect amount of parameters for the requested function\n");
      free_image(&im);
      return RC_INVALID_OP_ARGS;
    }
    im = zoomout(im);
  } 
  else if(!strcmp(argv[3], "rotate-right")){
    if(argc != 4){
      printf("Error: Incorrect amount of parameters for the requested function\n");
      free_image(&im);
      return RC_INVALID_OP_ARGS;
    }
    im = rotateright(im);
  } 
  // fully implement swirl and edge-detection
  else if(!strcmp(argv[3], "swirl")){
    if(argc != 7){
      printf("Error: Incorrect amount of parameters for the requested function\n");
      free_image(&im);
      return RC_INVALID_OP_ARGS;
    }
    if(atoi(argv[4]) < -1 || atoi(argv[5]) < -1 || atoi(argv[6]) < 0){
      printf("Error: Incorrect range for the parameters of swirl function");
      free_image(&im);
      return RC_OP_ARGS_RANGE_ERR;
    }
    im = swirl(im, atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
  } 
  else if(!strcmp(argv[3], "edge-detection")){
    if(argc != 5){
      printf("Error: Incorrect amount of parameters for the requested function\n");
      free_image(&im);
      return RC_INVALID_OP_ARGS;
    }
    if(atoi(argv[4]) < 0){
      printf("Error: Incorrect range for the parameters of edge function");
      free_image(&im);
      return RC_OP_ARGS_RANGE_ERR;
    }
    im = edgeDetection(im, atoi(argv[4]));
  }
  else{
    printf("Error: Given function is not listed or output file not specified\n");
    free_image(&im);
    return RC_INVALID_OPERATION;
  }

  FILE *output = fopen(argv[2], "w");
  int res = write_ppm(output, im);
  fclose(output);


  // check for writing error
  if(res==-1){
    printf("Error: Invalid image was given or there was an error in writing the file\n");
    free_image(&im);
    return RC_WRITE_FAILED;
  }

  // Free image
  free_image(&im);
  return RC_SUCCESS;
}

void print_usage() {
  printf("USAGE: ./project <input-image> <output-image> <command-name> <command-args>\n");
  printf("SUPPORTED COMMANDS:\n");
  printf("   swap\n");
  printf("   invert\n");
  printf("   zoom-out\n");
  printf("   rotate-right\n");
  printf("   swirl <cx> <cy> <strength>\n");
  printf("   edge-detection <threshold>\n");
}
