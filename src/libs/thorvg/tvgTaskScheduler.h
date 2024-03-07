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

#ifndef _TVG_TASK_SCHEDULER_H_
#define _TVG_TASK_SCHEDULER_H_

#include "tvgCommon.h"

namespace tvg
{

struct Task;

struct TaskScheduler
{
    static void init(unsigned threads);
    static void request(Task* task);
    static void async(bool on);
};

struct Task
{
private:

public:
    virtual ~Task() = default;

    void done()
    {
    }

protected:
    virtual void run(unsigned tid) = 0;

private:
    void operator()(unsigned tid)
    {
        run(tid);

    }

    void prepare()
    {
    }

    friend struct TaskSchedulerImpl;
};

}

#endif //_TVG_TASK_SCHEDULER_H_


#endif /* LV_USE_THORVG_INTERNAL */

