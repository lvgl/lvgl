/**
 * @file lv_gltf_data_animations.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_data_internal.hpp"

#if LV_USE_GLTF
#include <fastgltf/tools.hpp>

/*********************
 *      DEFINES
 *********************/

#define TIME_LOC_PREPASS_COUNT 16

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#include "fastgltf/math.hpp"
#include "lv_gltf_data_internal.hpp"
static fastgltf::math::fvec3 animation_get_vec3_at_timestamp(lv_gltf_model_t * data,
                                                             fastgltf::AnimationSampler * sampler,
                                                             float seconds);

static fastgltf::math::fquat animation_get_quat_at_timestamp(lv_gltf_model_t * data,
                                                             fastgltf::AnimationSampler * sampler,
                                                             float _seconds);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_gltf_data_get_animation_total_time(lv_gltf_model_t * data, uint32_t index)
{
    LV_ASSERT(data->asset.animations.size() > index);
    auto & animation = data->asset.animations[index];
    float max_time = -1.0f;
    for(uint64_t i = 0; i < animation.channels.size(); i++) {
        auto & accessor = data->asset.accessors[animation.samplers[i].inputAccessor];
        max_time = std::max(max_time, fastgltf::getAccessorElement<float>(data->asset, accessor, accessor.count - 1));
    }
    return (uint32_t)(max_time * 1000);
}

std::vector<uint32_t> * lv_gltf_data_animation_get_channel_set(std::size_t anim_num, lv_gltf_model_t * data,
                                                               fastgltf::Node & node)
{
    const auto & asset = lv_gltf_data_get_asset(data);
    size_t animation_count = lv_gltf_model_get_animation_count(data);
    if(data->channel_set_cache.find(&node) == data->channel_set_cache.end()) {
        std::vector<uint32_t> new_cache = std::vector<uint32_t>();
        if(animation_count > anim_num) {
            auto & anim = asset->animations[anim_num];

            for(uint64_t c = 0; c < anim.channels.size(); c++) {
                auto & channel = anim.channels[c];
                if(&(asset->nodes[channel.nodeIndex.value()]) == &node) {
                    new_cache.push_back(c);
                }
            }
        }
        data->channel_set_cache[&node] = new_cache;
    }
    return &data->channel_set_cache[&node];
}

void lv_gltf_data_animation_matrix_apply(float timestamp, std::size_t anim_num, lv_gltf_model_t * gltf_data,
                                         fastgltf::Node & node,
                                         fastgltf::math::fmat4x4 & matrix)
{
    const auto & asset = lv_gltf_data_get_asset(gltf_data);

    size_t animation_count = lv_gltf_model_get_animation_count(gltf_data);
    auto _channel_set = lv_gltf_data_animation_get_channel_set(anim_num, gltf_data, node);
    if(_channel_set->size() == 0) {
        return;
    }
    if(animation_count > anim_num) {
        auto & anim = asset->animations[anim_num];
        fastgltf::math::fvec3 newPos, newScale;
        fastgltf::math::fmat3x3 rotmat;
        for(const auto & c : (*_channel_set)) {
            switch(anim.channels[c].path) {
                case fastgltf::AnimationPath::Translation:
                    newPos = animation_get_vec3_at_timestamp(gltf_data, &anim.samplers[c], timestamp);
                    matrix[3][0] = newPos[0];
                    matrix[3][1] = newPos[1];
                    matrix[3][2] = newPos[2];
                    break;
                case fastgltf::AnimationPath::Rotation:
                    rotmat = fastgltf::math::asMatrix(animation_get_quat_at_timestamp(gltf_data, &anim.samplers[c], timestamp));
                    matrix[0][0] = rotmat[0][0];
                    matrix[0][1] = rotmat[0][1];
                    matrix[0][2] = rotmat[0][2];

                    matrix[1][0] = rotmat[1][0];
                    matrix[1][1] = rotmat[1][1];
                    matrix[1][2] = rotmat[1][2];

                    matrix[2][0] = rotmat[2][0];
                    matrix[2][1] = rotmat[2][1];
                    matrix[2][2] = rotmat[2][2];
                    break;
                case fastgltf::AnimationPath::Scale:
                    newScale = animation_get_vec3_at_timestamp(gltf_data, &anim.samplers[c], timestamp);
                    for(int32_t rs = 0; rs < 3; ++rs) {
                        matrix[0][rs] *= newScale[0];
                        matrix[1][rs] *= newScale[1];
                        matrix[2][rs] *= newScale[2];
                    }
                    break;
                case fastgltf::AnimationPath::Weights:
                    LV_LOG_WARN("Unhandled weights animation");
                    break;
            }
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static fastgltf::math::fquat animation_get_quat_at_timestamp(lv_gltf_model_t * data,
                                                             fastgltf::AnimationSampler * sampler,
                                                             float _seconds)
{
    const auto & asset = lv_gltf_data_get_asset(data);
    auto & _inAcc = asset->accessors[sampler->inputAccessor];
    auto & _outAcc = asset->accessors[sampler->outputAccessor];
    std::size_t _inAccCount = _inAcc.count;
    float _maxTime = fastgltf::getAccessorElement<float>(*asset, _inAcc, _inAccCount - 1);
    std::size_t _lowerIndex = 0;
    float _lowerTimestamp = 0.0f;

    if(_seconds < 0.001f) {
        _lowerIndex = 0;
    }
    else {
        std::size_t _firstCheckOffset = 0;
        std::size_t _lastCheckOffset = _inAccCount;
        std::size_t _prepassLeft = TIME_LOC_PREPASS_COUNT;
        while(_prepassLeft > 0) {
            _prepassLeft -= 1;
            if(_seconds >= fastgltf::getAccessorElement<float>(*asset, _inAcc, (_firstCheckOffset + _lastCheckOffset) >> 1)) {
                _firstCheckOffset = (_firstCheckOffset + _lastCheckOffset) >> 1;
            }
            else {
                _lastCheckOffset = (_firstCheckOffset + _lastCheckOffset) >> 1;
                if(_lastCheckOffset <= _firstCheckOffset + 1) {
                    _prepassLeft = 0;
                }
            }
        }
        for(uint64_t ii = _firstCheckOffset; ii < _inAccCount; ii++) {
            float _stampTime = fastgltf::getAccessorElement<float>(*asset, _inAcc, ii);
            if(_stampTime > _seconds) {
                _lowerIndex = ii - 1;
                break;
            }
            _lowerTimestamp = _stampTime;
        }
    }

    fastgltf::math::fquat _lowerValue = fastgltf::getAccessorElement<fastgltf::math::fquat>(*asset, _outAcc, _lowerIndex);
    if(_seconds >= _maxTime || _seconds <= 0.0f) {
        return _lowerValue;
    }
    std::size_t _upperIndex = _lowerIndex + 1;
    float _linDist = fastgltf::getAccessorElement<float>(*asset, _inAcc, _upperIndex) - _lowerTimestamp;
    return fastgltf::math::slerp(_lowerValue, fastgltf::getAccessorElement<fastgltf::math::fquat>(*asset, _outAcc,
                                                                                                  _upperIndex), (_seconds - _lowerTimestamp) / _linDist);
}

fastgltf::math::fvec3 animation_get_vec3_at_timestamp(lv_gltf_model_t * data, fastgltf::AnimationSampler * sampler,
                                                      float _seconds)
{
    const auto & asset = lv_gltf_data_get_asset(data);
    auto & _inAcc = asset->accessors[sampler->inputAccessor];
    auto & _outAcc = asset->accessors[sampler->outputAccessor];
    std::size_t _inAccCount = _inAcc.count;
    float _maxTime = fastgltf::getAccessorElement<float>(*asset, _inAcc, _inAccCount - 1);
    std::size_t _lowerIndex = 0;
    float _lowerTimestamp = 0.0f;

    if(_seconds < 0.001f) {
        _lowerIndex = 0;
    }
    else {
        std::size_t _firstCheckOffset = 0;
        std::size_t _lastCheckOffset = _inAccCount;
        std::size_t _prepassLeft = TIME_LOC_PREPASS_COUNT;
        while(_prepassLeft > 0) {
            _prepassLeft -= 1;
            if(_seconds >= fastgltf::getAccessorElement<float>(*asset, _inAcc, (_firstCheckOffset + _lastCheckOffset) >> 1)) {
                _firstCheckOffset = (_firstCheckOffset + _lastCheckOffset) >> 1;
            }
            else {
                _lastCheckOffset = (_firstCheckOffset + _lastCheckOffset) >> 1;
                if(_lastCheckOffset <= _firstCheckOffset + 1) {
                    _prepassLeft = 0;
                }
            }
        }
        for(uint64_t ii = _firstCheckOffset; ii < _inAccCount; ii++) {
            float _stampTime = fastgltf::getAccessorElement<float>(*asset, _inAcc, ii);
            if(_stampTime > _seconds) {
                _lowerIndex = ii - 1;
                break;
            }
            _lowerTimestamp = _stampTime;
        }
    }

    fastgltf::math::fvec3 _lowerValue = fastgltf::getAccessorElement<fastgltf::math::fvec3>(*asset, _outAcc, _lowerIndex);
    if(_seconds >= _maxTime || _seconds <= 0.0f) {
        return _lowerValue;
    }
    std::size_t _upperIndex = _lowerIndex + 1;
    fastgltf::math::fvec3 _upperValue = fastgltf::getAccessorElement<fastgltf::math::fvec3>(*asset, _outAcc, _upperIndex);
    float _upperTimestamp = fastgltf::getAccessorElement<float>(*asset, _inAcc, _upperIndex);
    return fastgltf::math::lerp(_lowerValue, _upperValue,
                                (_seconds - _lowerTimestamp) / (_upperTimestamp - _lowerTimestamp));
}

#endif /*LV_USE_GLTF*/
