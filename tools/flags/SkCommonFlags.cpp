/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkCommonFlags.h"
#include "SkThreadPool.h"

DEFINE_string(config, "565 8888 pdf gpu nonrendering angle",
              "Options: 565 8888 pdf gpu nonrendering msaa4 msaa16 nvprmsaa4 nvprmsaa16 "
              "gpunull gpudebug angle mesa");

DEFINE_bool(cpu, true, "master switch for running CPU-bound work.");

DEFINE_bool(dryRun, false,
            "just print the tests that would be run, without actually running them.");

DEFINE_bool(gpu, true, "master switch for running GPU-bound work.");

DEFINE_string(gpuAPI, "", "Force use of specific gpu API.  Using \"gl\" "
              "forces OpenGL API. Using \"gles\" forces OpenGL ES API. "
              "Defaults to empty string, which selects the API native to the "
              "system.");

DEFINE_bool2(leaks, l, false, "show leaked ref cnt'd objects.");

DEFINE_string2(match, m, NULL,
               "[~][^]substring[$] [...] of GM name to run.\n"
               "Multiple matches may be separated by spaces.\n"
               "~ causes a matching GM to always be skipped\n"
               "^ requires the start of the GM to match\n"
               "$ requires the end of the GM to match\n"
               "^ and $ requires an exact match\n"
               "If a GM does not match any list entry,\n"
               "it is skipped unless some list entry starts with ~");

DEFINE_bool2(quiet, q, false, "if true, don't print status updates.");

DEFINE_bool(resetGpuContext, true, "Reset the GrContext before running each test.");
DEFINE_bool(abandonGpuContext, false, "Abandon the GrContext after running each test. "
                                      "Implies --resetGpuContext.");

DEFINE_bool2(single, z, false, "run tests on a single thread internally.");

DEFINE_string(skps, "skps", "Directory to read skps from.");

DEFINE_int32(threads, SkThreadPool::kThreadPerCore,
             "run threadsafe tests on a threadpool with this many threads.");

DEFINE_bool2(verbose, v, false, "enable verbose output from the test driver.");

DEFINE_bool2(veryVerbose, V, false, "tell individual tests to be verbose.");

DEFINE_string2(writePath, w, "", "If set, write bitmaps here as .pngs.");
