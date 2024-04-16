/*
 * Copyright (c) 2023 - 2024 the ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL

#include "tvgLottieLoader.h"
#include "tvgLottieModel.h"
#include "tvgLottieParser.h"
#include "tvgLottieBuilder.h"
#include "tvgStr.h"

/************************************************************************/
/* Internal Class Implementation                                        */
/************************************************************************/

static bool _checkDotLottie(const char *str)
{
    //check the .Lottie signature.
    if (str[0] == 0x50 && str[1] == 0x4B && str[2] == 0x03 && str[3] == 0x04) return true;
    else return false;
}


static float _str2float(const char* str, int len)
{
    auto tmp = strDuplicate(str, len);
    auto ret = strToFloat(tmp, nullptr);
    free(tmp);
    return ret;
}


void LottieLoader::run(unsigned tid)
{
    //update frame
    if (comp) {
        builder->update(comp, frameNo);
    //initial loading
    } else {
        LottieParser parser(content, dirName);
        if (!parser.parse()) return;
        comp = parser.comp;
        builder->build(comp);
    }
}


/************************************************************************/
/* External Class Implementation                                        */
/************************************************************************/

LottieLoader::LottieLoader() : FrameModule(FileType::Lottie), builder(new LottieBuilder)
{

}


LottieLoader::~LottieLoader()
{
    this->done();

    if (copy) free((char*)content);
    free(dirName);

    //TODO: correct position?
    delete(comp);
    delete(builder);
}


bool LottieLoader::header()
{
    //A single thread doesn't need to perform intensive tasks.
    if (TaskScheduler::threads() == 0) {
        run(0);
        if (comp) {
            w = static_cast<float>(comp->w);
            h = static_cast<float>(comp->h);
            frameDuration = comp->duration();
            frameCnt = comp->frameCnt();
            return true;
        } else {
            return false;
        }
    }

    //Quickly validate the given Lottie file without parsing in order to get the animation info.
    auto startFrame = 0.0f;
    auto endFrame = 0.0f;
    auto frameRate = 0.0f;
    uint32_t depth = 0;

    auto p = content;

    while (*p != '\0') {
        if (*p == '{') {
            ++depth;
            ++p;
            continue;
        }
        if (*p == '}') {
            --depth;
            ++p;
            continue;
        }
        if (depth != 1) {
            ++p;
            continue;
        }
        //version.
        if (!strncmp(p, "\"v\":", 4)) {
            p += 4;
            continue;
        }

        //framerate
        if (!strncmp(p, "\"fr\":", 5)) {
            p += 5;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            frameRate = _str2float(p, e - p);
            p = e;
            continue;
        }

        //start frame
        if (!strncmp(p, "\"ip\":", 5)) {
            p += 5;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            startFrame = _str2float(p, e - p);
            p = e;
            continue;
        }

        //end frame
        if (!strncmp(p, "\"op\":", 5)) {
            p += 5;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            endFrame = _str2float(p, e - p);
            p = e;
            continue;
        }

        //width
        if (!strncmp(p, "\"w\":", 4)) {
            p += 4;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            w = static_cast<float>(str2int(p, e - p));
            p = e;
            continue;
        }
        //height
        if (!strncmp(p, "\"h\":", 4)) {
            p += 4;
            auto e = strstr(p, ",");
            if (!e) e = strstr(p, "}");
            h = static_cast<float>(str2int(p, e - p));
            p = e;
            continue;
        }
        ++p;
    }

    if (frameRate < FLT_EPSILON) {
        TVGLOG("LOTTIE", "Not a Lottie file? Frame rate is 0!");
        return false;
    }

    frameCnt = (endFrame - startFrame);
    frameDuration = frameCnt / frameRate;

    TVGLOG("LOTTIE", "info: frame rate = %f, duration = %f size = %d x %d", frameRate, frameDuration, (int)w, (int)h);

    return true;
}


bool LottieLoader::open(const char* data, uint32_t size, bool copy)
{
    //If the format is dotLottie
    auto dotLottie = _checkDotLottie(data);
    if (dotLottie) {
        TVGLOG("LOTTIE", "Requested .Lottie Format, Not Supported yet.");
        return false;
    }

    if (copy) {
        content = (char*)malloc(size);
        if (!content) return false;
        memcpy((char*)content, data, size);
    } else content = data;

    this->size = size;
    this->copy = copy;

    return header();
}


bool LottieLoader::open(const string& path)
{
    auto f = fopen(path.c_str(), "r");
    if (!f) return false;

    fseek(f, 0, SEEK_END);

    size = ftell(f);
    if (size == 0) {
        fclose(f);
        return false;
    }

    auto content = (char*)(malloc(sizeof(char) * size + 1));
    fseek(f, 0, SEEK_SET);
    auto ret = fread(content, sizeof(char), size, f);
    if (ret < size) {
        fclose(f);
        return false;
    }
    content[size] = '\0';

    fclose(f);

    //If the format is dotLottie
    auto dotLottie = _checkDotLottie(content);
    if (dotLottie) {
        TVGLOG("LOTTIE", "Requested .Lottie Format, Not Supported yet.");
        return false;
    }

    this->dirName = strDirname(path.c_str());
    this->content = content;
    this->copy = true;

    return header();
}


bool LottieLoader::resize(Paint* paint, float w, float h)
{
    if (!paint) return false;

    auto sx = w / this->w;
    auto sy = h / this->h;
    Matrix m = {sx, 0, 0, 0, sy, 0, 0, 0, 1};
    paint->transform(m);

    //apply the scale to the base clipper
    const Paint* clipper;
    paint->composite(&clipper);
    if (clipper) const_cast<Paint*>(clipper)->transform(m);

    return true;
}


bool LottieLoader::read()
{
    if (!content || size == 0) return false;

    //the loading has been already completed
    if (comp || !LoadModule::read()) return true;

    TaskScheduler::request(this);

    return true;
}


Paint* LottieLoader::paint()
{
    this->done();
    if (!comp) return nullptr;
    comp->initiated = true;
    return comp->root->scene;
}


bool LottieLoader::override(const char* slot)
{
    if (!slot || !comp || comp->slots.count == 0) return false;

    //TODO: Crashed, does this necessary?
    auto temp = strdup(slot);

    //parsing slot json
    LottieParser parser(temp, dirName);
    auto sid = parser.sid();
    if (!sid) {
        free(temp);
        return false;
    }

    bool ret = false;
    for (auto s = comp->slots.begin(); s < comp->slots.end(); ++s) {
        if (strcmp((*s)->sid, sid)) continue;
        ret = parser.parse(*s);
        break;
    }

    free(temp);
    return ret;
}


bool LottieLoader::frame(float no)
{
    //no meaing to update if frame diff is less then 1ms
    if (fabsf(this->frameNo - no) < 0.001f) return false;

    this->done();

    this->frameNo = no;

    TaskScheduler::request(this);

    return true;
}


float LottieLoader::totalFrame()
{
    return frameCnt;
}


float LottieLoader::curFrame()
{
    return frameNo;
}


float LottieLoader::duration()
{
    return frameDuration;
}


void LottieLoader::sync()
{
    this->done();
}

#endif /* LV_USE_THORVG_INTERNAL */

