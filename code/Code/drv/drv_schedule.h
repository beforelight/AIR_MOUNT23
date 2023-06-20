#ifndef T12PEN_DRV_SCHEDULE_H
#define T12PEN_DRV_SCHEDULE_H
#include <stdbool.h>
#ifdef __cplusplus
void ScheduleInit(unsigned int Priority = 15);
#else
void ScheduleInit(unsigned int Priority);
#endif

#ifdef __cplusplus
extern "C"{
#endif

bool ScheduleActive(void (*call)());
void ScheduleIRQHandler();

#ifdef __cplusplus
};
#endif
#endif //T12PEN_DRV_SCHEDULE_H
