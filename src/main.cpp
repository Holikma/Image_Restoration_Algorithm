#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unordered_set>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <omp.h>

using namespace std;
using namespace Eigen;

typedef VectorXi ImageData;

// Function declarations
ImageData loadPGM(const string& filename, int& width, int& height);
void removePixels(ImageData& img, int width, int height, double percent);
void savePGM(const ImageData& img, int width, int height, const string& filename);
SparseMatrix<double, RowMajor> buildSparseMatrix(const ImageData& img, int width, int height);
void solveImage(const SparseMatrix<double, RowMajor>& m, ImageData& img, int width, int height, double percent);


ImageData loadPGM(const string& filename, int& width, int& height) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    string format;
    file >> format;
    if (format != "P2") {
        cout << "Error: Unsupported PGM format. Only P2 is supported." << endl;
        exit(1);
    }

    string test;

    while (file.peek() == '#') {
		getline(file, test);
	}

    file >> width >> height;
    if (width <= 0 || height <= 0) {
		cout << "Error: Invalid width or height." << endl;
		exit(1);
	}


    int maxGrayValue;
    file >> maxGrayValue;
    if (maxGrayValue != 255) {
        cout << "Error: Unsupported max gray value. Only 255 is supported." << endl;
        exit(1);
    }

    ImageData img(width * height);
    for (int i = 0; i < width * height; ++i) {
        int pixel;
        if (!(file >> pixel)) {
            cout << "Error: Could not read pixel value." << endl;
            exit(1);
        }
        img[i] = pixel;
    }

    file.close();
    cout << "Image loaded successfully. Total pixels: " << width * height << endl;
    return img;
}

void removePixels(ImageData& img, int width, int height, double percent) {
    int size = width * height;
    int num_pixels_to_remove = size * percent / 100;
    cout << "Total pixels: " << size << endl;
    cout << "Pixels to remove: " << num_pixels_to_remove << endl;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> row_dist(1, height - 2);
    uniform_int_distribution<> col_dist(1, width - 2);

    unordered_set<int> removed_pixels;
    int counter2 = 0;

    while (counter2 < num_pixels_to_remove) {
        int row = row_dist(gen);
        int col = col_dist(gen);
        int index = row * width + col;

        if (removed_pixels.find(index) == removed_pixels.end()) {
            img[index] = 0;
            removed_pixels.insert(index);
            counter2++;

            if (counter2 % 10000 == 0) {
                cout << "Removed " << counter2 << " pixels so far." << endl;
            }
        }
    }

    cout << "Finished removing pixels." << endl;
}

void savePGM(const ImageData& img, int width, int height, const string& filename) {
    ofstream output_file(filename);
    output_file << "P2\n";
    output_file << width << " " << height << "\n";
    output_file << 255 << "\n";

    for (int i = 0; i < width * height; ++i) {
        output_file << img[i] << " ";
        if ((i + 1) % width == 0) {
            output_file << "\n";
        }
    }

    output_file.close();
    cout << "Image saved successfully to " << filename << endl;
}

SparseMatrix<double, RowMajor> buildSparseMatrix(const ImageData& img, int width, int height) {
    int size = width * height;
    SparseMatrix<double, RowMajor> m(size, size);
    m.reserve(size * 5);

    for (size_t i = 0; i < size; i++) {
        if (img[i] == 0) {
            m.insert(i, i) = 4.0;

            if (i - 1 >= 0) {
                m.insert(i, i - 1) = -1.0;
            }
            if (i + 1 < size) {
                m.insert(i, i + 1) = -1.0;
            }
            if (i - width >= 0) {
                m.insert(i, i - width) = -1.0;
            }
            if (i + width < size) {
                m.insert(i, i + width) = -1.0;
            }
        }
        else {
            m.insert(i, i) = 1.0;
        }

        double progress = static_cast<double>(i + 1) / size * 100;
        printf("\r%.2f%%", progress);
        fflush(stdout);
    }

    return m;
}

void solveImage(const SparseMatrix<double, RowMajor>& m, ImageData& img, int width, int height, double percent) {
    initParallel();
    int n = 8;
    omp_set_num_threads(n);
    setNbThreads(n);

    BiCGSTAB<SparseMatrix<double, RowMajor>> solver;
    cout << "Computing BiCGSTAB..." << endl;
    solver.setMaxIterations(100000);
    solver.setTolerance(1E-8);

    solver.compute(m);
    cout << "BiCGSTAB done" << endl;
    cout << "Solver initialized" << endl;

    VectorXd img_d = img.cast<double>();
    VectorXd restored_d = solver.solve(img_d);
    img = restored_d.cast<int>();

    string output_filename = "..\\output_" + to_string(static_cast<int>(percent)) + ".pgm";
    savePGM(img, width, height, output_filename);

    cout << "# Iterations: " << solver.iterations() << endl;
    cout << "Estimated error: " << solver.error() << endl;
    cout << "Solving Done" << endl;
}

int main(int argc, char** argv) {

    string filename = "..\\Mario_damaged.pgm"; // enter here path to your image
    double percent = 50; // enter here the percentage of pixels to remove if testing

    int width, height;
    
    //if data are damaged by black pixels, comment the following lines
    //ImageData img = loadPGM(filename, width, height);
    //removePixels(img, width, height, percent);

    // if you dont want to save the image with removed pixels, comment the following line
    //savePGM(img, width, height, "..\\output_removed_" + to_string(static_cast<int>(percent)) + ".pgm");

    // calculation of the sparse matrix
    SparseMatrix<double, RowMajor> m = buildSparseMatrix(img, width, height);

    // Solve and restore the image
    solveImage(m, img, width, height, percent);

    return 0;
}












