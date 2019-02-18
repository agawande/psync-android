package net.named_data.jni.psync;

import java.nio.ByteBuffer;
import java.util.ArrayList;

public class PSync {
    private static PSync s_psync;

    static {
        System.loadLibrary("psync");
    }

    public interface OnSyncDataCallBack {
        void onSyncDataCallBack(ArrayList<MissingDataInfo> updates);
    }

    // Singleton pattern
    private PSync(String homePath) {
        initialize(homePath);
    }

    public static PSync getInstance(String homeFolder) {
        if(s_psync == null) {
            s_psync = new PSync(homeFolder);
        }
        return s_psync;
    }

    public void cleanAndStop() {
        s_psync.stop();
        s_psync = null;
    }

    private native void initialize(String homePath);

    private native void stop();

    public static class FullProducer {
        private OnSyncDataCallBack m_onReceivedSyncData;
        private ByteBuffer m_buffer;

        public FullProducer(int ibfSize,
                     String syncPrefix,
                     String userPrefix,
                     OnSyncDataCallBack onSyncUpdate,
                     long syncInterestLifetimeMillis,
                     long syncReplyFreshnessMillis) {
            m_onReceivedSyncData = onSyncUpdate;
            m_buffer = startFullProducer(ibfSize, syncPrefix, userPrefix,
                    syncInterestLifetimeMillis, syncReplyFreshnessMillis);
        }

        public FullProducer(int ibfSize,
                     String syncPrefix,
                     String userPrefix,
                     OnSyncDataCallBack onSyncUpdate) {
            this(ibfSize , syncPrefix, userPrefix, onSyncUpdate, 1000, 1000);
        }

        private native ByteBuffer startFullProducer(int ibfSize,
                                                    String syncPrefix,
                                                    String userPrefix,
                                                    long syncInterestLifetimeMillis,
                                                    long syncReplyFreshnessMillis);

        public void cleanAndStop() {
            stop(m_buffer);
        }

        public void addUserNode(String prefix) {
            addUserNode(m_buffer, prefix);
        }

        public void removeUserNode(String prefix) {
            removeUserNode(m_buffer, prefix);
        }

        public void getSeqNo(String prefix) {
            getSeqNo(m_buffer, prefix);
        }

        public void publishName(String prefix) {
            publishName(m_buffer, prefix);
        }

        private native void stop(ByteBuffer buffer);

        private native boolean addUserNode(ByteBuffer buffer, String prefix);

        private native void removeUserNode(ByteBuffer buffer, String prefix);

        private native long getSeqNo(ByteBuffer buffer, String prefix);

        private native void publishName(ByteBuffer buffer, String prefix);

        // Called from C++
        public void onSyncUpdate(ArrayList<MissingDataInfo> updates) {
            m_onReceivedSyncData.onSyncDataCallBack(updates);
        }
    }

    public static class PartialProducer {
        private ByteBuffer m_buffer;

        public PartialProducer(int ibfSize,
                            String syncPrefix,
                            String userPrefix,
                            OnSyncDataCallBack onSyncUpdate,
                            long syncInterestLifetimeMillis,
                            long syncReplyFreshnessMillis) {
           // m_buffer = startFullProducer(ibfSize, syncPrefix, userPrefix,
            //        syncInterestLifetimeMillis, syncReplyFreshnessMillis);
        }
    }
}
