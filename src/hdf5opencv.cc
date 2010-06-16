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


hid_t open_or_create(const char *filename, bool overwrite) {
  // invoke create if overwrite is true OR file does not exist.
  hid_t file_id;
  if (overwrite || !boost::filesystem::exists(filename)) {
    file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT); 
    if (file_id < 0) {
      std::string error_msg("Error creating ");
      error_msg += filename;
      throw Hdf5OpenCVException(error_msg); 
    }
    return file_id;
  } 
  // Use open, not create. open will fail if file does not exist.
  file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT); 
  if (file_id < 0) {
    std::string error_msg("Error opening ");
    error_msg += filename;
    throw Hdf5OpenCVException(error_msg); 
  }
  return file_id;
}

/**
 * Save a string buffer. string *must* be null terminated.
 */
void hdf5save(const char * filename,
              const char * dataset_name,
              const char * strbuf,
              bool overwrite) {
  hid_t file_id = open_or_create(filename, overwrite);
  // TODO error catching for non null terminated strings

  // do it.
  if (H5LTfind_dataset(file_id, dataset_name)==1) {
    std::string error_msg("Error: ");
    error_msg += filename;
    error_msg += " exists.";
    throw Hdf5OpenCVException(error_msg); 
  }
  herr_t status = H5LTmake_dataset_string(file_id, dataset_name, strbuf);
  if (status < 0) {
    std::string error_msg("Error making dataset ");
    error_msg += dataset_name;
    throw Hdf5OpenCVException(error_msg); 
  }
  // cleanup.
  status = H5Fclose(file_id);
  if (status < 0) {
    std::string error_msg("Error closing ");
    error_msg += filename;
    throw Hdf5OpenCVException(error_msg); 
  }
}




void hdf5save(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset, 
              bool overwrite) {
  if (!dataset.isContinuous()) {
    throw Hdf5OpenCVException("Matrix is not continuous"); 
  }
  hid_t file_id = open_or_create(filename, overwrite);
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
      throw Hdf5OpenCVException("Unknown data type"); 
  }

  // get dims. Just 2D for now.
  hsize_t dims[2];
  dims[0] = dataset.rows;
  dims[1] = dataset.cols;

  // do it.
  if (H5LTfind_dataset(file_id, dataset_name)==1) {
    std::string error_msg("Error: ");
    error_msg += filename;
    error_msg += " exists.";
    throw Hdf5OpenCVException(error_msg); 
  }
  herr_t status = H5LTmake_dataset(file_id,
                                   dataset_name,
                                   2,
                                   dims,
                                   native_dtype,
                                   dataset.ptr());
  if (status < 0) {
    std::string error_msg("Error making dataset ");
    error_msg += dataset_name;
    throw Hdf5OpenCVException(error_msg); 
  }
  // cleanup.
  status = H5Fclose(file_id);
  if (status < 0) {
    std::string error_msg("Error closing ");
    error_msg += filename;
    throw Hdf5OpenCVException(error_msg); 
  }
}

void hdf5load(const char * filename,
              const char * dataset_name,
              cv::Mat& dataset) {
  // open file.
  hid_t file_id = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT); 
  if (file_id < 0) {
    std::string error_msg("Error opening ");
    error_msg += filename;
    throw Hdf5OpenCVException(error_msg); 
  }

  // get dset info 
  hsize_t dims[2];
  size_t type_sz;
  H5T_class_t dtype;
  herr_t status = H5LTget_dataset_info(file_id, dataset_name, dims, &dtype, &type_sz);
  if (status < 0) {
    std::string error_msg("Error getting dataset ");
    error_msg += dataset_name;
    error_msg += " info.";
    throw Hdf5OpenCVException(error_msg); 
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
          throw Hdf5OpenCVException("Unknown data type."); 
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
          throw Hdf5OpenCVException("Bad data type: only 32 bit ints are supported"); 
      }
      break;
    default:
      throw Hdf5OpenCVException("Unknown data type."); 
  }
  dataset.create(dims[0], dims[1], cv_dtype);
  status = H5LTread_dataset(file_id, dataset_name, native_dtype, dataset.ptr());
  if (status < 0) {
    std::string error_msg("Error reading ");
    error_msg += dataset_name;
    throw Hdf5OpenCVException(error_msg); 
  }
  status = H5Fclose(file_id);
  if (status < 0) {
    std::string error_msg("Error closing ");
    error_msg += filename;
    throw Hdf5OpenCVException(error_msg); 
  }
}

void hdf5load(const char * filename,
              const char * dataset_name,
              char ** strbuf) {
  // open file.
  hid_t file_id = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT); 
  if (file_id < 0) {
    std::string error_msg("Error opening ");
    error_msg += filename;
    throw Hdf5OpenCVException(error_msg); 
  }
  // get dset info 
  hsize_t dims[2];
  size_t type_sz;
  H5T_class_t dtype;
  // type_sz has string length (including the final '\0' character)
  herr_t status = H5LTget_dataset_info(file_id, dataset_name, dims, &dtype, &type_sz);
  if (status < 0) {
    std::string error_msg("Error getting dataset ");
    error_msg += dataset_name;
    error_msg += " info.";
    throw Hdf5OpenCVException(error_msg); 
  }
  *strbuf = new char[type_sz];
  status = H5LTread_dataset_string(file_id, dataset_name, *strbuf);
  if (status < 0) {
    std::string error_msg("Error reading ");
    error_msg += dataset_name;
    throw Hdf5OpenCVException(error_msg); 
  }
  status = H5Fclose(file_id);
  if (status < 0) {
    std::string error_msg("Error closing ");
    error_msg += filename;
    throw Hdf5OpenCVException(error_msg); 
  }
}
 
} /* hdf5opencv */
