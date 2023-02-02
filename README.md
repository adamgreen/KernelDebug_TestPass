# KernelDebug Test Pass
This README documents the manual steps that I conduct in GDB when running a test pass on the [ArduinoCore-mbed KernelDebug library](https://github.com/arduino/ArduinoCore-mbed/tree/master/libraries/KernelDebug). This repository also contains the source code for the [Test sketch](KernelDebug_TestPass.ino) that I run on the Arduino device while running through the steps in this test pass.


## Verify KernelDebug sample
* Use the Arduino IDE to open the KernelDebug.ino sample included with the ArduinoCore-mbed based board packages.
* Build this sample and deploy to device.
  * Under ```ArduinoIDE/Preferences...``` check the ```compile``` box of the ```Show verbose output during``` setting so that the next step will work.
  * The Arduino IDE's build output will now indicate the **temp** output folder where the .elf can be found. Take note of this location.
  * The JTAG/SWD port for connecting the JLink debugger can be found on top of the development board, next to the Ethernet jack as seen in the following photo: ![Photo of my Portenta setup](images/20221128-01.jpg)
  * The FTDI adapter should also be connected to the 0.1" header on the test jig as seen in the above photo.
  * Can start JLink to debug the Portenta H7 with the following command on my machine: ```jh7```
  * Open another Terminal window and ``cd`` into the temp folder containing the generated .elf binary.
  * From this Terminal window, we can run GDB against the recently opened JLink connection using this command on my machine: ```dh7 jlink```
  * The GDB ```load``` command can be used to load the binary into the Portenta H7.
  * Can ``quit`` this instance of GDB and use CTRL+C in the JLink window to exit it as well.
* Connect to KernelDebug with GDB by using this command on my machine: ```dh7 mri1```
* ```continue``` from the initial stop in ```setup()```.
* Press CTRL+C to break in.
* ```continue```
* Repeat the last 2 steps a few times to make sure that GDB always always successfully halts the processor.
```console
$ dh7 mri1
GNU gdb (GNU Arm Embedded Toolchain 9-2020-q4-major) 8.3.1.20191211-git
Copyright (C) 2019 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-apple-darwin10 --target=arm-none-eabi".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from KernelDebug.ino.elf...
Remote debugging using /dev/tty.usbserial-A505C8GJ
setup ()
    at /Users/username/Documents/Arduino/hardware/arduino/mbed/libraries/KernelDebug/examples/KernelDebug/KernelDebug.ino:18
18	}
(gdb) c
Continuing.
^C
Thread 1 received signal SIGINT, Interrupt.
main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:48
48			if (arduino::serialEventRun) arduino::serialEventRun();
(gdb) c
Continuing.
^C
Thread 1 received signal SIGINT, Interrupt.
main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:48
48			if (arduino::serialEventRun) arduino::serialEventRun();
(gdb) c
Continuing.
^C
Thread 1 received signal SIGINT, Interrupt.
0x0804034e in loop ()
(gdb) bt
#0  0x0804034e in loop ()
#1  0x08045c9a in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:47
(gdb) q
A debugging session is active.

	Inferior 1 [Remote target] will be killed.

Quit anyway? (y or n) y
```


## Verify DEBUG_NO_BREAK_IN_SETUP functionality
* Run first iteration on KernelDebug_TestPass.ino with ```DEBUG_NO_BREAK_IN_SETUP``` used as the second parameter to the KernelDebug constructor.
```c++
KernelDebug kernelDebug(SERIAL1_TX, SERIAL1_RX, USART1_IRQn, 230400, DEBUG_NO_BREAK_IN_SETUP);
```
* Build the code and deploy to the Arduino device.
* Attach with GDB and look at the state of device and it should look something like:
```console
$ dh7 mri1
GNU gdb (GNU Arm Embedded Toolchain 9-2020-q4-major) 8.3.1.20191211-git
Copyright (C) 2019 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-apple-darwin10 --target=arm-none-eabi".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from KernelDebug_TestPass.ino.elf...
Remote debugging using /dev/tty.usbserial-A505C8GJ
0x08043fe6 in mriNewlib_SemihostWrite ()
(gdb) bt
#0  0x08043fe6 in mriNewlib_SemihostWrite ()
#1  0x08040a86 in arduino::DebugSerial::write (this=<optimized out>, pBuffer=<optimized out>, size=43)
    at /Users/username/Documents/Arduino/hardware/arduino/mbed/libraries/KernelDebug/src/KernelDebug.cpp:620
#2  0x08045fc8 in arduino::Print::print (str=0x806378c <_fini+76> "1) Set registers to known values and crash.",
    this=0x24002318 <arduino::DebugSerial>)
    at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/api/Print.cpp:65
#3  arduino::Print::println (this=0x24002318 <arduino::DebugSerial>,
    c=c@entry=0x806378c <_fini+76> "1) Set registers to known values and crash.")
    at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/api/Print.cpp:163
#4  0x08040458 in loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:27
#5  0x08046052 in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:47
(gdb) q
A debugging session is active.

	Inferior 1 [Remote target] will be killed.

Quit anyway? (y or n) y
```
* Set the constructor parameter back to ```DEBUG_BREAK_IN_SETUP``` in the Arduino IDE to run the rest of the tests.
```c++
KernelDebug kernelDebug(SERIAL1_TX, SERIAL1_RX, USART1_IRQn, 230400, DEBUG_BREAK_IN_SETUP);
```
* Build and deploy this updated code to the Arduino device for running the rest of the tests.


## Verify Correct XML for Registers & Memory
KernelDebug should expose XML dictating the register and memory layout of the device on which it is running.

There should be the expected r0-r12, sp, lr, pc, xpsr, fpscr, msp, psp, primask, faultfmask, control, and s0-s32 as seen below:
```console
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
setup () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:20
20	}
(gdb) info all-reg
r0             0x2400e7b0          604039088
r1             0x0                 0
r2             0x0                 0
r3             0x0                 0
r4             0x0                 0
r5             0x0                 0
r6             0x0                 0
r7             0x0                 0
r8             0x0                 0
r9             0x0                 0
r10            0x0                 0
r11            0x0                 0
r12            0x8047521           134509857
sp             0x2400e8f0          0x2400e8f0 <_main_stack+32760>
lr             0x804604f           0x804604f <main()+34>
pc             0x8040444           0x8040444 <setup()>
xpsr           0x61000000          1627389952
d0             0                   (raw 0x0000000000000000)
d1             0                   (raw 0x0000000000000000)
d2             0                   (raw 0x0000000000000000)
d3             0                   (raw 0x0000000000000000)
d4             0                   (raw 0x0000000000000000)
d5             0.0078125           (raw 0x3f80000000000000)
d6             2.0000005647887491  (raw 0x400000004bcdfe60)
d7             1.7790108226531607e-179 (raw 0x1ad274804dd693a4)
d8             0                   (raw 0x0000000000000000)
d9             0                   (raw 0x0000000000000000)
d10            0                   (raw 0x0000000000000000)
d11            0                   (raw 0x0000000000000000)
d12            0                   (raw 0x0000000000000000)
d13            0                   (raw 0x0000000000000000)
d14            0                   (raw 0x0000000000000000)
d15            -nan(0xfffff00000000) (raw 0xffffffff00000000)
fpscr          0x0                 0
msp            0x2407ff78          604503928
psp            0x2400e888          604039304
primask        0x0                 0
basepri        0x0                 0
faultmask      0x0                 0
control        0x0                 0
s0             0                   (raw 0x00000000)
s1             0                   (raw 0x00000000)
s2             0                   (raw 0x00000000)
s3             0                   (raw 0x00000000)
s4             0                   (raw 0x00000000)
s5             0                   (raw 0x00000000)
s6             0                   (raw 0x00000000)
s7             0                   (raw 0x00000000)
s8             0                   (raw 0x00000000)
s9             0                   (raw 0x00000000)
s10            0                   (raw 0x00000000)
s11            1                   (raw 0x3f800000)
s12            27000000            (raw 0x4bcdfe60)
s13            2                   (raw 0x40000000)
s14            450000000           (raw 0x4dd693a4)
s15            8.70421802e-23      (raw 0x1ad27480)
s16            0                   (raw 0x00000000)
s17            0                   (raw 0x00000000)
s18            0                   (raw 0x00000000)
s19            0                   (raw 0x00000000)
s20            0                   (raw 0x00000000)
s21            0                   (raw 0x00000000)
s22            0                   (raw 0x00000000)
s23            0                   (raw 0x00000000)
s24            0                   (raw 0x00000000)
s25            0                   (raw 0x00000000)
s26            0                   (raw 0x00000000)
s27            0                   (raw 0x00000000)
s28            0                   (raw 0x00000000)
s29            0                   (raw 0x00000000)
s30            0                   (raw 0x00000000)
s31            -nan(0x7fffff)      (raw 0xffffffff)
```

The various RAM and FLASH locations should also be exposed as seen next:
```console
(gdb) info mem
Using memory regions provided by the target.
Num Enb Low Addr   High Addr  Attrs
0   y  	0x00000000 0x00010000 rw nocache
1   y  	0x08000000 0x08200000 flash blocksize 0x20000 nocache
2   y  	0x10000000 0x10048000 rw nocache
3   y  	0x1ff00000 0x1ff20000 rw nocache
4   y  	0x20000000 0x20020000 rw nocache
5   y  	0x24000000 0x24080000 rw nocache
6   y  	0x30000000 0x30048000 rw nocache
7   y  	0x38000000 0x38010000 rw nocache
8   y  	0x38800000 0x38801000 rw nocache
9   y  	0x58020000 0x58022c00 rw nocache
10  y  	0x58024400 0x58025000 rw nocache
11  y  	0x58025400 0x58025c00 rw nocache
12  y  	0x58026000 0x58026800 rw nocache
13  y  	0x58027000 0x58027400 rw nocache
14  y  	0x60000000 0x60800000 rw nocache
15  y  	0x90000000 0xa0000000 flash blocksize 0x200 nocache
```


## Verify Bit Shift for Interrupt Priorities
The correct number of bits that the interrupt priority values should be shifted left on the Cortex-M7 in the Portenta H7 is **4**:
```
(gdb) p mriCortexMState.priorityBitShift
$1 = 4
```


## Verify Initial Threads
When the test binary starts, it should have 4 threads as seen below:
```
(gdb) info thread
  Id   Target Id                                        Frame
* 1    Thread 603998056 ("main" Running)                setup ()
    at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:20
  2    Thread 604051136 ("USBevt" Ready)                mbed::Callback<void ()>::operator bool() const (
    this=0x2401164c) at ./mbed-os/platform/include/platform/Callback.h:520
  3    Thread 603996216 ("rtx_idle" Ready)              0x08046cce in osKernelResume (sleep_ticks=1)
    at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_kernel.c:498
  4    Thread 603996148 ("rtx_timer" WaitingMessageGet) __svcMessageQueueGet (a4=<optimized out>, a3=<optimized out>,
    a2=<optimized out>, a1=<optimized out>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c:723
(gdb) thread apply all bt

Thread 4 (Thread 603996148):
#0  __svcMessageQueueGet (a4=<optimized out>, a3=<optimized out>, a2=<optimized out>, a1=<optimized out>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c:723
#1  osMessageQueueGet (mq_id=mq_id@entry=0x24003f70 <os_timer_mq_cb>, msg_ptr=0x24003ff4 <os_timer_thread_cb>, msg_ptr@entry=0x240046f0 <os_timer_thread_stack+752>, msg_prio=0x240046f0 <os_timer_thread_stack+752> "p?", msg_prio@entry=0x0, timeout=timeout@entry=4294967295) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c:882
#2  0x0804867a in osRtxTimerThread (argument=0x24003f70 <os_timer_mq_cb>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_timer.c:159
#3  0x08047d40 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 3 (Thread 603996216):
#0  0x08046cce in osKernelResume (sleep_ticks=1) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_kernel.c:498
#1  0x08048a70 in rtos_idle_loop () at ./mbed-os/cmsis/device/rtos/source/mbed_rtx_idle.cpp:174
#2  0x0804887a in osRtxIdleThread (argument=<optimized out>) at ./mbed-os/cmsis/device/rtos/source/mbed_rtx_handlers.c:51
#3  0x08047d40 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 2 (Thread 604051136):
#0  mbed::Callback<void ()>::operator bool() const (this=0x2401164c) at ./mbed-os/platform/include/platform/Callback.h:520
#1  mbed::Callback<void ()>::call() const (this=0x2401164c) at ./mbed-os/platform/include/platform/Callback.h:506
#2  mbed::Callback<void ()>::operator()() const (this=0x2401164c) at ./mbed-os/platform/include/platform/Callback.h:515
#3  rtos::Thread::_thunk (thread_ptr=0x24011648) at ./mbed-os/rtos/source/Thread.cpp:340
#4  0x08047d40 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 1 (Thread 603998056):
#0  setup () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:20
#1  0x0804604e in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:44
```


## Verify GDB Generated Memory Faults are Handled Properly
Try to read and write from an invalid address like **0xFFFFFFF0** to verify that KernelDebug/MRI catches the fact that a memory access initiated by GDB has led to a HardFault:
```console
(gdb) set mem inaccessible-by-default off
(gdb) x/1wx 0xfffffff0
0xfffffff0:	Cannot access memory at address 0xfffffff0
(gdb) set var *(uint32_t*)0xfffffff0=0
Cannot access memory at address 0xfffffff0
(gdb) set mem inaccessible-by-default on
```


## Test Generated Hard Fault
Continue execution in GDB and then use GDB to set g_selection to 1 to run ```1) Set registers to known values and crash.```

```console
(gdb) c
Continuing.

1) Set registers to known values and crash.
2) Set registers to known values and stop at hardcoded bkpt.
3) Call breakOnMe() to increment g_global
4) Run 2 threads at normal priority
5) Run 2 threads with testThread2 at osPriorityLow
6) Trigger mbed hard fault handler
Set selection in g_selection:
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:38
38	    __debugbreak();
(gdb) set var g_selection=1
(gdb) c
Continuing.
```

This test should generate a Hard Fault caused by a precise data access to address 0xFFFFFFF0 as seen below:
```
(gdb) c
Continuing.

**Hard Fault**
  Status Register: 0x40000000
    Forced
**Bus Fault**
  Status Register: 0x82
    Fault Address: 0xfffffff0
    Precise Data Access

Thread 1 received signal SIGSEGV, Segmentation fault.
0x0804062e in testContextWithCrash ()
```

The registers should have known values at this time, most ascending as seen in the GDB session below:
```console
(gdb) info all-reg
r0             0xfffffff0          -16
r1             0x1                 1
r2             0x2                 2
r3             0x3                 3
r4             0x4                 4
r5             0x5                 5
r6             0x6                 6
r7             0x7                 7
r8             0x8                 8
r9             0x9                 9
r10            0xa                 10
r11            0xb                 11
r12            0xc                 12
sp             0x2400e890          0x2400e890 <_main_stack+32664>
lr             0x8046053           0x8046053 <main()+38>
pc             0x804062e           0x804062e <testContextWithCrash+198>
xpsr           0x810b0000          -2129985536
d0             0.0078125055733835325 (raw 0x3f800000bf800000)
d1             32.000007629394531  (raw 0x4040000040000000)
d2             2048.0004920959473  (raw 0x40a0000040800000)
d3             32768.007904052734  (raw 0x40e0000040c00000)
d4             262144.0634765625   (raw 0x4110000041000000)
d5             1048576.2543945312  (raw 0x4130000041200000)
d6             4194305.01953125    (raw 0x4150000041400000)
d7             16777220.0859375    (raw 0x4170000041600000)
d8             50331656.1875       (raw 0x4188000041800000)
d9             100663312.390625    (raw 0x4198000041900000)
d10            201326624.8125      (raw 0x41a8000041a00000)
d11            402653249.6875      (raw 0x41b8000041b00000)
d12            805306499.5         (raw 0x41c8000041c00000)
d13            1610612999.25       (raw 0x41d8000041d00000)
d14            3221225999          (raw 0x41e8000041e00000)
d15            6442451999          (raw 0x41f8000041f00000)
fpscr          0xb280008d          -1300234099
msp            0x2407ff78          604503928
psp            0x2400e828          604039208
primask        0x0                 0
basepri        0x0                 0
faultmask      0x0                 0
control        0x0                 0
s0             -1                  (raw 0xbf800000)
s1             1                   (raw 0x3f800000)
s2             2                   (raw 0x40000000)
s3             3                   (raw 0x40400000)
s4             4                   (raw 0x40800000)
s5             5                   (raw 0x40a00000)
s6             6                   (raw 0x40c00000)
s7             7                   (raw 0x40e00000)
s8             8                   (raw 0x41000000)
s9             9                   (raw 0x41100000)
s10            10                  (raw 0x41200000)
s11            11                  (raw 0x41300000)
s12            12                  (raw 0x41400000)
s13            13                  (raw 0x41500000)
s14            14                  (raw 0x41600000)
s15            15                  (raw 0x41700000)
s16            16                  (raw 0x41800000)
s17            17                  (raw 0x41880000)
s18            18                  (raw 0x41900000)
s19            19                  (raw 0x41980000)
s20            20                  (raw 0x41a00000)
s21            21                  (raw 0x41a80000)
s22            22                  (raw 0x41b00000)
s23            23                  (raw 0x41b80000)
s24            24                  (raw 0x41c00000)
s25            25                  (raw 0x41c80000)
s26            26                  (raw 0x41d00000)
s27            27                  (raw 0x41d80000)
s28            28                  (raw 0x41e00000)
s29            29                  (raw 0x41e80000)
s30            30                  (raw 0x41f00000)
s31            31                  (raw 0x41f80000)
(gdb)
```

Once the register contents have been verified, we can skip over the crashing instruction and let the test continue executing:
```
(gdb) set var $pc=$pc+2
(gdb) c
Continuing.
```


## Test Generated Breakpoint
Use GDB to select ```2) Set registers to known values and stop at hardcoded bkpt.```

This test should generate a debug trap caused by a hardcoded breakpoint in the test routine as seen below:
```
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:38
38	    __debugbreak();
(gdb) set var g_selection=2
(gdb) c
Continuing.


Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
0x08040702 in testContextWithHardcodedBreakpoint ()
```

The registers should have known values at this time, most ascending as seen in GDB session below:
```
(gdb) info all-reg
r0             0x0                 0
r1             0x1                 1
r2             0x2                 2
r3             0x3                 3
r4             0x4                 4
r5             0x5                 5
r6             0x6                 6
r7             0x7                 7
r8             0x8                 8
r9             0x9                 9
r10            0xa                 10
r11            0xb                 11
r12            0xc                 12
sp             0x2400e890          0x2400e890 <_main_stack+32664>
lr             0x8046053           0x8046053 <main()+38>
pc             0x8040702           0x8040702 <testContextWithHardcodedBreakpoint+194>
xpsr           0x810b0000          -2129985536
d0             0.0078125055733835325 (raw 0x3f800000bf800000)
d1             32.000007629394531  (raw 0x4040000040000000)
d2             2048.0004920959473  (raw 0x40a0000040800000)
d3             32768.007904052734  (raw 0x40e0000040c00000)
d4             262144.0634765625   (raw 0x4110000041000000)
d5             1048576.2543945312  (raw 0x4130000041200000)
d6             4194305.01953125    (raw 0x4150000041400000)
d7             16777220.0859375    (raw 0x4170000041600000)
d8             50331656.1875       (raw 0x4188000041800000)
d9             100663312.390625    (raw 0x4198000041900000)
d10            201326624.8125      (raw 0x41a8000041a00000)
d11            402653249.6875      (raw 0x41b8000041b00000)
d12            805306499.5         (raw 0x41c8000041c00000)
d13            1610612999.25       (raw 0x41d8000041d00000)
d14            3221225999          (raw 0x41e8000041e00000)
d15            6442451999          (raw 0x41f8000041f00000)
fpscr          0xb280008d          -1300234099
msp            0x2407ff78          604503928
psp            0x2400e828          604039208
primask        0x0                 0
basepri        0x0                 0
faultmask      0x0                 0
control        0x0                 0
s0             -1                  (raw 0xbf800000)
s1             1                   (raw 0x3f800000)
s2             2                   (raw 0x40000000)
s3             3                   (raw 0x40400000)
s4             4                   (raw 0x40800000)
s5             5                   (raw 0x40a00000)
s6             6                   (raw 0x40c00000)
s7             7                   (raw 0x40e00000)
s8             8                   (raw 0x41000000)
s9             9                   (raw 0x41100000)
s10            10                  (raw 0x41200000)
s11            11                  (raw 0x41300000)
s12            12                  (raw 0x41400000)
s13            13                  (raw 0x41500000)
s14            14                  (raw 0x41600000)
s15            15                  (raw 0x41700000)
s16            16                  (raw 0x41800000)
s17            17                  (raw 0x41880000)
s18            18                  (raw 0x41900000)
s19            19                  (raw 0x41980000)
s20            20                  (raw 0x41a00000)
s21            21                  (raw 0x41a80000)
s22            22                  (raw 0x41b00000)
s23            23                  (raw 0x41b80000)
s24            24                  (raw 0x41c00000)
s25            25                  (raw 0x41c80000)
s26            26                  (raw 0x41d00000)
s27            27                  (raw 0x41d80000)
s28            28                  (raw 0x41e00000)
s29            29                  (raw 0x41e80000)
s30            30                  (raw 0x41f00000)
s31            31                  (raw 0x41f80000)
(gdb)
(gdb) bt
#0  0x08040702 in testContextWithHardcodedBreakpoint ()
#1  0x08046052 in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:47
```

Single step over the breakpoint exception as this should just cause KernelDebug to soft step over the instruction and immediately trap on the next instruction from which we can continue execution:
```
(gdb) si
0x08040704 in testContextWithHardcodedBreakpoint ()
(gdb) c
Continuing.
```

Use GDB to select ```2) Set registers to known values and stop at hardcoded bkpt``` again and then just issue a ```continue``` to let the test program resume execution. KernelDebug will first single step over the hardcoded breakpoint and then resume execution.
```
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
0x08040702 in testContextWithHardcodedBreakpoint ()
(gdb) c
Continuing.
```


## Test Handler and Thread Mode Breakpoints
KernelDebug can set breakpoints in both handler and thread mode code, unlike ThreadDebug which can only debug thread mode code.

Start by setting a breakpoint in an interrupt handler that we know will be executed on a regular basis, ```SysTick_Handler``` and verifying that it gets hit soon after execution is resumed.
```console
Set selection in g_selection:
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:39
39	    __debugbreak();
(gdb) break SysTick_Handler
Breakpoint 1 at 0x804640e: file ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/TOOLCHAIN_GCC/TARGET_RTOS_M4_M7/irq_cm4f.S, line 218.
(gdb) c
Continuing.
Note: automatically using hardware breakpoints for read-only addresses.

Thread 1 hit Breakpoint 1, SysTick_Handler ()
    at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/TOOLCHAIN_GCC/TARGET_RTOS_M4_M7/irq_cm4f.S:218
218	./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/TOOLCHAIN_GCC/TARGET_RTOS_M4_M7/irq_cm4f.S: No such file or directory.
(gdb) bt
#0  SysTick_Handler () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/TOOLCHAIN_GCC/TARGET_RTOS_M4_M7/irq_cm4f.S:218
#1  <signal handler called>
#2  loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:40
#3  0x08046052 in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:47
```

Now we can test breakpoints set in thread mode code by disabling the ```SysTick_Handler``` one, setting one on ```breakOnMe()``` instead, and then running test 3 which calls this function. Verify that it stops at the ```breakOnMe()``` breakpoint.
```console
(gdb) delete
Delete all breakpoints? (y or n) y
(gdb) break breakOnMe()
Breakpoint 2 at 0x804034c: file /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino, line 74.
(gdb) set var g_selection=3
(gdb) c
Continuing.

Delaying 10 seconds...

Thread 1 hit Breakpoint 2, breakOnMe () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:75
75	    g_global++;
(gdb) bt
#0  breakOnMe () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:75
#1  0x08046052 in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:47
```


## Test Watchpoints
The following tests are similar to the previous except that they make sure that watchpoints work as well.

Start with a write watchpoint:
```console
(gdb) delete
Delete all breakpoints? (y or n) y
(gdb) c
Continuing.

1) Set registers to known values and crash.
2) Set registers to known values and stop at hardcoded bkpt.
3) Call breakOnMe() to increment g_global
4) Run 2 threads at normal priority
5) Run 2 threads with testThread2 at osPriorityLow
6) Trigger mbed hard fault handler
Set selection in g_selection:
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:40
40	    __debugbreak();
(gdb) watch g_global
Hardware watchpoint 3: g_global
(gdb) set var g_selection =3
(gdb) c
Continuing.

Delaying 10 seconds...

Thread 1 hit Hardware watchpoint 3: g_global

Old value = 1
New value = 2
breakOnMe () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:77
77	}
```

Run the same test as above but for a read watchpoint:
```
(gdb) delete
Delete all breakpoints? (y or n) y
(gdb) c
Continuing.

1) Set registers to known values and crash.
2) Set registers to known values and stop at hardcoded bkpt.
3) Call breakOnMe() to increment g_global
4) Run 2 threads at normal priority
5) Run 2 threads with testThread2 at osPriorityLow
6) Trigger mbed hard fault handler
Set selection in g_selection:
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:40
40	    __debugbreak();
(gdb) rwatch g_global
Hardware read watchpoint 1: g_global
(gdb) set var g_selection=3
(gdb) c
Continuing.

Delaying 10 seconds...

Thread 1 hit Hardware read watchpoint 1: g_global

Value = 3
breakOnMe () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:77
77	}
```


## Test Threaded Single Stepping at Normal Priority
When single stepping through a thread with GDB, KernelDebug will typically disable interrupts that would allow context switching to other threads. When single stepping over functions (next/nexti) then the other threads will be allowed to run. This test shows debug output spew being generated from 2 threads while single stepping through one of them.

Set a breakpoint on "thread2Func" and then use GDB to select ```4) Run 2 threads at normal priority```

```console
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:43
43	    __debugbreak();
(gdb) delete
Delete all breakpoints? (y or n) y
(gdb) break thread2Func
Breakpoint 1 at 0x8040360: file /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino, line 131.
(gdb) set var g_selection=4
(gdb) c
Continuing.

Set g_stop to true to end test...
[New Thread 603986164]
[New Thread 603986096]
[Switching to Thread 603986164]

Thread 5 hit Breakpoint 1, thread2Func (pv=0x0)
    at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:131
131	static void thread2Func(void* pv) {
(gdb) info thread
  Id   Target Id                                        Frame
  1    Thread 603998056 ("main" Ready)                  0x0804040c in runThreads (thread2Priority=osPriorityNormal,
    thread1Priority=osPriorityNormal) at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:116
  2    Thread 604051136 ("USBevt" WaitingEventFlags)    osEventFlagsWait (ef_id=0x24003ec0 <event+4>, flags=604051136,
    options=0, timeout=4294967295) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_evflags.c:567
  3    Thread 603996216 ("rtx_idle" Ready)              0x08046cf6 in osKernelResume (sleep_ticks=250)
    at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_kernel.c:498
  4    Thread 603996148 ("rtx_timer" WaitingMessageGet) __svcMessageQueueGet (a4=<optimized out>, a3=<optimized out>,
    a2=<optimized out>, a1=<optimized out>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c:723
* 5    Thread 603986164 ("testThread2" Running)         thread2Func (pv=0x0)
    at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:131
  6    Thread 603986096 ("testThread1" WaitingDelay)    osDelay (ticks=<optimized out>)
    at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_delay.c:96
(gdb) thread apply all bt

Thread 6 (Thread 603986096):
#0  osDelay (ticks=<optimized out>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_delay.c:96
#1  0x0804ef7c in rtos::ThisThread::sleep_for (rel_time=...) at ./mbed-os/rtos/source/ThisThread.cpp:226
#2  0x0804038e in thread1Func (pv=<optimized out>) at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:126
#3  0x08047d68 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 5 (Thread 603986164):
#0  thread2Func (pv=0x0) at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:131
#1  0x08047d68 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 4 (Thread 603996148):
#0  __svcMessageQueueGet (a4=<optimized out>, a3=<optimized out>, a2=<optimized out>, a1=<optimized out>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c:723
#1  osMessageQueueGet (mq_id=mq_id@entry=0x24003f70 <os_timer_mq_cb>, msg_ptr=0x24003ff4 <os_timer_thread_cb>, msg_ptr@entry=0x240046f0 <os_timer_thread_stack+752>, msg_prio=0x240046f0 <os_timer_thread_stack+752> "p?", msg_prio@entry=0x0, timeout=timeout@entry=4294967295) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c:882
#2  0x080486a2 in osRtxTimerThread (argument=0x24003f70 <os_timer_mq_cb>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_timer.c:159
#3  0x08047d68 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 3 (Thread 603996216):
#0  0x08046cf6 in osKernelResume (sleep_ticks=250) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_kernel.c:498
#1  0x08048a98 in rtos_idle_loop () at ./mbed-os/cmsis/device/rtos/source/mbed_rtx_idle.cpp:174
#2  0x080488a2 in osRtxIdleThread (argument=<optimized out>) at ./mbed-os/cmsis/device/rtos/source/mbed_rtx_handlers.c:51
#3  0x08047d68 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 2 (Thread 604051136):
#0  osEventFlagsWait (ef_id=0x24003ec0 <event+4>, flags=604051136, options=0, timeout=4294967295) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_evflags.c:567
#1  0x0804edba in rtos::EventFlags::wait_for (this=this@entry=0x24003ebc <event>, flags=flags@entry=255, opt=<optimized out>, opt@entry=0, rel_time=..., clear=true) at ./mbed-os/rtos/source/EventFlags.cpp:130
#2  0x0804edc6 in rtos::EventFlags::wait_any_for (this=this@entry=0x24003ebc <event>, flags=flags@entry=255, rel_time=..., rel_time@entry=..., clear=<optimized out>) at ./mbed-os/rtos/source/EventFlags.cpp:108
#3  0x0804edd0 in rtos::EventFlags::wait_any (this=this@entry=0x24003ebc <event>, flags=flags@entry=255, millisec=millisec@entry=4294967295, clear=clear@entry=true) at /Users/username/depots/gcc-arm-none-eabi/install-native/arm-none-eabi/include/c++/9.3.1/chrono:331
#4  0x080459c0 in waitForPortClose () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/USB/USBSerial.cpp:33
#5  0x0804efae in mbed::Callback<void ()>::call() const (this=0x2401164c) at ./mbed-os/platform/include/platform/Callback.h:230
#6  mbed::Callback<void ()>::operator()() const (this=0x2401164c) at ./mbed-os/platform/include/platform/Callback.h:515
#7  rtos::Thread::_thunk (thread_ptr=0x24011648) at ./mbed-os/rtos/source/Thread.cpp:340
#8  0x08047d68 in svcRtxThreadGetId () at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_thread.c:843
Backtrace stopped: previous frame identical to this frame (corrupt stack?)

Thread 1 (Thread 603998056):
#0  0x0804040c in runThreads (thread2Priority=osPriorityNormal, thread1Priority=osPriorityNormal) at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:116
#1  0x0804607a in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:47
(gdb) n
134	        DebugSerial.println("Thread2 Output");
(gdb)
133	        delay(2000);
(gdb)
Thread1 Output
Thread1 Output

134	        DebugSerial.println("Thread2 Output");
(gdb)
Thread2 Output
133	        delay(2000);
(gdb)
Thread1 Output
Thread1 Output
134	        DebugSerial.println("Thread2 Output");
(gdb)
Thread1 Output
Thread2 Output
133	        delay(2000);
(gdb)
Thread1 Output
Thread1 Output
134	        DebugSerial.println("Thread2 Output");
(gdb)
Thread2 Output
133	        delay(2000);
(gdb)
Thread1 Output
Thread1 Output
134	        DebugSerial.println("Thread2 Output");
(gdb) ni
0x08040370	134	        DebugSerial.println("Thread2 Output");
(gdb)
0x08040372	134	        DebugSerial.println("Thread2 Output");
(gdb)
Thread2 Output
0x08040376	134	        DebugSerial.println("Thread2 Output");
(gdb)
133	        delay(2000);
(gdb)
0x0804036a	133	        delay(2000);
(gdb)
Thread1 Output

Thread1 Output
134	        DebugSerial.println("Thread2 Output");
(gdb)
0x08040370	134	        DebugSerial.println("Thread2 Output");
(gdb)
0x08040372	134	        DebugSerial.println("Thread2 Output");
(gdb)
Thread1 Output
Thread2 Output
0x08040376	134	        DebugSerial.println("Thread2 Output");
```


# CTRL+C During Long Single Step
When stepping over the ```delay(2000)``` calls, try pressing CTRL+C to break in and dump the callstack.

```console
(gdb) n
134	        DebugSerial.println("Thread2 Output");
(gdb)
133	        delay(2000);
(gdb)
Thread1 Output
^C
Thread 1 received signal SIGINT, Interrupt.
[Switching to Thread 603998056]
runThreads (thread2Priority=osPriorityNormal, thread1Priority=osPriorityNormal)
    at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:116
116	    while (!g_stop) {
(gdb) bt
#0  runThreads (thread2Priority=osPriorityNormal, thread1Priority=osPriorityNormal)
    at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:116
#1  0x0804607a in main () at /Users/username/Documents/Arduino/hardware/arduino/mbed/cores/arduino/main.cpp:47
```

Set ```g_stop``` to 1 to stop the current test.
```console
gdb) set var g_stop=1
(gdb) c
Continuing.
Thread2 OutputThread1 Output


1) Set registers to known values and crash.
2) Set registers to known values and stop at hardcoded bkpt.
3) Call breakOnMe() to increment g_global
4) Run 2 threads at normal priority
5) Run 2 threads with testThread2 at osPriorityLow
6) Trigger mbed hard fault handler
Set selection in g_selection:
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/adamgreen/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:43
43	    __debugbreak();
```


## HardFault in Handler Mode
KernelDebug can debug crashes encountered in handler mode code, unlike ThreadDebug.

Use GDB to select ```6) Trigger mbed hard fault handler```

This should lead to a stacking exception as seen below:

```console
Thread 1 received signal SIGTRAP, Trace/breakpoint trap.
loop () at /Users/username/depots/KernelDebug_TestPass/KernelDebug_TestPass.ino:43
43	    __debugbreak();
(gdb) set var g_selection=6
(gdb) c
Continuing.


**Hard Fault**
  Status Register: 0x40000000
    Forced
**MPU Fault**
  Status Register: 0x30
    FP Lazy Preservation
    Stacking Error w/ SP = 0xffffff88

Thread 1 received signal SIGSEGV, Segmentation fault.
0xdeaddeac in ?? ()
(gdb) info thread
  Id   Target Id                                        Frame
* 1    Thread 603998056 ("main" Running)                0xdeaddeac in ?? ()
  2    Thread 604051136 ("USBevt" WaitingEventFlags)    osEventFlagsWait (ef_id=0x24003ec0 <event+4>, flags=604051136,
    options=0, timeout=4294967295) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_evflags.c:567
  3    Thread 603996216 ("rtx_idle" Ready)              0x08046cf6 in osKernelResume (sleep_ticks=181)
    at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_kernel.c:498
  4    Thread 603996148 ("rtx_timer" WaitingMessageGet) __svcMessageQueueGet (a4=<optimized out>, a3=<optimized out>,
    a2=<optimized out>, a1=<optimized out>) at ./mbed-os/cmsis/CMSIS_5/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c:723
```
