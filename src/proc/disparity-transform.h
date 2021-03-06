// Disparity transformation block is responsible to convert stereoscopic depth to disparity data
// and vice versa
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#pragma once

#include "../include/librealsense2/hpp/rs_frame.hpp"
#include "../include/librealsense2/hpp/rs_processing.hpp"

namespace librealsense
{

    class disparity_transform : public processing_block
    {
    public:
        disparity_transform(bool transform_to_disparity);

    protected:
        rs2::frame prepare_target_frame(const rs2::frame& f, const rs2::frame_source& source);

        template<typename Tin, typename Tout>
        void convert(const void* in_data, void* out_data)
        {
            static_assert((std::is_arithmetic<Tin>::value), "disparity transform requires numeric type for input data");
            static_assert((std::is_arithmetic<Tout>::value), "disparity transform requires numeric type for output data");

            auto in = reinterpret_cast<const Tin*>(in_data);
            auto out = reinterpret_cast<Tout*>(out_data);

            //TODO SSE optimize
            for (auto i = 0; i < _height; i++)
                for (auto j = 0; j < _width; j++)
                {
                    float input = *in;
                    if (std::isnormal(input))
                        *out++ = static_cast<Tout>(_d2d_convert_factor / input);
                    else
                        *out++ = 0;
                    in++;
                }
        }

    private:
        void    update_transformation_profile(const rs2::frame& f);

        void    on_set_mode(bool to_disparity);

        bool                    _transform_to_disparity;
        rs2::stream_profile     _source_stream_profile;
        rs2::stream_profile     _target_stream_profile;
        bool                    _update_target;
        bool                    _stereoscopic_depth;
        float                   _focal_lenght_mm;
        float                   _stereo_baseline_mm;
        float                   _d2d_convert_factor;
        size_t                  _width, _height;
        size_t                  _bpp;
    };
}
