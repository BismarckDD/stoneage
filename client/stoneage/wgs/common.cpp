/*-----------------------------------------------
   Common.cpp -- common function
   (c) 许百胜Cary Hsu, 1999.10.4
-----------------------------------------------*/
#include "version.h"
#include "message.h"
#include "systeminc/login.h"
#include "systeminc/netmain.h"
#include <time.h>
#include <windows.h>

#ifdef _MEMORYLEAK_CHECK
#include "LeakDetector/vld.h"
#endif

char szPid[10];
char szCSIP[125];
char szError[1024];
char szAnnouncement[1024];

// 正式版
const char szWGS[] = "210.64.97.28";
// const char *szWGS = "wgs.hwaei.com.tw";
WORD wWGS = 13005;

WORD wCS;
char szPlain[80];
char szCipher[80];
char P_Key[8];
char P_Key1[256];
extern int iEncrypt;
DWORD dwServer = NULL;
int iWGS = 0;

BOOL ReadWGSMessage(LPSTR lpstr, int &iStrLen) {
  TCHAR cMessage = 0;
  Parameter *param;
  if (RET_OK == AnalyzeMessage(lpstr, iStrLen)) {
    if (WGS == dwServer) {
      while (messHead) {
        cMessage = *messHead->param->lpstrParam;
        param = ReleaseMessHeadParam();
        if (M_REDI == cMessage) {
          if (GetIP_PORT(param)) {
            iWGS = 2;
            ReleaseMessList();
          }
        } else if (M_PKEY == cMessage) {
          iEncrypt = E_ECB;
          dwServer = CS;
          GetEncrypt(param);
          PKey_Init();
          iWGS = 4;
          ReleaseMessList();
        } else if (M_ERRO == cMessage) {
          iWGS = 9;
          if (messHead->param && messHead->param->lpstrParam)
            lstrcpy(szError, messHead->param->lpstrParam);
        }
        RemoveMessHead();
      }
    } else {
      while (messHead) {
        cMessage = *messHead->param->lpstrParam;
        param = ReleaseMessHeadParam();
        if (M_PKEY == cMessage) {
          iEncrypt = E_ECB;
          GetEncrypt(param);
          iWGS = 4;
        } else if (M_CONT == cMessage) {
          iWGS = 6;
        } else if (M_GOTO == cMessage) {
          iWGS = 8;
          iEncrypt = E_NO;
          ipAddressAnalyze();
          ReleaseMessList();
        } else if (M_ERRO == cMessage) {
          iWGS = 9;
          if (messHead->param && messHead->param->lpstrParam)
            lstrcpy(szError, messHead->param->lpstrParam);
          ReleaseMessList();
        }
        RemoveMessHead();
      }
    }
  } else
    return FALSE;
  iStrLen = 0;
  return TRUE;
}

BOOL GetIP_PORT(Parameter *param) {
  if ((param) && (param->lpstrParam)) {
    lstrcpy(szCSIP, param->lpstrParam);
    if ((param = ReleaseMessHeadParam()) && (param->lpstrParam)) {
      wCS = (WORD)atoi(param->lpstrParam);
      if ((param = ReleaseMessHeadParam()) && (param->lpstrParam)) {
        lstrcpy(szPid, param->lpstrParam);
        return TRUE;
      }
    }
  }
  return FALSE;
}

void GetEncrypt(Parameter *param) {
  if (param && param->lpstrParam) {
    int iLen = strlen(param->lpstrParam);
    iLen >>= 1;
    strncpy_s(szPlain, param->lpstrParam, iLen);
    szPlain[iLen] = '\0';
    strcpy(szCipher, param->lpstrParam + iLen);
  }
}

void PKey_Init() {
  TCHAR szTemp[17], szTemp1[3];
  srand((unsigned)time(NULL));
  for (int i = 0; i < 7; i++)
    szTemp[i] = P_Key[i] = rand() % 256;
  P_Key[7] = '\0';
  for (int i = 7; i < 16; i++)
    szTemp[i] = rand() % 256;
  P_Key1[0] = '\0';
  for (int i = 0; i < 16; i++) {
    wsprintf(szTemp1, "%02X", (unsigned char)~szTemp[i]);
    lstrcat(P_Key1, szTemp1);
  }
}

#include "systeminc/chat.h"
extern STR_BUFFER idKey;
extern STR_BUFFER passwd;
int appendWriteBuf(int index, char *buf, int size);

void DoHellow() {
  AddParamForFormat(M_HELO2, FORMAT_START);
  AddParamForFormat(idKey.buffer, idKey.cnt, NULL);
  AddParamForFormat(passwd.buffer, passwd.cnt, FORMAT_FINISH);
  int iLen;
  LPTSTR lpstr = GetString(iLen, P_Key);
  appendWriteBuf(NULL, lpstr, iLen);
}

void DoSKey() {
  int iLen;
  AddParamForFormat(M_SKEY, FORMAT_START);
  PKey_Init();
  AddParamForFormat(P_Key1, 32, FORMAT_FINISH);
  LPTSTR lpstr = GetString(iLen, SZ_ECB);
  appendWriteBuf(NULL, lpstr, iLen);
}

void ipAddressAnalyze() {
  char szIP[50];
  DWORD dwLevel = 0;
  DWORD dwID, dwResult;
  int nGroup1, indexgm;
  LPSTR lpstrTemp;
  DWORD dwCafeKey = GetCafeNumber();
  Parameter *param = messHead->param;
  HKEY hkey;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Blizzard\\StoneAge\\2.50.000", 0,
                   KEY_READ, &hkey) == ERROR_SUCCESS) {
    dwCafeKey |= 0x02;
    RegCloseKey(hkey);
  }
  if (param && param->lpstrParam) {
    nGroup = 0; //
    indexgm = 0;
    ZeroMemory(gmsv, sizeof(GameServer) * MAX_GMSV);
    ZeroMemory(gmgroup, sizeof(GameGroup) * MAX_GMGROUP);
    nGroup1 = atoi(param->lpstrParam);
    if (nGroup1 > MAX_GMGROUP)
      nGroup1 = MAX_GMGROUP;
    param = ReleaseMessHeadParam();

    for (int i = 0; i < nGroup1; i++) {
      if (param && param->lpstrParam) {
        dwID = atoi(param->lpstrParam);
        param = ReleaseMessHeadParam();
        dwResult = dwID & dwCafeKey;
        dwLevel = 1; // 可以选
        if (dwCafeKey) {
        } else {
          if (dwID) {
            dwLevel = 0; // 看不到
          }
        }
      }
      if (param && param->lpstrParam) {
        if (dwLevel)
          strcpy(gmgroup[nGroup].name, param->lpstrParam);
        param = ReleaseMessHeadParam();
      }
      if (param && param->lpstrParam) {
        gmgroup[nGroup].num = atoi(param->lpstrParam);
        param = ReleaseMessHeadParam();
      }
      if (param && param->lpstrParam) {
        strcpy(szIP, param->lpstrParam);
        param = ReleaseMessHeadParam();
        gmgroup[nGroup].startindex = indexgm;

#ifdef _SHOW_COUNT // WON ADD 秀服务器流量
        if (param && param->lpstrParam) {
          param = ReleaseMessHeadParam();
        }
#endif

        for (int j = 0; j < gmgroup[nGroup].num && indexgm < MAX_GMSV;
             j++, indexgm++) {
          if (param && param->lpstrParam) {
            if (dwLevel)
              strcpy(gmsv[indexgm].name, param->lpstrParam);
            param = ReleaseMessHeadParam();
          }
          if (param && param->lpstrParam) {
            if (dwLevel) {
              char *sss;
              if (lpstrTemp = strtok_s(param->lpstrParam, ":", &sss)) {
                if (szIP[0]) { // 同一group在同一C class的范围
                  strcpy(gmsv[indexgm].ipaddr, szIP);
                  strcat_s(gmsv[indexgm].ipaddr, ".");
                  strcat_s(gmsv[indexgm].ipaddr, lpstrTemp);
                } else // Copy 完整的IP address
                  strcpy(gmsv[indexgm].ipaddr, lpstrTemp);
                char *sss;
                if (lpstrTemp = strtok_s(NULL, ":", &sss)) {
                  strcpy(gmsv[indexgm].port, lpstrTemp);
                  gmsv[indexgm].used = '1';
                }
              }
            }
            param = ReleaseMessHeadParam();
          }
#ifdef _SHOW_COUNT // WON ADD 秀服务器流量
          if (param && param->lpstrParam) {
            strcpy(gmsv[indexgm].count, param->lpstrParam);
            param = ReleaseMessHeadParam();
          }
#endif
        }
        if (dwLevel) {
          gmgroup[nGroup].used = (char)dwLevel;
          nGroup++;
        }
      }
    }
    if (param && param->lpstrParam) {
      strcpy(szAnnouncement, param->lpstrParam);
      param = ReleaseMessHeadParam();
    } else
      szAnnouncement[0] = '\0';
  }
}

#ifdef _MEMORYLEAK_CHECK

#endif
