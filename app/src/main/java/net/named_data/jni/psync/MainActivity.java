package net.named_data.jni.psync;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    FullProducer fullProducer, fullProducer2;

    FullProducer.OnSyncDataCallBack onSyncUpdate = new FullProducer.OnSyncDataCallBack() {
        public void onSyncDataCallBack(ArrayList<MissingDataInfo> updates) {
            for (MissingDataInfo update : updates) {
                System.out.println(update.prefix + " " + update.lowSeq + " " + update.highSeq);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d("main-act", "skjdnvksn");

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        System.out.println(getFilesDir().getAbsolutePath());

        fullProducer = new FullProducer(getFilesDir().getAbsolutePath(), 80, "/psync/sync", "/andriod-1", onSyncUpdate);

        fullProducer2 = new FullProducer(getFilesDir().getAbsolutePath(), 80, "/psync/2sync", "/android-2", onSyncUpdate);
        //tv.setText(/*fullProducer.startFullProducer()*/);
    }
}
