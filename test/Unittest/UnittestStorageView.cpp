//===-- Unittest/UnittestStorageView.cpp --------------------------------------------*- C++ -*-===//
//
//                                    S E R I A L B O X
//
// This file is distributed under terms of BSD license.
// See LICENSE.txt for more information
//
//===------------------------------------------------------------------------------------------===//
//
/// \file
/// This file contains the unittests for the StorageView and StorageViewIterator.
///
//===------------------------------------------------------------------------------------------===//

#include "Storage.h"
#include "serialbox/Core/STLExtras.h"
#include "serialbox/Core/StorageView.h"
#include "serialbox/Core/Type.h"
#include <cstring>
#include <gtest/gtest.h>

using namespace serialbox;
using namespace unittest;

namespace {

class StorageViewTest : public testing::Test {
public:
  // -----------------------------------------------------------------------------------------------
  // Dimensions
  // -----------------------------------------------------------------------------------------------
  int dim1;
  int dim2;
  int dim3;
  int dim4;
  int dim5;

  // -----------------------------------------------------------------------------------------------
  // Padding
  // -----------------------------------------------------------------------------------------------
  int pad1_left;
  int pad1_right;

  int pad2_left;
  int pad2_right;

  int pad3_left;
  int pad3_right;

  int pad4_left;
  int pad4_right;

  int pad5_left;
  int pad5_right;

  // -----------------------------------------------------------------------------------------------
  // 1D
  // -----------------------------------------------------------------------------------------------
  std::unique_ptr<Storage<double>> storage_1d;
  std::unique_ptr<Storage<double>> storage_1d_padded;

  // -----------------------------------------------------------------------------------------------
  // 2D
  // -----------------------------------------------------------------------------------------------
  std::unique_ptr<Storage<double>> storage_2d_col_major;
  std::unique_ptr<Storage<double>> storage_2d_col_major_padded;

  std::unique_ptr<Storage<double>> storage_2d_row_major;
  std::unique_ptr<Storage<double>> storage_2d_row_major_padded;

  // -----------------------------------------------------------------------------------------------
  // 3D
  // -----------------------------------------------------------------------------------------------
  std::unique_ptr<Storage<double>> storage_3d_col_major;
  std::unique_ptr<Storage<double>> storage_3d_col_major_padded;

  std::unique_ptr<Storage<double>> storage_3d_row_major;
  std::unique_ptr<Storage<double>> storage_3d_row_major_padded;

  // -----------------------------------------------------------------------------------------------
  // 5D
  // -----------------------------------------------------------------------------------------------
  std::unique_ptr<Storage<double>> storage_5d_col_major_padded;
  std::unique_ptr<Storage<double>> storage_5d_row_major_padded;

protected:
  virtual void SetUp() override {
    dim1 = 2;
    dim2 = 3;
    dim3 = 4;
    dim4 = 2;
    dim5 = 2;

    pad1_left = 1;
    pad1_right = 2;

    pad2_left = 3;
    pad2_right = 1;

    pad3_left = 0;
    pad3_right = 1;

    pad4_left = 0;
    pad4_right = 0;

    pad5_left = 1;
    pad5_right = 0;

    auto colMajor = Storage<double>::ColMajor;
    auto rowMajor = Storage<double>::RowMajor;

    storage_1d = make_unique<Storage<double>>(colMajor, Dims{dim1});
    storage_1d_padded =
        make_unique<Storage<double>>(colMajor, Dims{dim1}, Padding{{pad1_left, pad1_right}});

    storage_2d_col_major = make_unique<Storage<double>>(colMajor, Dims{dim1, dim2});
    storage_2d_col_major_padded = make_unique<Storage<double>>(
        colMajor, Dims{dim1, dim2}, Padding{{pad1_left, pad1_right}, {pad2_left, pad2_right}});
    ;

    storage_2d_row_major = make_unique<Storage<double>>(rowMajor, Dims{dim1, dim2});
    storage_2d_row_major_padded = make_unique<Storage<double>>(
        rowMajor, Dims{dim1, dim2}, Padding{{pad1_left, pad1_right}, {pad2_left, pad2_right}});

    storage_3d_col_major = make_unique<Storage<double>>(colMajor, Dims{dim1, dim2, dim3});
    storage_3d_col_major_padded = make_unique<Storage<double>>(
        colMajor, Dims{dim1, dim2, dim3},
        Padding{{pad1_left, pad1_right}, {pad2_left, pad2_right}, {pad3_left, pad3_right}});

    storage_3d_row_major = make_unique<Storage<double>>(rowMajor, Dims{dim1, dim2, dim3});
    storage_3d_row_major_padded = make_unique<Storage<double>>(
        rowMajor, Dims{dim1, dim2, dim3},
        Padding{{pad1_left, pad1_right}, {pad2_left, pad2_right}, {pad3_left, pad3_right}});

    storage_5d_col_major_padded = make_unique<Storage<double>>(
        colMajor, Dims{dim1, dim2, dim3, dim4, dim5}, Padding{{pad1_left, pad1_right},
                                                              {pad2_left, pad2_right},
                                                              {pad3_left, pad3_right},
                                                              {pad4_left, pad4_right},
                                                              {pad5_left, pad5_right}});

    storage_5d_row_major_padded = make_unique<Storage<double>>(
        rowMajor, Dims{dim1, dim2, dim3, dim4, dim5}, Padding{{pad1_left, pad1_right},
                                                              {pad2_left, pad2_right},
                                                              {pad3_left, pad3_right},
                                                              {pad4_left, pad4_right},
                                                              {pad5_left, pad5_right}});
  }

  virtual void TearDown() override {}
};

} // anonymous namespace

template <class T>
static std::unique_ptr<StorageView> toStorageView(std::unique_ptr<Storage<T>>& storage) {
  auto type = ToTypeID<T>::value;
  return make_unique<StorageView>(storage->data.data(), type, storage->dims, storage->strides,
                                  storage->padding);
}

TEST_F(StorageViewTest, Construction) {

  // Constructor
  auto sv_1d = storage_1d->toStorageView();
  EXPECT_EQ(static_cast<void*>(storage_1d->data().data()), static_cast<void*>(sv_1d.data()));
  EXPECT_EQ(storage_1d->strides(), sv_1d.strides());
  EXPECT_EQ(storage_1d->dims(), sv_1d.dims());
  EXPECT_EQ(storage_1d->padding(), sv_1d.padding());
  EXPECT_EQ(sv_1d.bytesPerElement(), sizeof(Storage<double>::value_type));
  EXPECT_EQ(sv_1d.size(), dim1);
  EXPECT_EQ(sv_1d.sizeInBytes(), sv_1d.bytesPerElement() * dim1);

  // Copy constructor
  auto sv_2d_col_major = storage_2d_col_major->toStorageView();

  StorageView copy_sv_1d(sv_1d);
  ASSERT_EQ(copy_sv_1d, sv_1d);

  StorageView copy_sv_2d(sv_2d_col_major);
  ASSERT_EQ(copy_sv_2d, sv_2d_col_major);

  EXPECT_EQ(sv_2d_col_major.size(), dim1 * dim2);
  EXPECT_EQ(sv_2d_col_major.sizeInBytes(), sv_2d_col_major.bytesPerElement() * (dim1 * dim2));

  // Swap
  sv_1d.swap(sv_2d_col_major);
  EXPECT_EQ(sv_1d, copy_sv_2d);
  EXPECT_EQ(sv_2d_col_major, copy_sv_1d);

  // Copy assignment
  sv_1d = sv_2d_col_major;
  EXPECT_EQ(sv_1d, sv_2d_col_major);
}

TEST_F(StorageViewTest, IteratorConstruction) {
  auto sv_1d = storage_1d->toStorageView();
  auto sv_1d_pad = storage_1d_padded->toStorageView();
  auto sv_2d_col_major = storage_2d_col_major->toStorageView();
  auto sv_2d_col_major_pad = storage_2d_col_major_padded->toStorageView();
  auto sv_2d_row_major = storage_2d_row_major->toStorageView();
  auto sv_2d_row_major_pad = storage_2d_row_major_padded->toStorageView();
  auto sv_3d_col_major = storage_3d_col_major->toStorageView();
  auto sv_3d_col_major_pad = storage_3d_col_major_padded->toStorageView();
  auto sv_3d_row_major = storage_3d_row_major->toStorageView();
  auto sv_3d_row_major_pad = storage_3d_row_major_padded->toStorageView();
  auto sv_5d_col_major_pad = storage_5d_col_major_padded->toStorageView();
  auto sv_5d_row_major_pad = storage_5d_row_major_padded->toStorageView();

  EXPECT_NE(sv_1d.begin(), sv_1d.end());
  EXPECT_EQ(sv_1d.begin(), sv_1d.begin());
  EXPECT_EQ(sv_1d.end(), sv_1d.end());
  EXPECT_NE(sv_1d.begin(), sv_2d_col_major.begin());

  // Check if begin() points to the beginning of the data i.e skips any padding

  // -----------------------------------------------------------------------------------------------
  // 1D
  // -----------------------------------------------------------------------------------------------
  EXPECT_EQ(static_cast<void*>(sv_1d.begin().ptr()), static_cast<void*>(sv_1d.data()));
  EXPECT_EQ(static_cast<void*>(sv_1d.begin().ptr()), static_cast<void*>(storage_1d->originPtr()));
  EXPECT_EQ(static_cast<void*>(sv_1d_pad.begin().ptr()),
            static_cast<void*>(storage_1d_padded->originPtr()));

  // -----------------------------------------------------------------------------------------------
  // 2D
  // -----------------------------------------------------------------------------------------------
  EXPECT_EQ(static_cast<void*>(sv_2d_col_major.begin().ptr()),
            static_cast<void*>(sv_2d_col_major.data()));
  EXPECT_EQ(static_cast<void*>(sv_2d_col_major.begin().ptr()),
            static_cast<void*>(storage_2d_col_major->originPtr()));
  EXPECT_EQ(static_cast<void*>(sv_2d_col_major_pad.begin().ptr()),
            static_cast<void*>(storage_2d_col_major_padded->originPtr()));

  EXPECT_EQ(static_cast<void*>(sv_2d_row_major.begin().ptr()),
            static_cast<void*>(sv_2d_row_major.data()));
  EXPECT_EQ(static_cast<void*>(sv_2d_row_major.begin().ptr()),
            static_cast<void*>(storage_2d_row_major->originPtr()));
  EXPECT_EQ(static_cast<void*>(sv_2d_row_major_pad.begin().ptr()),
            static_cast<void*>(storage_2d_row_major_padded->originPtr()));

  // -----------------------------------------------------------------------------------------------
  // 3D
  // -----------------------------------------------------------------------------------------------
  EXPECT_EQ(static_cast<void*>(sv_3d_col_major.begin().ptr()),
            static_cast<void*>(sv_3d_col_major.data()));
  EXPECT_EQ(static_cast<void*>(sv_3d_col_major.begin().ptr()),
            static_cast<void*>(storage_3d_col_major->originPtr()));
  EXPECT_EQ(static_cast<void*>(sv_3d_col_major_pad.begin().ptr()),
            static_cast<void*>(storage_3d_col_major_padded->originPtr()));

  EXPECT_EQ(static_cast<void*>(sv_3d_row_major.begin().ptr()),
            static_cast<void*>(sv_3d_row_major.data()));
  EXPECT_EQ(static_cast<void*>(sv_3d_row_major.begin().ptr()),
            static_cast<void*>(storage_3d_row_major->originPtr()));
  EXPECT_EQ(static_cast<void*>(sv_3d_row_major_pad.begin().ptr()),
            static_cast<void*>(storage_3d_row_major_padded->originPtr()));

  // -----------------------------------------------------------------------------------------------
  // 5D
  // -----------------------------------------------------------------------------------------------
  EXPECT_EQ(static_cast<void*>(sv_5d_col_major_pad.begin().ptr()),
            static_cast<void*>(storage_5d_col_major_padded->originPtr()));
  EXPECT_EQ(static_cast<void*>(sv_5d_row_major_pad.begin().ptr()),
            static_cast<void*>(storage_5d_row_major_padded->originPtr()));

  // Copy constructor
  auto it_sv_1d(sv_1d.begin());
  auto copy_it_sv_1d(it_sv_1d);
  EXPECT_EQ(copy_it_sv_1d, sv_1d.begin());

  auto it_sv_2d(sv_2d_col_major.begin());
  auto copy_it_sv_2d(it_sv_2d);
  EXPECT_EQ(copy_it_sv_2d, sv_2d_col_major.begin());

  // Copy assignment
  auto copy_assign_it_sv_1d = it_sv_1d;
  EXPECT_EQ(copy_assign_it_sv_1d, sv_1d.begin());

  // Swap
  it_sv_2d.swap(it_sv_1d);
  EXPECT_EQ(it_sv_1d, copy_it_sv_2d);
  EXPECT_EQ(it_sv_2d, copy_it_sv_1d);
}

TEST_F(StorageViewTest, IteratorCopy) {

  // ----------------------------------------------------------------------------------------------
  // The idea of this test is to copy a contiguous piece of data into the storages using the
  // StorageView and StorageViewIterator interfaces.
  //
  // This will make sure the operator++ as well as end() of StoraViewIterator are working correclty.
  // ----------------------------------------------------------------------------------------------

  // Setup data (5D)
  std::vector<double> data(dim1 * dim2 * dim3 * dim4 * dim5);
  std::iota(data.begin(), data.end(), 1);
  char* dataPtr;
  const int bytesPerElement = sizeof(double);

#define COPY_INTO_STORAGE(name)                                                                    \
  auto sv_##name = name->toStorageView();                                                          \
  dataPtr = reinterpret_cast<char*>(data.data());                                                  \
  for(auto it = sv_##name.begin(), end = sv_##name.end(); it != end;                               \
      ++it, dataPtr += bytesPerElement)                                                            \
    std::memcpy(it.ptr(), dataPtr, bytesPerElement);

  // --- 1D ---

  COPY_INTO_STORAGE(storage_1d);
  COPY_INTO_STORAGE(storage_1d_padded);

  // Check data was copied correctly
  for(int i = 0; i < dim1; ++i) {
    ASSERT_DOUBLE_EQ(data[i], storage_1d->at(i));
    ASSERT_DOUBLE_EQ(data[i], storage_1d_padded->at(i));
  }

  // --- 2D ---

  COPY_INTO_STORAGE(storage_2d_col_major);
  COPY_INTO_STORAGE(storage_2d_row_major);
  COPY_INTO_STORAGE(storage_2d_col_major_padded);
  COPY_INTO_STORAGE(storage_2d_row_major_padded);

  // Check data was copied correctly
  for(int j = 0; j < dim2; ++j)
    for(int i = 0; i < dim1; ++i) {
      double val = data[j * dim1 + i];
      ASSERT_DOUBLE_EQ(val, storage_2d_col_major->at(i, j));
      ASSERT_DOUBLE_EQ(val, storage_2d_row_major->at(i, j));
      ASSERT_DOUBLE_EQ(val, storage_2d_col_major_padded->at(i, j));
      ASSERT_DOUBLE_EQ(val, storage_2d_row_major_padded->at(i, j));
    }

  // --- 3D ---
  COPY_INTO_STORAGE(storage_3d_col_major);
  COPY_INTO_STORAGE(storage_3d_row_major);
  COPY_INTO_STORAGE(storage_3d_col_major_padded);
  COPY_INTO_STORAGE(storage_3d_row_major_padded);

  // Check data was copied correctly
  for(int k = 0; k < dim3; ++k)
    for(int j = 0; j < dim2; ++j)
      for(int i = 0; i < dim1; ++i) {
        double val = data[k * dim1 * dim2 + j * dim1 + i];
        ASSERT_DOUBLE_EQ(val, storage_3d_col_major->at(i, j, k));
        ASSERT_DOUBLE_EQ(val, storage_3d_row_major->at(i, j, k));
        ASSERT_DOUBLE_EQ(val, storage_3d_col_major_padded->at(i, j, k));
        ASSERT_DOUBLE_EQ(val, storage_3d_row_major_padded->at(i, j, k));
      }

  // --- 5D ---
  COPY_INTO_STORAGE(storage_5d_col_major_padded);
  COPY_INTO_STORAGE(storage_5d_row_major_padded);

  // Check data was copied correctly
  for(int m = 0; m < dim5; ++m)
    for(int l = 0; l < dim4; ++l)
      for(int k = 0; k < dim3; ++k)
        for(int j = 0; j < dim2; ++j)
          for(int i = 0; i < dim1; ++i) {
            double val = data[m * dim1 * dim2 * dim3 * dim4 + l * dim1 * dim2 * dim3 +
                              k * dim1 * dim2 + j * dim1 + i];
            ASSERT_DOUBLE_EQ(val, storage_5d_col_major_padded->at(i, j, k, l, m));
            ASSERT_DOUBLE_EQ(val, storage_5d_row_major_padded->at(i, j, k, l, m));
          }
}

TEST_F(StorageViewTest, isMemCopyable) {
  EXPECT_TRUE(storage_1d->toStorageView().isMemCopyable());
  EXPECT_FALSE(storage_1d_padded->toStorageView().isMemCopyable());
  EXPECT_TRUE(storage_2d_col_major->toStorageView().isMemCopyable());
  EXPECT_FALSE(storage_2d_col_major_padded->toStorageView().isMemCopyable());
  EXPECT_FALSE(storage_2d_row_major->toStorageView().isMemCopyable());
  EXPECT_FALSE(storage_2d_row_major_padded->toStorageView().isMemCopyable());
}
