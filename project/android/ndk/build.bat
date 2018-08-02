@echo off
echo "build slib ---------------------------------"
cd jni
call ndk-build
cd ..
echo "build slib ok ---------------------------------"

::xcopy /e /y libs ..\app\src\main\jniLibs