#include "systeminc\tool.h"
#include "itemsmall.h"
#include <windows.h>


ItemsMall* ItemsMall::instance = NULL;
std::map<int, ITEMSMALL> ItemsMall::g_mapItemsMall;

ItemsMall& ItemsMall::MakeInstance()
{
    if (instance == NULL)
        instance = new ItemsMall;
    return *instance;
}

void ItemsMall::Destroy()
{
    if (instance != NULL)
    {
        delete instance;
        instance = NULL;
    }
}

void ItemsMall::SetItemDatas(char* pszData)
{
    if (pszData[0] == '#')
        return ;
    for (int i = 0; i < (int)strlen(pszData); i++)
    {
        if (pszData[i] == '#')
        {
            pszData[i] = '\0';
            break;
        }
    }

    ITEMSMALL item;
    char szToken[64];
    item.id = getIntegerToken(pszData, ',', 1);
    item.itemId = getIntegerToken(pszData, ',', 2);

    int debug;
    if (item.id == 200)
        debug = 0;

    memset(szToken, 0, sizeof(szToken));
    getStringToken(pszData, ',', 3, sizeof(szToken) - 1, szToken);
    strcpy(item.itemName, szToken);

    item.itemFlag = getIntegerToken(pszData, ',', 4);
    item.bmpNo = getIntegerToken(pszData, ',', 5);
    item.currencyType = getIntegerToken(pszData, ',', 6);
    item.currencyNum = getIntegerToken(pszData, ',', 7);
    item.recordType = getIntegerToken(pszData, ',', 8);

    char szLine1[128];
    char szLine2[128];
    char szLine3[128];
    memset(szLine1, 0, sizeof(szLine1));
    memset(szLine2, 0, sizeof(szLine2));
    memset(szLine3, 0, sizeof(szLine3));

    memset(szToken, 0, sizeof(szToken));
    getStringToken(pszData, ',', 9, sizeof(szToken) - 1, szToken);

    getStringToken(szToken, '|', 1, sizeof(szLine1) - 1, szLine1);
    strcpy(item.szLine1, szLine1);

    getStringToken(szToken, '|', 2, sizeof(szLine2) - 1, szLine2);
    strcpy(item.szLine2, szLine2);

    getStringToken(szToken, '|', 3, sizeof(szLine3) - 1, szLine3);
    strcpy(item.szLine3, szLine3);

    g_mapItemsMall.insert(std::make_pair(item.id, item));
}

int ItemsMall::GetItemsNums()
{
    int nNums = 0;
    std::map<int, ITEMSMALL>::iterator iter = g_mapItemsMall.begin();
    for (; iter != g_mapItemsMall.end(); iter++)
        if (iter->second.itemFlag == 1)
            ++nNums;
    return nNums;
}

int ItemsMall::GetPetsNums()
{
    int nNums = 0;
    std::map<int, ITEMSMALL>::iterator iter = g_mapItemsMall.begin();
    for (; iter != g_mapItemsMall.end(); iter++)
    {
        if (iter->second.itemFlag == 2)
            ++nNums;
    }
    return nNums;
}

ITEMSMALL* ItemsMall::GetItemByIndex(int nIndex)
{
    if (nIndex >= 0 && nIndex <= GetAllItemSize())
    {
        return &g_mapItemsMall[nIndex];
    }

    return NULL;
}

ITEMSMALL* ItemsMall::GetItemByType(int nIndex, int nType, int recordType)
{
    int nNums = 0;
    std::map<int, ITEMSMALL>::iterator iter = g_mapItemsMall.begin();
    for (; iter != g_mapItemsMall.end(); iter++)
    {
        if (nType == iter->second.currencyType && recordType == iter->second.recordType)
        {
            if (nNums == nIndex)
            {
                return &(iter->second);
            }

            ++nNums;
        }
    }

    return NULL;
}

ITEMSMALL* ItemsMall::GetAllItemsByIndex(int nIndex, int nType)
{
    int nNums = 0;
    std::map<int, ITEMSMALL>::iterator iter = g_mapItemsMall.begin();
    for (; iter != g_mapItemsMall.end(); iter++)
    {
        if (nType == iter->second.currencyType && 1 == iter->second.itemFlag)  //ITEM
        {
            if (nNums == nIndex)
            {
                return &(iter->second);
            }

            ++nNums;
        }
    }

    return NULL;
}

ITEMSMALL* ItemsMall::GetAllPetsByIndex(int nIndex, int nType)
{
    int nNums = 0;
    std::map<int, ITEMSMALL>::iterator iter = g_mapItemsMall.begin();
    for (; iter != g_mapItemsMall.end(); iter++)
    {
        if (nType == iter->second.currencyType && 2 == iter->second.itemFlag)  //PET
            if (nNums == nIndex)
                return &(iter->second);
            ++nNums;
    }

    return NULL;
}

ITEMSMALL* ItemsMall::GetAllOtherItemsByIndex(int nIndex, int nType)
{
    int nNums = 0;
    std::map<int, ITEMSMALL>::iterator iter = g_mapItemsMall.begin();
    for (; iter != g_mapItemsMall.end(); iter++)
    {
        if (nType == iter->second.currencyType && 3 == iter->second.itemFlag)  //������Ʒ
            if (nNums == nIndex)
                return &(iter->second);
            ++nNums;
    }
    return NULL;
}

int ItemsMall::GetCurrentItemNum(int nCurrencyType, int nType)
{
    int nItemNums = 0;
    std::map<int, ITEMSMALL>::iterator iter = g_mapItemsMall.begin();
    if (nType == 1)
    {
        for (; iter != g_mapItemsMall.end(); iter++)
        {
            if (iter->second.currencyType == nCurrencyType && iter->second.recordType == nType)
                ++nItemNums;
        }
    }
    else
    {
        int nFlag = -1;
        if (nType == 2)
        {
            nFlag = 1;
        }
        else if (nType == 3)
        {
            nFlag = 2;
        }
        else if (nType == 4)
        {
            nFlag = 3;
        }
        for (; iter != g_mapItemsMall.end(); iter++)
        {
            if (iter->second.currencyType == nCurrencyType && iter->second.itemFlag == nFlag)
                ++nItemNums;
        }
    }

    return nItemNums;
}
