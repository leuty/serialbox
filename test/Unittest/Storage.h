//===-- Unittest/Storage.h ----------------------------------------------------------*- C++ -*-===//
//
//                                    S E R I A L B O X
//
// This file is distributed under terms of BSD license.
// See LICENSE.txt for more information
//
//===------------------------------------------------------------------------------------------===//
//
/// \file
/// This file implements a dummy storage used for testing.
///
//===------------------------------------------------------------------------------------------===//

#ifndef SERIALBOX_UNITTEST_STORAGE_H
#define SERIALBOX_UNITTEST_STORAGE_H

#include "serialbox/Core/Logging.h"
#include "serialbox/Core/StorageView.h"
#include "serialbox/Core/STLExtras.h"
#include <initializer_list>
#include <iosfwd>
#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

namespace serialbox {

namespace unittest {

/// \brief Represent dimensions of a Storage
struct Dims {
  Dims(std::initializer_list<int> d) : dims(d) {}
  std::initializer_list<int> dims;
};

/// \brief Represent padding of a Storage
struct Padding {
  Padding(std::initializer_list<std::pair<int, int>> p) : padding(p) {}
  std::initializer_list<std::pair<int, int>> padding;
};

/// \brief Return idx as type T
template<class T>
T sequential(int idx) {
  return T(idx);
}

/// \brief Represent a dummy storage to test the StorageView in absence of gridtools or STELLA
template <class T>
struct Storage {
  using value_type = T;
  
  /// \brief Storage order
  enum StorageOrderKind {
    RowMajor, ///< Stride 1 in last dimension
    ColMajor  ///< Stride 1 in first dimension
  };

  /// \brief Constructor
  /// @{
  Storage() = default;
  Storage(const Storage&) = default;
  Storage(Storage&&) = default;
  Storage& operator=(const Storage&) = default;
  Storage& operator=(Storage&&) = default;

//  Storage(StorageOrderKind ordering, std::initializer_list<int> dims, std::function<T(int)> init = sequential<T>)
//      : ordering_(ordering), dims_(dims) {
//    padding_.resize(dims_.size(), std::make_pair<int, int>(0, 0));
//    data_.resize(size(), T());
//    computeStrides();
//    forEach(init);
//  }
  
    Storage(StorageOrderKind ordering, std::initializer_list<int> dims, bool init)
        : ordering_(ordering), dims_(dims) {
      padding_.resize(dims_.size(), std::make_pair<int, int>(0, 0));
      data_.resize(size(), T());
      computeStrides();
      if(init)
        initData();
    }
  
  Storage(StorageOrderKind ordering, Dims dims, bool init = true)
      : ordering_(ordering), dims_(dims.dims) {
    padding_.resize(dims_.size(), std::make_pair<int, int>(0, 0));
    data_.resize(size(), T());
    computeStrides();

    if(init)
      initData();
  }

  Storage(StorageOrderKind ordering, std::initializer_list<int> dims,
          std::initializer_list<std::pair<int, int>> padding, bool init = true)
      : ordering_(ordering), dims_(dims), padding_(padding) {
    data_.resize(size(), T());
    computeStrides();

    if(init)
      initData();
  }
  
  Storage(StorageOrderKind ordering, Dims dims, Padding padding, bool init = true)
      : ordering_(ordering), dims_(dims.dims), padding_(padding.padding) {
    data_.resize(size(), T());
    computeStrides();

    if(init)
      initData();
  }
  
  /// @}  

  /// \brief Get total allocated size
  int size() const noexcept {
    int size = 1;
    for(std::size_t i = 0; i < dims_.size(); ++i)
      size *= (padding_[i].first + dims_[i] + padding_[i].second);
    return size;
  }
  
  /// \brief Get origin pointer
  T* originPtr() noexcept {
    T* ptr = data_.data();
    for(int i = 0; i < padding_.size(); ++i)
      ptr += (padding_[i].first * strides_[i]);
    return ptr;
  }
  const T* originPtr() const noexcept {
    T* ptr = data_.data();
    for(int i = 0; i < padding_.size(); ++i)
      ptr += (padding_[i].first * strides_[i]);
    return ptr;
  }

  /// \brief Access data
  /// @{
  template<class... Indices>
  T& at(const Indices&... indices) noexcept {
    return *(data_.data() + computeIndex(indices...));
  }
  
  template<class... Indices>
  const T& at(const Indices&... indices) const noexcept {
    return *(data_.data() + computeIndex(indices...));
  }
  
  template<class... Indices>
  T& operator()(const Indices&... indices) noexcept {
    return *(data_.data() + computeIndex(indices...));
  }
  
  template<class... Indices>
  const T& operator()(const Indices&... indices) const noexcept {
    return *(data_.data() + computeIndex(indices...));
  }
  
  /// @}

  /// \brief Convert to stream
  friend std::ostream& operator<<(std::ostream& stream, const Storage& s) {
    stream << "Storage [\n";
    stream << "  data = {\n";
    for(auto d : s.data_)
      stream << "    " << d << "\n";

    stream << "  }\n  size = " << s.size() << "\n";
    stream << "  dims = {";
    for(auto i : s.dims_)
      stream << " " << i;

    stream << " }\n  strides = {";
    for(auto i : s.strides_)
      stream << " " << i;

    stream << " }\n  padding = {";
    for(auto i : s.padding_)
      stream << " [" << i.first << "," << i.second << "]";

    stream << " }\n]\n";
    return stream;
  }

  /// \brief Convert to StorageView
  StorageView toStorageView() {
    auto type = ToTypeID<T>::value;
    return StorageView(data_.data(), type, dims_, strides_, padding_);
  }
  
  /// \brief Getter
  /// @{
  StorageOrderKind ordering() const noexcept { return ordering_; }
  
  std::vector<T>& data() noexcept { return data_; }
  const std::vector<T>& data() const noexcept { return data_; }
  
  std::vector<int>& dims() noexcept { return dims_; }
  const std::vector<int>& dims() const noexcept { return dims_; }
  
  std::vector<int>& strides() noexcept { return strides_; }
  const std::vector<int>& strides() const noexcept { return strides_; }
  
  std::vector<std::pair<int, int>>& padding() noexcept { return padding_; }
  const std::vector<std::pair<int, int>>& padding() const noexcept { return padding_; }
  /// @}

private:
  template<class... Indices>
  int computeIndex(const Indices&... indices) const noexcept {
    std::array<int, sizeof...(Indices)> index{{indices...}};
    int pos = 0;
    for(int i = 0; i < index.size(); ++i)
      pos += (padding_[i].first + index[i]) * strides_[i];
    return pos;
  }
  
  void computeStrides() noexcept {
    int numDim = dims_.size();

    strides_.resize(numDim);

    if(ordering_ == ColMajor) {
      int stride = 1;
      strides_[0] = stride;

      for(int i = 1; i < numDim; ++i) {
        stride *= (padding_[i - 1].first + dims_[i - 1] + padding_[i - 1].second);
        strides_[i] = stride;
      }
    } else {
      int stride = 1;
      strides_[numDim - 1] = stride;

      for(int i = numDim - 2; i >= 0; --i) {
        stride *= (padding_[i + 1].first + dims_[i + 1] + padding_[i + 1].second);
        strides_[i] = stride;
      }
    }
  }

  void initData() noexcept {
    for(std::size_t i = 0; i < data_.size(); ++i)
      data_[i] = i;
  }
  
  void forEach(std::function<T(int)>& f) {
    for(std::size_t i = 0; i < data_.size(); ++i)
      data_[i] = f(i);    
  }
  
private:
  StorageOrderKind ordering_;
  std::vector<T> data_;
  std::vector<int> dims_;
  std::vector<int> strides_;
  std::vector<std::pair<int, int>> padding_;
};

/// \brief Verify that the two Storages are equal 


} // namespace unittest

} // namespace serialbox

#endif
