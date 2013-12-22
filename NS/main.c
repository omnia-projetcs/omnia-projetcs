//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/aa390422%28v=vs.85%29.aspx
#include "resources.h"
//------------------------------------------------------------------------------
//subclass of hdbclk_info
//------------------------------------------------------------------------------
LRESULT APIENTRY subclass_hdbclk_info(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CLOSE)ShowWindow (hwnd, SW_HIDE);
  else return CallWindowProc(wndproc_hdbclk_info, hwnd, uMsg, wParam, lParam);
  return 0;
}
//----------------------------------------------------------------
void init(HWND hwnd)
{
  h_main            = hwnd;
  scan_start        = FALSE;
  tri_order         = FALSE;
  config.nb_accounts= 0;

  SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));

  //edit for dblck view
  hdbclk_info = CreateWindowEx(0x200|WS_EX_CLIENTEDGE, WC_EDIT, "", 0x00E80844|WS_SIZEBOX|WS_MAXIMIZEBOX,
                         GetSystemMetrics(SM_CXSCREEN)/3, GetSystemMetrics(SM_CYSCREEN)/3,
                         GetSystemMetrics(SM_CXSCREEN)/3, GetSystemMetrics(SM_CYSCREEN)/3,
                         h_main, NULL, hinst, NULL);

  SendMessage(hdbclk_info, WM_SETFONT,(WPARAM)CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New"), TRUE);
  SendMessage(hdbclk_info, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));

#ifdef _WIN64_VERSION_
  wndproc_hdbclk_info = (WNDPROC)SetWindowLongPtr(hdbclk_info, GWL_WNDPROC,(LONG)subclass_hdbclk_info);
#else
  wndproc_hdbclk_info = (WNDPROC)SetWindowLong(hdbclk_info, GWL_WNDPROC,(LONG)subclass_hdbclk_info);
#endif

  CheckDlgButton(hwnd,CHK_NULL_SESSION,BST_CHECKED);
  EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),FALSE);
  EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),FALSE);
  EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),FALSE);

  //LV
  HWND hlv = GetDlgItem(hwnd,LV_results);
  LVCOLUMN lvc;
  lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
  lvc.fmt  = LVCFMT_LEFT;

  //IP + DNS
  lvc.cx = 110;
  lvc.pszText = (LPSTR)"IP";
  ListView_InsertColumn(hlv, COL_IP, &lvc);
  lvc.cx = 110;
  lvc.pszText = (LPSTR)"DNS";
  ListView_InsertColumn(hlv, COL_DNS, &lvc);
  lvc.cx = 90;
  lvc.pszText = (LPSTR)"TTL";
  ListView_InsertColumn(hlv, COL_TTL, &lvc);
  lvc.cx = 90;
  lvc.pszText = (LPSTR)"OS";
  ListView_InsertColumn(hlv, COL_OS, &lvc);

  lvc.cx = 150;
  lvc.pszText = (LPSTR)"Config";
  ListView_InsertColumn(hlv, COL_CONFIG, &lvc);
  lvc.cx = 80;
  lvc.pszText = (LPSTR)"Share";
  ListView_InsertColumn(hlv, COL_SHARE, &lvc);
  lvc.cx = 80;
  lvc.pszText = (LPSTR)"Policy";
  ListView_InsertColumn(hlv, COL_POLICY, &lvc);

  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Files";
  ListView_InsertColumn(hlv, COL_FILES, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Registry";
  ListView_InsertColumn(hlv, COL_REG, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Services";
  ListView_InsertColumn(hlv, COL_SERVICE, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"Software";
  ListView_InsertColumn(hlv, COL_SOFTWARE, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"USB";
  ListView_InsertColumn(hlv, COL_USB, &lvc);
  lvc.cx = 100;
  lvc.pszText = (LPSTR)"SSH";
  ListView_InsertColumn(hlv, COL_SSH, &lvc);
  //State
  lvc.cx = 60;
  lvc.pszText = (LPSTR)"State";
  ListView_InsertColumn(hlv, COL_STATE, &lvc);
  SendMessage(hlv,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

  //log file
  h_log = CreateFile("NS_log.txt", GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  SetFilePointer(h_log,0,0,FILE_END);
  DWORD copiee = 0;
  WriteFile(h_log,"----------------\r\n",18,&copiee,0);
  WriteFile(h_log,TITLE,strlen(TITLE),&copiee,0);
  WriteFile(h_log,"\r\n",2,&copiee,0);

  //insert test
  //SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:ARP");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:PING");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:DNS");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:NetBIOS");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:Accounts policy");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"------------------------------");
/*
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Services");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Users");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Software");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:USB");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Start");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:Revers SID");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CONFIG:RPC");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"------------------------------");*/

  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Files");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Registry");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Services");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Software");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:USB");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:SSH");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"------------------------------");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"WRITE:Registry Values");

  //critical section
  InitializeCriticalSection(&Sync);

  //load ICMP functions
  IcmpOk = FALSE;
  if ((hndlIcmp = LoadLibrary("ICMP.DLL"))!=0)
  {
    pIcmpCreateFile  = (HANDLE (WINAPI *)(void))GetProcAddress((HMODULE)hndlIcmp,"IcmpCreateFile");
    pIcmpCloseHandle = (BOOL (WINAPI *)(HANDLE))GetProcAddress((HMODULE)hndlIcmp,"IcmpCloseHandle");
    pIcmpSendEcho = (DWORD (WINAPI *)(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD))	GetProcAddress((HMODULE)hndlIcmp,"IcmpSendEcho");
    pIcmpSendEcho2 = (DWORD (WINAPI *)(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,IPAddr,LPVOID,WORD,PIP_OPTION_INFORMATION,LPVOID,DWORD,DWORD))	GetProcAddress((HMODULE)hndlIcmp,"IcmpSendEcho2");

    if (pIcmpCreateFile!=0 && pIcmpCloseHandle!=0 && pIcmpSendEcho!=0 && pIcmpSendEcho2)
      IcmpOk = TRUE;
  }
}
//------------------------------------------------------------------------------
unsigned long int Contient(char*data, char*chaine)
{
  unsigned long int i=0;
  char *d = data;
  char *c = chaine;

  if (!*c || !*d) return 0;

  while (*d)
  {
    c = chaine;
    while (*d == *c && *c && *d){d++;c++;i++;}

    if (*c == 0) return i;
    d++;i++;
  }
  return FALSE;
}
//----------------------------------------------------------------
char *charToLowChar(char *src)
{
  unsigned int i;
  for (i=0;i<strlen(src);i++)
  {
    if (src[i]>64 && src[i]<91)src[i] = src[i]+32;
  }
  return src;
}
//------------------------------------------------------------------------------
char *ConvertLinuxToWindows(char *src, DWORD max_size)
{
  char dst[max_size*2];
  DWORD i, j;
  DWORD src_size = strlen(src);
  DWORD dst_size_max = max_size*2;
  for (i=0,j=0;i<src_size && j<dst_size_max;i++)
  {
    switch(src[i])
    {
      case '\n':
        if (j+1 < dst_size_max && src[i+1] != '\n')
        {
          dst[j++] = '\r';
          dst[j++] = '\n';
        }
      break;
      //case '\t':dst[j++] = ' ';break;
      case '"':
        if (j+1 < dst_size_max)
        {
          dst[j++] = '\'';
          dst[j++] = '\'';
        }
      break;
      default:
        if (src[i] != 0 && src[i] > 31 && src[i] < 127)dst[j++] = src[i];
        else dst[j++] = ' ';
      break;
    }
  }
  dst[j] = 0;
  snprintf(src,max_size,"%s",dst);
  return src;
}
//----------------------------------------------------------------
int CALLBACK CompareStringTri(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
  SORT_ST *st = (SORT_ST *)lParam3;

  static char buf1[MAX_LINE_SIZE], buf2[MAX_LINE_SIZE];

  ListView_GetItemText(st->hlv, lParam1, st->col, buf1, MAX_LINE_SIZE);
  ListView_GetItemText(st->hlv, lParam2, st->col, buf2, MAX_LINE_SIZE);

  if (st->sort) return (strcmp(charToLowChar(buf1), charToLowChar(buf2)));
  else return (strcmp(charToLowChar(buf1), charToLowChar(buf2))*-1);
}
//----------------------------------------------------------------
void c_Tri(HWND hlv, unsigned short colonne_ref, BOOL sort)
{
  static SORT_ST st;
  st.hlv  = hlv;
  st.sort = sort;
  st.col  = colonne_ref;

  ListView_SortItemsEx(st.hlv,CompareStringTri, (LPARAM)&st);
}
//----------------------------------------------------------------
void AddMsg(HWND hwnd, char *type, char *txt, char *info)
{
  //read actual time
  time_t dateEtHMs;
  time(&dateEtHMs);
  struct tm *today = localtime(&dateEtHMs);

  char msg[MAX_MSG_SIZE],date[DATE_SIZE];
  strftime(date, DATE_SIZE,"%Y/%m/%d-%H:%M:%S",today);
  if (info != NULL) snprintf(msg,MAX_MSG_SIZE,"[%s] %s - %s %s",date,type,txt,info);
  else snprintf(msg,MAX_MSG_SIZE,"[%s] %s - %s",date,type,txt);

  SendDlgItemMessage(hwnd,CB_infos,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)msg);
  if (h_log != INVALID_HANDLE_VALUE)
  {
    DWORD copiee = 0;
    char msg2[MAX_MSG_SIZE];
    snprintf(msg2,MAX_MSG_SIZE,"%s\r\n", msg);
    WriteFile(h_log,msg,strlen(msg),&copiee,0);
  }
}
//------------------------------------------------------------------------------
void AddLSTVUpdateItem(char *add, DWORD column, DWORD iitem)
{
  HWND hlstv  = GetDlgItem(h_main,LV_results);
  char buffer[MAX_MSG_SIZE] = "", buffer2[MAX_MSG_SIZE]="";
  ListView_GetItemText(hlstv,iitem,column,buffer,MAX_MSG_SIZE);
  if (buffer[0] != 0)
  {
    snprintf(buffer2,MAX_MSG_SIZE,"%s\r\n%s",buffer,add);
    ListView_SetItemText(hlstv,iitem,column,buffer2);
  }else
  {
    ListView_SetItemText(hlstv,iitem,column,add);
  }
}
//----------------------------------------------------------------
DWORD AddLSTVItem(char *ip, char *dns, char *ttl, char *os, char *config, char *share, char*policy, char *files, char *registry, char *Services, char *software, char *USB, char *state)
{
  LVITEM lvi;
  HWND hlstv    = GetDlgItem(h_main,LV_results);
  lvi.mask      = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem  = 0;
  lvi.lParam    = LVM_SORTITEMS;
  lvi.pszText   = (LPSTR)"";
  EnterCriticalSection(&Sync);
  lvi.iItem     = ListView_GetItemCount(hlstv);
  DWORD itemPos = ListView_InsertItem(hlstv, &lvi);
  LeaveCriticalSection(&Sync);

  if(ip!=NULL)      ListView_SetItemText(hlstv,itemPos,COL_IP,ip);
  if(dns!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_DNS,dns);
  if(ttl!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_TTL,ttl);
  if(os!=NULL)      ListView_SetItemText(hlstv,itemPos,COL_OS,os);
  if(config!=NULL)  ListView_SetItemText(hlstv,itemPos,COL_CONFIG,config);
  if(share!=NULL)   ListView_SetItemText(hlstv,itemPos,COL_SHARE,config);
  if(policy!=NULL)  ListView_SetItemText(hlstv,itemPos,COL_POLICY,config);
  if(files!=NULL)   ListView_SetItemText(hlstv,itemPos,COL_FILES,files);
  if(registry!=NULL)ListView_SetItemText(hlstv,itemPos,COL_REG,registry);
  if(Services!=NULL)ListView_SetItemText(hlstv,itemPos,COL_SERVICE,Services);
  if(software!=NULL)ListView_SetItemText(hlstv,itemPos,COL_SOFTWARE,software);
  if(USB!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_USB,USB);
  if(state!=NULL)   ListView_SetItemText(hlstv,itemPos,COL_STATE,state);

  return itemPos;
}
//------------------------------------------------------------------------------
void replace_one_char(char *buffer, unsigned long int taille, char chtoreplace, char chreplace)
{
  char *c = buffer;

  while (c != buffer+taille)
  {
    if (*c == chtoreplace) *c = chreplace;
    c++;
  }
}
//------------------------------------------------------------------------------
BOOL SaveLSTV(HWND hlv, char *file, unsigned int type, unsigned int nb_column)
{
  //get item count
  unsigned long int nb_items = ListView_GetItemCount(hlv);
  if (nb_items > 0 && nb_column > 0)
  {
    //open file
    HANDLE hfile = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (hfile == INVALID_HANDLE_VALUE)
    {
      return FALSE;
    }

    char lines[MAX_MSG_SIZE_LINE]="", buffer[MAX_MSG_SIZE]="";
    DWORD copiee;
    unsigned long int i=0,j=0;

    LVCOLUMN lvc;
    lvc.mask        = LVCF_TEXT;
    lvc.cchTextMax  = MAX_MSG_SIZE;
    lvc.pszText     = buffer;

    switch(type)
    {
      case SAVE_TYPE_CSV:
        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"\"%s\";",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_MSG_SIZE;
          lvc.pszText = buffer;
        }
        //strncat(lines,"\r\n\0",MAX_MSG_SIZE);
        snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"\r\n");
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          lines[0]=0;
          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_MSG_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"\"%s\";",buffer);
            }else snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"\"\";");
          }
          snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
      break;
      case SAVE_TYPE_XML:
      {
        //char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<NS>\r\n <Description><![CDATA[NS report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<NS>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        LINE_ITEM lv_line[nb_column+1];

        //title line
        for (i=0;i<nb_column;i++)
        {
          lv_line[i].c[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_MSG_SIZE;
          lvc.pszText = lv_line[i].c;
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          replace_one_char(lv_line[i].c, strlen(lv_line[i].c), ' ', '_');
        }

        //save all line
        for (j=0;j<nb_items;j++)
        {
          WriteFile(hfile," <Data>\r\n",9,&copiee,0);
          for (i=0;i<nb_column;i++)
          {
            copiee = 0;
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_MSG_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines,MAX_MSG_SIZE_LINE,"  <%s><![CDATA[%s]]></%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
              WriteFile(hfile,lines,strlen(lines),&copiee,0);
            }
          }
          WriteFile(hfile," </Data>\r\n",10,&copiee,0);
        }
        WriteFile(hfile,"</NS>",5,&copiee,0);
      }
      break;
      case SAVE_TYPE_HTML:
      {
        char head[]="<html>\r\n <head><title>NS report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n <table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n  <tr bgcolor=\"#CCCCCC\">\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"  <th>%s</th>",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_MSG_SIZE;
          lvc.pszText = buffer;

        }
        snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"\r\n  </tr>\r\n");
        //strncat(lines,"\r\n  </tr>\r\n\0",MAX_MSG_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          if (j%2==1)strcpy(lines,"  <tr bgcolor=\"#ddddff\">");
          else strcpy(lines,"  <tr>");

          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_MSG_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"<td>%s</td>",buffer);
            }else snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"<td></td>");
          }
          snprintf(lines+strlen(lines),MAX_MSG_SIZE_LINE-strlen(lines),"</tr>\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
        WriteFile(hfile," </table>\r\n</html>",17,&copiee,0);
      }
      break;
    }
    CloseHandle(hfile);
    return TRUE;
  }else return FALSE;
}
//------------------------------------------------------------------------------
void addIPInterval(char *ip_src, char *ip_dst)
{
  //load IP interval
  char IpAdd[IP_SIZE];
  BYTE L11,L12,L13,L14,L21,L22,L23,L24;

  //get ip1
  char tmp[5]="\0\0\0\0";
  char *d = tmp;
  char *s = ip_src;

  while (*s && *s!='.')*d++ = *s++;
  if (*s == '.')
  {
    *d = 0;
    L11 = atoi(tmp);
    s++;

    d = tmp;
    while (*s && *s!='.')*d++ = *s++;
    if (*s == '.')
    {
      *d = 0;
      L12 = atoi(tmp);
      s++;

      d = tmp;
      while (*s && *s!='.')*d++ = *s++;
      if (*s == '.')
      {
        *d = 0;
        L13 = atoi(tmp);
        s++;

        d = tmp;
        while (*s && *s!='.')*d++ = *s++;
        if (*s == 0)
        {
          *d = 0;
          L14 = atoi(tmp);
        }else return;
      }else return;
    }else return;
  }else return;

  //get ip2
  d = tmp;
  s = ip_dst;

  while (*s && *s!='.')*d++ = *s++;
  if (*s == '.')
  {
    *d = 0;
    L21 = atoi(tmp);
    s++;

    d = tmp;
    while (*s && *s!='.')*d++ = *s++;
    if (*s == '.')
    {
      *d = 0;
      L22 = atoi(tmp);
      s++;

      d = tmp;
      while (*s && *s!='.')*d++ = *s++;
      if (*s == '.')
      {
        *d = 0;
        L23 = atoi(tmp);
        s++;

        d = tmp;
        while (*s && *s!='.')*d++ = *s++;
        if (*s == 0)
        {
          *d = 0;
          L24 = atoi(tmp);
        }else return;
      }else return;
    }else return;
  }else return;

  if ((L11+L12+L13+L14)!=0 && L14 >0 && L14 <= 255 && L13 <=255 && L12 <=255 && L11<255 || (L21+L22+L23+L24)!=0 && L24 >0 && L24 <= 255 && L23 <=255 && L22 <=255 && L21<255)
  {
    if (((L11+L12+L13+L14) == 0 && L21>0) || !strcmp(ip_src,ip_dst))SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_src);
    else if ((L21+L22+L23+L24) == 0 && L14 >0 && L14 <=255 && L13 <=255 && L12 <=255 && L11<255)SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_dst);
    else if (L21 >= L11 && (L22 >= L12 || L21 > L11) && (L23 >= L13 || (L22 > L12 || L21 > L11)) && (L24 >= L14 || (L23 > L13 || (L22 > L12 || L21 > L11))))
    {
      int a,b,c,d, initb,initc,initd,finb,finc,find;
      if (L14 == 0)L14 = 1;

      for (a = L11; a<=L21;a++)
      {
        if (a == L11 && a == L21)
        {
          initb = L12;
          finb = L22;
        }else if (a == L11)
        {
          initb = L12;
          finb = 255;
        }else if (a == L21)
        {
          initb = 0;
          finb = L22;
        }else
        {
          initb = 0;
          finb = 255;
        }
        for (b = initb; b<=finb;b++ )
        {
          if (b == L12 && b == L22)
          {
            initc = L13;
            finc = L23;
          }else if (b == L12)
          {
            initc = L13;
            finc = 255;
          }else if (b == L22)
          {
            initc = 0;
            finc = L23;
          }else
          {
            initc = 0;
            finc = 255;
          }

          for (c = initc; c<=finc;c++)
          {
            if (c == L13 && c == L23)
            {
              initd = L14;
              find = L24;
            }else if (c == L13)
            {
              initd = L14;
              find = 255;
            }else if (c == L23)
            {
              initd = 0;
              find = L24;
            }else
            {
              initd = 0;
              find = 255;
            }

            for (d = initd; d<=find;d++)
            {
              snprintf(IpAdd,IP_SIZE,"%d.%d.%d.%d",a,b,c,d);
              SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)IpAdd);
            }
          }
        }
      }
    }else
    {
      char msg[MAX_PATH];
      snprintf(msg,MAX_PATH,"%s->%s",ip_src,ip_dst);
      AddMsg(h_main,(char*)"ERROR",(char*)"Invalid interval",msg);
    }
  }else
  {
    char msg[MAX_PATH];
    snprintf(msg,MAX_PATH,"%s->%s",ip_src,ip_dst);
    AddMsg(h_main,(char*)"ERROR",(char*)"Invalid interval",msg);
  }
}
//------------------------------------------------------------------------------
BOOL verifieName(char *name)
{
  char *n = name;
  while (*n && *n != 47 &&((*n > 44 && *n < 58) || (*n > 64 && *n < 91) || (*n > 96 && *n < 123)) )n++;

  if (*n == 0) return TRUE;
  else return FALSE;
}
//------------------------------------------------------------------------------
void addIPTest(char *ip_format)
{
  if (ip_format == NULL) return;
  unsigned int size = strlen(ip_format);
  if (size < 2)return;

  //check if name or ip
  if (ip_format[0]> '9' || ip_format[0]< '0' || ((ip_format[1]> '9' || ip_format[1]< '0') && ip_format[1] != '.'))
  {
    if (verifieName(ip_format))
      SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_format);

    return;
  }

  //search if '-'
  char *c = ip_format;
  while (*c && (*c !='-') && (*c != '/'))c++;
  if (*c == 0)//ip
  {
    SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_format);
  }else if (*c == '-')//ip-ip
  {
    char ip1[MAX_PATH];
    char *i1 = ip1;
    char *s = ip_format;
    while (*s && (*s!='-'))*i1++ = *s++;
    if (*s == '-')
    {
      s++;
      *i1 = 0;
      addIPInterval(ip1, s);
    }
  }else if (*c == '/')//ip/24
  {
    char ip1[MAX_PATH],ip2[MAX_PATH];
    char *i1 = ip1;
    char *s = ip_format;
    while (*s && (*s!='/'))*i1++ = *s++;
    if (*s == '/')
    {
      s++;
      *i1 = 0;

      //get interval type : /24 -> /0
      DWORD ip_tmp, interval  = pow(2, 32-atoi(s))-2;
      BYTE L11,L12,L13,L14;
      char tmp[5]="\0\0\0\0";

      char *d = tmp;
      char *s = ip1;
      while (*s && *s!='.')*d++ = *s++;
      if (*s == '.')
      {
        *d = 0;
        L11 = atoi(tmp);
        s++;

        d = tmp;
        while (*s && *s!='.')*d++ = *s++;
        if (*s == '.')
        {
          *d = 0;
          L12 = atoi(tmp);
          s++;

          d = tmp;
          while (*s && *s!='.')*d++ = *s++;
          if (*s == '.')
          {
            *d = 0;
            L13 = atoi(tmp);
            s++;

            d = tmp;
            while (*s && *s!='.')*d++ = *s++;
            if (*s == 0)
            {
              *d = 0;
              L14 = atoi(tmp);
            }else return;
          }else return;
        }else return;
      }else return;

      ip_tmp = interval + (L11 << 24) + (L12 << 16) + (L13 << 8) + (L14);
      L11 = ip_tmp >> 24;
      L12 = (ip_tmp >> 16) & 0xFF;
      L13 = (ip_tmp >> 8) & 0xFF;
      L14 = ip_tmp & 0xFF;

      snprintf(ip2,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      addIPInterval(ip1, ip2);
    }
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI load_file_ip(LPVOID lParam)
{
  //disable GUI
  EnableWindow(GetDlgItem(h_main,BT_START),FALSE);
  EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),FALSE);
  EnableWindow(GetDlgItem(h_main,IP1),FALSE);
  EnableWindow(GetDlgItem(h_main,BT_IP_CP),FALSE);
  EnableWindow(GetDlgItem(h_main,IP2),FALSE);
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),FALSE);

  //init IP list
  SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

  //load file
  char file[LINE_SIZE]="";
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize   = sizeof(OPENFILENAME);
  ofn.hwndOwner     = h_main;
  ofn.lpstrFile     = file;
  ofn.nMaxFile      = LINE_SIZE;
  ofn.lpstrFilter   = "*.txt \0*.txt\0*.* \0*.*\0";
  ofn.nFilterIndex  = 1;
  ofn.Flags         = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER|OFN_SHOWHELP;
  ofn.lpstrDefExt   = "*.*";
  if (GetOpenFileName(&ofn)==TRUE)
  {
    //load file
    HANDLE hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
    if (hfile != INVALID_HANDLE_VALUE)
    {
      DWORD size      = GetFileSize(hfile,NULL);
      char *buffer    = (char *) malloc(size+1);
      if (buffer != NULL)
      {
        DWORD copiee =0;
        ReadFile(hfile, buffer, size,&copiee,0);
        if (size != copiee)AddMsg(h_main, (char*)"ERROR",(char*)"In loading file",file);

        //line by line
        char tmp[MAX_PATH];
        char *s = buffer, *d = tmp;
        while (*s)
        {
          tmp[0] = 0;
          d      = tmp;
          while(*s /*&& (d-tmp < MAX_PATH)*/ && (*s != '\r') && (*s != '\n'))*d++ = *s++;
          while(*s && ((*s == '\n') || (*s == '\r')))s++;
          *d = 0;

          if (tmp[0] != 0)
          {
            addIPTest(tmp);
          }
        }

        snprintf(tmp,LINE_SIZE,"Loaded file with %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
        AddMsg(h_main,(char*)"INFORMATION",tmp,file);
        free(buffer);
      }
    }
    CloseHandle(hfile);
  }
  //reinit GUI
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI load_file_accounts(LPVOID lParam)
{
  //disable GUI
  EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),FALSE);
  EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),FALSE);
  EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),FALSE);
  EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),FALSE);
  EnableWindow(GetDlgItem(h_main,BT_LOAD_MDP_FILES),FALSE);

  //"domain";"login";"mdp";\n or \r\n
  config.nb_accounts = 0;

  //load file
  char file[LINE_SIZE]="";
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize   = sizeof(OPENFILENAME);
  ofn.hwndOwner     = h_main;
  ofn.lpstrFile     = file;
  ofn.nMaxFile      = LINE_SIZE;
  ofn.lpstrFilter   = "*.csv \0*.csv\0*.* \0*.*\0";
  ofn.nFilterIndex  = 1;
  ofn.Flags         = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER|OFN_SHOWHELP;
  ofn.lpstrDefExt   = "*.*";
  if (GetOpenFileName(&ofn)==TRUE)
  {
    //load file
    HANDLE hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
    if (hfile != INVALID_HANDLE_VALUE)
    {
      DWORD size      = GetFileSize(hfile,NULL);
      char *buffer    = (char *) malloc(size+1);
      if (buffer != NULL)
      {
        DWORD copiee =0;
        ReadFile(hfile, buffer, size,&copiee,0);
        if (size != copiee)AddMsg(h_main, (char*)"ERROR",(char*)"In loading file",file);

        //line by line
        char tmp[MAX_PATH],bf[MAX_PATH];
        char *s = buffer, *d = tmp, *c;
        while (*s)
        {
          tmp[0] = 0;
          d      = tmp;
          while(*s /*&& (d-tmp < MAX_PATH)*/ && (*s != '\r') && (*s != '\n'))*d++ = *s++;
          while(*s && ((*s == '\n') || (*s == '\r')))s++;
          *d = 0;

          if (tmp[0] != 0)
          {
            //check if a good line
            d = tmp;
            if (*d == '"')
            {
              //get domain
              d++; //pass '"'
              bf[0] = 0;
              c = bf;
              while(*d && (*d != '"'))*c++ = *d++;
              *c = 0;
              strncpy(config.accounts[config.nb_accounts].domain,bf,MAX_PATH);

              if (*(d+2) == '"')
              {
                //get login
                d = d+3; //pass '";"'
                bf[0] = 0;
                c = bf;
                while(*d && (*d != '"'))*c++ = *d++;
                *c = 0;
                strncpy(config.accounts[config.nb_accounts].login,bf,MAX_PATH);

                if (*(d+2) == '"')
                {
                  //get password
                  d = d+3; //pass '";"'
                  bf[0] = 0;
                  c = bf;
                  while(*d && (*d != '"'))*c++ = *d++;
                  *c = 0;
                  strncpy(config.accounts[config.nb_accounts].mdp,bf,MAX_PATH);

                  if (config.accounts[config.nb_accounts].domain[0] != 0)
                    snprintf(bf,MAX_PATH,"%s\\%s",config.accounts[config.nb_accounts].domain,config.accounts[config.nb_accounts].login);
                  else
                    snprintf(bf,MAX_PATH,"(local account)\\%s",config.accounts[config.nb_accounts].login);

                  AddMsg(h_main, (char*)"INFORMATION",(char*)"Loading account",bf);

                  //next
                  if (config.nb_accounts+1 == MAX_ACCOUNTS)break;
                  else config.nb_accounts++;
                }
              }
            }
          }
        }

        snprintf(tmp,LINE_SIZE,"Loaded file with %lu accounts",config.nb_accounts);
        AddMsg(h_main,(char*)"INFORMATION",tmp,file);
        free(buffer);
      }
    }
    CloseHandle(hfile);
  }
  //reinit GUI
  EnableWindow(GetDlgItem(h_main,BT_LOAD_MDP_FILES),TRUE);

  if (config.nb_accounts == 0)
  {
    CheckDlgButton(h_main,BT_LOAD_MDP_FILES,BST_UNCHECKED);
    if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)==BST_CHECKED)
    {
      EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),TRUE);
    }else
    {
      EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),TRUE);
      EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),TRUE);
      EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),TRUE);
      EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),TRUE);
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
DWORD load_file_list(DWORD lsb, char *file)
{
  //init IP list
  SendDlgItemMessage(h_main,lsb,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

  //load file with on item by line to lstv
  HANDLE hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    DWORD size      = GetFileSize(hfile,NULL);
    char *buffer    = (char *) malloc(size+1);
    if (buffer != NULL)
    {
      DWORD copiee =0;
      memset(buffer,0,size+1);
      ReadFile(hfile, buffer, size,&copiee,0);
      if (size != copiee)AddMsg(h_main, (char*)"ERROR",(char*)"In loading file",file);

      //line by line
      char tmp[MAX_PATH];
      char *s = buffer, *d = tmp;
      while (*s)
      {
        tmp[0] = 0;
        d      = tmp;
        while(*s && (*s != '\r') && (*s != '\n'))*d++ = *s++;
        while(*s && ((*s == '\n') || (*s == '\r')))s++;
        *d = 0;

        if (tmp[0] != 0 && tmp[0] != '#')
        {
          if (tmp[0] == '\\' || tmp[0] == '/')
          {
            d = tmp;
            d++;
            SendDlgItemMessage(h_main,lsb,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)d);
            tmp[0] = 0;
          }else
          {
            SendDlgItemMessage(h_main,lsb,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)tmp);
            tmp[0] = 0;
          }
        }
      }

      //message
      snprintf(tmp,LINE_SIZE,"Loaded file with %lu item(s)",SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
      AddMsg(h_main,(char*)"INFORMATION",tmp,file);
      free(buffer);
    }
    CloseHandle(hfile);
    return SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL LSBExist(DWORD lsb, char *sst)
{
  char buffer[LINE_SIZE];
  if (sst == NULL) return FALSE;
  if (sst[0] == 0) return FALSE;
  DWORD i, _nb_i = SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  for (i=0;i<_nb_i;i++)
  {
    if (SendDlgItemMessage(h_main,lsb,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) < LINE_SIZE)
    {
      if (SendDlgItemMessage(h_main,lsb,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
      {
        if (!strcmp(charToLowChar(buffer),charToLowChar(sst)))return TRUE;
        //if (!strcmp(buffer,sst))return TRUE;
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------
HANDLE NetConnexionAuthenticateTest(char *ip, char*remote_name, SCANNE_ST config, DWORD iitem)
{
  HANDLE htoken = NULL;
  char msg[LINE_SIZE];
  char user_netbios[LINE_SIZE] = "";
  if (LogonUser((LPTSTR)"NETWORK SERVICE", (LPTSTR)"NT AUTHORITY", NULL, /*LOGON32_LOGON_NEW_CREDENTIALS*/9, /*LOGON32_PROVIDER_WINNT50*/3, &htoken))
  {
    if (htoken != 0)
    {
      ImpersonateLoggedOnUser(htoken);
      if (htoken != 0)
      {
        if (config.nb_accounts == 0)
        {
          if (config.domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config.domain,config.login);
          else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config.login);

          snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
          NETRESOURCE NetRes;
          NetRes.dwScope      = RESOURCE_GLOBALNET;
          NetRes.dwType	      = RESOURCETYPE_ANY;
          NetRes.lpLocalName  = (LPSTR)"";
          NetRes.lpProvider   = (LPSTR)"";
          NetRes.lpRemoteName	= remote_name;
          if (WNetAddConnection2(&NetRes,config.mdp,user_netbios,CONNECT_PROMPT)==NO_ERROR)
          {
            snprintf(msg,LINE_SIZE,"Login (ScanReg:NET) in %s\\IPC$ with %s account.",ip,user_netbios);
            AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
            AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          }else
          {
            CloseHandle(htoken);
            htoken = FALSE;
            #ifdef DEBUG_MODE
            AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
            #endif
          }
        }else
        {
          unsigned int i;
          for (i=0; i<config.nb_accounts ;i++)
          {
            if (config.accounts[i].domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config.accounts[i].domain,config.accounts[i].login);
            else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config.accounts[i].login);

            snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
            NETRESOURCE NetRes;
            NetRes.dwScope      = RESOURCE_GLOBALNET;
            NetRes.dwType	      = RESOURCETYPE_ANY;
            NetRes.lpLocalName  = (LPSTR)"";
            NetRes.lpProvider   = (LPSTR)"";
            NetRes.lpRemoteName	= remote_name;
            if (WNetAddConnection2(&NetRes,config.accounts[i].mdp,user_netbios,CONNECT_PROMPT)==NO_ERROR)
            {
              snprintf(msg,LINE_SIZE,"Login (ScanReg:NET) in %s IP with %s (%02d) account.",ip,user_netbios,i);

              AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
              AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
              return htoken;
            }else
            {
              CloseHandle(htoken);
              htoken = FALSE;
              #ifdef DEBUG_MODE
              AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
              #endif
            }
          }
        }
      }
    }
  }
  return htoken;
}
//----------------------------------------------------------------
DWORD WINAPI ScanIp(LPVOID lParam)
{
  DWORD index = (DWORD)lParam;
  char ip[MAX_PATH]="", ip_mac[MAX_PATH]="", dns[MAX_PATH]="", ttl_s[MAX_PATH]="", os_s[MAX_PATH]="",cfg[MAX_LINE_SIZE]="",test_title[MAX_PATH];
  long long int iitem = -1;
  int ttl = -1;
  BOOL exist  = FALSE, dnsok = FALSE, netBIOS = FALSE;
  BOOL windows_OS = FALSE;

  if (SendDlgItemMessage(h_main, CB_IP, LB_GETTEXTLEN, (WPARAM)index,(LPARAM)NULL) > MAX_PATH)
  {
    ReleaseSemaphore(hs_threads,1,NULL);

    //tracking
    if (scan_start)
    {
      //#endif
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","LB_GETTEXTLEN ERROR","");
      #endif

      EnterCriticalSection(&Sync);
      snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,++nb_test_ip,nb_i);
      LeaveCriticalSection(&Sync);
      SetWindowText(h_main,test_title);
    }
    return 0;
  }
  SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)index,(LPARAM)ip);

  if (ip[0]!=0)
  {
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_threads",ip);
    AddMsg(h_main,"DEBUG","SCAN:BEGIN",ip);
    #endif
    //check if exist + NetBIOS
    if (config.disco_icmp||config.disco_dns||config.disco_netbios)
    {
      WaitForSingleObject(hs_disco,INFINITE);
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_disco",ip);
      #endif

      if (ip[0]> '9' || ip[0]< '0' || ((ip[1]> '9' || ip[1]< '0') && ip[1] != '.'))
      {
        //resolution inverse
        strncpy(dns,ip,MAX_PATH);

        struct in_addr **a;
        struct hostent *host;

        if (host=gethostbyname(ip))
        {
          a = (struct in_addr **)host->h_addr_list;
          snprintf(ip,16,"%s",inet_ntoa(**a));
          exist = TRUE;
          dnsok = TRUE;

          iitem = AddLSTVItem(ip, dns, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }else
        {
          iitem = AddLSTVItem((char*)"[ERROR DNS]", ip, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (char*)"OK");
          ReleaseSemaphore(hs_disco,1,NULL);
          ReleaseSemaphore(hs_threads,1,NULL);

          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_disco",ip);
          AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_threads",ip);
          #endif

          //tracking
          EnterCriticalSection(&Sync);
          snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,++nb_test_ip,nb_i);
          LeaveCriticalSection(&Sync);
          SetWindowText(h_main,test_title);
          return 0;
        }
      }

      //ICMP
      if (config.disco_icmp && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ICMP:BEGIN",ip);
        #endif
        ttl = Ping(ip);
        if (ttl > -1)
        {
          snprintf(ttl_s,MAX_PATH,"TTL:%d",ttl);

          if (!exist)
          {
            if (ttl <= MACH_LINUX)iitem = AddLSTVItem(ip, NULL, ttl_s, (char*)"Linux",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            else if (ttl <= MACH_WINDOWS)iitem = AddLSTVItem(ip, NULL, ttl_s, (char*)"Windows",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            else if (ttl <= MACH_WINDOWS)iitem = AddLSTVItem(ip, NULL, ttl_s, (char*)"Router",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            exist = TRUE;
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_TTL,ttl_s);

            if (ttl <= MACH_LINUX)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,(LPSTR)"Linux")
            else if (ttl <= MACH_WINDOWS)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,(LPSTR)"Windows")
            else if (ttl <= MACH_WINDOWS)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,(LPSTR)"Router")
          }
        }
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ICMP:END",ip);
        #endif
      }

      //DNS
      if (config.disco_dns && scan_start && dns[0] == 0)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","DNS:BEGIN",ip);
        #endif
        if(exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"DNS");
        if(ResDNS(ip, dns, MAX_PATH))
        {
          if (!exist)
          {
            iitem = AddLSTVItem(ip, dns, NULL, (char*)"Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            exist = TRUE;
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_DNS,dns);
          }
          dnsok = TRUE;
        }
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","DNS:END",ip);
        #endif
      }
      ReleaseSemaphore(hs_disco,1,NULL);

      //NetBIOS
      if (exist && (iitem > -1) && (dnsok || !config.disco_dns) && config.disco_netbios && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","NetBIOS:BEGIN",ip);
        #endif
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"NetBIOS");
        WaitForSingleObject(hs_netbios,INFINITE);

        char domain[MAX_PATH] = "";
        char os[MAX_PATH]     = "";

        if (scan_start)
        {
          if (dns[0] == 0) windows_OS = Netbios_OS(ip, os, dns, domain, MAX_PATH);
          else windows_OS = Netbios_OS(ip, os, NULL, domain, MAX_PATH);

          if (windows_OS)nb_windows++;
        }

        if (os[0] != 0)
        {
          if (ttl > -1)snprintf(ttl_s,MAX_PATH,"TTL:%d",ttl);
          else snprintf(os,MAX_PATH,"Firewall");

          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_TTL,ttl_s);
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,os);
          netBIOS = TRUE;
        }

        char tmp[MAX_LINE_SIZE] = "";
        if (domain[0] != 0)
        {
          //snprintf(tmp,MAX_LINE_SIZE,"Domain: %s\r\n\0",domain);

          snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Domain: %s\r\n",domain);

          //strncat(cfg,tmp,MAX_LINE_SIZE);
          netBIOS = TRUE;
        }

        //NULL session
        if (scan_start)
        {
          if(Netbios_NULLSession(ip))
          {
            snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"NULL Session: Enable\r\n");
            //strncat(cfg,"NULL Session: Enable\r\n\0",MAX_LINE_SIZE);
            //ReversSID (only administrator + guest + defaults account test)
            /*if(TestReversSID(ip,(char*)"invité"))             strncat(cfg,(char*)"Revers SID: Enable (OK with \"invité\" account)\r\n\0",MAX_LINE_SIZE);
            else if(TestReversSID(ip,(char*)"guest"))         strncat(cfg,(char*)"Revers SID: Enable (OK with \"guest\" account)\r\n\0",MAX_LINE_SIZE);
            else if(TestReversSID(ip,(char*)"gast"))          strncat(cfg,(char*)"Revers SID: Enable (OK with \"gast\" account)\r\n\0",MAX_LINE_SIZE);
            else if(TestReversSID(ip,(char*)"invitado"))      strncat(cfg,(char*)"Revers SID: Enable (OK with \"invitado\" account)\r\n\0",MAX_LINE_SIZE);
            else if(TestReversSID(ip,(char*)"HelpAssistant")) strncat(cfg,(char*)"Revers SID: Enable (OK with \"HelpAssistant\" account)\r\n\0",MAX_LINE_SIZE);
            else if(TestReversSID(ip,(char*)"ASPNET"))        strncat(cfg,(char*)"Revers SID: Enable (OK with \"ASPNET\" account)\r\n\0",MAX_LINE_SIZE);
            else if(TestReversSID(ip,(char*)"administrateur"))strncat(cfg,(char*)"Revers SID: Enable (OK with \"administrateur\" account)\r\n\0",MAX_LINE_SIZE);
            else if(TestReversSID(ip,(char*)"administrator")) strncat(cfg,(char*)"Revers SID: Enable (OK with \"administrator\" account)\r\n\0",MAX_LINE_SIZE);  */

            if(TestReversSID(ip,(char*)"invité"))             snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"invité\" account)\r\n");
            else if(TestReversSID(ip,(char*)"guest"))         snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"guest\" account)\r\n");
            else if(TestReversSID(ip,(char*)"gast"))          snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"gast\" account)\r\n");
            else if(TestReversSID(ip,(char*)"invitado"))      snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"invitado\" account)\r\n");
            else if(TestReversSID(ip,(char*)"HelpAssistant")) snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"HelpAssistant\" account)\r\n");
            else if(TestReversSID(ip,(char*)"ASPNET"))        snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"ASPNET\" account)\r\n");
            else if(TestReversSID(ip,(char*)"administrateur"))snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"administrateur\" account)\r\n");
            else if(TestReversSID(ip,(char*)"administrator")) snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Revers SID: Enable (OK with \"administrator\" account)\r\n");
            netBIOS     = TRUE;

            if (scan_start)
            {
              char c_time[MAX_PATH]="";
              wchar_t server[MAX_PATH];
              snprintf(tmp,MAX_PATH,"\\\\%s",ip);
              mbstowcs(server, tmp, MAX_PATH);
              Netbios_Time(server, c_time, MAX_PATH);
              if (c_time[0] != 0)
              {
                //snprintf(tmp,MAX_PATH,"Time: %s\r\n\0",c_time);
                //strncat(cfg,tmp,MAX_LINE_SIZE);

                snprintf(cfg+strlen(cfg),MAX_LINE_SIZE-strlen(cfg),"Time: %s\r\n",c_time);
                netBIOS = TRUE;
              }

              //Share
              if (scan_start)
              {
                char shares[MAX_LINE_SIZE+1]="";
                Netbios_Share(server, shares, MAX_LINE_SIZE);
                if (shares[0] != 0)
                {
                  AddLSTVUpdateItem(shares, COL_SHARE, iitem);
                  netBIOS = TRUE;
                }
              }
            }
          }
        }

        if (cfg[0] != 0)
        {
          AddLSTVUpdateItem(cfg, COL_CONFIG, iitem);
        }
        ReleaseSemaphore(hs_netbios,1,NULL);

        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_netbios",ip);
        AddMsg(h_main,"DEBUG","NetBIOS:END",ip);
        #endif
      }

      if(((!config.disco_netbios)||(config.disco_netbios && netBIOS) || (ttl > 64) || (dnsok && ttl < 1)) && scan_start && exist)
      {
        //registry
        BOOL regisyty_remote = FALSE;
        if (config.check_registry || config.check_services || config.check_software || config.check_USB || config.write_key || config.disco_netbios_policy)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:BEGIN",ip);
          #endif
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Registry");
          WaitForSingleObject(hs_registry,INFINITE);
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_registry",ip);
          #endif
          regisyty_remote = RemoteConnexionScan(iitem, dns, ip, config,windows_OS);
          if (regisyty_remote)nb_registry++;
          ReleaseSemaphore(hs_registry,1,NULL);
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_registry",ip);
          AddMsg(h_main,"DEBUG","registry:END",ip);
          #endif
        }

        //files
        BOOL file_remote = FALSE;
        if (config.check_files && scan_start)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","files:BEGIN",ip);
          #endif

          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files");
          WaitForSingleObject(hs_file,INFINITE);
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_file",ip);
          #endif

          file_remote = RemoteConnexionFilesScan(iitem, dns, ip, config);
          ReleaseSemaphore(hs_file,1,NULL);
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_file",ip);
          AddMsg(h_main,"DEBUG","files:END",ip);
          #endif
        }

        /*if ((!file_remote || !regisyty_remote) && dnsok)
        {
          AddMsg(h_main,(char*)"DEBUG",(char*)"WMI - start",(char*)ip);
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"WMI");
          GetWMITests(iitem, ip, dns, config);
        }*/
      }else if(exist)
      {
        if (config.check_files)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_FILES, iitem);
        if (config.check_registry)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_REG, iitem);
        if (config.check_services)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_SERVICE, iitem);
        if (config.check_software)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_SOFTWARE, iitem);
        if (config.check_USB)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_USB, iitem);
      }

      //SSH
      if(exist && (config.check_ssh || config.check_ssh_os) && scan_start)
      {
        char tmp_os[MAX_MSG_SIZE]="";
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"SSH");

        //check if the port is open

        if (TCP_port_open(iitem, ip, SSH_DEFAULT_PORT, FALSE))
        {
          WaitForSingleObject(hs_ssh,INFINITE);
          if (config.nb_accounts == 0)
          {
            int ret_ssh = ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.login, config.mdp, -1,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,TRUE,TRUE);
            if (ret_ssh == SSH_ERROR_OK)
            {
              if (tmp_os[0] != 0)
              {
                ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                if (config.check_ssh)ssh_exec(iitem,ip, SSH_DEFAULT_PORT, config.login, config.mdp);
              }else  if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.login, config.mdp, -1,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE,FALSE) == SSH_ERROR_OK)
              {
                if (tmp_os[0] != 0)
                {
                  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                  if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.login, config.mdp);
                }
              }
            }else
            {
              #ifdef DEBUG_MODE_SSH
              char msg[MAX_LINE_SIZE];
              snprintf(msg,MAX_LINE_SIZE,"%s:22 account %s (%d)",ip,config.login,ret_ssh);
              switch(ret_ssh)
              {
                case SSH_ERROR_CHANNEL_EXEC:  AddMsg(h_main,(char*)"ERROR (SSH)",msg,"Unable to execute commands!");break;
                case SSH_ERROR_CHANNEL:       AddMsg(h_main,(char*)"ERROR (SSH)",msg,"Unable to open a channel command execution!");break;
                case SSH_ERROR_AUTHENT:       AddMsg(h_main,(char*)"ERROR (SSH)",msg,"Wrong account for authentication!");break;
                case SSH_ERROR_SESSIONSTART:  AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to initialize SSH session!");break;
                case SSH_ERROR_SESSIONINIT:   AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to initialize SSH session variables!");break;
                case SSH_ERROR_CONNECT:       AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unabe to connect on port 22!");break;
                case SSH_ERROR_SOCKET:        AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to create socket!");break;
                case SSH_ERROR_LIBINIT:       AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to initialize the SSH library!");break;
              }
              #endif
            }
          }else
          {
            DWORD j = 0;
            int ret_ssh = 0;
            BOOL first_msg = TRUE;
            BOOL msg_auth  = TRUE;
            char msg[MAX_LINE_SIZE];
            for (j=0;j<config.nb_accounts;j++)
            {
              //OS rescue
              tmp_os[0] = 0;
              ret_ssh = ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].mdp, j,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,first_msg,msg_auth);
              if (ret_ssh == SSH_ERROR_OK)
              {
                msg_auth = FALSE;
                if (tmp_os[0] != 0)
                {
                  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                  if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].mdp);
                  break;
                }else if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].mdp, j,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE, FALSE) == SSH_ERROR_OK)
                {
                  if (tmp_os[0] != 0)
                  {
                    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                    if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].mdp);
                    break;
                  }
                }
              }else if (ret_ssh !=  SSH_ERROR_AUTHENT)
              {
                #ifdef DEBUG_MODE_SSH
                snprintf(msg,MAX_LINE_SIZE,"%s:22 account %s(%02d)",ip,config.accounts[j].login,j);
                switch(ret_ssh)
                {
                  //case SSH_ERROR_CHANNEL_EXEC:  AddMsg(h_main,(char*)"ERROR (SSH)",msg,"Unable to execute commands!");break;
                  case SSH_ERROR_CHANNEL:       AddMsg(h_main,(char*)"ERROR (SSH)",msg,"Unable to open a channel command execution!");break;
                  //case SSH_ERROR_AUTHENT:       AddMsg(h_main,(char*)"ERROR (SSH)",msg,"Wrong account for authentication!");break;
                  case SSH_ERROR_SESSIONSTART:  AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to initialize SSH session!");break;
                  case SSH_ERROR_SESSIONINIT:   AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to initialize SSH session variables!");break;
                  case SSH_ERROR_CONNECT:       AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unabe to connect on port 22!");break;
                  case SSH_ERROR_SOCKET:        AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to create socket!");break;
                  case SSH_ERROR_LIBINIT:       AddMsg(h_main,(char*)"ERROR (SSH)",ip,"Unable to initialize the SSH library!");break;
                }
                #endif
                break;
              }
              first_msg = FALSE;
            }
          }
          ReleaseSemaphore(hs_ssh,1,NULL);
        }
      }

      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","SCAN:END",ip);
      #endif

      if (exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"OK");
    }
  }
  ReleaseSemaphore(hs_threads,1,NULL);

  //tracking
  if (scan_start)
  {
    //#endif
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_threads",ip);
    #endif

    EnterCriticalSection(&Sync);
    snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,++nb_test_ip,nb_i);
    LeaveCriticalSection(&Sync);
    SetWindowText(h_main,test_title);
  }
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI scan(LPVOID lParam)
{
  time_t exec_time_start, exec_time_end;
  time(&exec_time_start);

  //load IP
  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    //load IP interval
    SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

    DWORD LIp1,LIp2;
    BYTE L11,L12,L13,L14,L21,L22,L23,L24;
    SendDlgItemMessage(h_main,IP1, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp1);
    SendDlgItemMessage(h_main,IP2, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp2);

    L11 = LIp1 >> 24;
    L12 = (LIp1 >> 16) & 0xFF;
    L13 = (LIp1 >> 8) & 0xFF;
    L14 = LIp1 & 0xFF;

    L21 = LIp2 >> 24;
    L22 = (LIp2 >> 16) & 0xFF;
    L23 = (LIp2 >> 8) & 0xFF;
    L24 = LIp2 & 0xFF;

    if ((L21 | L22 | L23 | L24) == 0)
    {
      char sip1[IP_SIZE];
      snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      addIPTest(sip1);
    }else if ((L11 | L12 | L13 | L14) == 0)
    {
      char sip2[IP_SIZE];
      snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);
      addIPTest(sip2);
    }else
    {
      char sip1[IP_SIZE],sip2[IP_SIZE];
      snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);
      addIPInterval(sip1, sip2);
    }
  }

  char tmp[MAX_PATH];
  snprintf(tmp,LINE_SIZE,"Loaded %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
  AddMsg(h_main,(char*)"INFORMATION",tmp,(char*)"");

  //load config
  unsigned int ref  = 0;
  nb_files          = 0;
  nb_registry       = 0;
  nb_windows        = 0;

  //config.disco_arp          = 0;//SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)0,(LPARAM)NULL);
  config.disco_icmp           = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_dns            = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_netbios        = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_netbios_policy = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  ref++;
  config.check_files          = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_registry       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_services       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_software       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_USB            = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_ssh            = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_ssh_os         = config.check_ssh;
  ref++;
  config.write_key            = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);

  //load files
  if (config.check_files)   config.check_files    = (BOOL)load_file_list(CB_T_FILES,     (char*)DEFAULT_LIST_FILES);
  if (config.check_registry)config.check_registry = (BOOL)load_file_list(CB_T_REGISTRY,  (char*)DEFAULT_LIST_REGISTRY);
  if (config.check_services)config.check_services = (BOOL)load_file_list(CB_T_SERVICES,  (char*)DEFAULT_LIST_SERVICES);
  if (config.check_software)config.check_software = (BOOL)load_file_list(CB_T_SOFTWARE,  (char*)DEFAULT_LIST_SOFTWARE);
  if (config.check_USB)     config.check_USB      = (BOOL)load_file_list(CB_T_USB,       (char*)DEFAULT_LIST_USB);

  if (config.write_key)     config.write_key      = (BOOL)load_file_list(CB_T_REGISTRY_W,(char*)DEFAULT_LIST_REGISTRY_W);
  if (config.check_ssh)     config.check_ssh      = (BOOL)load_file_list(CB_T_SSH,(char*)DEFAULT_LIST_SSH);

  if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
  {
    config.local_account   = FALSE;
    GetWindowText(GetDlgItem(h_main,ED_NET_LOGIN),config.login,MAX_PATH);
    GetWindowText(GetDlgItem(h_main,ED_NET_PASSWORD),config.mdp,MAX_PATH);
    GetWindowText(GetDlgItem(h_main,ED_NET_DOMAIN),config.domain,MAX_PATH);
  }else
  {
    config.local_account = TRUE;
    config.login[0]      = 0;
    config.domain[0]     = 0;
    config.mdp[0]        = 0;
  }

  //---------------------------------------------
  //scan_start
  DWORD i;
  nb_i = SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  char ip_test[MAX_PATH];

  //threads
  #ifndef DEBUG_SIMPLE
  hs_threads  = CreateSemaphore(NULL,NB_MAX_THREAD,NB_MAX_THREAD,NULL);
  hs_disco    = CreateSemaphore(NULL,NB_MAX_DISCO_THREADS,NB_MAX_DISCO_THREADS,NULL);
  hs_netbios  = CreateSemaphore(NULL,NB_MAX_NETBIOS_THREADS,NB_MAX_NETBIOS_THREADS,NULL);
  hs_file     = CreateSemaphore(NULL,NB_MAX_FILE_THREADS,NB_MAX_FILE_THREADS,NULL);
  hs_registry = CreateSemaphore(NULL,NB_MAX_REGISTRY_THREADS,NB_MAX_REGISTRY_THREADS,NULL);
  hs_tcp      = CreateSemaphore(NULL,NB_MAX_TCP_TEST_THREADS,NB_MAX_TCP_TEST_THREADS,NULL);
  hs_ssh      = CreateSemaphore(NULL,NB_MAX_SSH_THREADS,NB_MAX_SSH_THREADS,NULL);
  #endif

  //wsa init
  WSADATA WSAData;
  WSAStartup(0x02, &WSAData );
  nb_test_ip = 0;
  InitializeCriticalSection(&Sync);

  for (i=0;(i<nb_i) && scan_start;i++)
  {
    //ScanIp((LPVOID)i);
    WaitForSingleObject(hs_threads,INFINITE);
    CreateThread(NULL,0,ScanIp,(PVOID)i,0,0);
  }

  //wait
  AddMsg(h_main,(char*)"INFORMATION",(char*)"Start waiting threads.",(char*)"");

  if (!scan_start)
  {
    while (nb_test_ip < i && scan_start)Sleep(100);
  }else
  {
    for(i=0;i<NB_MAX_THREAD;i++)WaitForSingleObject(hs_threads,INFINITE);
    //for(i=0;i<NB_MAX_DISCO_THREADS;i++)WaitForSingleObject(hs_disco,INFINITE);
    WaitForSingleObject(hs_netbios,INFINITE);
    WaitForSingleObject(hs_file,INFINITE);
    WaitForSingleObject(hs_registry,INFINITE);
    WaitForSingleObject(hs_tcp,INFINITE);
    WaitForSingleObject(hs_ssh,INFINITE);
  }

  WSACleanup();

  //calcul run time
  time(&exec_time_end);

  AddMsg(h_main,(char*)"INFORMATION",(char*)"End of scan!",(char*)"");
  snprintf(tmp,MAX_PATH,"Ip view : %lu/%lu in %d.%0d minutes",ListView_GetItemCount(GetDlgItem(h_main,LV_results)),nb_i,(exec_time_end - exec_time_start)/60,(exec_time_end - exec_time_start)%60);
  AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");

  if (config.check_files)
  {
    snprintf(tmp,MAX_PATH,"Remote file authentication OK : %lu/%lu",nb_files,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");
  }

  if (config.check_registry || config.check_services || config.check_software || config.check_USB || config.write_key)
  {
    snprintf(tmp,MAX_PATH,"Remote registry authentication OK : %lu/%lu",nb_registry,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");
    snprintf(tmp,MAX_PATH,"Computer in Microsoft Windows OS : %lu/%lu",nb_windows,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");
  }

  //close
  DeleteCriticalSection(&Sync);
  CloseHandle(hs_threads);
  CloseHandle(hs_disco);
  CloseHandle(hs_netbios);
  CloseHandle(hs_file);
  CloseHandle(hs_registry);
  CloseHandle(hs_tcp);
  CloseHandle(hs_ssh);

  //---------------------------------------------
  //init
  if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),TRUE);
  }

  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),TRUE);
    EnableWindow(GetDlgItem(h_main,IP1),TRUE);
    EnableWindow(GetDlgItem(h_main,BT_IP_CP),TRUE);
    EnableWindow(GetDlgItem(h_main,IP2),TRUE);
  }

  EnableWindow(GetDlgItem(h_main,BT_LOAD_MDP_FILES),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
  EnableWindow(GetDlgItem(h_main,CB_tests),TRUE);
  scan_start = FALSE;

  SetWindowText(GetDlgItem(h_main,BT_START),"Start");
  SetWindowText(h_main,TITLE);
  return 0;
}
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
      if (!save_done && (ListView_GetItemCount(GetDlgItem(hwnd,LV_results))>0))
      {
        if (MessageBox(h_main,"No data saved, are you really sure to quit?","Quit?",MB_YESNO|MB_ICONQUESTION|MB_TOPMOST) == IDYES)save_done = TRUE;
      }else save_done = TRUE;

      if (save_done)
      {
        scan_start = FALSE;
        DeleteCriticalSection(&Sync);
        CloseHandle(h_log);
        FreeLibrary((HMODULE)hndlIcmp);
        EndDialog(hwnd,0);
      }
    }
    break;
    //--------------------------------------
    case WM_COMMAND:
    {
      switch (HIWORD(wParam))
      {
        case LBN_DBLCLK:
          switch(LOWORD(wParam))
          {
            case CB_infos:
            {
              char msg[MAX_LINE_SIZE];
              if (ListBox_GetText((HWND)lParam,ListBox_GetCurSel((HWND)lParam),msg))
              {
                SetWindowText(hdbclk_info, msg);
                ShowWindow (hdbclk_info, SW_SHOW);
              }
            }
            break;
          }
        break;
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
              ofn.lpstrFilter    = "*.xml \0*.xml\0*.csv \0*.csv\0*.html \0*.html\0All formats\0*.*\0";
              ofn.lpstrDefExt    = "\0";
              if (GetSaveFileName(&ofn)==TRUE)
              {
                if (ofn.nFilterIndex == SAVE_TYPE_ALL)
                {
                  char file2[MAX_PATH];
                  snprintf(file2,MAX_PATH,"%s.xml",file);
                  if(SaveLSTV(GetDlgItem(hwnd,LV_results), file2, SAVE_TYPE_XML, NB_COLUMN)) AddMsg(hwnd, (char*)"INFORMATION",(char*)"Recorded data",file2);
                  else AddMsg(hwnd, (char*)"ERROR",(char*)"No data saved to!",file2);

                  snprintf(file2,MAX_PATH,"%s.csv",file);
                  if(SaveLSTV(GetDlgItem(hwnd,LV_results), file2, SAVE_TYPE_CSV, NB_COLUMN)) AddMsg(hwnd, (char*)"INFORMATION",(char*)"Recorded data",file2);
                  else AddMsg(hwnd, (char*)"ERROR",(char*)"No data saved to!",file2);

                  snprintf(file2,MAX_PATH,"%s.html",file);
                  if(SaveLSTV(GetDlgItem(hwnd,LV_results), file2, SAVE_TYPE_HTML, NB_COLUMN)) AddMsg(hwnd, (char*)"INFORMATION",(char*)"Recorded data",file2);
                  else AddMsg(hwnd, (char*)"ERROR",(char*)"No data saved to!",file2);
                }else
                {
                  if(SaveLSTV(GetDlgItem(hwnd,LV_results), file, ofn.nFilterIndex, NB_COLUMN)) AddMsg(hwnd, (char*)"INFORMATION",(char*)"Recorded data",file);
                  else AddMsg(hwnd, (char*)"ERROR",(char*)"No data saved to!",file);
                }

                save_done = TRUE;
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
                EnableWindow(GetDlgItem(hwnd,BT_LOAD_MDP_FILES),FALSE);

                ListView_DeleteAllItems(GetDlgItem(h_main,LV_results));
                SendDlgItemMessage(hwnd,CB_infos,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

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
              SendDlgItemMessage(hwnd,CB_tests,LB_SETSEL,(WPARAM)FALSE,(LPARAM)4);
              SendDlgItemMessage(hwnd,CB_tests,LB_SETSEL,(WPARAM)FALSE,(LPARAM)11);
              SendDlgItemMessage(hwnd,CB_tests,LB_SETSEL,(WPARAM)FALSE,(LPARAM)12);
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
            //------------------------------
            case BT_LOAD_MDP_FILES:
            {
              //load file with format csv :
              //"domain";"login";"mdp";\n or \r\n
              if (IsDlgButtonChecked(hwnd,LOWORD(wParam))==BST_CHECKED)CreateThread(NULL,0,load_file_accounts,0,0,0);
              else
              {
                if (IsDlgButtonChecked(hwnd,CHK_NULL_SESSION)==BST_CHECKED)
                {
                  EnableWindow(GetDlgItem(hwnd,CHK_NULL_SESSION),TRUE);
                }else
                {
                  EnableWindow(GetDlgItem(hwnd,ED_NET_DOMAIN),TRUE);
                  EnableWindow(GetDlgItem(hwnd,ED_NET_LOGIN),TRUE);
                  EnableWindow(GetDlgItem(hwnd,ED_NET_PASSWORD),TRUE);
                  EnableWindow(GetDlgItem(hwnd,CHK_NULL_SESSION),TRUE);
                }
              }
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
          char msg[MAX_MSG_SIZE+1]="",tmp[MAX_MSG_SIZE+1];
          long int index = SendDlgItemMessage(hwnd,LV_results,LVM_GETNEXTITEM,(WPARAM)-1,(LPARAM)LVNI_FOCUSED);
          if (index != -1)
          {
            tmp[0] = 0;
            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_IP,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"%s",tmp);

              tmp[0] = 0;
              ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_DNS,tmp,MAX_MSG_SIZE);
              if (tmp[0] != 0)
              {
                //strncat(msg," ",MAX_LINE_SIZE);
                //strncat(msg,tmp,MAX_LINE_SIZE);
                snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg)," %s",tmp);
                tmp[0] = 0;
              }

              ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_TTL,tmp,MAX_MSG_SIZE);
              if (tmp[0] != 0)
              {
                //strncat(msg," ",MAX_LINE_SIZE);
                //strncat(msg,tmp,MAX_LINE_SIZE);
                snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg)," %s",tmp);
                tmp[0] = 0;
              }

              ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_OS,tmp,MAX_MSG_SIZE);
              if (tmp[0] != 0)
              {
                //strncat(msg," ",MAX_LINE_SIZE);
                //strncat(msg,tmp,MAX_LINE_SIZE);
                snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg)," %s",tmp);
                tmp[0] = 0;
              }

            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_CONFIG,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[Config]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[Config]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SHARE,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[Share]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[Share]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_POLICY,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[Account Policy]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[Account Policy]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_FILES,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[Files]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[Files]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_REG,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[Registry]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[Registry]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SERVICE,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[Services]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[Services]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SOFTWARE,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[Softwares]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[Softwares]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_USB,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[USB]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[USB]\r\n%s",tmp);
              tmp[0] = 0;
            }

            ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,COL_SSH,tmp,MAX_MSG_SIZE);
            if (tmp[0] != 0)
            {
              //strncat(msg,"\r\n\r\n[SSH]\r\n",MAX_LINE_SIZE);
              //strncat(msg,tmp,MAX_LINE_SIZE);
              snprintf(msg+strlen(msg),MAX_MSG_SIZE-strlen(msg),"\r\n\r\n[SSH]\r\n%s",tmp);
              tmp[0] = 0;
            }

            //strncat(msg,"\0",MAX_LINE_SIZE);
            if (strlen(msg))
            {
              SetWindowText(hdbclk_info, msg);
              ShowWindow (hdbclk_info, SW_SHOW);
              //MessageBox(h_main,msg,"Global View",MB_OK|MB_TOPMOST);
            }
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
