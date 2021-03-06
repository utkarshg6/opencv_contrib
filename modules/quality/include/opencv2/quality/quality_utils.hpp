// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef OPENCV_QUALITY_QUALITY_UTILS_HPP
#define OPENCV_QUALITY_QUALITY_UTILS_HPP

#include <limits>   // numeric_limits
#include "qualitybase.hpp"

namespace cv
{
namespace quality
{
namespace quality_utils
{

// default type of matrix to expand to
static CV_CONSTEXPR const int EXPANDED_MAT_DEFAULT_TYPE = CV_32F;

// expand matrix to target type
template <typename OutT, typename InT>
inline OutT expand_mat(const InT& src, int TYPE_DEFAULT = EXPANDED_MAT_DEFAULT_TYPE)
{
    OutT result = {};

    // by default, expand to 32F unless we already have >= 32 bits, then go to 64
    //  if/when we can detect OpenCL CV_16F support, opt for that when input depth == 8
    //  note that this may impact the precision of the algorithms and would need testing
    int type = TYPE_DEFAULT;

    switch (src.depth())
    {
    case CV_32F:
    case CV_32S:
    case CV_64F:
        type = CV_64F;
    };  // switch

    src.convertTo(result, type);
    return result;
}

// convert input array to vector of expanded mat types
template <typename R>
inline std::vector<R> expand_mats(InputArrayOfArrays arr, int TYPE_DEFAULT = EXPANDED_MAT_DEFAULT_TYPE)
{
    std::vector<R> result = {};
    std::vector<UMat> umats = {};
    std::vector<Mat> mats = {};

    if (arr.isUMatVector())
        arr.getUMatVector(umats);
    else if (arr.isUMat())
        umats.emplace_back(arr.getUMat());
    else if (arr.isMatVector())
        arr.getMatVector(mats);
    else if (arr.isMat())
        mats.emplace_back(arr.getMat());
    else
        CV_Error(Error::StsNotImplemented, "Unsupported input type");

    // convert umats, mats to expanded internal type
    for (auto& umat : umats)
        result.emplace_back(expand_mat<R>(umat, TYPE_DEFAULT ));

    for (auto& mat : mats)
        result.emplace_back(expand_mat<R>(mat, TYPE_DEFAULT ));

    return result;
}

// convert mse to psnr
inline double mse_to_psnr(double mse, double max_pixel_value)
{
    return (mse == 0.)
        ? std::numeric_limits<double>::infinity()
        : 10. * std::log10((max_pixel_value * max_pixel_value) / mse)
        ;
}

// convert scalar of mses to psnrs
inline cv::Scalar mse_to_psnr(cv::Scalar mse, double max_pixel_value)
{
    for (int i = 0; i < mse.rows; ++i)
        mse(i) = mse_to_psnr(mse(i), max_pixel_value);
    return mse;
}

}   // quality_utils
}   // quality
}   // cv
#endif