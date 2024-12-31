#pragma once

#define PAI 3.1415926535897932384626433832795

#define ABS(a) (((a)<0)?-(a):(a))

/* adjust the angle to the range [0-360]. */
const float AdjustDir(const float angle);
const int   AdjustDir(const int angle);

/* 查表法求sine值. */
const float SinT(const float theat);

/* 查表法求cosine值. */
const float CosT(const float theat);

/* 快速求arctan值. */
const float Atan(float x, float y);

/* 快速获取一个位于min和max之间的随机数. */
const int Rnd(const int min, const int max);

void initRand2(void);
int rand2(void);

/* 计算L2距离和L2距离平方. */
float pointLen(float x1, float y1, float x2, float y2);
float pointLen2(float x1, float y1, float x2, float y2);

