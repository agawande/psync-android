/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class net_named_data_jni_psync_FullProducer */

#ifndef _Included_net_named_data_jni_psync_FullProducer
#define _Included_net_named_data_jni_psync_FullProducer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     net_named_data_jni_psync_FullProducer
 * Method:    initializeFaceAndProcessEvents
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_initializeFaceAndProcessEvents
  (JNIEnv *, jobject, jstring);

/*
 * Class:     net_named_data_jni_psync_FullProducer
 * Method:    startFullProducer
 * Signature: (ILjava/lang/String;Ljava/lang/String;JJ)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_net_named_1data_jni_psync_FullProducer_startFullProducer
  (JNIEnv *, jobject, jint, jstring, jstring, jlong, jlong);

/*
 * Class:     net_named_data_jni_psync_FullProducer
 * Method:    stop
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_stop
  (JNIEnv *, jobject, jobject);

/*
 * Class:     net_named_data_jni_psync_FullProducer
 * Method:    addUserNode
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_net_named_1data_jni_psync_FullProducer_addUserNode
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     net_named_data_jni_psync_FullProducer
 * Method:    removeUserNode
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_removeUserNode
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     net_named_data_jni_psync_FullProducer
 * Method:    getSeqNo
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_net_named_1data_jni_psync_FullProducer_getSeqNo
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     net_named_data_jni_psync_FullProducer
 * Method:    publishName
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_net_named_1data_jni_psync_FullProducer_publishName
  (JNIEnv *, jobject, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
