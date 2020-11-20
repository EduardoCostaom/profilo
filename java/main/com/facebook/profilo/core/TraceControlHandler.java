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

import android.annotation.SuppressLint;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import com.facebook.androidinternals.android.os.SystemPropertiesInternal;
import com.facebook.common.build.BuildConstants;
import com.facebook.profilo.ipc.TraceContext;
import com.facebook.profilo.logger.Logger;
import com.facebook.profilo.logger.Trace;
import java.util.HashSet;
import java.util.Random;
import javax.annotation.Nullable;
import javax.annotation.concurrent.GuardedBy;

/**
 * Thread responsible for stopping the trace, which will synchronously gather contextual data for
 * the trace.
 */
@SuppressLint({
  "BadMethodUse-android.util.Log.d",
})
public class TraceControlHandler extends Handler {

  private static class AnnotationContext {
    public TraceContext mTraceContext;
    public String mAnnotation;

    public AnnotationContext(TraceContext context, String annotation) {
      mTraceContext = context;
      mAnnotation = annotation;
    }
  }

  private static final String LOG_TAG = "Profilo/TraceControlThread";
  private static final int MSG_TIMEOUT_TRACE = 0;
  private static final int MSG_START_TRACE = 1;
  private static final int MSG_STOP_TRACE = 2;
  private static final int MSG_ABORT_TRACE = 3;
  private static final int MSG_END_TRACE = 4;

  // Messages related to upload condition management
  private static final int MSG_EVENT_DURATION = 5;
  private static final int MSG_CONDITIONAL_TRACE_STOP = 6;
  private static final int MSG_EVENT_QPL_ANNOTATION = 7;

  // Set this system property to enable logs.
  private static final String PROFILO_LOG_LEVEL_SYSTEM_PROPERTY = "com.facebook.profilo.log";

  @GuardedBy("this")
  private final @Nullable TraceControl.TraceControlListener mListener;

  @GuardedBy("this")
  private final HashSet<Long> mTraceContexts;

  @GuardedBy("this")
  private final TraceConditionManager mTraceConditionManager;

  private final Random mRandom;

  static class LogLevel {
    // Lazy load system properties.
    private static final boolean LOG_DEBUG_MESSAGE =
        BuildConstants.isInternalBuild()
            || "debug".equals(SystemPropertiesInternal.get(PROFILO_LOG_LEVEL_SYSTEM_PROPERTY));
  }

  public TraceControlHandler(@Nullable TraceControl.TraceControlListener listener, Looper looper) {
    super(looper);
    mListener = listener;
    mTraceContexts = new HashSet<>();
    mTraceConditionManager = new TraceConditionManager();
    mRandom = new Random();
  }

  @Override
  public void handleMessage(Message msg) {
    TraceContext traceContext = null;
    if (msg.what != MSG_EVENT_QPL_ANNOTATION) {
      traceContext = (TraceContext) msg.obj;
    }
    switch (msg.what) {
      case MSG_TIMEOUT_TRACE:
        timeoutTrace(traceContext.traceId);
        break;
      case MSG_START_TRACE:
        startTraceAsync(traceContext);
        break;
      case MSG_STOP_TRACE:
        stopTrace(traceContext);
        break;
      case MSG_END_TRACE:
        endTrace(traceContext);
        break;
      case MSG_ABORT_TRACE:
        abortTrace(traceContext);
        break;
      case MSG_EVENT_DURATION:
        processDurationEvent(traceContext, msg.arg1);
        break;
      case MSG_CONDITIONAL_TRACE_STOP:
        conditionalTraceStop(traceContext);
        break;
      case MSG_EVENT_QPL_ANNOTATION:
        AnnotationContext annotationContext = (AnnotationContext) msg.obj;
        processAnnotationEvent(annotationContext.mTraceContext, annotationContext.mAnnotation);
        break;
      default:
        break;
    }
  }

  protected synchronized void stopTrace(TraceContext context) {
    // stop any timeout timer associated with the thread
    removeMessages(MSG_TIMEOUT_TRACE, context);

    if ((context.flags & Trace.FLAG_MEMORY_ONLY) != 0) {
      // For in-memory trace everything in the tracing buffer trace is started and then immediately
      // stopped after dumping the contents of the buffer.
      Logger.postCreateBackwardTrace(context.traceId, context.flags);
    }

    // Schedule an optionally delayed *actual* end of the trace
    sendMessageDelayed(
        obtainMessage(MSG_END_TRACE, context),
        context.mTraceConfigExtras.getIntParam(
            ProfiloConstants.TRACE_CONFIG_PARAM_POST_TRACE_EXTENSION_MSEC,
            ProfiloConstants.TRACE_CONFIG_PARAM_POST_TRACE_EXTENSION_MSEC_DEFAULT));
  }

  protected void conditionalTraceStop(TraceContext context) {
    int uploadSampleRate = 0;
    uploadSampleRate = mTraceConditionManager.getUploadSampleRate(context.traceId);
    if (uploadSampleRate == 0 || mRandom.nextInt(uploadSampleRate) != 0) {
      Logger.postAbortTrace(context.traceId);
      onTraceAbort(new TraceContext(context, ProfiloConstants.ABORT_REASON_CONDITION_NOT_MET));
    } else {
      Logger.postConditionalUploadRate(uploadSampleRate);
      Logger.postPreCloseTrace(context.traceId);
      onTraceStop(context);
    }

    mTraceConditionManager.unregisterTrace(context);
  }

  protected void processDurationEvent(TraceContext context, long duration) {
    mTraceConditionManager.processDurationEvent(context.traceId, duration);
  }

  protected void processAnnotationEvent(TraceContext context, String annotation) {
    mTraceConditionManager.processAnnotationEvent(context.traceId, annotation);
  }

  protected void endTrace(TraceContext context) {
    // This also runs teardown for all providers
    if (mListener != null) {
      mListener.onTraceStop(context);
    }
    Logger.postCloseTrace(context.traceId);
  }

  protected void abortTrace(TraceContext context) {
    // stop any timeout timer associated with the thread
    synchronized (this) {
      removeMessages(MSG_TIMEOUT_TRACE, context);
    }
    if (mListener != null) {
      mListener.onTraceAbort(context);
    }
  }

  protected static void timeoutTrace(long traceId) {
    if (LogLevel.LOG_DEBUG_MESSAGE) {
      Log.d(LOG_TAG, "Timing out trace " + traceId);
    }
    TraceControl control = TraceControl.get();
    control.timeoutTrace(traceId);
  }

  protected void startTraceAsync(TraceContext context) {
    if (LogLevel.LOG_DEBUG_MESSAGE) {
      Log.d(
          LOG_TAG,
          "Started trace " + context.encodedTraceId + "  for controller " + context.controller);
    }
    if (mListener != null) {
      mListener.onTraceStartAsync(context);
    }
  }

  public synchronized void processMarkerStop(TraceContext context, final int eventDuration) {
    Message eventDurationMessage =
        obtainMessage(
            MSG_EVENT_DURATION, /* what */
            eventDuration, /* arg1 */
            0, /* arg2, unused */
            context /* obj */);
    sendMessage(eventDurationMessage);
  }

  public synchronized void processAnnotation(TraceContext context, String annotation) {
    AnnotationContext annotationContext = new AnnotationContext(context, annotation);
    Message annotationMessage =
        obtainMessage(MSG_EVENT_QPL_ANNOTATION, /* what */ annotationContext /* obj */);
    sendMessage(annotationMessage);
  }

  public synchronized void onConditionalTraceStop(TraceContext context) {
    Message stopMessage = obtainMessage(MSG_CONDITIONAL_TRACE_STOP, context);
    sendMessage(stopMessage);
  }

  public synchronized void onTraceStart(TraceContext context, final int timeoutMillis) {
    boolean traceRegistered = mTraceConditionManager.registerTrace(context);

    if (!traceRegistered) {
      // The only way this can happen is if one of the conditions in the config
      // is malformed. Let's abort the trace in this case.
      Logger.postAbortTrace(context.traceId);
      onTraceAbort(new TraceContext(context, ProfiloConstants.ABORT_REASON_MALFORMED_CONDITION));
      return;
    }

    mTraceContexts.add(context.traceId);
    if (mListener != null) {
      mListener.onTraceStartSync(context);
    }
    Message startMessage = obtainMessage(MSG_START_TRACE, context);
    sendMessage(startMessage);

    Message timeoutMessage = obtainMessage(MSG_TIMEOUT_TRACE, context);
    sendMessageDelayed(timeoutMessage, timeoutMillis);
  }

  public synchronized void onTraceStop(TraceContext context) {
    if (mTraceContexts.contains(context.traceId)) {
      Message stopMessage = obtainMessage(MSG_STOP_TRACE, context);
      sendMessage(stopMessage);
      mTraceContexts.remove(context.traceId);
    }
    if (LogLevel.LOG_DEBUG_MESSAGE) {
      Log.d(LOG_TAG, "Stopped trace " + context.encodedTraceId);
    }
  }

  public synchronized void onTraceAbort(TraceContext context) {
    if (mTraceContexts.contains(context.traceId)) {
      Message abortMessage = obtainMessage(MSG_ABORT_TRACE, context);
      sendMessage(abortMessage);
      mTraceContexts.remove(context.traceId);
    }
    if (LogLevel.LOG_DEBUG_MESSAGE) {
      int unpackedAbortReason = ProfiloConstants.unpackRemoteAbortReason(context.abortReason);
      Log.d(
          LOG_TAG,
          "Aborted trace "
              + context.encodedTraceId
              + " for reason "
              + unpackedAbortReason
              + (ProfiloConstants.isRemoteAbort(context.abortReason) ? " (remote process) " : " ")
              + ProfiloConstants.abortReasonName(unpackedAbortReason));
    }
  }
}
