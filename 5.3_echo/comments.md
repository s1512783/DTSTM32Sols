To send data from a file and save to a file open two terminal windows.

In one do:
```
cat -v /dev/ttyUSB0
```
to listen to the device

In another:
```
cat test.txt > /dev/ttyUSB0
```
