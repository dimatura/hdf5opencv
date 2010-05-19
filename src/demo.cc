
#include <iostream>
#include <boost/filesystem.hpp>

#include "opencv/cv.h"
#include "H5LT.h"

#include "hdf5opencv.hh"

template <class T> 
void print_mat(cv::Mat_<T>& m) {
  for (int i=0; i<m.rows; ++i) {
    for (int j=0; j<m.cols; ++j) {
      std::cout << m(i,j) << " ";
    }
    std::cout << std::endl;
  }
}

template <> 
void print_mat(cv::Mat_<unsigned char>& m) {
  for (int i=0; i<m.rows; ++i) {
    for (int j=0; j<m.cols; ++j) {
      std::cout << static_cast<int>(m(i,j)) << " ";
    }
    std::cout << std::endl;
  }
}

template <class T>
void test() {
  cv::Mat_<T> a(cv::Size(2, 8));
  for (int i = 0; i < a.rows; ++i) {
    for (int j = 0; j < a.cols; ++j) {
      a(i,j) = (a.cols*i) + j;
    }
  }
  std::cout << "\nMatrix to write:\n";
  print_mat(a);
  std::cout << "\nWriting\n";
  // overwrite over old "test.h5" file
  hdf5opencv::hdf5save("test.h5", "/data", a, true);
  std::cout << "Reading\n";
  cv::Mat_<T> b;
  hdf5opencv::hdf5load("test.h5", "/data", b); 
  std::cout << "\nRead matrix:\n";
  print_mat(b);
}

int main (int argc, char const* argv[]) {

  std::cout << "\n*** FLOAT TEST ***\n";
  test<float>();

  std::cout << "\n*** DOUBLE TEST ***\n";
  test<double>();

  std::cout << "\n*** INT TEST ***\n";
  test<int>();
  
  std::cout << "\n*** CHAR TEST ***\n";
  test<unsigned char>();

  return 0;
}
