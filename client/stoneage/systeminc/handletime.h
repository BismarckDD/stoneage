#ifndef __HANDLETIME_H__
#define __HANDLETIME_H__

#define NIGHT_TO_MORNING 700
#define MORNING_TO_NOON 930
#define NOON_TO_EVENING 200
#define EVENING_TO_NIGHT 300

struct LSTIME {
  int year;
  int day;
  int hour;
};

typedef enum {
  LS_NOON,
  LS_EVENING,
  LS_NIGHT,
  LS_MORNING,
} LSTIME_SECTION;

#ifdef __HANDLETIME_CPP__
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN LSTIME SaTime;
EXTERN long serverTime;
EXTERN long FirstTime;
EXTERN long clientTime;
EXTERN int SaTimeZoneNo;
EXTERN int TimeZonePalChangeFlag;

#undef EXTERN

void RealTimeToSATime(LSTIME *lstime);
LSTIME_SECTION getLSTime(LSTIME *lstime);
void TimeZoneProc(void);

#endif /*__HANDLETIME_H__*/
