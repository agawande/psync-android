# PSync JNI for Android

## Build instructions

- Install NFD-android by following the instructions [here](https://github.com/named-data-mobile/NFD-android#prerequisites). (Specify --no-check-shasum if problem with shasum of ndn-cxx)

- Build PSync library, by going to crew.dir created in above step.

	```
	cd crew.dir
    git remote add psync-crew https://github.com/agawande/android-crew-staging
    git fetch psync-crew
    git checkout 0.1.0
    cd ..
    ./crew.dir/crew source target/psync
    ./crew.dir/crew build target/psync
    ```

- Build PSync JNI library:

    Set sdk.dir and ndk.dir in local.properties, then:

	```
    ./gradlew build
    ```

	or

	```
    In Android studio: Build -> Make Project
    ```

## Usage instructions

- Import aar file in your project from psync/build/outputs/aar/ as explained [here](https://developer.android.com/studio/projects/android-library#AddDependency)

- Configure PSync library

    ```java
    PSync psync;
    PSync.FullProducer fullProducer;

    protected void onCreate(Bundle savedInstanceState) {
        // Initialize with home folder of the app where .ndn will be stored
        psync = PSync.getInstance(getFilesDir().getAbsolutePath());

        // Instantiate fullProducer
        fullProducer = new PSync.FullProducer(80, "/psync/sync", "/andriod-1", onSyncUpdate);
    }
    ```

- Clean up

    ```java
    fullProducer.cleanAndStop();
    psync.cleanAndStop();
    ```