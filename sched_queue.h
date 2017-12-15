//
// Created by MightyPork on 2017/11/21.
//

#ifndef GEX_SCHED_QUEUE_H
#define GEX_SCHED_QUEUE_H

#include <TinyFrame/TinyFrame.h>

typedef struct sched_que_item Job;
typedef void (*ScheduledJobCb) (Job *job);

struct sched_que_item {
    ScheduledJobCb cb;
    union {
        TF_ID frame_id;
        void *data1;
    };
    union {
        uint32_t d32;
        uint8_t *buf;
        const uint8_t *cbuf;
        const char *str;
        void *data2;
    };
    union {
        uint32_t len;
        void *data3;
    };
};

#endif //GEX_SCHED_QUEUE_H
