//  Copyright (C) 2010 Daniel Maturana
//  This file is part of hdf5opencv.
// 
//  hdf5opencv is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  hdf5opencv is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with hdf5opencv. If not, see <http://www.gnu.org/licenses/>.
// 

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
struct Hdf5MatNotContinuousException : public std::exception { };

void hdf5save(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset, 
              bool overwrite = false);

void hdf5load(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset);

} /* hdf5opencv */

#endif /* end of include guard: HDF5OPENCV_KVHWT01S */
