#ifndef MAIN_CONFIG
#define MAIN_CONFIG
#include <stdint.h>

typedef enum {NONE, ON, OFF, ACCEPTED, INVITED, REJECTED} pipeline_status;
typedef struct {
    uint64_t MUG_ID;
    pipeline_status PIPELINE_STATUS;
} MUG_STATUS;


#endif
