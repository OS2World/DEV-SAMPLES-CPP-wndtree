#define INCL_GRAPHICBUTTON
#define INCL_WIN
#include <os2.h>
#include <sw.h>
#include <stdio.h>
#include <stdlib.h>

HAB hab;

int dumpDesktopWindows=1;
int dumpObjectWindows=1;
int dumpWindowData=0;
int dumpWindowID=1;
int dumpWindowRect=1;
int dumpWindowClass=1;
int dumpWindowOwner=1;
int dumpWindowStyle=1;

static void indent(int level) {
        while(level--)
                printf("  ");
}

static void dumpPredefinedClassName(const char *className) {
        static struct pc {
                const char *niceName;
                PSZ WC_name;
        } pcs[] = {
                {"Frame",               WC_FRAME                },
                {"ComboBox",            WC_COMBOBOX             },
                {"Button",              WC_BUTTON               },
                {"Menu",                WC_MENU                 },
                {"Static",              WC_STATIC               },
                {"EntryField",          WC_ENTRYFIELD           },
                {"ListBox",             WC_LISTBOX              },
                {"ScrollBar",           WC_SCROLLBAR            },
                {"TitleBar",            WC_TITLEBAR             },
                {"MLE",                 WC_MLE                  },
                {"AppStat",             WC_APPSTAT              },
                {"KbdStat",             WC_KBDSTAT              },
                {"Pecic",               WC_PECIC                },
                {"SpinButton",          WC_SPINBUTTON           },
                {"Container",           WC_CONTAINER            },
                {"Slider",              WC_SLIDER               },
                {"ValueSet",            WC_VALUESET             },
                {"NoteBook",            WC_NOTEBOOK             },
                {"CircularSlider",      WC_CIRCULARSLIDER       },
                {"GraphicButton",       WC_GRAPHICBUTTON        },
                {0,0}
        };

        int classNumber=atoi(className+1);
        for(pc *p=pcs; p->niceName; p++) {
                if((SHORT)(p->WC_name)==classNumber) {
                        printf("%s ",p->niceName);
                        return;
                }
        }
        printf("%s ",className);
}

static void dumpWindowInfo(HWND hwnd, int level) {
        indent(level);

        //handle
        printf("hwnd:%08lx ",(long)hwnd);

        if(dumpWindowClass) {
                //class name
                char className[128];
                WinQueryClassName(hwnd,128,className);
                if(className[0]=='#') {
                        dumpPredefinedClassName(className);
                } else
                        printf("%s ",className);
        }

        if(dumpWindowID) {
                //id
                printf("id:%04lx ",(long)WinQueryWindowUShort(hwnd,QWS_ID));
        }

        if(dumpWindowOwner) {
                printf("owner:%08lx ",(long)WinQueryWindow(hwnd,QW_OWNER));
        }

        if(dumpWindowStyle) {
                printf("style:%08lx ",(long)WinQueryWindowULong(hwnd,QWL_STYLE));
        }
        
        printf("\n");


        if(dumpWindowRect) {
                indent(level);
                //size&position
                RECTL wr;
                WinQueryWindowRect(hwnd,&wr);
                printf("%dx%d ",wr.xRight,wr.yTop);
                RECTL tmp;
                HWND hwndParent=WinQueryWindow(hwnd,QW_PARENT);
                if(hwndParent) {
                        tmp=wr;
                        WinMapWindowPoints(hwnd,hwndParent,(PPOINTL)&tmp,2);
                        printf("p%d,%d-%d,%d ",tmp.xLeft,tmp.yBottom,tmp.xRight,tmp.yTop);
                }
                tmp=wr;
                WinMapWindowPoints(hwnd,HWND_DESKTOP,(PPOINTL)&tmp,2);
                printf("d%d,%d-%d,%d ",tmp.xLeft,tmp.yBottom,tmp.xRight,tmp.yTop);
                printf("\n");
        }

        if(dumpWindowData) {
                indent(level);
                char className[128];
                WinQueryClassName(hwnd,128,className);
                CLASSINFO ci;
                printf("data: ");
                if(WinQueryClassInfo(hab,className,&ci)) {
                        printf("(%dbytes) ",(int)ci.cbWindowData);
                        for(int index=0; index<ci.cbWindowData*2; index+=2) {
                                USHORT us=WinQueryWindowUShort(hwnd,index);
                                printf("%04x ",(int)us);
                        }
                } else
                        printf("n/a");
                printf("\n");
        }
}

static void dumpWindow(HWND hwnd, int level) {
        dumpWindowInfo(hwnd,level);
        HWND hwndChild=WinQueryWindow(hwnd,QW_TOP);
        while(hwndChild) {
                dumpWindow(hwndChild,level+1);
                hwndChild=WinQueryWindow(hwndChild,QW_NEXT);
        }
}

int main(void) {
        hab=WinInitialize(0);
        if(!hab) {
                fprintf(stderr,"could not create hab\n");
                return -1;
        }
/*        HMQ hmq=WinCreateMsgQueue(hab,0);
        if(!hmq) {
                fprintf(stderr,"could not create msgqueue\n");
                WinTerminate(hab);
                return -1;
        }
*/
        if(dumpDesktopWindows) {
                printf("--- Desktop windows: ---\n");
                dumpWindow(WinQueryDesktopWindow(hab,0),0);
        }
        if(dumpObjectWindows) {
                printf("--- Desktop windows: ---\n");
                dumpWindow(WinQueryObjectWindow(HWND_DESKTOP),0);
        }

//        WinDestroyMsgQueue(hmq);
        WinTerminate(hab);
        return 0;
}
