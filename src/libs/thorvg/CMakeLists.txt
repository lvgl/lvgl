set(LVGL_LIBS_THORVG_SOURCES
    tvgAccessor.cpp
    tvgAnimation.cpp
    tvgBinaryDesc.cpp
    tvgCapi.cpp
    tvgCanvas.cpp
    tvgCompressor.cpp
    tvgFill.cpp
    tvgGlCanvas.cpp
    tvgInitializer.cpp
    tvgIteratorAccessor.cpp
    tvgLoader.cpp
    tvgLottieAnimation.cpp
    tvgLottieBuilder.cpp
    tvgLottieExpressions.cpp
    tvgLottieInterpolator.cpp
    tvgLottieLoader.cpp
    tvgLottieModel.cpp
    tvgLottieModifier.cpp
    tvgLottieParser.cpp
    tvgLottieParserHandler.cpp
    tvgMath.cpp
    tvgPaint.cpp
    tvgPicture.cpp
    tvgRawLoader.cpp
    tvgRender.cpp
    tvgSaver.cpp
    tvgScene.cpp
    tvgShape.cpp
    tvgStr.cpp
    tvgSvgCssStyle.cpp
    tvgSvgLoader.cpp
    tvgSvgPath.cpp
    tvgSvgSceneBuilder.cpp
    tvgSvgUtil.cpp
    tvgSwCanvas.cpp
    tvgSwFill.cpp
    tvgSwImage.cpp
    tvgSwMath.cpp
    tvgSwMemPool.cpp
    tvgSwPostEffect.cpp
    tvgSwRaster.cpp
    tvgSwRle.cpp
    tvgSwShape.cpp
    tvgSwRenderer.cpp
    tvgSwStroke.cpp
    tvgTaskScheduler.cpp
    tvgText.cpp
    tvgWgCanvas.cpp
    tvgXmlParser.cpp
)

# Create the main static library for this directory
add_library(lvgl_libs_thorvg STATIC ${LVGL_LIBS_THORVG_SOURCES})

# Add the current directory as an include directory
target_include_directories(lvgl_libs_thorvg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
