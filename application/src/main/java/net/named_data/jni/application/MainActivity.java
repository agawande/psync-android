package net.named_data.jni.application;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import net.named_data.jni.psync.MissingDataInfo;
import net.named_data.jni.psync.PSync;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    PSync psync;
    PSync.Consumer consumer;

    PSync.OnHelloDataCallBack helloDataCallBack = new PSync.OnHelloDataCallBack() {
        @Override
        public void onHelloDataCallBack(ArrayList<String> names) {
            for (String name : names) {
                consumer.addSubscription(name);
            }
            System.out.println("Got Hello Data");
            consumer.sendSyncInterest();
        }
    };

    PSync.OnSyncDataCallBack syncDataCallBack = new PSync.OnSyncDataCallBack() {
        @Override
        public void onSyncDataCallBack(ArrayList<MissingDataInfo> updates) {
            for (MissingDataInfo update : updates) {
                System.out.println("Got Sync Data: " + update.prefix + " " + update.lowSeq + " " + update.highSeq);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        psync = PSync.getInstance(getFilesDir().getAbsolutePath());
        // Instantiate Consumer
        consumer = new PSync.Consumer("/sync/partial", helloDataCallBack, syncDataCallBack, 40, 0.001);
        consumer.sendHelloInterest();
    }
}
