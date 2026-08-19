#include <stdio.h>
#include <string.h>

#include <canfestival.h>
#include <Slave.h>
#include <TestSlave.h>

s_BOARD SlaveBoard = {"can0","125k"};

static void InitNode(CO_Data *d, UNS32 id) {
    setNodeId(&TestSlave_Data, 0x02);
    setState(&TestSlave_Data, Initialisation);
}

static void Exit(CO_Data *d, UNS32 id) {
    setState(&TestSlave_Data, Stopped);
}

int main(int argc, char **argv) {
    if (argc > 1 ) {
        SlaveBoard.busname = argv[1];
    }

    TimerInit();

    TestSlave_Data.heartbeatError = TestSlave_heartbeatError;
    TestSlave_Data.initialisation = TestSlave_initialisation;
    TestSlave_Data.preOperational = TestSlave_preOperational;
    TestSlave_Data.operational = TestSlave_operational;
    TestSlave_Data.stopped = TestSlave_stopped;
    TestSlave_Data.post_sync = TestSlave_post_sync;
    TestSlave_Data.post_TPDO = TestSlave_post_TPDO;
    TestSlave_Data.storeODSubIndex = TestSlave_storeODSubIndex;
    TestSlave_Data.post_emcy = TestSlave_post_emcy;

    if (!canOpen(&SlaveBoard, &TestSlave_Data)) {
        printf("canopen_slave: cannot open board (%s,%s)\n",
            SlaveBoard.busname, SlaveBoard.baudrate);
        return 1;
    }

    printf("canopen_slave: node started on '%s', node-id  0x02\n", SlaveBoard.busname);

    StartTimerLoop(&InitNode);

    printf("canopen_slave: press Enter to stop\n");
    getchar();

    Exit(&TestSlave_Data, 0);
    canClose(&TestSlave_Data);
    TimerCleanup();

    return 0;
}
