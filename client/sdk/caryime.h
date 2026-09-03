/******************************************************************************
    caryIme.h:    cary's ImeDllMain
                                                (c) 砛κ秤Cary Hsu,    2000.7.10
******************************************************************************/
#ifndef __CARYIME
#define __CARYIME

typedef void(*RESULTCALLBACK)(LPSTR);

/*    IME﹍て
parameter:    hwnd:            ┮ㄏノHWND
            ResultCallback:    いゅ块猭挡狦厨
return:        TRUE:            腨岿粇叫挡祘Α玥礚猭ㄏノいゅ块猭*/
BOOL InitIme(HWND hwnd,RESULTCALLBACK ResultCallback);

/*    IME挡*/
void EndIme();

/*    э跑陪ボ家Αwindow mode┪fullscreen mode
parameter:    bWindowMode:    ㄏノ跌怠家Α        */
void ChangeMode(BOOL bWindowMode);

/*    矪瞶IMEmessage
parameter:    Message:    跌怠癟
            wParam,lParam:跌怠癟把计
return:    TRUE:    竒Τ矪瞶IMEmessage
        FALSE:    临Τㄤmessage惠璶矪瞶    */
BOOL ImeMessage(UINT Message,WPARAM wParam,LPARAM lParam);

/*    clear IME buffer    */
void ClearIme();

/*    IMEON,OFF篈絋粄
return:    ON:    TRUE
        OFF:FALSE        */
BOOL ImeOpenState();

/*    眔块猭﹃candiate┪compsition
return:    NULL:    ⊿Τ﹃叫︽矪瞶
        獶NULL:    Τ块猭﹃                */
LPSTR GetImeString();

/*    眔块猭磞瓃﹃
return:    块猭磞瓃﹃    */
LPSTR GetImeDescString();

///////////////////////////////////////////////////////////////////////////////

/*    眔lpstr
parameter:    lpstrHead:    ﹃繷
            lpstr:        璶﹃
return:                            */
char *GetCharPrev(char *lpstrHead,char *lpstr);

#endif
