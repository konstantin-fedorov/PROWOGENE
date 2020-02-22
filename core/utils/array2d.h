#ifndef PROWOGENE_CORE_UTILS_ARRAY2D_H_
#define PROWOGENE_CORE_UTILS_ARRAY2D_H_

#include <vector>

namespace prowogene {
namespace utils {

/** @brief 2D data storage. */
template<typename T>
class Array2D {
 public:
    /** Constructor.
    @param [in] width - Width of 2D array.
    @param [in] height - Height of 2D array.
    @param [in] val - Default value. */
    Array2D(int width = 0, int height = 0, const T val = T()) {
        default_val_ = val;
        if (width || height) {
            Resize(width, height, val);
        } else {
            width_ =  0;
            height_ = 0;
        }
    }

    /** Copy constructor. */
    Array2D(const Array2D& src) {
        default_val_ = src.default_val_;
        width_  = src.width_;
        height_ = src.height_;
        data_   = src.data_;
    }

    /** Get element in array by coordinates.
    @param [in] w - Horisontal position from left.
    @param [in] h - Vertical position from left.*/
    virtual T& operator()(int w, int h) {
        if (w < width_ && h < height_) {
            const int idx = h * width_ + w;
            return data_[idx];
        }
        return default_val_;
    }

    /** @copydoc Array2D::operator()(int,int) */
    virtual const T& operator()(int w, int h) const {
        if (w < width_ && h < height_) {
            const int idx = h * width_ + w;
            return data_[idx];
        }
        return default_val_;
    }

    /** Get 2D array width.
    @return Width of array. */
    virtual int Width() const {
        return width_;
    }

    /** Get 2D array height.
    @return Height of array. */
    virtual int Height() const {
        return height_;
    }

    /** Get elements count.
    @return Elements count. */
    virtual size_t Size() const {
        return data_.size();
    }

    /** Get raw 2D array data. */
    virtual T* Data() {
        return data_.data();
    }

    /** @copydoc Array2D::Data() */
    virtual const T* Data() const {
        return data_.data();
    }

    /** Resize array.
    @param [in] width  - Width of 2D array.
    @param [in] height - Height of 2D array. */
    virtual void Resize(int w, int h) {
        Resize(w, h, default_val_);
    }

    /** Resize array with default value.
    @param [in] width  - Width of 2D array.
    @param [in] height - Height of 2D array.
    @param [in] val    - Default value. */
    virtual void Resize(int w, int h, T val) {
        try {
            if (w < 0) {
                w = 0;
            }
            if (h < 0) {
                h = 0;
            }
            data_.clear();
            const int size = w * h;
            data_.resize(size, val);
            width_ =  w;
            height_ = h;
        } catch (std::bad_alloc ba) {
            width_ =  0;
            height_ = 0;
        }
    }

    /** Clear array data. */
    virtual void Clear() {
        data_.clear();
        width_ = 0;
        height_ = 0;
    }

    /** Get first iterator.
    @return First iterator. */
    virtual typename std::vector<T>::iterator begin() {
        return data_.begin();
    }

    /** @copydoc Array2D::begin() */
    virtual typename std::vector<T>::const_iterator begin() const {
        return data_.begin();
    }

    /** Get last iterator.
    @return Last iterator. */
    virtual typename std::vector<T>::iterator end() {
        return data_.end();
    }

    /** @copydoc Array2D::end() */
    virtual typename std::vector<T>::const_iterator end() const {
        return data_.end();
    }

 protected:
    /** Width of array. */
    int            width_;
    /** Height of array. */
    int            height_;
    /** Array data. */
    std::vector<T> data_;
    /** Default array value. */
    T              default_val_;
};

} // namespase utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_ARRAY2D_H_
