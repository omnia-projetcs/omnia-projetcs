//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "resources.h"
//----------------------------------------------------------------
void init(HWND hwnd)
{
  h_main            = hwnd;
  scan_start        = FALSE;
  tri_order         = FALSE;

  SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));

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
  lvc.pszText = "IP";
  ListView_InsertColumn(hlv, COL_IP, &lvc);
  lvc.cx = 110;
  lvc.pszText = "DNS";
  ListView_InsertColumn(hlv, COL_DNS, &lvc);
  lvc.cx = 150;
  lvc.pszText = "TTL/OS";
  ListView_InsertColumn(hlv, COL_TTL, &lvc);

  lvc.cx = 150;
  lvc.pszText = "Config";
  ListView_InsertColumn(hlv, COL_CONFIG, &lvc);

  lvc.cx = 100;
  lvc.pszText = "Files";
  ListView_InsertColumn(hlv, COL_FILES, &lvc);
  lvc.cx = 100;
  lvc.pszText = "Registry";
  ListView_InsertColumn(hlv, COL_REG, &lvc);
  lvc.cx = 100;
  lvc.pszText = "Services";
  ListView_InsertColumn(hlv, COL_SERVICE, &lvc);
  lvc.cx = 100;
  lvc.pszText = "Software";
  ListView_InsertColumn(hlv, COL_SOFTWARE, &lvc);
  lvc.cx = 100;
  lvc.pszText = "USB";
  ListView_InsertColumn(hlv, COL_USB, &lvc);
  //State
  lvc.cx = 50;
  lvc.pszText = "State";
  ListView_InsertColumn(hlv, COL_STATE, &lvc);
  SendMessage(hlv,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

  //log file
  h_log = CreateFile("NS_log.txt", GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  SetFilePointer(h_log,0,0,FILE_END);
  DWORD copiee = 0;
  WriteFile(h_log,"----------------\r\n",18,&copiee,0);

  //insert test
  //SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:ARP");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:PING");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:DNS");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:NetBIOS");
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
unsigned long int Contient(char*data,char*chaine)
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
  char msg[MAX_PATH],date[DATE_SIZE];
  time_t dateEtHMs;
  time(&dateEtHMs);
  DWORD copiee = 0;

  snprintf(date,20,"%s",(char *)ctime(&dateEtHMs));
  if (info != NULL) snprintf(msg,MAX_PATH,"[%s] %s - %s %s",date+11,type,txt,info);
  else snprintf(msg,MAX_PATH,"[%s] %s - %s",date+11,type,txt);
  SendDlgItemMessage(hwnd,CB_infos,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)msg);
  if (h_log != INVALID_HANDLE_VALUE)
  {
    strncat(msg,"\r\n\0",MAX_PATH);
    WriteFile(h_log,msg,strlen(msg),&copiee,0);
  }
}
//------------------------------------------------------------------------------
void AddLSTVUpdateItem(char *add, DWORD column, DWORD iitem)
{
  HANDLE hlstv  = GetDlgItem(h_main,LV_results);
  char buffer[MAX_LINE_SIZE] = "";
  ListView_GetItemText(hlstv,iitem,column,buffer,MAX_LINE_SIZE);
  if (buffer[0] != 0)
  {
    strncat(buffer,"\r\n",MAX_LINE_SIZE);
    strncat(buffer,add,MAX_LINE_SIZE);
    strncat(buffer,"\0",MAX_LINE_SIZE);
  }else
  {
    strncpy(buffer,add,MAX_LINE_SIZE);
  }
  ListView_SetItemText(hlstv,iitem,column,buffer);
}
//----------------------------------------------------------------
DWORD AddLSTVItem(char *ip, char *dns, char *ttl, char *config, char *files, char *registry, char *Services, char *software, char *USB, char *state)
{
  LVITEM lvi;
  HANDLE hlstv  = GetDlgItem(h_main,LV_results);
  lvi.mask      = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem  = 0;
  lvi.lParam    = LVM_SORTITEMS;
  lvi.pszText   = "";
  EnterCriticalSection(&Sync);
  lvi.iItem     = ListView_GetItemCount(hlstv);
  DWORD itemPos = ListView_InsertItem(hlstv, &lvi);
  LeaveCriticalSection(&Sync);

  if(ip!=NULL)      ListView_SetItemText(hlstv,itemPos,COL_IP,ip);
  if(dns!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_DNS,dns);
  if(ttl!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_TTL,ttl);
  if(config!=NULL)  ListView_SetItemText(hlstv,itemPos,COL_CONFIG,config);
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
BOOL SaveLSTV(HANDLE hlv, char *file, unsigned int type, unsigned int nb_column)
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

    char lines[MAX_LINE_SIZE]="", buffer[MAX_LINE_SIZE]="";
    DWORD copiee;
    unsigned long int i=0,j=0;

    LVCOLUMN lvc;
    lvc.mask        = LVCF_TEXT;
    lvc.cchTextMax  = MAX_LINE_SIZE;
    lvc.pszText     = buffer;

    switch(type)
    {
      case SAVE_TYPE_CSV:
        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }
        strncat(lines,"\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(hfile,lines,strlen(lines),&copiee,0);

        //save all line
        for (j=0;j<nb_items;j++)
        {
          lines[0]=0;
          for (i=0;i<nb_column;i++)
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"%s\";",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\"\";");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"\r\n");
          copiee = 0;
          WriteFile(hfile,lines,strlen(lines),&copiee,0);
        }
      break;
      case SAVE_TYPE_XML:
      {
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        LINE_ITEM lv_line[nb_column+1];

        //title line
        for (i=0;i<nb_column;i++)
        {
          lv_line[i].c[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
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
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines,MAX_LINE_SIZE,"  <%s><![CDATA[%s]]></%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
              WriteFile(hfile,lines,strlen(lines),&copiee,0);
            }
          }
          WriteFile(hfile," </Data>\r\n",10,&copiee,0);
        }
        WriteFile(hfile,"</RtCA>",7,&copiee,0);
      }
      break;
      case SAVE_TYPE_HTML:
      {
        char head[]="<html>\r\n <head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n <table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n  <tr bgcolor=\"#CCCCCC\">\r\n";
        WriteFile(hfile,head,strlen(head),&copiee,0);

        //title line
        for (i=0;i<nb_column;i++)
        {
          if (!SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))break;
          if (strlen(buffer)>0)
            snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"  <th>%s</th>",buffer);

          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;

        }

        strncat(lines,"\r\n  </tr>\r\n\0",MAX_LINE_SIZE);
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
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td>%s</td>",buffer);
            }else snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"<td></td>");
          }
          snprintf(lines+strlen(lines),MAX_LINE_SIZE-strlen(lines),"</tr>\r\n");
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
  char tmp[4]="\0\0\0\0";
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
      AddMsg(h_main,"ERROR","Invalid interval",msg);
    }
  }else
  {
    char msg[MAX_PATH];
    snprintf(msg,MAX_PATH,"%s->%s",ip_src,ip_dst);
    AddMsg(h_main,"ERROR","Invalid interval",msg);
  }
}
//------------------------------------------------------------------------------
void addIPTest(char *ip_format)
{
  if (ip_format == NULL) return;
  unsigned int size = strlen(ip_format);

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
      char tmp[4]="\0\0\0\0";

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
        if (size != copiee)AddMsg(h_main, "ERROR","In loading file",file);

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
        AddMsg(h_main,"INFORMATION",tmp,file);
        free(buffer);
      }
    }
    CloseHandle(hfile);
  }
  //reinit GUI
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  return 0;
}

//------------------------------------------------------------------------------
void load_file_list(DWORD lsb, char *file)
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
      ReadFile(hfile, buffer, size,&copiee,0);
      if (size != copiee)AddMsg(h_main, "ERROR","In loading file",file);

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
          SendDlgItemMessage(h_main,lsb,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)tmp);
        }
      }

      //message
      snprintf(tmp,LINE_SIZE,"Loaded file with %lu item(s)",SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
      AddMsg(h_main,"INFORMATION",tmp,file);
      free(buffer);
    }
    CloseHandle(hfile);
  }
}
