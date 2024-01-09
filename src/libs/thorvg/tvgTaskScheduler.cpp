/*
 * Copyright (c) 2020 - 2023 the ThorVG project. All rights reserved.

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

#include <vector>
#include <atomic>
#include "tvgTaskScheduler.h"

/************************************************************************/
/* Internal Class Implementation                                        */
/************************************************************************/

namespace tvg {

static thread_local bool _async = true;  //toggle async tasking for each thread on/off


struct TaskSchedulerImpl
{
    TaskSchedulerImpl(unsigned threadCnt)
    {
    }

    ~TaskSchedulerImpl()
    {
    }

    void request(Task* task)
    {
        task->run(0);
    }
};

}

static TaskSchedulerImpl* inst = nullptr;

/************************************************************************/
/* External Class Implementation                                        */
/************************************************************************/

void TaskScheduler::init(unsigned threads)
{
    if (inst) return;
    inst = new TaskSchedulerImpl(threads);
}

void TaskScheduler::request(Task* task)
{
    if (inst) inst->request(task);
}

void TaskScheduler::async(bool on)
{
    _async = on;
}

#endif /* LV_USE_THORVG_INTERNAL */

