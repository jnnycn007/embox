#include <stdio.h>
#include <string.h>

#include <canfestival.h>
#include <Master.h>
#include <TestMaster.h>
#include <nmtMaster.h>

s_BOARD MasterBoard = {"can0","125k"};

static void InitNode(CO_Data *d, UNS32 id) {
    setNodeId(&TestMaster_Data, 0x01);
    setState(&TestMaster_Data, Initialisation);
}

static void Exit(CO_Data *d, UNS32 id) {
    masterSendNMTstateChange(&TestMaster_Data, 0, NMT_Reset_Node);
    setState(&TestMaster_Data, Stopped);
}

int main(int argc, char **argv) {
    if (argc > 1 ) {
        MasterBoard.busname = argv[1];
    }

    TimerInit();

    TestMaster_Data.heartbeatError = TestMaster_heartbeatError;
    TestMaster_Data.initialisation = TestMaster_initialisation;
    TestMaster_Data.preOperational = TestMaster_preOperational;
    TestMaster_Data.operational = TestMaster_operational;
    TestMaster_Data.stopped = TestMaster_stopped;
    TestMaster_Data.post_sync = TestMaster_post_sync;
    TestMaster_Data.post_TPDO = TestMaster_post_TPDO;
    TestMaster_Data.post_emcy = TestMaster_post_emcy;

    if (!canOpen(&MasterBoard, &TestMaster_Data)) {
        printf("canopen_master: cannot open board (%s,%s)\n",
            MasterBoard.busname, MasterBoard.baudrate);
        return 1;
    }

    printf("canopen_master: node started on '%s', node-id  0x01\n", MasterBoard.busname);

    StartTimerLoop(&InitNode);

    masterSendNMTstateChange(&TestMaster_Data, 0, NMT_Start_Node);

    printf("canopen_master: press Enter to stop\n");
    getchar();

    Exit(&TestMaster_Data, 0);
    canClose(&TestMaster_Data);
    TimerCleanup();

    return 0;
}
