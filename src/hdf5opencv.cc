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

#include "hdf5opencv.hh"
#include <boost/filesystem.hpp>

namespace hdf5opencv
{

void hdf5save(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset, 
              bool overwrite) {
  if (!dataset.isContinuous()) {
    throw Hdf5MatNotContinuousException();
  }
  hid_t file_id;
  if (!overwrite && boost::filesystem::exists(filename)) {
    file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT); 
    if (file_id < 0) {
      throw Hdf5FileOpenException();
    }
  } else {
    file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT); 
    if (file_id < 0) {
      throw Hdf5FileCreateException();
    }
  }

  // get type 
  hid_t native_dtype = -1;
  switch (dataset.depth()) {
    case CV_32F:
      native_dtype = H5T_NATIVE_FLOAT;
      break;
    case CV_64F:
      native_dtype = H5T_NATIVE_DOUBLE;
      break;
    case CV_32S:
      native_dtype = H5T_NATIVE_INT32;
      break;
    case CV_8U:
      native_dtype = H5T_NATIVE_UCHAR;
      break;
    default:
      throw Hdf5UnknownTypeException();
  }

  // get dims. Just 2D for now.
  hsize_t dims[2];
  dims[0] = dataset.rows;
  dims[1] = dataset.cols;

  // do it.
  if (H5LTfind_dataset(file_id, dataset_name)==1) {
    throw Hdf5DatasetExistsException();
  }
  herr_t status = H5LTmake_dataset(file_id,
                                   dataset_name,
                                   2,
                                   dims,
                                   native_dtype,
                                   dataset.ptr());
  if (status < 0) {
    throw Hdf5MakeDatasetException();
  }
  // cleanup.
  status = H5Fclose(file_id);
  if (status < 0) {
    throw Hdf5FileCloseException();
  }
}

void hdf5load(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset) {
  // open file.
  hid_t file_id = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT); 
  if (file_id < 0) {
    throw Hdf5FileOpenException();
  }

  // get dset info 
  hsize_t dims[2];
  size_t type_sz;
  H5T_class_t dtype;
  herr_t status = H5LTget_dataset_info(file_id, dataset_name, dims, &dtype, &type_sz);
  if (status < 0) {
    throw Hdf5GetDatasetInfoException();
  }
  // TODO See Shogun hdf5 code
  // convert datatype to native datatype
  int cv_dtype = -1;
  hid_t native_dtype = -1;
  switch (dtype) {
    case H5T_FLOAT:
      switch (type_sz) {
        // you'd think there's a Better Way (TM) but apparently not.
        case 4:
          cv_dtype = CV_32F;
          native_dtype = H5T_NATIVE_FLOAT;
          break;
        case 8:
          cv_dtype = CV_64F;
          native_dtype = H5T_NATIVE_DOUBLE;
          break;
        default:
          throw Hdf5UnknownTypeException();
      }
      break;
    case H5T_INTEGER:
      switch (type_sz) {
        case 4:
          cv_dtype = CV_32S;
          native_dtype = H5T_NATIVE_INT32;
          break;
        case 1:
          cv_dtype = CV_8U;
          native_dtype = H5T_NATIVE_UCHAR;
          break;
        default:
          // only 32 bit ints
          throw Hdf5UnknownTypeException();
      }
      break;
    default:
      throw Hdf5UnknownTypeException();
  }
  dataset.create(dims[0], dims[1], cv_dtype);
  status = H5LTread_dataset(file_id, dataset_name, native_dtype, dataset.ptr());
  if (status < 0) {
    throw Hdf5ReadDatasetException();
  }
  status = H5Fclose(file_id);
  if (status < 0) {
    throw Hdf5FileCloseException();
  }
}
 
} /* hdf5opencv */
