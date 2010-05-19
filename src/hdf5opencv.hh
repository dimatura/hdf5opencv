#ifndef HDF5OPENCV_KVHWT01S
#define HDF5OPENCV_KVHWT01S

#include <exception>

#include "opencv/cv.h"
#include "H5LT.h"

namespace hdf5opencv
{

struct Hdf5FileOpenException : public std::exception { };
struct Hdf5FileCreateException : public std::exception { };
struct Hdf5FileCloseException : public std::exception { };
struct Hdf5UnknownTypeException : public std::exception { };
struct Hdf5MakeDatasetException : public std::exception { };
struct Hdf5DatasetExistsException : public std::exception { };
struct Hdf5GetDatasetInfoException : public std::exception { };
struct Hdf5ReadDatasetInfoException : public std::exception { };
struct Hdf5ReadDatasetException : public std::exception { };

void hdf5save(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset, 
              bool overwrite = false);

void hdf5load(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset);

} /* hdf5opencv */

#endif /* end of include guard: HDF5OPENCV_KVHWT01S */
