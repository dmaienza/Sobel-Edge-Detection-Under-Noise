# Sobel-Edge-Detection-Under-Noise

Intro/Purpose:
This project focuses on exploring spatial-domain convolution with an emphasis on Sobel edge detection under various conditions, particularly in the presence of noise. It aims to analyze and demonstrate the impact of noise on the efficiency of the Sobel edge detection algorithm and the effectiveness of basic noise reduction techniques. The project is divided into two parts, each run by its own C file, demonstrating edge detection in both ideal and noisy environments.

The "Lab3_Part1" program implements and tests Sobel edge detection in a noise-free environment. It includes the functions for the Sobel edge detection process, which involves applying horizontal and vertical Sobel filters through spatial-domain convolution and thresholding the resultant image to emphasize edges. This part of the project uses MRI and clover images to demonstrate the algorithm's ability to detect edges under ideal conditions. The code also includes normalization and histogram functionality to visualize edge-detected images.

The "Lab3_Part2" program extends this study into noisy environments. This file includes the implementation of a noise addition function to simulate real-world conditions where images are often affected by various levels of noise. It applies the Sobel edge detection algorithm to both the original and noisy versions of a simulated wedding cake image. The code also introduces a 3x3 averaging filter as a noise reduction technique, applied before edge detection, to assess its effectiveness in improving the accuracy of edge detection in noisy images. The file includes an accuracy function to evaluate the performance of edge detection with and without the smoothing filter.


Executables:
  Lab3_Part1: Implements and evaluates the Sobel edge detection algorithm on noise-free images, focusing on convolution, thresholding, and visualization through histograms.

  Lab3_Part2: Extends the analysis to noisy images, adding noise, applying noise reduction via a smoothing filter, and assessing the effectiveness of edge detection under these varied conditions.  

Instructions:
  Have all the files in the same directory, input "gcc Lab3_Part1.c -o Part1 -lm" into the terminal to generate an executable named Part1, and then run command ./Part1. Repeat for part 2.
  (./Part1, ./Part2)
  

Files:
  Lab3_Part1.c: Performs part 1 of the lab.

  Lab3_Part2.c: Performs part 2 of the lab.
  
  cake.pgm: image of cake

  mri.pgm: image of MRI
  
  clover.pgm: image of clover
