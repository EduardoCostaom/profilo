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
package com.facebook.profilo.util;

import com.facebook.profilo.config.Config;
import com.facebook.profilo.config.ConfigImpl;
import com.facebook.profilo.config.ConfigParams;
import com.facebook.profilo.config.ConfigProvider;
import com.facebook.profilo.core.ProfiloConstants;
import java.util.TreeMap;

public final class TestConfigProvider implements ConfigProvider {

  private static final int TEST_TRACE_TIMEOUT_MS = 30000;
  private int mTimedOutUploadSampleRate;
  private final boolean mAllowConfigV2;

  public TestConfigProvider() {
    this(false);
  }

  public TestConfigProvider(boolean allowConfigV2) {
    mAllowConfigV2 = allowConfigV2;
  }

  public void setTimedOutUploadSampleRate(int timedOutUploadSampleRate) {
    mTimedOutUploadSampleRate = timedOutUploadSampleRate;
  }

  @Override
  public Config getFullConfig() {
    ConfigParams systemConfig = new ConfigParams();
    systemConfig.intParams = new TreeMap<>();
    systemConfig.intParams.put(
        ProfiloConstants.SYSTEM_CONFIG_TIMED_OUT_UPLOAD_SAMPLE_RATE, mTimedOutUploadSampleRate);
    return new ConfigImpl(0, systemConfig);
  }
}
