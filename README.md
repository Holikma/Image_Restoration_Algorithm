# Image Restoration using BiCGSTAB Solver
This repository contains a C++ program for restoring an image using a sparse matrix and the BiCGSTAB iterative solver. The program reads a PGM image, removes a specified percentage of its pixels, builds a sparse matrix to model the image degradation, and then uses the BiCGSTAB method to solve the image recovery problem.

## Table of Contents
- [Image Restoration using BiCGSTAB Solver](#image-restoration-using-bicgstab-solver)
  - [Table of Contents](#table-of-contents)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Comparison of Original, Damaged and Restored Image](#comparison-of-original-damaged-and-restored-image)
  - [License](#license)
  - [Author](#author)


## Installation

The easiest way to download this repository is by cloning repository using the following command:
```bash
git clone https://github.com/Holikma/Image_Restoration_Algorithm.git
```
Once cloned in local directory, use cmake to build and generate project. Program is dependent on Eigen library, which is included in the repository and cMakeLists.txt file is already configured to include it.

## Usage

 Since the program lacks a user interface, it is crucial to modify the default parameters directly within the code.

Firstly, the program reads a PGM image file, which is a grayscale image format and header must be in specific format to load data correctly:
```
P2
# CREATOR: GIMP PNM Filter Version 1.1
512 512
255
Color values.. .... ... 
```
Its crucial to remove the comments:

```
P2
512 512
255
Color values.. .... ... 
```
Ensure that the header is in this format. The program reads the image and stores it in special data structure, which is then used to build a sparse matrix. The matrix is constructed by iterating over the image and adding the pixel values to the matrix.

Secondly, user have to adjust settings for personal use, navigate to the main function where you will find two key variables: one for the filename and the other for the percentage of pixels. These are intended for testing purposes. The program randomly removes a specified percentage of pixels from the image and then utilizes the BiCGSTAB iterative solver to restore them.

```cpp

int main()
{
    // Load image
     string filename = "..\\test.pgm"; // enter here path to your image
    double percent = 50; // enter here the percentage of pixels to remove if testing

    int width, height;
    
    //if data are damaged by black pixels, comment the following lines
    ImageData img = loadPGM(filename, width, height);
    removePixels(img, width, height, percent);

    // if you dont want to save the image with removed pixels, comment the following line
    savePGM(img, width, height, "..\\output_removed_" + to_string(static_cast<int>(percent)) + ".pgm");

    // calculation of the sparse matrix
    SparseMatrix<double, RowMajor> m = buildSparseMatrix(img, width, height);

    // Solve and restore the image
    solveImage(m, img, width, height, percent);

    return 0;
}
```

## Comparison of Original, Damaged and Restored Image

<div align="center">
  
 <img src="https://github.com/user-attachments/assets/b427061c-0ebe-4ad1-804c-b503ff142baa" width="300px" alt="Damaged image">

<div align="left">
Program operates on the damaged image and restores it to the original image. It takes all pixels set to zero and replaces them with the restored pixel values. The restored image is then saved as a PGM file.
  
<div align="center">
<img src="https://github.com/user-attachments/assets/702efbad-f7c6-4c29-a2ad-99798e0ed831" width="300px" alt="Damaged image">

<div align="left">
As we can see in restored image, removed pixels are replaced with the values calculated by the BiCGSTAB solver. Solution is not perfect, but it is a good approximation of the original image.
To greater enhance the quality of the restored image, my another project: Enhanching Images with oriented Gaussians" can be used.
  
<div align="center" width="200px" height="200px">
  
<img src="https://github.com/user-attachments/assets/eb54c937-8b56-4a83-8ffc-d5801ad8fe01" width="300px" alt="Damaged image">

<div align="left">
  
## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE.txt) file for details

## Author
 Me
