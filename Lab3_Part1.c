
//Code to run sobel filter of an image
//Fall 2023
//Daniel Maienza

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void mask_conv(unsigned char *inputImage, unsigned char *outputImage, int rows, int cols, char *maskType) {
    int x, y, i, j;
    int mask[3][3];

    // Choose the appropriate Sobel mask
    if (strcmp(maskType, "sobel_x") == 0) {
        // Sobel Operator for horizontal edge detection
        int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
        memcpy(mask, sobel_x, 9 * sizeof(int));
    } else if (strcmp(maskType, "sobel_y") == 0) {
        // Sobel Operator for vertical edge detection
        int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
        memcpy(mask, sobel_y, 9 * sizeof(int));
    }

    // Create a padded image
    int paddedRows = rows + 2;
    int paddedCols = cols + 2;
    unsigned char *paddedImage = (unsigned char *) calloc(paddedRows * paddedCols, sizeof(unsigned char));

    // Copy the input image into the center of the padded image
    for (y = 0; y < rows; y++) {
        memcpy(paddedImage + (y + 1) * paddedCols + 1, inputImage + y * cols, cols);
    }

    // Perform convolution with padding
    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            int sum = 0;
            for (i = -1; i <= 1; i++) {
                for (j = -1; j <= 1; j++) {
                    int pixelVal = paddedImage[(y + 1 + i) * paddedCols + (x + 1 + j)];
                    sum += pixelVal * mask[i + 1][j + 1];
                }
            }
            outputImage[y * cols + x] = (unsigned char) abs(sum);
        }
    }

    // Free the padded image memory
    free(paddedImage);
}

void apply_sobel(unsigned char *inputImage, unsigned char *outputImage, int rows, int cols) {
    unsigned char *grad_x = (unsigned char *) malloc(rows * cols);
    unsigned char *grad_y = (unsigned char *) malloc(rows * cols);

    // Apply Sobel X-mask
    mask_conv(inputImage, grad_x, rows, cols, "sobel_x");

    // Apply Sobel Y-mask
    mask_conv(inputImage, grad_y, rows, cols, "sobel_y");

    // Compute the magnitude of gradients
    for (int i = 0; i < rows * cols; i++) {
        // Use either the square root of the sum of the squares or the sum of absolute values
        outputImage[i] = sqrt(grad_x[i] * grad_x[i] + grad_y[i] * grad_y[i]);

        // outputImage[i] = abs(grad_x[i]) + abs(grad_y[i]);
    }

    free(grad_x);
    free(grad_y);
}

void thresholding(unsigned char *inputImage, int rows, int cols, unsigned char thresholdValue) {
    int x, y;
    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            if (inputImage[y * cols + x] >= thresholdValue)
                inputImage[y * cols + x] = 255; // White
            else
                inputImage[y * cols + x] = 0;   // Black
        }
    }
}

void histogramFunction(unsigned char *inputImage, int arraySize, int *histogramPlot, int histogramSize) {
    int i;
    for (i = 0; i < histogramSize; i++) {
        histogramPlot[i] = 0;
    }

    for (i = 0; i < arraySize; i++) {
        histogramPlot[inputImage[i]]++;
    }

    // Print the histogram values
    for (i = 0; i < histogramSize; i++) {
        printf("Value: %d, Frequency: %d\n", i, histogramPlot[i]);
    }
}


int main(int argc, char *argv[]){
  unsigned sizeX; // image width
  unsigned sizeY; // image height
  unsigned char *image; // image array
  unsigned char *outputImage; // output image array
  unsigned levels;
  unsigned i;
  float min_val; // max of spectrum magnitude
  float max_val; // min of spectrum magnitude
  
    /* Read Image */
  FILE *iFile = fopen("mri.pgm","r");
  if(iFile==0) return 1;
  if(3!=fscanf(iFile, "P5 %d %d %d ", &sizeX, &sizeY, &levels)) return 1;
  image=(unsigned char *) malloc(sizeX*sizeY);
  fread(image,sizeof(unsigned char),sizeX*sizeY,iFile);
  fclose(iFile);
  
  // Allocate memory for output image
  outputImage = (unsigned char *) malloc(sizeX * sizeY);

  // Apply Sobel edge detection
  apply_sobel(image, outputImage, sizeY, sizeX);

  /*write sobel image to file*/
  iFile = fopen("mri_sobel.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(outputImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);
  
  // Apply Thresholding
  unsigned char thresholdValue = 100; // Set an appropriate threshold value
  thresholding(outputImage, sizeY, sizeX, thresholdValue);
  
  // Compute and print histogram
  int histogram[sizeX];
  histogramFunction(outputImage, sizeX * sizeY, histogram, sizeX);

  //Find min and max
  min_val = outputImage[0];
  max_val = outputImage[0];
  
  for(i=0; i<sizeY*sizeX; i++) {
    if (outputImage[i] < min_val) {
      min_val = outputImage[i];
      }
    if (outputImage[i] > max_val) {
      max_val = outputImage[i];
      }
  }
  
  //Normalize the spectrum
  unsigned char *normalized_image;
  normalized_image = (unsigned char*) malloc(sizeY*sizeX*sizeof(unsigned char));
  
  for(i=0; i<sizeY*sizeX; i++) {
    normalized_image[i] = (255 * (outputImage[i]-min_val) / (max_val-min_val));
  }
    
  /*write threshold image to file*/
  iFile = fopen("mri_threshold.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(outputImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);
  return 0;
  
}

