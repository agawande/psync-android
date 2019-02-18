package net.named_data.jni.psync;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    PSync psync;
    PSync.FullProducer fullProducer;

    PSync.OnSyncDataCallBack onSyncUpdate = new PSync.OnSyncDataCallBack() {
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

        psync = PSync.getInstance(getFilesDir().getAbsolutePath());
        fullProducer = new PSync.FullProducer(80, "/psync/sync", "/andriod-1", onSyncUpdate);

        //PSync2 = new PSync(80, "/psync/2sync", "/android-2", onSyncUpdate);
        //tv.setText(/*fullProducer.startFullProducer()*/

        /*try {
            Thread.sleep(10000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        fullProducer.cleanAndStop();
        psync.cleanAndStop();*/
    }
}
