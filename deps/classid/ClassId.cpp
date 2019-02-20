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

#include <fb/fbjni.h>
#include <fb/xplat_init.h>
#include <atomic>

#include <classid/classid_8_0_0.h>
#include <classid/classid_8_1_0.h>
#include <classid/classid_9_0_0.h>

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
  return facebook::xplat::initialize(vm, [] {
    facebook::jni::registerNatives(
        "com/facebook/common/dextricks/classid/ClassId",
        {
            makeNativeMethod(
                "getSignatureFromDexFile_8_0_0",
                facebook::classid::getSignatureFromDexFile_8_0_0),

            makeNativeMethod(
                "getSignatureFromDexFile_8_1_0",
                facebook::classid::getSignatureFromDexFile_8_1_0),

            makeNativeMethod(
                "getSignatureFromDexFile_9_0_0",
                facebook::classid::getSignatureFromDexFile_9_0_0),
        });
  });
}
