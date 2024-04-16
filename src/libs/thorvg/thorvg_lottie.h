#ifndef _THORVG_LOTTIE_H_
#define _THORVG_LOTTIE_H_

#include "thorvg.h"

namespace tvg
{

/**
 * @class LottieAnimation
 *
 * @brief The LottieAnimation class enables control of advanced Lottie features.
 *
 * This class extends the Animation and has additional interfaces.
 *
 * @see Animation
 *
 * @note Experimental API
 */

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL
class TVG_API LottieAnimation final : public Animation
{
public:
    ~LottieAnimation();

    /**
     * @brief Override Lottie properties using slot data.
     *
     * @param[in] slot The Lottie slot data in JSON format.
     *
     * @retval Result::Success When succeed.
     * @retval Result::InsufficientCondition In case the animation is not loaded.
     * @retval Result::InvalidArguments When the given parameter is invalid.
     *
     * @note Experimental API
     */
    Result override(const char* slot) noexcept;

    /**
     * @brief Creates a new LottieAnimation object.
     *
     * @return A new LottieAnimation object.
     *
     * @note Experimental API
     */
    static std::unique_ptr<LottieAnimation> gen() noexcept;
};

} //namespace

#endif //_THORVG_LOTTIE_H_

#endif /* LV_USE_THORVG_INTERNAL */

