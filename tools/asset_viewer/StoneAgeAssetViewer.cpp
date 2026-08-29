#define UNICODE
#define _UNICODE
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Adrn {
    uint32_t bitmapNo, offset, size;
    int32_t x, y;
    uint32_t width, height;
    uint8_t attr[44];
};
static_assert(sizeof(Adrn) == 72);
struct SpriteAddress { uint32_t id, offset; uint16_t count; };
#pragma pack(push, 1)
struct RdHeader { char id[2]; uint8_t flag; uint8_t pad; uint32_t width, height, size; };
struct AnimHeader { uint16_t dir, action; uint32_t duration, count; };
struct Frame { uint32_t image; int16_t x, y; uint16_t sound; };
#pragma pack(pop)

struct Image { int w=0,h=0; std::vector<uint32_t> px; bool ok() const { return w>0&&h>0&&!px.empty(); } };
struct SpriteAnim { uint16_t dir=0, action=0; uint32_t duration=0; std::vector<Frame> frames; };

static HWND gMain, gRoot, gPick, gType, gTabs, gId, gLoad, gPrev, gNext, gPlay, gList, gCanvas, gInfo, gStatus;
static fs::path gClient; static std::ifstream gReal; static std::map<uint32_t,Adrn> gAdrn;
static std::ifstream gSpr; static std::map<uint32_t,SpriteAddress> gSprAdrn;
static uint32_t gPalette[256]; static Image gImage; static std::vector<SpriteAnim> gAnims;
static std::vector<uint32_t> gIds;
static int gCategory=0;
static int gAnim=0,gFrame=0; static bool gPlaying=false; static UINT_PTR gTimer=0;

static std::wstring W(const fs::path& p){return p.wstring();}
static void Text(HWND h,const std::wstring&s){SetWindowTextW(h,s.c_str());}
static std::wstring GetText(HWND h){int n=GetWindowTextLengthW(h);std::wstring s(n,L'\0');GetWindowTextW(h,s.data(),n+1);return s;}
static void Status(const std::wstring&s){Text(gStatus,s);}
static void SaveLastClient(const fs::path&p){HKEY key;if(RegCreateKeyExW(HKEY_CURRENT_USER,L"Software\\StoneAgeAssetViewer",0,nullptr,0,KEY_SET_VALUE,nullptr,&key,nullptr)==ERROR_SUCCESS){auto s=p.wstring();RegSetValueExW(key,L"LastClient",0,REG_SZ,(const BYTE*)s.c_str(),(DWORD)((s.size()+1)*sizeof(wchar_t)));RegCloseKey(key);}}
static fs::path LoadLastClient(){HKEY key;if(RegOpenKeyExW(HKEY_CURRENT_USER,L"Software\\StoneAgeAssetViewer",0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS)return {};DWORD type=0,size=0;if(RegQueryValueExW(key,L"LastClient",nullptr,&type,nullptr,&size)!=ERROR_SUCCESS||type!=REG_SZ){RegCloseKey(key);return {};}std::vector<wchar_t>b(size/sizeof(wchar_t)+1);if(RegQueryValueExW(key,L"LastClient",nullptr,&type,(BYTE*)b.data(),&size)!=ERROR_SUCCESS){RegCloseKey(key);return {};}RegCloseKey(key);return fs::path(b.data());}
static std::vector<fs::path> Files(const fs::path& d,const wchar_t* stem){
    std::vector<fs::path> out; if(!fs::exists(d)) return out;
    for(auto&e:fs::directory_iterator(d)) if(e.is_regular_file()){
        auto n=e.path().filename().wstring(); std::wstring low=n; std::transform(low.begin(),low.end(),low.begin(),::towlower);
        if(low.rfind(stem,0)==0 && e.path().extension()==L".bin") out.push_back(e.path());
    } std::sort(out.begin(),out.end()); return out;
}
static int Version(const fs::path&p,const wchar_t*stem){auto s=p.stem().wstring();auto pos=s.find(L'_');if(pos==std::wstring::npos)return 0;return _wtoi(s.c_str()+pos+1);}
static fs::path Best(const fs::path& d,const wchar_t* stem){auto v=Files(d,stem);if(v.empty())return {};return *std::max_element(v.begin(),v.end(),[&](auto&a,auto&b){return Version(a,stem)<Version(b,stem);});}
static std::pair<fs::path,fs::path> BestPair(const fs::path&d,const wchar_t*a,const wchar_t*b){
    auto av=Files(d,a),bv=Files(d,b);fs::path ap,bp;int best=-1;
    for(auto&x:av)for(auto&y:bv)if(Version(x,a)==Version(y,b)&&Version(x,a)>best){best=Version(x,a);ap=x;bp=y;}
    if(ap.empty()&&!av.empty()&&!bv.empty()){ap=Best(d,a);bp=Best(d,b);}return {ap,bp};
}
static bool ReadExact(std::ifstream&f,void*p,size_t n){f.read((char*)p,n);return (size_t)f.gcount()==n;}

static void DefaultPalette(){
    for(int i=0;i<256;i++)gPalette[i]=RGB(i,i,i); gPalette[0]=RGB(24,28,36);
}
static void LoadPalette(){
    DefaultPalette(); std::vector<fs::path> candidates={gClient/L"data"/L"pal"/L"Palet_1.sap",gClient/L"data"/L"pal"/L"palet_1.sap",gClient/L"data"/L"pal"/L"Palet_0.sap",gClient/L"data"/L"pal"/L"palet_00.pal",gClient/L"data"/L"palet_00.pal",gClient/L"data"/L"pal"/L"palette.pal"};
    for(auto&p:candidates) if(fs::exists(p)){
        std::ifstream f(p,std::ios::binary); for(int i=16;i<240;i++){unsigned char bgr[3];if(!ReadExact(f,bgr,3))break;gPalette[i]=RGB(bgr[2],bgr[1],bgr[0]);}return;
    }
}
static bool Decode(const std::vector<uint8_t>&src,int&w,int&h,std::vector<uint8_t>&dst){
    if(src.size()<sizeof(RdHeader))return false; auto*r=(const RdHeader*)src.data(); if(r->id[0]!='R'||r->id[1]!='D'||!r->width||!r->height)return false;
    w=(int)r->width;h=(int)r->height; if((uint64_t)w*h>100000000)return false;dst.assign((size_t)w*h,0);
    const uint8_t*p=src.data()+sizeof(RdHeader),*end=src.data()+std::min<size_t>(src.size(),r->size);size_t o=0;
    if(r->flag==0){size_t n=std::min(dst.size(),(size_t)(end-p));memcpy(dst.data(),p,n);return n==dst.size();}
    while(p<end&&o<dst.size()){
        uint8_t idx=*p++;size_t cnt;
        if(idx&0x80){uint8_t val=(idx&0x40)?0:(p<end?*p++:0);if(idx&0x20){if(p+2>end)break;cnt=((idx&15)<<16)|(p[0]<<8)|p[1];p+=2;}else if(idx&0x10){if(p>=end)break;cnt=((idx&15)<<8)|*p++;}else cnt=idx&15;cnt=std::min(cnt,dst.size()-o);memset(dst.data()+o,val,cnt);o+=cnt;
        }else{if(idx&0x10){if(p>=end)break;cnt=((idx&15)<<8)|*p++;}else cnt=idx&15;cnt=std::min({cnt,dst.size()-o,(size_t)(end-p)});memcpy(dst.data()+o,p,cnt);p+=cnt;o+=cnt;}
    } return o==dst.size();
}
static Image LoadAssetImage(uint32_t id){
    Image out; auto it=gAdrn.find(id);if(it==gAdrn.end())return out;const auto&a=it->second;if(!a.size||a.size>128*1024*1024)return out;
    std::vector<uint8_t> packed(a.size),idx;gReal.clear();gReal.seekg(a.offset);if(!ReadExact(gReal,packed.data(),packed.size()))return out;
    if(!Decode(packed,out.w,out.h,idx))return {};out.px.resize(idx.size());for(size_t i=0;i<idx.size();i++){auto c=gPalette[idx[i]];out.px[i]=0xff000000|GetRValue(c)<<16|GetGValue(c)<<8|GetBValue(c);}return out;
}
static uint32_t LogicalToImage(uint32_t logical){for(auto&[id,a]:gAdrn){uint32_t n;memcpy(&n,a.attr+40,4);if(n==logical)return id;}return logical;}
static bool LoadSprite(uint32_t id){
    gAnims.clear();auto it=gSprAdrn.find(id);if(it==gSprAdrn.end()&&id<100000)it=gSprAdrn.find(id+100000);if(it==gSprAdrn.end())return false;
    gSpr.clear();gSpr.seekg(it->second.offset);for(unsigned i=0;i<it->second.count;i++){AnimHeader h;if(!ReadExact(gSpr,&h,sizeof(h))||h.count>10000)return false;SpriteAnim a;a.dir=h.dir;a.action=h.action;a.duration=h.duration;a.frames.resize(h.count);if(!ReadExact(gSpr,a.frames.data(),h.count*sizeof(Frame)))return false;gAnims.push_back(std::move(a));}return !gAnims.empty();
}
static bool OpenClient(const fs::path&root){
    auto data=root/L"data";auto imagePair=BestPair(data,L"adrn",L"real");auto spritePair=BestPair(data,L"spradrn",L"spr");auto adrn=imagePair.first;auto real=imagePair.second;auto spradrn=spritePair.first;auto spr=spritePair.second;
    if(adrn.empty()||real.empty()){Status(L"未找到 data\\adrn*.bin 和 data\\real*.bin");return false;}
    gAdrn.clear();std::ifstream af(adrn,std::ios::binary);Adrn a;while(ReadExact(af,&a,sizeof(a)))if(a.width&&a.height)gAdrn[a.bitmapNo]=a;
    gReal.close();gReal.open(real,std::ios::binary);gSprAdrn.clear();gSpr.close();if(!spradrn.empty()&&!spr.empty()){std::ifstream sf(spradrn,std::ios::binary);SpriteAddress s;while(ReadExact(sf,&s,sizeof(s)))gSprAdrn[s.id]=s;gSpr.open(spr,std::ios::binary);}
    gClient=root;SaveLastClient(root);LoadPalette(); wchar_t msg[768];swprintf_s(msg,L"已加载 %zu 张图片、%zu 个造型 | %s + %s",gAdrn.size(),gSprAdrn.size(),adrn.filename().c_str(),real.filename().c_str());Status(msg);return true;
}
static Image LoadMap(uint32_t id){
    Image out;auto p=gClient/L"map"/(std::to_wstring(id)+L".dat");std::ifstream f(p,std::ios::binary);int32_t w,h;if(!ReadExact(f,&w,4)||!ReadExact(f,&h,4)||w<=0||h<=0||(uint64_t)w*h>100000000)return out;
    std::vector<uint16_t> tile((size_t)w*h),part((size_t)w*h);if(!ReadExact(f,tile.data(),tile.size()*2)||!ReadExact(f,part.data(),part.size()*2))return out;
    double scale=std::min(1.0,1400.0/std::max(w,h));out.w=std::max(1,(int)(w*scale));out.h=std::max(1,(int)(h*scale));out.px.assign((size_t)out.w*out.h,0xff171b22);
    std::map<uint16_t,uint32_t> colors;auto color=[&](uint16_t n){auto q=colors.find(n);if(q!=colors.end())return q->second;Image im=LoadAssetImage(LogicalToImage(n));uint64_t r=0,g=0,b=0,c=0;for(size_t k=0;k<im.px.size();k+=std::max<size_t>(1,im.px.size()/64)){uint32_t v=im.px[k];if((v&0xffffff)!=0){r+=(v>>16)&255;g+=(v>>8)&255;b+=v&255;c++;}}uint32_t v=c?0xff000000|(r/c)<<16|(g/c)<<8|(b/c):0xff303743;colors[n]=v;return v;};
    for(int y=0;y<out.h;y++)for(int x=0;x<out.w;x++){int sx=std::min(w-1,(int)(x/scale)),sy=std::min(h-1,(int)(y/scale));size_t q=(size_t)sy*w+sx;out.px[(size_t)y*out.w+x]=color(part[q]?part[q]:tile[q]);}return out;
}
static void PopulateList(){
    gIds.clear();int type=gCategory==0?2:(gCategory<=3?1:0);SendMessage(gType,CB_SETCURSEL,type,0);
    if(type==0){for(auto&[id,a]:gAdrn)gIds.push_back(id);}
    else if(type==1){for(auto&[id,a]:gSprAdrn){bool take=(gCategory==1&&id>=100250&&id<110000)||(gCategory==2&&id>=100000&&id<100250)||(gCategory==3&&(id<100000||id>=110000));if(take)gIds.push_back(id);}}
    else if(!gClient.empty()&&fs::exists(gClient/L"map")){for(auto&e:fs::directory_iterator(gClient/L"map"))if(e.is_regular_file()&&e.path().extension()==L".dat"){auto s=e.path().stem().wstring();if(!s.empty()&&std::all_of(s.begin(),s.end(),::iswdigit))gIds.push_back((uint32_t)_wtoi(s.c_str()));}}
    std::sort(gIds.begin(),gIds.end());gIds.erase(std::unique(gIds.begin(),gIds.end()),gIds.end());
    ListView_SetItemCountEx(gList,(int)gIds.size(),LVSICF_NOSCROLL);InvalidateRect(gList,nullptr,TRUE);
}
static void Refresh(){
    uint32_t id=(uint32_t)_wtoi(GetText(gId).c_str());int type=(int)SendMessage(gType,CB_GETCURSEL,0,0);gAnims.clear();gFrame=0;gAnim=0;
    if(type==0){gImage=LoadAssetImage(id);auto it=gAdrn.find(id);wchar_t s[256];if(it!=gAdrn.end())swprintf_s(s,L"图片 ID %u   %u×%u   偏移 (%d, %d)",id,it->second.width,it->second.height,it->second.x,it->second.y);else swprintf_s(s,L"没有找到图片 ID %u",id);Text(gInfo,s);
    }else if(type==1){gPlaying=false;Text(gPlay,L"播放");if(LoadSprite(id)){gImage=LoadAssetImage(gAnims[0].frames[0].image);wchar_t s[256];swprintf_s(s,L"造型 ID %u   %zu 个动作   当前动作 %u / 方向 %u",id,gAnims.size(),gAnims[0].action,gAnims[0].dir);Text(gInfo,s);}else{gImage={};Text(gInfo,L"没有找到该 Sprite ID");}
    }else{gImage=LoadMap(id);wchar_t s[256];swprintf_s(s,L"地图 ID %u   总览 %d×%d",id,gImage.w,gImage.h);Text(gInfo,s);}InvalidateRect(gCanvas,nullptr,TRUE);
}
static void Step(int d){if(gAnims.empty()||gAnims[gAnim].frames.empty())return;int count=(int)gAnims[gAnim].frames.size();for(int tries=0;tries<count;tries++){gFrame=(gFrame+d+count)%count;Image next=LoadAssetImage(gAnims[gAnim].frames[gFrame].image);if(next.ok()){gImage=std::move(next);InvalidateRect(gCanvas,nullptr,FALSE);return;}}}
static LRESULT CALLBACK CanvasProc(HWND h,UINT m,WPARAM w,LPARAM l){if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);RECT r;GetClientRect(h,&r);FillRect(dc,&r,(HBRUSH)GetStockObject(BLACK_BRUSH));if(gImage.ok()){int type=(int)SendMessage(gType,CB_GETCURSEL,0,0);BITMAPINFO bi{};bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=gImage.w;bi.bmiHeader.biHeight=type==2?-gImage.h:gImage.h;bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;bi.bmiHeader.biCompression=BI_RGB;double fit=std::min((double)(r.right-24)/gImage.w,(double)(r.bottom-24)/gImage.h);double preferred=(type==2||std::max(gImage.w,gImage.h)>160)?1.0:2.0;double s=std::max(0.01,std::min(fit,preferred));int dw=std::max(1,(int)(gImage.w*s)),dh=std::max(1,(int)(gImage.h*s));SetStretchBltMode(dc,COLORONCOLOR);StretchDIBits(dc,(r.right-dw)/2,(r.bottom-dh)/2,dw,dh,0,0,gImage.w,gImage.h,gImage.px.data(),&bi,DIB_RGB_COLORS,SRCCOPY);}EndPaint(h,&ps);return 0;}return DefWindowProc(h,m,w,l);}
static void Layout(int w,int h){int top=58,bottom=50,left=250,tabh=34;MoveWindow(gRoot,12,12,std::max(160,w-500),28,TRUE);MoveWindow(gPick,w-480,12,92,28,TRUE);MoveWindow(gId,w-380,12,110,28,TRUE);MoveWindow(gLoad,w-262,12,74,28,TRUE);MoveWindow(gPrev,w-180,12,46,28,TRUE);MoveWindow(gNext,w-128,12,46,28,TRUE);MoveWindow(gPlay,w-76,12,64,28,TRUE);MoveWindow(gTabs,12,top,left-20,tabh,TRUE);MoveWindow(gList,12,top+tabh,left-20,h-top-bottom-tabh,TRUE);MoveWindow(gCanvas,left,top,w-left-12,h-top-bottom,TRUE);MoveWindow(gInfo,12,h-bottom+5,w-24,20,TRUE);MoveWindow(gStatus,12,h-22,w-24,18,TRUE);}
static void PickFolder(){BROWSEINFOW b{};b.hwndOwner=gMain;b.lpszTitle=L"选择石器时代客户端根目录";b.ulFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;auto pid=SHBrowseForFolderW(&b);if(pid){wchar_t p[MAX_PATH];if(SHGetPathFromIDListW(pid,p)){Text(gRoot,p);if(OpenClient(p)){PopulateList();if(!gIds.empty()){Text(gId,std::to_wstring(gIds[0]));ListView_SetItemState(gList,0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);ListView_EnsureVisible(gList,0,FALSE);}}}CoTaskMemFree(pid);}}
static LRESULT CALLBACK MainProc(HWND h,UINT m,WPARAM w,LPARAM l){switch(m){case WM_SIZE:Layout(LOWORD(l),HIWORD(l));return 0;case WM_COMMAND:{int id=LOWORD(w);if(id==10)PickFolder();else if(id==11){uint32_t wanted=(uint32_t)_wtoi(GetText(gId).c_str());auto it=std::lower_bound(gIds.begin(),gIds.end(),wanted);if(it!=gIds.end()&&*it==wanted){int row=(int)(it-gIds.begin());ListView_SetItemState(gList,-1,0,LVIS_SELECTED|LVIS_FOCUSED);ListView_SetItemState(gList,row,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);ListView_EnsureVisible(gList,row,FALSE);}else Refresh();}else if(id==12)Step(-1);else if(id==13)Step(1);else if(id==14){gPlaying=!gPlaying;Text(gPlay,gPlaying?L"暂停":L"播放");}return 0;}case WM_NOTIFY:{auto*n=(NMHDR*)l;if(n->hwndFrom==gTabs&&n->code==TCN_SELCHANGE){gCategory=TabCtrl_GetCurSel(gTabs);gPlaying=false;Text(gPlay,L"播放");PopulateList();ListView_SetItemState(gList,-1,0,LVIS_SELECTED|LVIS_FOCUSED);if(!gIds.empty()){Text(gId,std::to_wstring(gIds[0]));ListView_SetItemState(gList,0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);ListView_EnsureVisible(gList,0,FALSE);Refresh();}else{gImage={};Text(gInfo,L"该分类没有可用素材");InvalidateRect(gCanvas,nullptr,TRUE);}return 0;}if(n->hwndFrom==gList&&n->code==LVN_GETDISPINFO){auto*d=(NMLVDISPINFOW*)l;if((d->item.mask&LVIF_TEXT)&&d->item.iItem>=0&&(size_t)d->item.iItem<gIds.size())swprintf_s(d->item.pszText,d->item.cchTextMax,L"%u",gIds[d->item.iItem]);return 0;}if(n->hwndFrom==gList&&n->code==LVN_ITEMCHANGED){auto*c=(NMLISTVIEW*)l;if((c->uNewState&LVIS_SELECTED)&&!(c->uOldState&LVIS_SELECTED)&&c->iItem>=0&&(size_t)c->iItem<gIds.size()){Text(gId,std::to_wstring(gIds[c->iItem]));Refresh();}return 0;}break;}case WM_TIMER:if(gPlaying&&!gAnims.empty())Step(1);return 0;case WM_DESTROY:PostQuitMessage(0);return 0;}return DefWindowProc(h,m,w,l);}
static HWND C(const wchar_t*cls,const wchar_t*txt,DWORD style,int id=0){return CreateWindowExW(0,cls,txt,WS_CHILD|WS_VISIBLE|style,0,0,0,0,gMain,(HMENU)(INT_PTR)id,GetModuleHandleW(nullptr),nullptr);}
int WINAPI wWinMain(HINSTANCE hi,HINSTANCE,LPWSTR,int show){
    INITCOMMONCONTROLSEX ic{sizeof(ic),ICC_STANDARD_CLASSES};InitCommonControlsEx(&ic);WNDCLASSW wc{};wc.hInstance=hi;wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);wc.lpfnWndProc=MainProc;wc.lpszClassName=L"SAAssetViewer";wc.hIcon=LoadIcon(nullptr,IDI_APPLICATION);RegisterClassW(&wc);wc.lpfnWndProc=CanvasProc;wc.lpszClassName=L"SAAssetCanvas";wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);RegisterClassW(&wc);
    gMain=CreateWindowExW(0,L"SAAssetViewer",L"石器时代素材浏览器",WS_OVERLAPPEDWINDOW,100,80,1100,760,nullptr,nullptr,hi,nullptr);HFONT font=CreateFontW(-16,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Microsoft YaHei UI");
    gRoot=C(L"EDIT",L"请选择客户端目录…",WS_BORDER|ES_AUTOHSCROLL);gPick=C(L"BUTTON",L"选择目录",BS_PUSHBUTTON,10);gType=C(L"COMBOBOX",L"",CBS_DROPDOWNLIST,15);SendMessage(gType,CB_ADDSTRING,0,(LPARAM)L"图片");SendMessage(gType,CB_ADDSTRING,0,(LPARAM)L"造型");SendMessage(gType,CB_ADDSTRING,0,(LPARAM)L"地图");ShowWindow(gType,SW_HIDE);gTabs=C(WC_TABCONTROLW,L"",TCS_FIXEDWIDTH);const wchar_t*tabNames[]={L"地图",L"宠物",L"人物",L"其他",L"图片"};for(int i=0;i<5;i++){TCITEMW ti{TCIF_TEXT,0,0,(LPWSTR)tabNames[i]};TabCtrl_InsertItem(gTabs,i,&ti);}TabCtrl_SetItemSize(gTabs,42,24);TabCtrl_SetCurSel(gTabs,0);gId=C(L"EDIT",L"0",WS_BORDER|ES_NUMBER);gLoad=C(L"BUTTON",L"跳转",BS_PUSHBUTTON,11);gPrev=C(L"BUTTON",L"◀",BS_PUSHBUTTON,12);gNext=C(L"BUTTON",L"▶",BS_PUSHBUTTON,13);gPlay=C(L"BUTTON",L"播放",BS_PUSHBUTTON,14);gList=C(WC_LISTVIEWW,L"",LVS_REPORT|LVS_OWNERDATA|LVS_SHOWSELALWAYS|LVS_SINGLESEL|WS_BORDER);ListView_SetExtendedListViewStyle(gList,LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);LVCOLUMNW col{LVCF_TEXT|LVCF_WIDTH,0,180,(LPWSTR)L"素材 ID"};ListView_InsertColumn(gList,0,&col);gCanvas=C(L"SAAssetCanvas",L"",0);gInfo=C(L"STATIC",L"左侧按分类选择素材 ID，右侧查看预览",0);gStatus=C(L"STATIC",L"选择实际客户端目录以加载素材",0);
    for(HWND x:{gRoot,gPick,gTabs,gId,gLoad,gPrev,gNext,gPlay,gList,gInfo,gStatus})SendMessage(x,WM_SETFONT,(WPARAM)font,TRUE);ShowWindow(gMain,show);UpdateWindow(gMain);gTimer=SetTimer(gMain,1,120,nullptr);auto last=LoadLastClient();if(!last.empty()&&OpenClient(last)){Text(gRoot,last.wstring());PopulateList();if(!gIds.empty()){Text(gId,std::to_wstring(gIds[0]));ListView_SetItemState(gList,0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);ListView_EnsureVisible(gList,0,FALSE);}}else PickFolder();MSG msg;while(GetMessage(&msg,nullptr,0,0)){TranslateMessage(&msg);DispatchMessage(&msg);}return 0;
}
