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
package com.facebook.profilo.core;

import com.facebook.profilo.config.ControllerConfig;
import com.facebook.profilo.ipc.TraceContext;
import javax.annotation.Nullable;

public abstract class TraceController {

  /**
   * Determine if the configuration allows for a trace to start.
   *
   * @param context the trace context object passed to {@link TraceControl#startTrace(int, int,
   *     Object, long)}
   * @param config the current config for this controller
   * @return 0 if a trace is not allowed, a non-0 mask of PROVIDER_ constants otherwise. These
   *     providers will be enabled for the duration of the trace.
   */
  public abstract int evaluateConfig(
      long longContext, @Nullable Object context, ControllerConfig config);

  /**
   * Determine current configuration extra parameters for providers.
   *
   * @param context the trace context object passed to {@link TraceControl#startTrace(int, int,
   *     Object, long)}
   * @param config the current config for this controller
   * @return sampling rate in milliseconds, 0 if not configured
   */
  public abstract TraceContext.TraceConfigExtras getTraceConfigExtras(
      long longContext, @Nullable Object context, ControllerConfig config);

  public abstract boolean contextsEqual(
      long fstLong, @Nullable Object fst, long sndLong, @Nullable Object snd);

  /**
   * Returns true if the controller is based on a config, and false otherwise which means hardcoded
   * behavior.
   */
  public abstract boolean isConfigurable();
}
