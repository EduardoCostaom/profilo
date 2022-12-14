/**
 * Copyright 2004-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <inttypes.h>
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#include <stddef.h>
#include <ucontext.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SUCCESS = 0,
  EMPTY_STACK = 1,
  STACK_OVERFLOW = 2,
  NO_STACK_FOR_THREAD = 3,
  SIGNAL_INTERRUPT = 4,
  NESTED_UNWIND = 5,
  TRACER_DISABLED = 6,
  IGNORE = 7,
  PARTIAL_STACK = 8,
  STACK_COPY_FAILED = 9,
  UNWINDER_QUEUE_OVERFLOW = 10,
  INVALID_MAP = 11,
  MAXVAL,
} StackCollectionRetcode;

// A list of available external tracer types.
// Must match the value in tracers::Tracer enumeration in BaseTracer.h.
const int TRACER_TYPE_JAVASCRIPT = 1 << 9;

typedef void (
    *profilo_int_mark_start)(const char* provider, const char* msg, size_t len);
typedef void (*profilo_int_mark_end)(const char* provider);
typedef void (*profilo_int_log_classload_start)(const char* provider);
typedef void (
    *profilo_int_log_classload_end)(const char* provider, int64_t classid);
typedef void (*profilo_int_log_classload_failed)(const char* provider);
typedef bool (*profilo_int_is_enabled)(const char* provider);
// This callback type is of similar signature as BaseTracer::collectStack()
// function. However, depth parameter is pointer type instead of reference
// so that this interface can be used with C language.
typedef StackCollectionRetcode (*profilo_int_collect_stack_fn)(
    ucontext_t* ucontext,
    int64_t* frames,
    uint16_t* depth,
    uint16_t max_depth);
typedef bool (*profilo_int_register_external_tracer_callback)(
    int tracerType,
    profilo_int_collect_stack_fn callback);

typedef struct ProfiloApi {
  profilo_int_mark_start mark_start;
  profilo_int_mark_end mark_end;
  profilo_int_log_classload_start log_classload_start;
  profilo_int_log_classload_end log_classload_end;
  profilo_int_log_classload_failed log_classload_failed;
  profilo_int_is_enabled is_enabled;
  profilo_int_register_external_tracer_callback
      register_external_tracer_callback;

} ProfiloApi;

extern ProfiloApi profilo_api_int;

#ifdef __cplusplus
} // extern "C"
#endif
