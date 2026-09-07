#define __HANDLETIME_CPP__
#include "version.h"
//
#include "systeminc/handletime.h"
#include "systeminc/system.h"

#define LSTIME_SECONDS_PER_DAY 5400 /* 每天的秒数 */
#define LSTIME_HOURS_PER_DAY 1024   /* SA游戏中每天的小时数 */
#define LSTIME_DAYS_PER_YEAR 100    /* SA游戏时间中每年的天数 */

/*******************************************************************
byHiO 1998/12/4 18:37
*******************************************************************/
static long era = (long)912766409 + 5400;

void RealTimeToSATime(LSTIME *lstime) {
  long lsseconds;
  long lsdays;
  // cary 坋拻
  lsseconds = (TimeGetTime() - FirstTime) / 1000 + serverTime - era;

  lstime->year =
      (int)(lsseconds / (LSTIME_SECONDS_PER_DAY * LSTIME_DAYS_PER_YEAR));
  lsdays = lsseconds / LSTIME_SECONDS_PER_DAY;
  lstime->day = lsdays % LSTIME_DAYS_PER_YEAR;
  lstime->hour = (int)(lsseconds % LSTIME_SECONDS_PER_DAY) *
                 LSTIME_HOURS_PER_DAY / LSTIME_SECONDS_PER_DAY;
  return;
}

LSTIME_SECTION getLSTime(LSTIME *lstime) {
  if (NIGHT_TO_MORNING < lstime->hour && lstime->hour <= MORNING_TO_NOON)
    return LS_MORNING;
  else if (NOON_TO_EVENING < lstime->hour && lstime->hour <= EVENING_TO_NIGHT)
    return LS_EVENING;
  else if (EVENING_TO_NIGHT < lstime->hour && lstime->hour <= NIGHT_TO_MORNING)
    return LS_NIGHT;
  else
    return LS_NOON;
}

void TimeZoneProc(void) {
  int timeZoneNo;
  timeZoneNo = getLSTime(&SaTime);
  if (SaTimeZoneNo != timeZoneNo) {
    SaTimeZoneNo = timeZoneNo;
    if (TimeZonePalChangeFlag == TRUE) {
      PaletteChange(SaTimeZoneNo, PAL_CHANGE_TIME);
    }
  }
}
