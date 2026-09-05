#ifndef __MENU_H__
#define __MENU_H__

#include "pc.h"
//
#include "chat.h"

#ifdef _TELLCHANNEL
#include "netproc.h"
#endif

#ifdef _NEW_SYSTEM_MENU
#define MENU_SYSTEM_0 28
#else
#define MENU_SYSTEM_0 9
#endif
#define MENU_MAP_0 3

#define MAIL_STR_LEN 140
#define MAIL_DATE_STR_LEN 20
#define MAIL_MAX_HISTORY 20
#define MAX_CHAT_REGISTY_STR 8

typedef struct {
  int x, y;       // 牙?火
  int defX, defY; // ??????火
  int bmpNo;      // ??吻?
  int dispPrio;   // 牙?！
  BOOL dragFlag;  // ???????
  BOOL mixFlag;   // ????
} ITEM_BUFFER;

//
typedef struct {
  char str[MAIL_MAX_HISTORY][MAIL_STR_LEN + 1];          // 
  char dateStr[MAIL_MAX_HISTORY][MAIL_DATE_STR_LEN + 1]; // 
  int noReadFlag[MAIL_MAX_HISTORY]; // ???????????????吻??
  int petLevel[MAIL_MAX_HISTORY];   // ??????????吻?
  char petName[MAIL_MAX_HISTORY][PET_NAME_LEN + 1]; // ?????〈
  int itemGraNo[MAIL_MAX_HISTORY];                  // ??????????????吻?
  int newHistoryNo;                                 // ????吻?
} MAIL_HISTORY;

typedef struct {
  int sizeX, sizeY;   // ???
  int titleNo;        // ????吻?
  int titleX, titleY; // ??????火
  int mx, my;         // 
  int nowX, nowY;     // ???????
  int cnt;            // ??火
  int wndType;        // ???????
  int hitFlag;        // ???刪叉????
  int yesDispNo;      // ??????刪叉吻?
  int noDispNo;       // ???????刪叉吻?
  int boxDispPrio;    // ???????牙?！??
  int yesNoResult;    // YesNo?????ㄩ???  ?ㄩ???
} WINDOW_DISP;

#if defined(__ALBUM_47)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND_40 542
#define MAX_PET_KIND_41 543
#define MAX_PET_KIND_42 545
#define MAX_PET_KIND_43 549
#define MAX_PET_KIND_44 551
#define MAX_PET_KIND_45 553
#define MAX_PET_KIND_46 555
// #define MAX_PET_KIND 557
// #define MAX_PET_KIND 558 // Robin fix
// #define MAX_PET_KIND 564 // Robin fix
// #define MAX_PET_KIND 566 // Robin fix
#define MAX_PET_KIND 570 // Robin fix

#elif defined(__ALBUM_46)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND_40 542
#define MAX_PET_KIND_41 543
#define MAX_PET_KIND_42 545
#define MAX_PET_KIND_43 549
#define MAX_PET_KIND_44 551
#define MAX_PET_KIND_45 553
#define MAX_PET_KIND 555

#elif defined(__ALBUM_45)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND_40 542
#define MAX_PET_KIND_41 543
#define MAX_PET_KIND_42 545
#define MAX_PET_KIND_43 549
#define MAX_PET_KIND_44 551
#define MAX_PET_KIND 553

#elif defined(__ALBUM_44)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND_40 542
#define MAX_PET_KIND_41 543
#define MAX_PET_KIND_42 545
#define MAX_PET_KIND_43 549
#define MAX_PET_KIND 551

#elif defined(__ALBUM_43)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND_40 542
#define MAX_PET_KIND_41 543
#define MAX_PET_KIND_42 545
#define MAX_PET_KIND 549

#elif defined(__ALBUM_42)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND_40 542
#define MAX_PET_KIND_41 543
#define MAX_PET_KIND 545

#elif defined(__ALBUM_41)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND_40 542
#define MAX_PET_KIND 543

#elif defined(__ALBUM_40)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND_39 540
#define MAX_PET_KIND 542

#elif defined(__ALBUM_39)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND_38 521
#define MAX_PET_KIND 540

#elif defined(__ALBUM_38)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND_37 498
#define MAX_PET_KIND 521

#elif defined(__ALBUM_37)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND_36 483
#define MAX_PET_KIND 496

#elif defined(__ALBUM_36)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND_35 462
#define MAX_PET_KIND 483
#elif defined(__ALBUM_35)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND_34 436
#define MAX_PET_KIND 462 // 460

#elif defined(__ALBUM_34)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND_33 432
#define MAX_PET_KIND 436

#elif defined(__ALBUM_33)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND_32 431
#define MAX_PET_KIND 432

#elif defined(__ALBUM_32)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND_15 357
#define MAX_PET_KIND_16 364
#define MAX_PET_KIND_17 389
#define MAX_PET_KIND_18 391
#define MAX_PET_KIND_19 395
#define MAX_PET_KIND_20 398
#define MAX_PET_KIND_21 401
#define MAX_PET_KIND_22 405
#define MAX_PET_KIND_23 407
#define MAX_PET_KIND_24 414
#define MAX_PET_KIND_25 415
#define MAX_PET_KIND_26 420
#define MAX_PET_KIND_27 422
#define MAX_PET_KIND_28 423
#define MAX_PET_KIND_29 424
#define MAX_PET_KIND_30 425
#define MAX_PET_KIND_31 429
#define MAX_PET_KIND 431

#elif defined(__ALBUM_15)
#define MAX_PET_KIND_1 124
#define MAX_PET_KIND_4 224
#define MAX_PET_KIND_5 228
#define MAX_PET_KIND_6 229
#define MAX_PET_KIND_7 230
#define MAX_PET_KIND_8 237
#define MAX_PET_KIND_9 240
#define MAX_PET_KIND_10 297
#define MAX_PET_KIND_11 301
#define MAX_PET_KIND_12 305
#define MAX_PET_KIND_13 306
#define MAX_PET_KIND_14 353
#define MAX_PET_KIND 357
#else
#define MAX_PET_KIND 10000 // ?????
#define MAX_PET_TBL 181    // ???????????
#endif

#define MAX_PET_PAGE_SIZE 8 // ?????牙????????
#ifdef _PET_ITEM
#define MAX_PET_SPECIES 42 // 唾昜昜笱杅講
#endif

enum {
  ANIM_DISP_PET,         // ???牙
  ANIM_DISP_PET_SEND,    // ??????????????去
  ANIM_DISP_PET_RECIEVE, // ?????????????去

  ANIM_DISP_PET_MAIN,   // ?????去︻???
  ANIM_DISP_PET_WALK,   // 行走
  ANIM_DISP_PET_STAND,  // 站立
  ANIM_DISP_PET_ATTACK, // 攻击
  ANIM_DISP_PET_GUARD,  // 防御
  ANIM_DISP_PET_DAMAGE, //
  ANIM_DISP_PET_DEAD,   //
  ANIM_DISP_PET_ITEM,   //

  ANIM_DISP_LETTER_MOVE, // 宠物信件移动
  ANIM_DISP_LETTER,      // 宠物新建

  ANIM_DISP_MIX_PET_INIT, // ???????
  ANIM_DISP_MIX_PET_MOVE, //
  ANIM_DISP_MIX_PET_MOJI, //
#ifdef _THEATER
  ANIM_DISP_THEATER_NPC, // 曄部NPC珆尨
#endif
#ifdef _AniCrossFrame // Syu ADD 雄賒脯蚔徹賒醱汜昜
  ANIM_DISP_CROSSFRAME = 18,
#endif
  ANIM_DISP_MIX_PET_END, // ????????
};

struct PET_ALBUM {
  char name[PET_NAME_LEN + 1];     // Pet本身的名字
  char freeName[PET_NAME_LEN + 1]; // Player给Pet起的名字
  int faceGraNo;                   // Pet的图像
  int level;                       // Pet的level
  int maxHp;                       // Pet的最大生命值
  int str;                         // Pet的力量
  int quick;                       // Pet的速度
  int def;                         // Pet的防御
  int earth, water, fire, wind;    // Pet的地水火风属性
  int flag;                        // Pet的Flag?
};

// 宠物基础信息: 用于宠物相册(编号，宠物原始名称，静态图片)
struct PET_ALBUM_TBL {
  int albumNo;                 // 相册编号: No.001
  char name[PET_NAME_LEN + 1]; // 宠物基础名称: 例如佩露夏、邦奇诺
  int faceGraNo;               // 宠物头像(静态图片)ID，不是动画ID. 
}; // 2026.09.30 药区分faceGraNo和动画(包括行动、攻击的每一帧)的区别？
// 当前ALBUM的实现实在是有点XX

// ????????
extern BOOL TaskBarFlag;
// ??????
extern unsigned int MenuToggleFlag;
// ?????????
extern ITEM_BUFFER ItemBuffer[];
// ????????????
extern int StatusUpPoint;
// ·???阪??????????
extern BOOL BattlePetReceiveFlag;
#ifdef _STANDBYPET
extern BOOL StandbyPetSendFlag;
#endif
// ????????吻?
extern int BattlePetReceivePetNo;
// ??????更??
extern MAIL_HISTORY MailHistory[];
// ?????更?仕??向??????
extern STR_BUFFER chatRegistryStr[];
// ????向?更??
extern STR_BUFFER MailStr;
// ??????????????
extern ACTION *pActMenuWnd3;
// ????????更??
extern STR_BUFFER shougouChange;

#ifdef _MONEYINPUT // Syu ADD
extern STR_BUFFER MymoneyBuffer;
#endif
#ifdef _ITEM_JIGSAW
extern int JigsawIdx;
void SetJigsaw(int img, char *str);
#endif


#ifdef __MENU_CPP__
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int bSwitchExperienceTips; // 战斗结束后的经验提示
EXTERN int bSwitchMuteOtherPlayers; // 屏蔽其他玩家
EXTERN int bRightClickAttack; // 右键攻击开关
#undef EXTERN

typedef enum {
  MAIL_WND_VIEW,
  MAIL_WND_SELECT,
  MAIL_WND_SEND,
  MAIL_WND_PET_SEND,
  MAIL_WND_ITEM,
  MAIL_WND_HISTORY,
} MAIL_WND_TYPE;

extern STR_BUFFER TradeBuffer;
// ?????〈阪??更??
extern STR_BUFFER petNameChange;
// Terry add 2002/01/04
#ifdef __EDEN_AUCTION
extern STR_BUFFER AuctionStr;   //
extern STR_BUFFER AuctionMoney; //
#endif

// Terry end
extern int petStatusNo;
extern int mailHistoryWndPageNo;   // ???????????吻?
extern int mailHistoryWndSelectNo; // ︻???????吻?
// extern int mailReceivePetGraNo;   // ??????????????????吻?

extern int MapWmdFlagBak;

#define RESULT_WND_CLOSE_TIME 300

extern int ResultWndTimer;

extern BOOL ItemMixRecvFlag;

extern int MouseCursorFlag;
#ifdef _TRADETALKWND
void TradeTalk(char *msg);
#endif

void InitItem(int x, int y, BOOL bPetItemFlag);

void InitMenu(void);

void InitMenu2(void);

BOOL CheckMenuFlag(void);

void MenuProc(void);

ACTION *MakeWindowDisp(int x, int y, int sizeX, int sizeY,
  int titleNo, int wndType);

// 画选中框(绿色的矩形框, 游戏中常用)
BOOL MakeHitBox(int x1, int y1, int x2, int y2, int dispPrio);

//
BOOL SaveMailHistory(int no);

BOOL LoadMailHistory(void);

BOOL CheckMailNoReadFlag(void);

void InitOhtaParam(void);

void DeathLetterAction(void);

BOOL SaveChatRegistyStr(void);

BOOL LoadChatRegistyStr(void);

// 按out_len的总长度，给in_str补充前空格和后空格，输出到out_str
void CenteringStr(const char *in, char *out, int out_len);

void CheckNewPet(int sprNo);

void CheckBattleNewPet(void);

BOOL LoadAlbum(void);

BOOL SaveAlbum(int no);

ACTION *MakeAnimDisp(int x, int y, int sprNo, int mode);

// Robin 04/16 trade
void tradeInit(void);

void BankmanInit(char *data);
void BankmanWN(void);
void closeBankman(void);
#endif

#ifdef _TELLCHANNEL // ROG ADD 躇逄耋
void InitSelectChar(char *msg, BOOL endFlag);
void SelectChar(void);
void DeathTellChannel(void);
extern int TalkMode;
extern char secretName[32];
#endif

#ifdef _FRIENDCHANNEL // ROG ADD 疑衭耋
#define MAX_ROOM_NUM 50
extern bool setRoomFlag;
extern bool secretFlag; // 躇逄梓
extern int selChar;
extern ACTION *pChtChanlWnd; // 謐毞弅弝敦
extern ACTION *pSelChanlWnd;
void SelectChatRoom(void);
void InitSelChatRoom(char *msg);
void InitCreateChatRoom(char *msg);
void ChatRoomWnd(void);
void initAssentWnd(char *data);
void AssentWnd(void);
extern STR_BUFFER chatRoomName; // 謐毞弅靡備
extern ACTION *pSetRoomWnd;
void setRoomName(void); // 扢隅謐毞弅靡備

#endif

/////////////////////////////////////////
#ifdef _TIMEBAR_FUNCTION
int SetTimeBarPos(int timBarIdent, int CurPos);
int SetTimeBar(char *title, int range);
#endif

#ifdef _STANDBYPET
void send_StandBy_Pet(void);
#endif

#ifdef _NPC_ITEMUP
void ShowItemup(int value, int x, int y);
#endif

#ifdef _NPC_ITEMUP
void ShowCounttime(int value, int x, int y);
#endif

#ifdef _NEW_SPEED_
extern BOOL GameSpeedFlag;
#endif
