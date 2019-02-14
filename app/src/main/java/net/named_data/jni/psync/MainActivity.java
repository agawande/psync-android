package net.named_data.jni.psync;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    FullProducer fullProducer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d("main-act", "skjdnvksn");

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        System.out.println(getFilesDir().getAbsolutePath());

        fullProducer = new FullProducer(getFilesDir().getAbsolutePath(), 80, "/psync/sync", "/psync/user", null);

        // fullProducer2 = new FullProducer(getFilesDir().getAbsolutePath(), 80, "/2psync/sync", "/psync/user", null);
        //tv.setText(/*fullProducer.startFullProducer()*/);
    }
}
