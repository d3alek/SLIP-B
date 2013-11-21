#ifndef MAIN_CONFIG
#define MAIN_CONFIG
#include <stdint.h>

typedef enum {NONE, ON, OFF, ACCEPTED, REJECTED} pipeline_status;
typedef struct {
    uint64_t MUG_ID;
    pipeline_status PIPELINE_STATUS;
} MUG_STATUS;

// typedef struct{
//     MUG_STATUS MUG_LIST[10];
// };


#endif
