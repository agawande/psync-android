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

#include "full-producer.hpp"

#include <ndn-cxx/name.hpp>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/interest.hpp>

#include <PSync/full-producer.hpp>

#include <thread>
#include <iostream>

#include <android/log.h>

#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

std::unique_ptr<ndn::Face> g_facePtr;
static std::thread g_thread;

class FullProducerWrapper
{
public:
  JavaVM* jvm;
  jobject thisObject;
  jclass arrayList;
  jmethodID addToArrayList;
  jmethodID arrayListConstructor;
  jmethodID onSyncUpdate;
  jclass missingDataInfoClass;
  jmethodID mdiConstructor;
  std::unique_ptr<psync::FullProducer> fullProducer;
};

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_initializeFaceAndProcessEvents
        (JNIEnv *env, jobject, jstring homePath)
{
  if (!g_facePtr) {
    ::setenv("HOME", env->GetStringUTFChars(homePath, 0), true);
    g_facePtr = std::make_unique<ndn::Face>("127.0.0.1");
    g_thread = std::thread([]{ g_facePtr->processEvents(); });
  }
}

void
processSyncUpdate(const std::vector<psync::MissingDataInfo>& updates, FullProducerWrapper* fullProducerWrapper)
{
  JNIEnv *env;
  fullProducerWrapper->jvm->AttachCurrentThread(&env, nullptr);

  jobject result = env->NewObject(fullProducerWrapper->arrayList, fullProducerWrapper->arrayListConstructor, updates.size());

  for (const auto& update : updates) {
    jstring jstr = env->NewStringUTF(update.prefix.toUri().c_str());
    jobject mdiObj = env->NewObject(fullProducerWrapper->missingDataInfoClass, fullProducerWrapper->mdiConstructor,
                                    jstr, update.lowSeq, update.highSeq);

    env->CallVoidMethod(result, fullProducerWrapper->addToArrayList, mdiObj);

    env->DeleteLocalRef(jstr);
    env->DeleteLocalRef(mdiObj);
  }

  env->CallVoidMethod(fullProducerWrapper->thisObject, fullProducerWrapper->onSyncUpdate, result);
  env->DeleteLocalRef(result);
  fullProducerWrapper->jvm->DetachCurrentThread();
}

JNIEXPORT
jobject
JNICALL Java_net_named_1data_jni_psync_FullProducer_startFullProducer(
  JNIEnv *env, jobject thisObject, jint ibfSize, jstring syncPrefix,
  jstring userPrefix, jlong syncInterestLifetimeMillis, jlong syncReplyFreshnessMillis)
{
  FullProducerWrapper* fullProducerWrapper = new FullProducerWrapper();
  env->GetJavaVM(&fullProducerWrapper->jvm);

  fullProducerWrapper->thisObject = env->NewGlobalRef(thisObject);
  fullProducerWrapper->arrayList = env->FindClass("java/util/ArrayList");
  fullProducerWrapper->addToArrayList = env->GetMethodID(fullProducerWrapper->arrayList, "add", "(Ljava/lang/Object;)Z");
  fullProducerWrapper->arrayListConstructor = env->GetMethodID(fullProducerWrapper->arrayList, "<init>", "(I)V");

  jclass fullProducerClass = env->FindClass("net/named_data/jni/psync/FullProducer");
  if (fullProducerClass == 0) {
    ALOG("%s", "Full Producer class not found");
    return 0;
  }

  fullProducerWrapper->onSyncUpdate = env->GetMethodID(fullProducerClass, "onSyncUpdate", "(Ljava/util/ArrayList;)V");
  fullProducerWrapper->missingDataInfoClass = env->FindClass("net/named_data/jni/psync/MissingDataInfo");

  if (fullProducerWrapper->missingDataInfoClass == 0) {
    ALOG("%s", "MissingDataInfo class not found");
    return 0;
  }

  fullProducerWrapper->mdiConstructor = env->GetMethodID(fullProducerWrapper->missingDataInfoClass, "<init>", "(Ljava/lang/String;JJ)V");

  ndn::Name syncPrefixName(env->GetStringUTFChars(syncPrefix, nullptr));
  ndn::Name userPrefixName(env->GetStringUTFChars(userPrefix, nullptr));

  ALOG("%s", "Initializing PSync Full Producer");
  fullProducerWrapper->fullProducer = std::make_unique<psync::FullProducer>((size_t)ibfSize, *g_facePtr,
                                        syncPrefixName,
                                        userPrefixName,
                                        [fullProducerWrapper] (const std::vector<psync::MissingDataInfo>& updates)
                                        {
                                          processSyncUpdate(updates, fullProducerWrapper);
                                        },
                                        ndn::time::milliseconds(syncInterestLifetimeMillis),
                                        ndn::time::milliseconds(syncReplyFreshnessMillis));

  return env->NewDirectByteBuffer(fullProducerWrapper, 0);
}

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_stop
  (JNIEnv *env, jobject obj, jobject handle)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  delete fullProducerWrapper;
  // Destroy face.processEvent face here too? maybe take a true and false from the application to decide
}

JNIEXPORT jboolean JNICALL Java_net_named_1data_jni_psync_FullProducer_addUserNode
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  return fullProducerWrapper->fullProducer->addUserNode(ndn::Name(env->GetStringUTFChars(prefix, nullptr)));
}

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_removeUserNode
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  fullProducerWrapper->fullProducer->removeUserNode(ndn::Name(env->GetStringUTFChars(prefix, nullptr)));
}


JNIEXPORT jlong JNICALL Java_net_named_1data_jni_psync_FullProducer_getSeqNo
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  return fullProducerWrapper->fullProducer->getSeqNo(ndn::Name(env->GetStringUTFChars(prefix, nullptr))).value();
}

JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_publishName
  (JNIEnv *env, jobject obj, jobject handle, jstring prefix)
{
  FullProducerWrapper* fullProducerWrapper = (FullProducerWrapper*) env->GetDirectBufferAddress(handle);
  fullProducerWrapper->fullProducer->publishName(ndn::Name(env->GetStringUTFChars(prefix, nullptr)));
}
