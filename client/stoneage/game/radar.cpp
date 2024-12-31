﻿#include "version.h"
#include "systeminc/system.h"
#include "oft/vg410.h"
#include "oft/work.h"

Sint8 Course_Tbl[]={
    16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    16,12,10,10, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    16,14,12,11,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    16,14,13,12,11,11,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    16,15,14,13,12,11,11,11,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    16,15,14,13,13,12,12,11,11,11,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    16,15,14,14,13,12,12,12,11,11,11,11,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8,
    16,15,15,14,13,13,12,12,12,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,15,15,14,14,13,13,12,12,12,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,15,15,14,14,13,13,13,12,12,12,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,15,15,15,14,14,13,13,13,12,12,12,12,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,14,14,13,13,13,13,12,12,12,12,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,14,14,14,13,13,13,12,12,12,12,12,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,14,14,14,13,13,13,13,12,12,12,12,12,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,15,14,14,14,13,13,13,13,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,15,14,14,14,14,13,13,13,13,13,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9, 9, 9, 9,
    16,16,15,15,15,15,14,14,14,14,13,13,13,13,13,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9, 9, 9, 9,
    16,16,15,15,15,15,14,14,14,14,14,13,13,13,13,13,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10, 9,
    16,16,15,15,15,15,15,14,14,14,14,13,13,13,13,13,13,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,14,14,14,14,14,13,13,13,13,13,13,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,14,14,14,14,14,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,14,14,14,14,14,14,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,15,14,14,14,14,14,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,15,14,14,14,14,14,14,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,15,14,14,14,14,14,14,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,15,15,14,14,14,14,14,14,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,15,15,14,14,14,14,14,14,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,15,15,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,10,
    16,16,16,15,15,15,15,15,15,15,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,
    16,16,16,16,15,15,15,15,15,15,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10,
    16,16,16,16,15,15,15,15,15,15,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,
    16,16,16,16,15,15,15,15,15,15,15,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,
    16,16,16,16,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,11,
    16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,11,11,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,11,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,12,
    16,16,16,16,16,16,16,15,15,15,15,15,15,15,15,15,15,15,15,15,14,14,14,14,14,14,14,14,14,14,14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,12,12,12,12,12,12,
};
Sint8 Distance_Tbl[]={
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     1, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     2, 2, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     3, 3, 3, 4, 5, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     4, 4, 4, 5, 5, 6, 7, 8, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     5, 5, 5, 5, 6, 7, 7, 8, 9,10,11,12,13,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     6, 6, 6, 6, 7, 7, 8, 9,10,10,11,12,13,14,15,16,17,18,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     7, 7, 7, 7, 8, 8, 9, 9,10,11,12,13,13,14,15,16,17,18,19,20,21,22,23,24,25,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     8, 8, 8, 8, 8, 9,10,10,11,12,12,13,14,15,16,17,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
     9, 9, 9, 9, 9,10,10,11,12,12,13,14,15,15,16,17,18,19,20,21,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
    10,10,10,10,10,11,11,12,12,13,14,14,15,16,17,18,18,19,20,21,22,23,24,25,26,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
    11,11,11,11,11,12,12,13,13,14,14,15,16,17,17,18,19,20,21,21,22,23,24,25,26,27,28,29,30,31,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,61,62,63,
    12,12,12,12,12,13,13,13,14,15,15,16,16,17,18,19,20,20,21,22,23,24,25,25,26,27,28,29,30,31,32,33,34,35,36,37,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
    13,13,13,13,13,13,14,14,15,15,16,17,17,18,19,19,20,21,22,23,23,24,25,26,27,28,29,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
    14,14,14,14,14,14,15,15,16,16,17,17,18,19,19,20,21,22,22,23,24,25,26,26,27,28,29,30,31,32,33,34,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
    15,15,15,15,15,15,16,16,17,17,18,18,19,19,20,21,21,22,23,24,25,25,26,27,28,29,30,30,31,32,33,34,35,36,37,38,39,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,57,58,59,60,61,62,63,64,
    16,16,16,16,16,16,17,17,17,18,18,19,20,20,21,21,22,23,24,24,25,26,27,28,28,29,30,31,32,33,34,34,35,36,37,38,39,40,41,42,43,44,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,
    17,17,17,17,17,17,18,18,18,19,19,20,20,21,22,22,23,24,24,25,26,27,27,28,29,30,31,31,32,33,34,35,36,37,38,38,39,40,41,42,43,44,45,46,47,48,49,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,
    18,18,18,18,18,18,18,19,19,20,20,21,21,22,22,23,24,24,25,26,26,27,28,29,30,30,31,32,33,34,34,35,36,37,38,39,40,41,42,42,43,44,45,46,47,48,49,50,51,52,53,54,55,55,56,57,58,59,60,61,62,63,64,65,
    19,19,19,19,19,19,19,20,20,21,21,21,22,23,23,24,24,25,26,26,27,28,29,29,30,31,32,33,33,34,35,36,37,38,38,39,40,41,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,61,62,63,64,65,
    20,20,20,20,20,20,20,21,21,21,22,22,23,23,24,25,25,26,26,27,28,29,29,30,31,32,32,33,34,35,36,36,37,38,39,40,41,42,42,43,44,45,46,47,48,49,50,51,52,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,
    21,21,21,21,21,21,21,22,22,22,23,23,24,24,25,25,26,27,27,28,29,29,30,31,31,32,33,34,35,35,36,37,38,39,39,40,41,42,43,44,45,46,46,47,48,49,50,51,52,53,54,55,56,57,57,58,59,60,61,62,63,64,65,66,
    22,22,22,22,22,22,22,23,23,23,24,24,25,25,26,26,27,27,28,29,29,30,31,31,32,33,34,34,35,36,37,38,38,39,40,41,42,43,43,44,45,46,47,48,49,50,50,51,52,53,54,55,56,57,58,59,60,61,62,62,63,64,65,66,
    23,23,23,23,23,23,23,24,24,24,25,25,25,26,26,27,28,28,29,29,30,31,31,32,33,33,34,35,36,37,37,38,39,40,41,41,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,55,56,57,58,59,60,61,62,63,64,65,66,67,
    24,24,24,24,24,24,24,25,25,25,26,26,26,27,27,28,28,29,30,30,31,31,32,33,33,34,35,36,36,37,38,39,40,40,41,42,43,44,44,45,46,47,48,49,50,51,51,52,53,54,55,56,57,58,59,60,60,61,62,63,64,65,66,67,
    25,25,25,25,25,25,25,25,26,26,26,27,27,28,28,29,29,30,30,31,32,32,33,33,34,35,36,36,37,38,39,39,40,41,42,43,43,44,45,46,47,48,48,49,50,51,52,53,54,55,55,56,57,58,59,60,61,62,63,64,65,65,66,67,
    26,26,26,26,26,26,26,26,27,27,27,28,28,29,29,30,30,31,31,32,32,33,34,34,35,36,36,37,38,38,39,40,41,42,42,43,44,45,46,46,47,48,49,50,51,51,52,53,54,55,56,57,58,59,59,60,61,62,63,64,65,66,67,68,
    27,27,27,27,27,27,27,27,28,28,28,29,29,29,30,30,31,31,32,33,33,34,34,35,36,36,37,38,38,39,40,41,41,42,43,44,45,45,46,47,48,49,49,50,51,52,53,54,55,55,56,57,58,59,60,61,62,63,63,64,65,66,67,68,
    28,28,28,28,28,28,28,28,29,29,29,30,30,30,31,31,32,32,33,33,34,35,35,36,36,37,38,38,39,40,41,41,42,43,44,44,45,46,47,48,48,49,50,51,52,53,53,54,55,56,57,58,59,59,60,61,62,63,64,65,66,67,68,68,
    29,29,29,29,29,29,29,29,30,30,30,31,31,31,32,32,33,33,34,34,35,35,36,37,37,38,38,39,40,41,41,42,43,43,44,45,46,47,47,48,49,50,51,51,52,53,54,55,56,56,57,58,59,60,61,62,63,63,64,65,66,67,68,69,
    30,30,30,30,30,30,30,30,31,31,31,31,32,32,33,33,34,34,34,35,36,36,37,37,38,39,39,40,41,41,42,43,43,44,45,46,46,47,48,49,50,50,51,52,53,54,54,55,56,57,58,59,60,60,61,62,63,64,65,66,67,67,68,69,
    31,31,31,31,31,31,31,31,32,32,32,32,33,33,34,34,34,35,35,36,36,37,38,38,39,39,40,41,41,42,43,43,44,45,46,46,47,48,49,49,50,51,52,53,53,54,55,56,57,57,58,59,60,61,62,63,64,64,65,66,67,68,69,70,
    32,32,32,32,32,32,32,32,32,33,33,33,34,34,34,35,35,36,36,37,37,38,38,39,40,40,41,41,42,43,43,44,45,45,46,47,48,48,49,50,51,52,52,53,54,55,56,56,57,58,59,60,61,61,62,63,64,65,66,67,68,68,69,70,
    33,33,33,33,33,33,33,33,33,34,34,34,35,35,35,36,36,37,37,38,38,39,39,40,40,41,42,42,43,43,44,45,45,46,47,48,48,49,50,51,51,52,53,54,55,55,56,57,58,59,59,60,61,62,63,64,65,65,66,67,68,69,70,71,
    34,34,34,34,34,34,34,34,34,35,35,35,36,36,36,37,37,38,38,38,39,39,40,41,41,42,42,43,44,44,45,46,46,47,48,48,49,50,50,51,52,53,54,54,55,56,57,58,58,59,60,61,62,62,63,64,65,66,67,68,68,69,70,71,
    35,35,35,35,35,35,35,35,35,36,36,36,37,37,37,38,38,38,39,39,40,40,41,41,42,43,43,44,44,45,46,46,47,48,48,49,50,50,51,52,53,53,54,55,56,57,57,58,59,60,61,61,62,63,64,65,66,66,67,68,69,70,71,72,
    36,36,36,36,36,36,36,36,36,37,37,37,37,38,38,39,39,39,40,40,41,41,42,42,43,43,44,45,45,46,46,47,48,48,49,50,50,51,52,53,53,54,55,56,56,57,58,59,60,60,61,62,63,64,64,65,66,67,68,69,69,70,71,72,
    37,37,37,37,37,37,37,37,37,38,38,38,38,39,39,39,40,40,41,41,42,42,43,43,44,44,45,45,46,47,47,48,48,49,50,50,51,52,53,53,54,55,55,56,57,58,59,59,60,61,62,63,63,64,65,66,67,67,68,69,70,71,72,73,
    38,38,38,38,38,38,38,38,38,39,39,39,39,40,40,40,41,41,42,42,42,43,43,44,44,45,46,46,47,47,48,49,49,50,50,51,52,53,53,54,55,55,56,57,58,58,59,60,61,62,62,63,64,65,66,66,67,68,69,70,71,71,72,73,
    39,39,39,39,39,39,39,39,39,40,40,40,40,41,41,41,42,42,42,43,43,44,44,45,45,46,46,47,48,48,49,49,50,51,51,52,53,53,54,55,55,56,57,58,58,59,60,61,61,62,63,64,65,65,66,67,68,69,69,70,71,72,73,74,
    40,40,40,40,40,40,40,40,40,41,41,41,41,42,42,42,43,43,43,44,44,45,45,46,46,47,47,48,48,49,50,50,51,51,52,53,53,54,55,55,56,57,58,58,59,60,60,61,62,63,64,64,65,66,67,68,68,69,70,71,72,72,73,74,
    41,41,41,41,41,41,41,41,41,41,42,42,42,43,43,43,44,44,44,45,45,46,46,47,47,48,48,49,49,50,50,51,52,52,53,53,54,55,55,56,57,57,58,59,60,60,61,62,63,63,64,65,66,67,67,68,69,70,71,71,72,73,74,75,
    42,42,42,42,42,42,42,42,42,42,43,43,43,43,44,44,44,45,45,46,46,46,47,47,48,48,49,49,50,51,51,52,52,53,54,54,55,55,56,57,58,58,59,60,60,61,62,63,63,64,65,66,66,67,68,69,70,70,71,72,73,74,74,75,
    43,43,43,43,43,43,43,43,43,43,44,44,44,44,45,45,45,46,46,47,47,47,48,48,49,49,50,50,51,51,52,53,53,54,54,55,56,56,57,58,58,59,60,60,61,62,62,63,64,65,65,66,67,68,69,69,70,71,72,73,73,74,75,76,
    44,44,44,44,44,44,44,44,44,44,45,45,45,45,46,46,46,47,47,47,48,48,49,49,50,50,51,51,52,52,53,53,54,55,55,56,56,57,58,58,59,60,60,61,62,62,63,64,65,65,66,67,68,68,69,70,71,72,72,73,74,75,76,76,
    45,45,45,45,45,45,45,45,45,45,46,46,46,46,47,47,47,48,48,48,49,49,50,50,51,51,51,52,53,53,54,54,55,55,56,57,57,58,58,59,60,60,61,62,62,63,64,65,65,66,67,68,68,69,70,71,71,72,73,74,75,75,76,77,
    46,46,46,46,46,46,46,46,46,46,47,47,47,47,48,48,48,49,49,49,50,50,50,51,51,52,52,53,53,54,54,55,56,56,57,57,58,59,59,60,60,61,62,62,63,64,65,65,66,67,67,68,69,70,70,71,72,73,74,74,75,76,77,78,
    47,47,47,47,47,47,47,47,47,47,48,48,48,48,49,49,49,49,50,50,51,51,51,52,52,53,53,54,54,55,55,56,56,57,58,58,59,59,60,61,61,62,63,63,64,65,65,66,67,67,68,69,70,70,71,72,73,73,74,75,76,77,77,78,
    48,48,48,48,48,48,48,48,48,48,49,49,49,49,50,50,50,50,51,51,52,52,52,53,53,54,54,55,55,56,56,57,57,58,58,59,60,60,61,61,62,63,63,64,65,65,66,67,67,68,69,70,70,71,72,73,73,74,75,76,76,77,78,79,
    49,49,49,49,49,49,49,49,49,49,50,50,50,50,50,51,51,51,52,52,52,53,53,54,54,55,55,55,56,56,57,57,58,59,59,60,60,61,62,62,63,63,64,65,65,66,67,67,68,69,70,70,71,72,72,73,74,75,75,76,77,78,79,79,
    50,50,50,50,50,50,50,50,50,50,50,51,51,51,51,52,52,52,53,53,53,54,54,55,55,55,56,56,57,57,58,58,59,59,60,61,61,62,62,63,64,64,65,65,66,67,67,68,69,70,70,71,72,72,73,74,75,75,76,77,78,78,79,80,
    51,51,51,51,51,51,51,51,51,51,51,52,52,52,52,53,53,53,54,54,54,55,55,55,56,56,57,57,58,58,59,59,60,60,61,61,62,63,63,64,64,65,66,66,67,68,68,69,70,70,71,72,72,73,74,75,75,76,77,77,78,79,80,81,
    52,52,52,52,52,52,52,52,52,52,52,53,53,53,53,54,54,54,55,55,55,56,56,56,57,57,58,58,59,59,60,60,61,61,62,62,63,63,64,65,65,66,66,67,68,68,69,70,70,71,72,72,73,74,74,75,76,77,77,78,79,80,80,81,
    53,53,53,53,53,53,53,53,53,53,53,54,54,54,54,55,55,55,55,56,56,57,57,57,58,58,59,59,59,60,60,61,61,62,62,63,64,64,65,65,66,67,67,68,68,69,70,70,71,72,72,73,74,74,75,76,77,77,78,79,80,80,81,82,
    54,54,54,54,54,54,54,54,54,54,54,55,55,55,55,56,56,56,56,57,57,57,58,58,59,59,59,60,60,61,61,62,62,63,63,64,64,65,66,66,67,67,68,69,69,70,70,71,72,72,73,74,74,75,76,77,77,78,79,79,80,81,82,82,
    55,55,55,55,55,55,55,55,55,55,55,56,56,56,56,57,57,57,57,58,58,58,59,59,60,60,60,61,61,62,62,63,63,64,64,65,65,66,66,67,68,68,69,69,70,71,71,72,73,73,74,75,75,76,77,77,78,79,79,80,81,82,82,83,
    56,56,56,56,56,56,56,56,56,56,56,57,57,57,57,57,58,58,58,59,59,59,60,60,60,61,61,62,62,63,63,64,64,65,65,66,66,67,67,68,68,69,70,70,71,71,72,73,73,74,75,75,76,77,77,78,79,79,80,81,82,82,83,84,
    57,57,57,57,57,57,57,57,57,57,57,58,58,58,58,58,59,59,59,60,60,60,61,61,61,62,62,63,63,63,64,64,65,65,66,66,67,67,68,69,69,70,70,71,72,72,73,73,74,75,75,76,77,77,78,79,79,80,81,82,82,83,84,84,
    58,58,58,58,58,58,58,58,58,58,58,59,59,59,59,59,60,60,60,61,61,61,62,62,62,63,63,63,64,64,65,65,66,66,67,67,68,68,69,69,70,71,71,72,72,73,74,74,75,75,76,77,77,78,79,79,80,81,82,82,83,84,84,85,
    59,59,59,59,59,59,59,59,59,59,59,60,60,60,60,60,61,61,61,61,62,62,62,63,63,64,64,64,65,65,66,66,67,67,68,68,69,69,70,70,71,71,72,73,73,74,74,75,76,76,77,77,78,79,79,80,81,82,82,83,84,84,85,86,
    60,60,60,60,60,60,60,60,60,60,60,61,61,61,61,61,62,62,62,62,63,63,63,64,64,65,65,65,66,66,67,67,68,68,68,69,69,70,71,71,72,72,73,73,74,75,75,76,76,77,78,78,79,80,80,81,82,82,83,84,84,85,86,87,
    61,61,61,61,61,61,61,61,61,61,61,61,62,62,62,62,63,63,63,63,64,64,64,65,65,65,66,66,67,67,67,68,68,69,69,70,70,71,71,72,72,73,74,74,75,75,76,77,77,78,78,79,80,80,81,82,82,83,84,84,85,86,86,87,
    62,62,62,62,62,62,62,62,62,62,62,62,63,63,63,63,64,64,64,64,65,65,65,66,66,66,67,67,68,68,68,69,69,70,70,71,71,72,72,73,73,74,74,75,76,76,77,77,78,79,79,80,80,81,82,82,83,84,84,85,86,86,87,88,
    63,63,63,63,63,63,63,63,63,63,63,63,64,64,64,64,65,65,65,65,66,66,66,67,67,67,68,68,68,69,69,70,70,71,71,72,72,73,73,74,74,75,75,76,76,77,78,78,79,79,80,81,81,82,82,83,84,84,85,86,87,87,88,89,
};
/********************************************************************
*
*      ????
*
*      IN     d1.w   ??火
*             d2.w   ??火
*
*      OUT    d0     ???( 0 ? 31 )
*             d1     ??  ( 0 ? ??? )
*
*      break  d2,d3,d4,a2
*
********************************************************************/
void radar(ACTION *a0,int *d1,int *d2)
{
    int d0,d3=0,d4;
#if 0
    *d1-=ATR_H_POS(a0);        /*???ㄅ*/
    if(*d1<0){
        *d1 = 0 - *d1;        /*‧▋π???*/
        d3++;        /*?劫????*/
    }
    d4=(*d1)>>6;

    *d2-=ATR_V_POS(a0);        /*???π*/
    if(*d2<0){
        *d2 = 0 - *d2;
        d3+=2;
    }
    d0=(*d2)>>6;            /*?????????????*/

    if(d4<d0) d4=d0;        /*???????*/

    *d1=(*d1)>>d4;        /*?????*/
    *d2=(*d2)>>d4;

    *d2=(*d2)<<6;
    *d2+=*d1;        /*??????π???*/

    *d1=(int)(Course_Tbl[*d2]);        /*?????????*/

    if(d3==1){
        *d1-=32;        /*劫???????*/
        *d1 = 0 - *d1;        /*‧▋π???*/
    }
    if(d3==2){
        *d1-=16;        /*劫????????*/
        *d1 = 0 - *d1;        /*‧▋π???*/
    }
    if(d3==3){
    *d1+=16;        /*??劫?*/
    *d1=*d1&31;
    }
    *d2=(int)(Distance_Tbl[*d2]);        /*??(89??)*/
    *d2=*d2<<d4;        /*??????*/
#else
    *d1-=ATR_H_POS(a0);        /*???ㄅ*/
    if(*d1<0){
        *d1 = 0 - *d1;        /*‧▋π???*/
        d3++;        /*?劫????*/
    }
    d4=(*d1)/64;

    *d2-=ATR_V_POS(a0);        /*???π*/
    if(*d2<0){
        *d2 = 0 - *d2;
        d3+=2;
    }
    d0=(*d2)/64;            /*?????????????*/

    if(d4<d0) d4=d0;        /*???????*/

    d4++;        //????

    *d1=(*d1)/d4;        /*?????*/
    *d2=(*d2)/d4;

    *d2=(*d2)<<6;
    *d2+=*d1;        /*??????π???*/

    *d1=(int)(Course_Tbl[*d2]);        /*?????????*/

    if(d3==1){
        *d1-=32;        /*劫???????*/
        *d1 = 0 - *d1;        /*‧▋π???*/
    }
    if(d3==2){
        *d1-=16;        /*劫????????*/
        *d1 = 0 - *d1;        /*‧▋π???*/
    }
    if(d3==3){
    *d1+=16;        /*??劫?*/
    *d1=*d1&31;
    }
    *d2=(int)(Distance_Tbl[*d2]);        /*??(89??)*/
    *d2=*d2*d4;        /*??????*/
#endif
}

int radar2( ACTION *pAct, int x, int y, int cnt )
{
    int d0,d3=0,d4;
    int dist;    // ?????
    
    x-=ATR_H_POS(pAct);        /*???ㄅ*/
    if(x<0){
        x = 0 - x;        /*‧▋π???*/
        d3++;        /*?劫????*/
    }
    d4=(x)>>6;

    y-=ATR_V_POS(pAct);        /*???π*/
    if(y<0){
        y = 0 - y;
        d3+=2;
    }
    d0=(y)>>6;            /*?????????????*/

    if(d4<d0) d4=d0;        /*???????*/

    x=(x)>>d4;        /*?????*/
    y=(y)>>d4;

    y=(y)<<6;
    y+=x;        /*??????π???*/

    x = (int)(Course_Tbl[y]);        /*?????????*/

    if(d3==1){
        x-=32;        /*劫???????*/
        x = 0 - x;
    }
    if(d3==2){
        x-=16;        /*劫????????*/
        x = 0 - x;
    }
    if(d3==3){
    x+=16;        /*??劫?*/
    x=x&31;
    }
    
    
    // ??阪?????????
    if( pAct->dirCnt == 0 ){ 
        // ???????????
        // ??
        if( x - pAct->crs >  16 ) x -= 32;    
        else if( x - pAct->crs < -16 ) x += 32;
        // ???
        if( x - pAct->crs != 0 ){
            if( x - pAct->crs > 0 ){ 
                pAct->crs++;
                if( pAct->crs == 32 ) pAct->crs = 0;
            }else{
                pAct->crs--;
                if( pAct->crs == -1 ) pAct->crs = 31;
            }
        }
        
    }
    // ????????
    pAct->dirCnt++;
    if( pAct->dirCnt >= cnt ) pAct->dirCnt = 0;
    
    dist =(int)(Distance_Tbl[ y ]);        /*??(89??)*/
    return  dist<<d4;        /*??????*/
}
