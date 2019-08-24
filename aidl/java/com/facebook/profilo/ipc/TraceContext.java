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
package com.facebook.profilo.ipc;

import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import java.util.Set;
import java.util.TreeMap;
import javax.annotation.Nullable;

public final class TraceContext implements Parcelable {

  // Trace config extra configuration parameters.
  // The granularity of these extras applies to everything within the trace
  // config, which means that they apply to all the providers.
  public static final class TraceConfigExtras implements Parcelable {
    public static final TraceConfigExtras EMPTY = new TraceConfigExtras(null, null, null);

    private final @Nullable TreeMap<String, Integer> mIntParams;
    private final @Nullable TreeMap<String, Boolean> mBoolParams;
    private final @Nullable TreeMap<String, int[]> mIntArrayParams;

    TraceConfigExtras(Parcel in) {
      Bundle intParamsBundle = in.readBundle(getClass().getClassLoader());
      Set<String> intParamKeys = intParamsBundle.keySet();
      if (!intParamKeys.isEmpty()) {
        mIntParams = new TreeMap<>();
        for (String key : intParamKeys) {
          mIntParams.put(key, intParamsBundle.getInt(key));
        }
      } else {
        mIntParams = null;
      }

      Bundle boolParamsBundle = in.readBundle(getClass().getClassLoader());
      Set<String> boolParamKeys = boolParamsBundle.keySet();
      if (!boolParamKeys.isEmpty()) {
        mBoolParams = new TreeMap<>();
        for (String key : boolParamKeys) {
          mBoolParams.put(key, boolParamsBundle.getBoolean(key));
        }
      } else {
        mBoolParams = null;
      }

      Bundle intArrayParamsBundle = in.readBundle(getClass().getClassLoader());
      Set<String> intArrayParamKeys = intArrayParamsBundle.keySet();
      if (!intArrayParamKeys.isEmpty()) {
        mIntArrayParams = new TreeMap<>();
        for (String key : boolParamKeys) {
          mIntArrayParams.put(key, intArrayParamsBundle.getIntArray(key));
        }
      } else {
        mIntArrayParams = null;
      }
    }

    public TraceConfigExtras(
        @Nullable TreeMap<String, Integer> intParams,
        @Nullable TreeMap<String, Boolean> boolParams,
        @Nullable TreeMap<String, int[]> intArrayParams) {
      mIntParams = intParams;
      mBoolParams = boolParams;
      mIntArrayParams = intArrayParams;
    }

    public int getIntParam(String key, int defaultValue) {
      if (mIntParams == null) {
        return defaultValue;
      }
      Integer value = mIntParams.get(key);
      return value == null ? defaultValue : value;
    }

    public boolean getBoolParam(String key, boolean defaultValue) {
      if (mBoolParams == null) {
        return defaultValue;
      }
      Boolean value = mBoolParams.get(key);
      return value == null ? defaultValue : value;
    }

    @Nullable
    public int[] getIntArrayParam(String key) {
      if (mIntArrayParams == null) {
        return null;
      }
      return mIntArrayParams.get(key);
    }

    public static final Parcelable.Creator<TraceConfigExtras> CREATOR =
        new Parcelable.Creator<TraceConfigExtras>() {
          public TraceConfigExtras createFromParcel(Parcel in) {
            return new TraceConfigExtras(in);
          }

          public TraceConfigExtras[] newArray(int size) {
            return new TraceConfigExtras[size];
          }
        };

    @Override
    public int describeContents() {
      return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
      Bundle intParamsBundle = new Bundle();
      if (mIntParams != null) {
        for (TreeMap.Entry<String, Integer> entry : mIntParams.entrySet()) {
          intParamsBundle.putInt(entry.getKey(), entry.getValue());
        }
      }
      dest.writeBundle(intParamsBundle);
      Bundle boolParamsBundle = new Bundle();
      if (mBoolParams != null) {
        for (TreeMap.Entry<String, Boolean> entry : mBoolParams.entrySet()) {
          boolParamsBundle.putBoolean(entry.getKey(), entry.getValue());
        }
      }
      dest.writeBundle(boolParamsBundle);
      Bundle intArrayParamsBundle = new Bundle();
      if (mIntArrayParams != null) {
        for (TreeMap.Entry<String, int[]> entry : mIntArrayParams.entrySet()) {
          intArrayParamsBundle.putIntArray(entry.getKey(), entry.getValue());
        }
      }
      dest.writeBundle(intArrayParamsBundle);
    }
  }

  public long traceId;
  public String encodedTraceId;
  public int controller;
  @Nullable public Object controllerObject;
  @Nullable public Object context;
  public long longContext;
  public int enabledProviders;
  public int flags;
  public int abortReason;
  public TraceConfigExtras mTraceConfigExtras;

  public static final Parcelable.Creator<TraceContext> CREATOR =
      new Parcelable.Creator<TraceContext>() {
        public TraceContext createFromParcel(Parcel in) {
          return new TraceContext(in);
        }

        public TraceContext[] newArray(int size) {
          return new TraceContext[size];
        }
      };

  public TraceContext() {}

  public TraceContext(
      long traceId,
      String encodedTraceId,
      int controller,
      @Nullable Object controllerObject,
      @Nullable Object context,
      long longContext,
      int enabledProviders,
      int flags,
      int abortReason,
      TraceConfigExtras traceConfigExtras) {
    this.traceId = traceId;
    this.encodedTraceId = encodedTraceId;
    this.controller = controller;
    this.controllerObject = controllerObject;
    this.context = context;
    this.longContext = longContext;
    this.enabledProviders = enabledProviders;
    this.flags = flags;
    this.abortReason = abortReason;
    this.mTraceConfigExtras = traceConfigExtras;
  }

  public TraceContext(
      long traceId,
      String encodedTraceId,
      int controller,
      Object controllerObject,
      Object context,
      long longContext,
      int enabledProviders,
      int flags,
      TraceConfigExtras traceConfigExtras) {
    this(
        traceId,
        encodedTraceId,
        controller,
        controllerObject,
        context,
        longContext,
        enabledProviders,
        flags,
        (short) 0,
        traceConfigExtras);
  }

  public TraceContext(TraceContext traceContext) {
    this(
        traceContext.traceId,
        traceContext.encodedTraceId,
        traceContext.controller,
        traceContext.controllerObject,
        traceContext.context,
        traceContext.longContext,
        traceContext.enabledProviders,
        traceContext.flags,
        traceContext.abortReason,
        traceContext.mTraceConfigExtras);
  }

  public TraceContext(TraceContext traceContext, int abortReason) {
    this(
        traceContext.traceId,
        traceContext.encodedTraceId,
        traceContext.controller,
        traceContext.controllerObject,
        traceContext.context,
        traceContext.longContext,
        traceContext.enabledProviders,
        traceContext.flags,
        abortReason,
        traceContext.mTraceConfigExtras);
  }

  public TraceContext(TraceContext traceContext, int controller, Object controllerObject) {
    this(
        traceContext.traceId,
        traceContext.encodedTraceId,
        controller,
        controllerObject,
        traceContext.context,
        traceContext.longContext,
        traceContext.enabledProviders,
        traceContext.flags,
        traceContext.abortReason,
        traceContext.mTraceConfigExtras);
  }

  public TraceContext(Parcel src) {
    readFromParcel(src);
  }

  public void readFromParcel(Parcel src) {
    this.traceId = src.readLong();
    this.encodedTraceId = src.readString();
    this.controller = src.readInt();
    this.controllerObject = null;
    this.context = null;
    this.longContext = src.readLong();
    this.enabledProviders = src.readInt();
    this.flags = src.readInt();
    this.abortReason = src.readInt();
    this.mTraceConfigExtras = TraceConfigExtras.CREATOR.createFromParcel(src);
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
    dest.writeLong(traceId);
    dest.writeString(encodedTraceId);
    dest.writeInt(controller);
    dest.writeLong(longContext);
    dest.writeInt(enabledProviders);
    dest.writeInt(this.flags);
    dest.writeInt(abortReason);
    mTraceConfigExtras.writeToParcel(dest, flags);
  }
}
