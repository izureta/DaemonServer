# DaemonServer
Проект по акосу, сделана только самая базовая часть.
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
./client IP:PORT spawn <command> [args...]
```
For example:
```
./client 0.0.0.0:31337 spawn ls --all
```
Another example:
```
./client 0.0.0.0:31337 spawn wc
>>> Aba caba racada
>>> CTRL+D
<<<       1       3      16
```
