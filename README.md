# DaemonServer
Daemon server for remote execution.
# Setup
```
mkdir build
cd build
cmake ..
make
```
# Starting server
```
./server PORT
```
For example:
```
./server 31337
```
# Starting client
```
./client IP:PORT <command> [args...]
```
For example:
```
./server 0.0.0.0:31337 ls --all
```
Another example:
```
./server 0.0.0.0:31337 wc
>>> Aba caba racada
>>> CTRL+D
<<<       1       3      16
```
