/**
 * Copyright 2004-present, Facebook, Inc.
 *
 * <p>Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the License at
 *
 * <p>http://www.apache.org/licenses/LICENSE-2.0
 *
 * <p>Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.facebook.profilo.mmapbuf;

import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;
import com.facebook.soloader.SoLoader;

@DoNotStrip
public class Buffer {

  static {
    SoLoader.loadLibrary("profilo_mmapbuf");
  }

  @DoNotStrip private final HybridData mHybridData;

  private Buffer(HybridData data) {
    mHybridData = data;
  }

  @DoNotStrip
  public native void updateHeader(int providers, long longContext, long traceId);

  @DoNotStrip
  public native void updateId(String sessionId);

  @DoNotStrip
  public native void updateFilePath(String filePath);

  @DoNotStrip
  public native void updateMemoryMappingFilename(String mappingFilePath);

  @DoNotStrip
  public native String getMemoryMappingFilename();

  @DoNotStrip
  public native String getFilePath();
}
