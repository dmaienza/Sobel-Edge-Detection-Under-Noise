
//Code to add noise to an image and then run sobel filter on it
//Fall 2023
//Daniel Maienza

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

void AddNoise(unsigned char *inputImage, unsigned char *outputImage, int rows, int cols, int maxNoise) {
    for (int i = 0; i < rows * cols; i++) {
        int noise = ((2 * rand() - RAND_MAX) / (double)RAND_MAX) * maxNoise;
        int noisyPixel = inputImage[i] + noise;
        outputImage[i] = (unsigned char) (noisyPixel > 255 ? 255 : (noisyPixel < 0 ? 0 : noisyPixel));
    }
}

void mask_conv(unsigned char *inputImage, unsigned char *outputImage, int rows, int cols, char *maskType) {
    int x, y, i, j;
    int mask[3][3];
    int sumMask = 1;

    // Choose the appropriate Sobel mask
    if (strcmp(maskType, "sobel_x") == 0) {
        // Sobel Operator for horizontal edge detection
        int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
        memcpy(mask, sobel_x, 9 * sizeof(int));
    } else if (strcmp(maskType, "sobel_y") == 0) {
        // Sobel Operator for vertical edge detection
        int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
        memcpy(mask, sobel_y, 9 * sizeof(int));
    } else if (strcmp(maskType, "average") == 0) {
        // 3x3 Averaging mask
        for(i = 0; i < 3; i++)
            for(j = 0; j < 3; j++)
                mask[i][j] = 1;
        sumMask = 9;
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
            outputImage[y * cols + x] = (unsigned char) (abs(sum) / sumMask);
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

        // or outputImage[i] = abs(grad_x[i]) + abs(grad_y[i]);
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

double accuracy(unsigned char *image1, unsigned char *image2, int rows, int cols) {
    int matchingPixels = 0;
    for (int i = 0; i < rows * cols; i++) {
        if (image1[i] == image2[i]) {
            matchingPixels++;
        }
    }
    return (double)matchingPixels / (rows * cols) * 100.0; // Percentage of correctly classified pixels
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
  unsigned levels;
  unsigned i;
  float min_val; // max of spectrum magnitude
  float max_val; // min of spectrum magnitude
  int maxNoise = 20; // noise level
  
    /* Read Image */
  FILE *iFile = fopen("cake.pgm","r");
  if(iFile==0) return 1;
  if(3!=fscanf(iFile, "P5 %d %d %d ", &sizeX, &sizeY, &levels)) return 1;
  image=(unsigned char *) malloc(sizeX*sizeY);
  fread(image,sizeof(unsigned char),sizeX*sizeY,iFile);
  fclose(iFile);
  
  // Allocate memory for various images
  unsigned char *binaryEdgeImage = (unsigned char *) malloc(sizeX * sizeY);
  unsigned char *noisyImage = (unsigned char *)malloc(sizeX * sizeY);
  unsigned char *edgeImage = (unsigned char *)malloc(sizeX * sizeY);
  unsigned char *noisyEdgeImage = (unsigned char *)malloc(sizeX * sizeY);
  unsigned char *smoothedImage = (unsigned char *)malloc(sizeX * sizeY);
  unsigned char *smoothedEdgeImage = (unsigned char *)malloc(sizeX * sizeY);

  // Seed the random number generator
  srand(time(NULL));
  
  // Add noise to the image
  AddNoise(image, noisyImage, sizeY, sizeX, maxNoise);
  
  // Apply Sobel edge detection to the regular and noisy image
  apply_sobel(image, binaryEdgeImage, sizeY, sizeX);
  apply_sobel(noisyImage, noisyEdgeImage, sizeY, sizeX);

  /*write images to file*/
  iFile = fopen("cake_noisy_image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(noisyImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);
  
  iFile = fopen("cake_edge-image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(binaryEdgeImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);
  
  iFile = fopen("cake_noisy_edge-image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(noisyEdgeImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);

  // Apply Thresholding to regular and noisy edge image
  unsigned char thresholdValue = 100; // Set an appropriate threshold value
  thresholding(binaryEdgeImage, sizeY, sizeX, thresholdValue);
  thresholding(noisyEdgeImage, sizeY, sizeX, thresholdValue);

  // Calculate accuracy of the binary noisy-edge-image with respect to the binary edge-image
  double acc = accuracy(binaryEdgeImage, noisyEdgeImage, sizeY, sizeX);
  printf("Accuracy: %.2f%%\n", acc);

  // Smooth the noisy image before applying Sobel detector
  // Apply a 3x3 averaging filter to the noisy image
  mask_conv(noisyImage, smoothedImage, sizeY, sizeX, "average");  
  // Apply Sobel edge detection to the smoothed image
  apply_sobel(smoothedImage, smoothedEdgeImage, sizeY, sizeX);
          
  // Compute and print histogram
  // int histogram[sizeX];
  // histogramFunction(smoothedImage, sizeX * sizeY, histogram, sizeX);
  
  /*write images to file*/
  iFile = fopen("cake_smoothed_image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(smoothedImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);
  
  iFile = fopen("cake_smoothed_edge-image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(smoothedEdgeImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);
  
  // Threshold the smoothed edge image to create a binary edge image
  thresholdValue = 55;
  thresholding(smoothedEdgeImage, sizeY, sizeX, thresholdValue);

  // Calculate accuracy for smoothed image
  double accSmoothed = accuracy(binaryEdgeImage, smoothedEdgeImage, sizeY, sizeX);
  printf("Accuracy after smoothing: %.2f%%\n", accSmoothed);
    
  /*write images to file*/
  iFile = fopen("cake_binary_edge-image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fclose(iFile);
  
  iFile = fopen("cake_binary_noisy-edge-image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(noisyEdgeImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);

  iFile = fopen("cake_binary_smoothed-edge-image.pgm","w");
  if(iFile==0) return 1; //error handling
  fprintf(iFile, "P5 %d %d %d ",sizeX,sizeY,255);//write header
  fwrite(smoothedEdgeImage, sizeof(unsigned char), sizeX*sizeY, iFile); //write binary image
  fclose(iFile);
  
  free(binaryEdgeImage);
  free(noisyImage);
  free(edgeImage);
  free(noisyEdgeImage);
  free(smoothedImage);
  free(smoothedEdgeImage);
  return 0;
}

