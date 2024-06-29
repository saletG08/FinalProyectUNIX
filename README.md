# Final Proyect UNIX

**Name:** Salet Yasmin Gutierrez Nava

Write a service running a NET socket, max connections should be 30.
+ it should receive the port as program argument.
+ every client connection should be handled by a thread or fork.
+ it should handle SIGINT signal `(Ctrl+C)`.
+ the service will receive the following messages .
  + **getInfo** that should return a service name and the version.
    + i.e. `“Mike’s service, v0.1”`
  + **getNumberOfPartitions**, should return number of partitions of the system
    + it can be implemented using bash scripting or calling directly to utilities, i.e. `lsblk -l | grep part`
  + **getCurrentKernelVersion**, should return the kernel version that the system is running.
    + just the numerical par, you can use `uname -r`, it also can be though bash scripting or calling directly to uname utility
  + **sshdRunning**, should return `true` or `false`.
    + it should be implemented using `popen()`.
    + you can use one of these methods, or another one that could detect that sshd is running.
      + `ps ax|grep sshd`
      + `netstat -l |grep ssh`
      + verify that sshd.pid file exists `(/var/run/sshd.pid)`
  + **for getInfo**, **getNumberOfPartitions**, and **getCurrentKernelVersion**, two of them can be implemented with bash scripting, the third one needs to be with popen or execv (or variations) if applicable.

How to execute

To check if the code works correctly, you can follow these steps:

1. **Compile the code**:
 - Save the code in a file, for example, `server.c`.
 - Compile the code using `gcc`:
 ```
 gcc -o server server.c
 ```

2. **Run the server**:
 - Run the server specifying the port (you can omit the port to use the default value of 8181):
 ```
 ./server 8181
 ```

[![server.jpg](https://i.postimg.cc/sXHp3rTm/server.jpg)](https://postimg.cc/CBDZ4WTn)

3. **Test connections and requests**:
 - Open another terminal to simulate the client.
 - Use `telnet` or `netcat (nc)` to connect to the server and send requests. For example, if you use `telnet`:
 ```
 telnet localhost 8181
 ```
 - Once connected, you can submit test requests by typing them directly into the `telnet` or `netcat` console. Here are some examples of how to test each request:

 - **getInfo**:
 ```
 getInfo
 ```
 - **getNumberOfPartitions**:
 ```
 getNumberOfPartitions
 ```
 - **getCurrentKernelVersion**:
 ```
 getCurrentKernelVersion
 ```
 - **sshdRunning**:
 ```
 sshdRunning
 ```

 [![client.jpg](https://i.postimg.cc/gjw3JT5W/client.jpg)](https://postimg.cc/hQBXZpcy)


4. **Check answers**:
 - The answers that came out were:

 - For `getInfo`:
 ```
 HTTP/1.0 200 OK
 Content-Type: text/plain

 Salet Gutierrez Service, v0.1
 ```
 - For `getNumberOfPartitions`:
 ```
 HTTP/1.0 200 OK
 Content-Type: text/plain

 Number of partitions:
 ```
 - For `getCurrentKernelVersion`:
 ```
 HTTP/1.0 200 OK
 Content-Type: text/plain

 Kernel version: X.X.X-XX-generic
 ```
 - For `sshdRunning`:
 ```
 HTTP/1.0 200 OK
 Content-Type: text/plain

 sshd is running: true/false
 ```

5. **Close the connection**:
 - You can close the connection by typing `quit` on the client.
 - For the server you can exit with `(Ctrl+C)`