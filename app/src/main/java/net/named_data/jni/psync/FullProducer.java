package net.named_data.jni.psync;

import java.nio.ByteBuffer;
import java.util.ArrayList;

public class FullProducer {
    static {
        System.loadLibrary("full-producer");
    }

    public interface OnSyncDataCallBack {
        void onSyncDataCallBack(ArrayList<MissingDataInfo> updates);
    }

    private OnSyncDataCallBack m_onReceivedSyncData;
    private ByteBuffer m_buffer;

    public FullProducer(String homePath,
                        int ibfSize,
                        String syncPrefix,
                        String userPrefix,
                        OnSyncDataCallBack onSyncUpdate,
                        long syncInterestLifetimeMillis,
                        long syncReplyFreshnessMillis) {
        m_onReceivedSyncData = onSyncUpdate;
        initializeFaceAndProcessEvents(homePath);
        m_buffer = startFullProducer(ibfSize, syncPrefix, userPrefix,
                                     syncInterestLifetimeMillis, syncReplyFreshnessMillis);
    }

    public FullProducer(String homePath,
                        int ibfSize,
                        String syncPrefix,
                        String userPrefix,
                        OnSyncDataCallBack onSyncUpdate) {
        this(homePath, ibfSize , syncPrefix, userPrefix, onSyncUpdate, 1000, 1000);
    }

    private native void initializeFaceAndProcessEvents(String homePath);

    private native ByteBuffer startFullProducer(int ibfSize,
                                                String syncPrefix,
                                                String userPrefix,
                                                long syncInterestLifetimeMillis,
                                                long syncReplyFreshnessMillis);

    public void stop() {
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
