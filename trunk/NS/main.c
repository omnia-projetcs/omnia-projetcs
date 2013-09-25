//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "resources.h"
//----------------------------------------------------------------
BOOL CALLBACK DlgMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    //--------------------------------------
    case WM_INITDIALOG:init(hwnd);break;
    //--------------------------------------
    case WM_CLOSE:
    {
      scan_start = FALSE;
      DeleteCriticalSection(&Sync);
      CloseHandle(h_log);
      FreeLibrary((HMODULE)hndlIcmp);
      EndDialog(hwnd,0);
    }
    break;
    //--------------------------------------
    case WM_COMMAND:
    {
      switch (HIWORD(wParam))
      {
        case BN_CLICKED:
          switch(LOWORD(wParam))
          {
            //------------------------------
            case CHK_NULL_SESSION:
              if (IsDlgButtonChecked(hwnd,LOWORD(wParam))==BST_CHECKED)
              {
                EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),FALSE);
              }else
              {
                EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),TRUE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),TRUE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),TRUE);
              }
            break;
            //------------------------------
            case CHK_LOAD_IP_FILE:
              if (IsDlgButtonChecked(hwnd,LOWORD(wParam))==BST_CHECKED)CreateThread(NULL,0,load_file_ip,0,0,0);
              else
              {
                EnableWindow(GetDlgItem(hwnd,GRP_PERIMETER),TRUE);
                EnableWindow(GetDlgItem(hwnd,IP1),TRUE);
                EnableWindow(GetDlgItem(hwnd,BT_IP_CP),TRUE);
                EnableWindow(GetDlgItem(hwnd,IP2),TRUE);
              }
            break;
            //------------------------------
            case BT_SAVE:
            {
              char file[MAX_PATH]= "";
              OPENFILENAME ofn;
              ZeroMemory(&ofn, sizeof(OPENFILENAME));
              ofn.lStructSize    = sizeof(OPENFILENAME);
              ofn.hwndOwner      = hwnd;
              ofn.lpstrFile      = file;
              ofn.nMaxFile       = MAX_PATH;
              ofn.nFilterIndex   = 1;
              ofn.Flags          = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
              ofn.lpstrFilter    = "*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0";
              ofn.lpstrDefExt    = ".csv\0";
              if (GetSaveFileName(&ofn)==TRUE)
              {
                if(SaveLSTV(GetDlgItem(hwnd,LV_results), file, ofn.nFilterIndex, NB_COLUMN)) AddMsg(hwnd, (char*)"INFORMATION",(char*)"Recorded data",file);
                else AddMsg(hwnd, (char*)"ERROR",(char*)"No data saved!",(char*)"");
              }
            }
            break;
            //------------------------------
            case BT_START:
              scan_start = !scan_start;
              if (scan_start)
              {
                EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),FALSE);
                EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),FALSE);
                EnableWindow(GetDlgItem(hwnd,CHK_NULL_SESSION),FALSE);
                EnableWindow(GetDlgItem(hwnd,GRP_PERIMETER),FALSE);
                EnableWindow(GetDlgItem(hwnd,IP1),FALSE);
                EnableWindow(GetDlgItem(hwnd,BT_IP_CP),FALSE);
                EnableWindow(GetDlgItem(hwnd,IP2),FALSE);
                EnableWindow(GetDlgItem(hwnd,CHK_LOAD_IP_FILE),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),FALSE);
                EnableWindow(GetDlgItem(hwnd,CB_tests),FALSE);
                ListView_DeleteAllItems(GetDlgItem(h_main,LV_results));

                SetWindowText(GetDlgItem(hwnd,BT_START),"Stop");
                AddMsg(hwnd, (char*)"INFORMATION",(char*)"Start scan",(char*)"");
                h_thread_scan = CreateThread(NULL,0,scan,0,0,0);
              }else EnableWindow(GetDlgItem(hwnd,BT_START),FALSE);
            break;
            //------------------------------
            case CHK_ALL_TEST:
            {
              //check all
              SendDlgItemMessage(hwnd,CB_tests,LB_SELITEMRANGEEX,(WPARAM)0,(LPARAM)NB_COLUMN);

              //uncheck only separator
              SendDlgItemMessage(hwnd,CB_tests,LB_SETSEL,(WPARAM)FALSE,(LPARAM)3);
            }
            break;
              //------------------------------
            case BT_IP_CP:
            {
              DWORD LIp1=0;
              SendMessage(GetDlgItem(hwnd,IP1),IPM_GETADDRESS, 0 ,(LPARAM) &LIp1);
              SendMessage(GetDlgItem(hwnd,IP2),IPM_SETADDRESS, 0 ,(LPARAM) MAKEIPADDRESS(LIp1 >> 24,(LIp1 >> 16) & 0xFF,(LIp1 >> 8) & 0xFF,254));
            }
            break;
          }
        break;
      }
    }
    break;
    //--------------------------------------
    case WM_NOTIFY:
      switch(((LPNMHDR)lParam)->code)
      {
        case LVN_COLUMNCLICK:
          if (!scan_start)
          {
            tri_order = !tri_order;
            c_Tri(GetDlgItem(hwnd,LV_results),((LPNMLISTVIEW)lParam)->iSubItem,tri_order);
          }
        break;
        case NM_DBLCLK:
        {
          char msg[MAX_LINE_SIZE]="",tmp[MAX_LINE_SIZE];
          long int index = SendDlgItemMessage(hwnd,LV_results,LVM_GETNEXTITEM,(WPARAM)-1,(LPARAM)LVNI_FOCUSED);
          if (index != -1)
          {
            tmp[0] = 0;
            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_IP,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,tmp,MAX_LINE_SIZE);

              tmp[0] = 0;
              ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_DNS,tmp,MAX_LINE_SIZE);
              if (tmp[0] != 0)
              {
                strncat(msg," ",MAX_LINE_SIZE);
                strncat(msg,tmp,MAX_LINE_SIZE);
                tmp[0] = 0;
              }

              ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_TTL,tmp,MAX_LINE_SIZE);
              if (tmp[0] != 0)
              {
                strncat(msg," ",MAX_LINE_SIZE);
                strncat(msg,tmp,MAX_LINE_SIZE);
                tmp[0] = 0;
              }
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_CONFIG,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Config]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_FILES,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[File]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_REG,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Registry]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SERVICE,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Services]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SOFTWARE,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[Softwares]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_USB,tmp,MAX_LINE_SIZE);
            if (tmp[0] != 0)
            {
              strncat(msg,"\r\n\r\n[USB]\r\n",MAX_LINE_SIZE);
              strncat(msg,tmp,MAX_LINE_SIZE);
              tmp[0] = 0;
            }
            strncat(msg,"\0",MAX_LINE_SIZE);
            if (strlen(msg))MessageBox(h_main,msg,"Global View",MB_OK|MB_TOPMOST);
          }
        }
        break;
      }
    break;
    //--------------------------------------
    case WM_SIZE:
    {
      unsigned int mWidth  = LOWORD(lParam);
      unsigned int mHeight = HIWORD(lParam);

      if (mWidth<800 ||mHeight<500)
      {
        RECT Rect;
        GetWindowRect(hwnd, &Rect);
        MoveWindow(hwnd,Rect.left,Rect.top,800+20,500+44,TRUE);
      }else
      {
        MoveWindow(GetDlgItem(hwnd,CB_tests)  ,0  ,75         ,200        ,mHeight-80 ,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_results),202,57         ,mWidth-204 ,mHeight-199,TRUE);
        MoveWindow(GetDlgItem(hwnd,CB_infos)  ,202,mHeight-140,mWidth-204 ,135        ,TRUE);
      }
      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hinst = hInstance;
    InitCommonControls();
    return DialogBox(hInstance, MAKEINTRESOURCE(DLG_NS), NULL, (DLGPROC)DlgMain);
}
