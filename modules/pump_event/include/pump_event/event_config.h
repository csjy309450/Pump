#ifndef PUMP_EVENT_CONFIG_H
#define PUMP_EVENT_CONFIG_H

// pump_event version info
#ifndef PUMP_EVENT_VERSION_MAJOR
#   define PUMP_EVENT_VERSION_MAJOR 0
#endif // !PUMP_EVENT_VERSION_MAJOR
#ifndef PUMP_EVENT_VERSION_MINOR
#   define PUMP_EVENT_VERSION_MINOR 0
#endif // !PUMP_EVENT_VERSION_MINOR
#ifndef PUMP_EVENT_VERSION_PATCH
#   define PUMP_EVENT_VERSION_PATCH 9
#endif // !PUMP_EVENT_VERSION_PATCH
#ifndef PUMP_EVENT_VERSION_BUILD
#   define PUMP_EVENT_VERSION_BUILD 0
#endif // !PUMP_EVENT_VERSION_BUILD
#define __PUMP_EVENT_VERSION_STR(str_) #str_
#define __PUMP_EVENT_VERJOIN(major_,minor_,patch_,build_) __PUMP_EVENT_VERSION_STR(major_.minor_.patch_.build_)
#define PUMP_EVENT_VERSION __PUMP_EVENT_VERJOIN(PUMP_EVENT_VERSION_MAJOR,PUMP_EVENT_VERSION_MINOR,PUMP_EVENT_VERSION_PATCH,PUMP_EVENT_VERSION_BUILD)

#endif //PUMP_EVENT_CONFIG_H