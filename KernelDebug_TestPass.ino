/* This sample is used to put the KernelDebug debug monitor through its paces before a release. */

#include <KernelDebug.h>
#include <MRI.h>

KernelDebug kernelDebug(SERIAL1_TX, SERIAL1_RX, USART1_IRQn, 230400, DEBUG_BREAK_IN_SETUP);

// Assembly language functions found in tests.S
extern "C" void testContextWithCrash(void);
extern "C" void testContextWithHardcodedBreakpoint(void);
extern "C" void testStackingHandlerException(void);

// Forward function declarations.
static void __attribute__ ((noinline)) breakOnMe();
static void runThreads(osPriority_t thread1Priority, osPriority_t thread2Priority);
static void thread1Func(void* pv);
static void thread2Func(void* pv);
static void disableLast32BytesUsingMPU();
static uint32_t getHighestMPUDataRegionIndex(void);
static uint32_t getMPUDataRegionCount(void);


void setup() {
    // Calling this gets RTX to enable the SysTick timer/interrupt used for some tests.
    osDelay(1);
}

// Selection variable to be set from GDB.
static volatile int g_selection = 0;

void loop() {
    DebugSerial.println();
    DebugSerial.println("1) Set registers to known values and crash.");
    DebugSerial.println("2) Set registers to known values and stop at hardcoded bkpt.");
    DebugSerial.println("3) Call breakOnMe() to increment g_global");
    DebugSerial.println("4) Run 2 threads at normal priority");
    DebugSerial.println("5) Run 2 threads with testThread2 at osPriorityLow");
    DebugSerial.println("6) Trigger mbed hard fault handler");


    DebugSerial.print("Set selection in g_selection: ");
    g_selection = 0;
    __debugbreak();
    while (g_selection == 0) {
    }
    DebugSerial.println();

    switch (g_selection) {
        case 1:
            testContextWithCrash();
            break;
        case 2:
            testContextWithHardcodedBreakpoint();
            break;
        case 3:
            DebugSerial.println("Delaying 10 seconds...");
            delay(10000);
            breakOnMe();
            break;
        case 4:
            runThreads(osPriorityNormal, osPriorityNormal);
            break;
        case 5:
            runThreads(osPriorityNormal, osPriorityLow);
            break;
        case 6:
            disableLast32BytesUsingMPU();
            testStackingHandlerException();
            break;
        default:
            DebugSerial.println("Invalid selection");
            break;
    }
}

static volatile uint32_t g_global;

static void __attribute__ ((noinline)) breakOnMe() {
    g_global++;
    __DSB();
}

static volatile bool g_stop = false;
static void runThreads(osPriority_t thread1Priority, osPriority_t thread2Priority) {
    delay(250);
    DebugSerial.println("Set g_stop to true to end test...");

    static uint64_t             thread1Stack[128];
    static osRtxThread_t        thread1Tcb;
    static const osThreadAttr_t thread1Attr =
    {
        .name = "testThread1",
        .attr_bits = osThreadDetached,
        .cb_mem  = &thread1Tcb,
        .cb_size = sizeof(thread1Tcb),
        .stack_mem = thread1Stack,
        .stack_size = sizeof(thread1Stack),
        .priority = thread1Priority
    };
    osThreadId_t thread1 = osThreadNew(thread1Func, NULL, &thread1Attr);

    static uint64_t             thread2Stack[128];
    static osRtxThread_t        thread2Tcb;
    static const osThreadAttr_t thread2Attr =
    {
        .name = "testThread2",
        .attr_bits = osThreadDetached,
        .cb_mem  = &thread2Tcb,
        .cb_size = sizeof(thread2Tcb),
        .stack_mem = thread2Stack,
        .stack_size = sizeof(thread2Stack),
        .priority = thread2Priority
    };
    osThreadId_t thread2 = osThreadNew(thread2Func, NULL, &thread2Attr);

    while (!g_stop) {
        // Wait for user input.
    }

    osThreadTerminate(thread1);
    osThreadTerminate(thread2);
}

static void thread1Func(void* pv) {
    while (true ) {
        delay(1000);
        DebugSerial.println("Thread1 Output");
    }
}

static void thread2Func(void* pv) {
    while (true ) {
        delay(2000);
        DebugSerial.println("Thread2 Output");
    }
}

static void disableLast32BytesUsingMPU()
{
    MPU->CTRL = 0;
        MPU->RNR = getHighestMPUDataRegionIndex();
        MPU->RBAR = 0xFFFFFFFF & MPU_RBAR_ADDR_Msk;
        MPU->RASR = MPU_RASR_XN_Msk | ((5-1) << MPU_RASR_SIZE_Pos) | MPU_RASR_ENABLE_Msk;
    MPU->CTRL = MPU_CTRL_ENABLE_Msk;
}

static uint32_t getHighestMPUDataRegionIndex(void)
{
    return getMPUDataRegionCount() - 1;
}

static uint32_t getMPUDataRegionCount(void)
{
    return (MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos;
}
