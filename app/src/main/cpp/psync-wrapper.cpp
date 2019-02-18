/*
 * Copyright (c) 2014-2019,  The University of Memphis
 *
 * This file is part of PSync.
 * See AUTHORS.md for complete list of PSync authors and contributors.
 *
 * PSync is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * PSync is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * PSync, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "net_named_data_jni_psync_PSync.h"
#include "net_named_data_jni_psync_PSync_FullProducer.h"

#include <ndn-cxx/name.hpp>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>

#include <PSync/full-producer.hpp>
#include <PSync/partial-producer.hpp>

#include <thread>
#include <iostream>

#include <android/log.h>
#include <android/native_activity.h>

#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

static std::unique_ptr<ndn::Face> g_facePtr;
static std::unique_ptr<std::thread> g_thread;

static JavaVM* g_jvm;
static jobject g_thisObject;
static jclass g_arrayList;
static jclass g_missingDataInfoClass;
static jmethodID g_addToArrayList;
static jmethodID g_arrayListConstructor;
static jmethodID g_onSyncUpdate;
static jmethodID g_mdiConstructor;

class FullProducerWrapper {
public:
  std::unique_ptr<psync::FullProducer> fullProducer;
};

class PartialProducerWrapper {
public:
    std::unique_ptr<psync::PartialProducer> partialProducer;
};

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_PSync_initialize
        (JNIEnv *env, jobject thisObject, jstring homePath)
{
  if (!g_facePtr) {
    ALOG("%s", "Initialising face");
    ::setenv("HOME", env->GetStringUTFChars(homePath, 0), true);
    g_facePtr = std::make_unique<ndn::Face>("127.0.0.1");
  }

  if (!g_thread) {
    g_thread = std::make_unique<std::thread>([] {
        ALOG("%s", "Starting proces events thread");
        try {
          // If keepThread is not passed as true, then PSync does not seem to set the interest filter in NFD
          g_facePtr->processEvents(ndn::time::milliseconds::zero(), true);
        }
        catch (const std::exception &e) {
          ALOG("%s", e.what());
        }
    });
  }

  // Save reference to global JVM and thisObject
  env->GetJavaVM(&g_jvm);
  g_thisObject = env->NewGlobalRef(thisObject);

  g_arrayList = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
  g_addToArrayList = env->GetMethodID(g_arrayList, "add", "(Ljava/lang/Object;)Z");
  g_arrayListConstructor = env->GetMethodID(g_arrayList, "<init>", "(I)V");

  jclass fullProducerClass = env->FindClass("net/named_data/jni/psync/PSync$FullProducer");
  if (fullProducerClass == 0) {
    ALOG("%s", "Full Producer class not found");
    return;
  }
  g_onSyncUpdate = env->GetMethodID(fullProducerClass, "onSyncUpdate", "(Ljava/util/ArrayList;)V");

  g_missingDataInfoClass = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("net/named_data/jni/psync/MissingDataInfo")));

  if (g_missingDataInfoClass == 0) {
    ALOG("%s", "MissingDataInfo class not found");
    return;
  }

  g_mdiConstructor = env->GetMethodID(g_missingDataInfoClass, "<init>", "(Ljava/lang/String;JJ)V");
}

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_PSync_stop
  (JNIEnv *, jobject)
{
  g_facePtr->shutdown();
}

void
processSyncUpdate(const std::vector<psync::MissingDataInfo>& updates)
{
  JNIEnv *env;
  jint res = g_jvm->AttachCurrentThread(&env, nullptr);

  if (JNI_OK != res) {
    ALOG("%s", "Failed to AttachCurrentThread");
    return;
  }

  jobject result = env->NewObject(g_arrayList, g_arrayListConstructor, updates.size());

  for (const auto& update : updates) {
    jstring jstr = env->NewStringUTF(update.prefix.toUri().c_str());

    jobject mdiObj = env->NewObject(g_missingDataInfoClass, g_mdiConstructor,
                                    jstr, update.lowSeq, update.highSeq);

    env->CallBooleanMethod(result, g_addToArrayList, mdiObj);

    env->DeleteLocalRef(jstr);
    env->DeleteLocalRef(mdiObj);
  }

  env->CallVoidMethod(g_thisObject, g_onSyncUpdate, result);
  env->DeleteLocalRef(result);
  g_jvm->DetachCurrentThread();
}

JNIEXPORT
jobject
JNICALL Java_net_named_1data_jni_psync_PSync_00024FullProducer_startFullProducer(
  JNIEnv *env, jobject thisObject, jint ibfSize, jstring syncPrefix,
  jstring userPrefix, jlong syncInterestLifetimeMillis, jlong syncReplyFreshnessMillis)
{
  FullProducerWrapper* fullProducerWrapper = new FullProducerWrapper();
  ndn::Name syncPrefixName(env->GetStringUTFChars(syncPrefix, nullptr));
  ndn::Name userPrefixName(env->GetStringUTFChars(userPrefix, nullptr));

  ALOG("%s", "Initializing PSync Full Producer");
  fullProducerWrapper->fullProducer = std::make_unique<psync::FullProducer>((size_t) ibfSize,
                  *g_facePtr,
                  syncPrefixName,
                  userPrefixName,
                  [](const std::vector<psync::MissingDataInfo> &updates) {
                    processSyncUpdate(updates);
                  },
                  ndn::time::milliseconds(syncInterestLifetimeMillis),
                  ndn::time::milliseconds(syncReplyFreshnessMillis));

  return env->NewDirectByteBuffer(fullProducerWrapper, 0);
}

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_PSync_00024FullProducer_stop
  (JNIEnv *env, jobject obj, jobject handle)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  delete fullProducerWrapper;
}

JNIEXPORT jboolean JNICALL Java_net_named_1data_jni_psync_PSync_00024FullProducer_addUserNode
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  return fullProducerWrapper->fullProducer->addUserNode(ndn::Name(env->GetStringUTFChars(prefix, nullptr)));
}

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_PSync_00024FullProducer_removeUserNode
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  fullProducerWrapper->fullProducer->removeUserNode(ndn::Name(env->GetStringUTFChars(prefix, nullptr)));
}

JNIEXPORT jlong JNICALL Java_net_named_1data_jni_psync_PSync_00024FullProducer_getSeqNo
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  return fullProducerWrapper->fullProducer->getSeqNo(ndn::Name(env->GetStringUTFChars(prefix, nullptr))).value();
}

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_PSync_00024FullProducer_publishName
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  fullProducerWrapper->fullProducer->publishName(ndn::Name(env->GetStringUTFChars(prefix, nullptr)));
}
