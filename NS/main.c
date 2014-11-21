//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/aa390422%28v=vs.85%29.aspx
#include "resources.h"
//------------------------------------------------------------------------------
BOOL compare_nocas(char *a, char *b)
{
  charToLowChar(a);
  charToLowChar(b);
  if (!strcmp(a,b))return TRUE;
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL Contient_nocas(char *data, char *chaine)
{
  charToLowChar(data);
  charToLowChar(chaine);
  if (Contient(data, chaine))return TRUE;
  return FALSE;
}
//------------------------------------------------------------------------------
char *extractFileFromPath(char *path, char *file, unsigned int file_size_max)
{
  char *c = path;
  file[0] = 0;

  while(*c++);
  while(*c!='\\' && *c!='/' && c>path)c--;

  if(*c == '\\' || *c=='/')c++;

  strncpy(file,c,file_size_max);
  return file;
}
//------------------------------------------------------------------------------
char *extractPath(char *path, char *path_dst, unsigned int path_size_max)
{
  strncpy(path_dst,path,path_size_max);
  char *c = path_dst;
  while(*c++);
  while(*c!='\\' && *c!='/' && c>path_dst)c--;

  if(*c == '\\' || *c=='/')c++;
  *c = 0;

  return path_dst;
}
//------------------------------------------------------------------------------
//supprimer tout le contenu
void RichEditInit(HWND HRichEdit)
{
  CHARFORMAT2 Format;

  //init de la zone de texte
  ZeroMemory(&Format, sizeof(CHARFORMAT2));
  Format.cbSize = sizeof(CHARFORMAT2);
  Format.dwMask = CFM_BOLD | CFM_COLOR | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
  Format.yHeight = 200; //taille du texte
  Format.yOffset = 0;
  Format.crTextColor = RGB(0,0,0);   //Couleur du texte
  Format.crBackColor = RGB(255,255,255);   //Couleur du fond
  Format.bCharSet = ANSI_CHARSET;
  Format.bPitchAndFamily = DEFAULT_PITCH;
  strcpy(Format.szFaceName,"MS Sans Serif");
  SendMessage(HRichEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &Format);   //appliquer le format a tout le composant
  SendMessage(HRichEdit, WM_SETTEXT, 0, (LPARAM)"");//on vide le texte
}
//------------------------------------------------------------------------------
//texte simple
void RichEditCouleur(HWND HRichEdit,COLORREF couleur,char* txt)
{
    CHARFORMAT2 Format; //format du texte d'un richedit
    CHARRANGE Selection; //sélection

    ZeroMemory(&Format, sizeof(CHARFORMAT2));
    Format.cbSize = sizeof(CHARFORMAT2);
    Format.dwMask = CFM_CHARSET|CFM_COLOR|CFM_UNDERLINE|CFM_ITALIC|CFM_BOLD|CFM_SIZE;
    Format.crTextColor = couleur;
    Format.dwEffects = 0;
    Format.yHeight = 200;

    //récupération de la taille du texte contenu, on ajoute le texte après
    GETTEXTLENGTHEX TxtLenfth;
    TxtLenfth.codepage=CP_ACP;
    TxtLenfth.flags=GTL_NUMCHARS;
    unsigned int pos= SendMessage(HRichEdit,EM_GETTEXTLENGTHEX,(WPARAM)&TxtLenfth,0);

    Selection.cpMin = pos;
    Selection.cpMax = pos+strlen(txt);

    SendMessage(HRichEdit, EM_EXSETSEL, 0, (LPARAM) &Selection);
    SendMessage(HRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &Format);
	// Il ne reste plus qu'à insérer le texte formaté:
    SendMessage(HRichEdit, EM_REPLACESEL, 0, (LPARAM)(LPCTSTR) txt);
}
//------------------------------------------------------------------------------
//En haut d ela fenêtre
void RichSetTopPos(HWND HRichEdit)
{
  POINT p;
  p.x = 0;
  p.y = 0;
  SendMessage(HRichEdit,EM_SETSCROLLPOS,(WPARAM)0,(LPARAM) &p);
}
//------------------------------------------------------------------------------
//Ajout de texte colorer et gras au RichEdit ; couleur = RGB(255, 0, 0)
void RichEditCouleurGras(HWND HRichEdit,COLORREF couleur,char* txt)
{
    CHARFORMAT2 Format; //format du texte d'un richedit
    CHARRANGE Selection; //sélection

    ZeroMemory(&Format, sizeof(CHARFORMAT2));
    Format.cbSize = sizeof(CHARFORMAT2);
    Format.dwMask = CFM_COLOR|CFM_CHARSET|CFM_UNDERLINE|CFM_ITALIC|CFM_BOLD|CFM_SIZE;
    Format.crTextColor = couleur;
    Format.dwEffects = CFE_BOLD;
    Format.yHeight = 200;

    //récupération de la taille du texte contenu, on ajoute le texte après
    GETTEXTLENGTHEX TxtLenfth;
    TxtLenfth.codepage=CP_ACP;
    TxtLenfth.flags=GTL_NUMCHARS;
    unsigned int pos= SendMessage(HRichEdit,EM_GETTEXTLENGTHEX,(WPARAM)&TxtLenfth,0);

    Selection.cpMin = pos;
    Selection.cpMax = pos+strlen(txt);

    SendMessage(HRichEdit, EM_EXSETSEL, 0, (LPARAM) &Selection);
    SendMessage(HRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &Format);
	// Il ne reste plus qu'à insérer le texte formaté:
    SendMessage(HRichEdit, EM_REPLACESEL, 0, (LPARAM)(LPCTSTR) txt);
}
//------------------------------------------------------------------------------
//subclass of hdbclk_info
//------------------------------------------------------------------------------
LRESULT APIENTRY subclass_hdbclk_info(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CLOSE)
  {
    ShowWindow (hwnd, SW_HIDE);
    ShowWindow (h_main, SW_HIDE);
    ShowWindow (h_main, SW_SHOW);
  }else return CallWindowProc(wndproc_hdbclk_info, hwnd, uMsg, wParam, lParam);
  return 0;
}
//----------------------------------------------------------------
void init(HWND hwnd)
{
  h_main            = hwnd;
  scan_start        = FALSE;
  tri_order         = FALSE;
  SHA1_enable       = FALSE;
  config.nb_accounts= 0;
  save_done         = FALSE;
  save_current      = FALSE;

  SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(ICON_APP)));

  //edit for dblck view
  richDll = LoadLibrary("RICHED32.DLL");
  hdbclk_info = CreateWindowEx(0x200|WS_EX_CLIENTEDGE, "RichEdit20A", "", 0x00E80844|WS_SIZEBOX|WS_MAXIMIZEBOX,
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
  lvc.cx = 20;
  lvc.pszText = (LPSTR)"DSC";
  ListView_InsertColumn(hlv, COL_DSC, &lvc);
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
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:PING");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:DNS");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:NetBIOS");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"DISCO:Accounts policy");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"------------------------------");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Files");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Registry");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Services");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:Software");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:USB");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"CHECK:SSH");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"------------------------------");
  SendDlgItemMessage(hwnd,CB_tests,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)"WRITE:Registry Values");

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

  //load WOW64 functions
  hDLL_kernel32 = LoadLibrary( "KERNEL32.dll");
  if (hDLL_kernel32 != NULL)
  {
    Wow64DisableWow64FsRedirect = (BOOL (WINAPI *)(PVOID *OldValue)) GetProcAddress(hDLL_kernel32,"Wow64DisableWow64FsRedirection");
    Wow64RevertWow64FsRedirect  = (BOOL (WINAPI *)(PVOID *OldValue)) GetProcAddress(hDLL_kernel32,"Wow64RevertWow64FsRedirection");

    if (!Wow64DisableWow64FsRedirect || !Wow64RevertWow64FsRedirect)
    {
      FreeLibrary(hDLL_kernel32);
      hDLL_kernel32 = 0;
    }
  }

  //critical section
  InitializeCriticalSection(&Sync);
  InitializeCriticalSection(&Sync_item);
  InitializeCriticalSection(&Sync_threads);
  InitializeCriticalSection(&Sync_threads_end);

  LOG_DISABLE = FALSE;
  auto_scan_config.DNS_DISCOVERY = TRUE;
  config.global_ip_file = FALSE;
  h_thread_scan = CreateThread(NULL,0,auto_scan,0,0,0);
}
//------------------------------------------------------------------------------
long int Contient(char*data, char*chaine)
{
  unsigned long int i=0;
  char *d = data;
  char *c = chaine;

  if (!*c || !*d) return -1;

  while (*d)
  {
    c = chaine;
    while (*d == *c && *c && *d){d++;c++;i++;}

    if (*c == 0) return i;
    d++;i++;
  }
  return -1;
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
  if (h_log != INVALID_HANDLE_VALUE && !LOG_DISABLE)
  {
    DWORD copiee = 0;
    char msg2[MAX_MSG_SIZE];
    snprintf(msg2,MAX_MSG_SIZE,"%s\r\n", msg);
    WriteFile(h_log,msg2,strlen(msg2),&copiee,0);
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
long int AddLSTVItem(char *ip, char *dsc, char *dns, char *ttl, char *os, char *config, char *share, char*policy, char *files, char *registry, char *Services, char *software, char *USB, char *state)
{
  LVITEM lvi;
  HWND hlstv    = GetDlgItem(h_main,LV_results);
  lvi.mask      = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem  = 0;
  lvi.lParam    = LVM_SORTITEMS;
  lvi.pszText   = (LPSTR)"";
  EnterCriticalSection(&Sync_item);
  lvi.iItem     = ListView_GetItemCount(hlstv);
  long int itemPos = ListView_InsertItem(hlstv, &lvi);
  LeaveCriticalSection(&Sync_item);

  if (itemPos < 0) return -1;

  if(ip!=NULL)      ListView_SetItemText(hlstv,itemPos,COL_IP,ip);
  if(dsc!=NULL)     ListView_SetItemText(hlstv,itemPos,COL_DSC,dsc);
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
BOOL SaveLV(HWND hlv, char *file)
{
   //get item count
  unsigned long int nb_items = SendMessage(hlv,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  if (nb_items > 0)
  {
    //open file
    HANDLE hfile = CreateFile(file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (hfile == INVALID_HANDLE_VALUE)
    {
      return FALSE;
    }

    char lines[MAX_MSG_SIZE_LINE];
    DWORD copiee;
    unsigned long int i=0;
    for (i=0;i<nb_items;i++)
    {
      lines[0] = 0;
      SendMessage(hlv, LB_GETTEXT, (WPARAM)0,(LPARAM)lines);
      strncat(lines,"\r\n\0",MAX_MSG_SIZE_LINE);
      WriteFile(hfile,lines,strlen(lines),&copiee,0);
    }
    CloseHandle(hfile);
    return TRUE;
  }else return FALSE;
}
//------------------------------------------------------------------------------
void addIPInterval(char *ip_src, char *ip_dst, char *dsc)
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
    if (((L11+L12+L13+L14) == 0 && L21>0) || !strcmp(ip_src,ip_dst)){SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_src);/*AddMsg(h_main,(char*)"DEBUG_IP",ip_src,"");*/}
    else if ((L21+L22+L23+L24) == 0 && L14 >0 && L14 <=255 && L13 <=255 && L12 <=255 && L11<255){SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_dst);/*AddMsg(h_main,(char*)"DEBUG_IP",ip_src,"");*/}
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
              //AddMsg(h_main,(char*)"DEBUG_IP",IpAdd,"");
              SendDlgItemMessage(h_main,CB_DSC,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)dsc);
            }
          }
        }
      }
    }else
    {
      char msg[MAX_PATH];
      snprintf(msg,MAX_PATH,"%s->%s (%s)",ip_src,ip_dst,dsc);
      AddMsg(h_main,(char*)"ERROR",(char*)"Invalid interval",msg);
    }
  }else
  {
    char msg[MAX_PATH];
    snprintf(msg,MAX_PATH,"%s->%s (%s)",ip_src,ip_dst,dsc);
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
BOOL addIPTest(char *ip_format, char*dsc)
{
  if (ip_format == NULL) return FALSE;
  unsigned int size = strlen(ip_format);
  if (size < 2)return FALSE;

  //check if name or ip
  if (ip_format[0]> '9' || ip_format[0]< '0' || ((ip_format[1]> '9' || ip_format[1]< '0') && ip_format[1] != '.'))
  {
    if (verifieName(ip_format))
      SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_format);
      //AddMsg(h_main,(char*)"DEBUG_IP",ip_format,"");
      SendDlgItemMessage(h_main,CB_DSC,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)dsc);
    return TRUE;
  }

  //search if '-'
  char *c = ip_format;
  while (*c && (*c !='-') && (*c != '/'))c++;
  if (*c == 0)//ip
  {
    SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip_format);
    //AddMsg(h_main,(char*)"DEBUG_IP",ip_format,"");
    SendDlgItemMessage(h_main,CB_DSC,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)dsc);
    return TRUE;
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
      addIPInterval(ip1, s, dsc);
      return TRUE;
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

      //only one IP
      if (atoi(s) == 32)
      {
        SendDlgItemMessage(h_main,CB_IP,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)ip1);
        SendDlgItemMessage(h_main,CB_DSC,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)dsc);
        return TRUE;
      }

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
            }else return FALSE;
          }else return FALSE;
        }else return FALSE;
      }else return FALSE;

      ip_tmp = interval + (L11 << 24) + (L12 << 16) + (L13 << 8) + (L14);
      L11 = ip_tmp >> 24;
      L12 = (ip_tmp >> 16) & 0xFF;
      L13 = (ip_tmp >> 8) & 0xFF;
      L14 = ip_tmp & 0xFF;

      snprintf(ip2,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      addIPInterval(ip1, ip2, dsc);
      return TRUE;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
void getInfosFromLine(char *ligne, char *ip, unsigned int ip_sz_max, char *dsc, unsigned int dsc_sz_max, char *domain, unsigned int domain_sz_max, char *login, unsigned int login_sz_max, char *password, unsigned int password_sz_max)
{
  ip[0]  = 0;
  if (dsc)dsc[0] = 0;
  if (domain)domain[0] = 0;
  if (login)login[0] = 0;
  if (password)password[0] = 0;

  //"IP";"DSC";"DOMAIN";"LOGIN";"MDP";
  if (*ligne)
  {
    char *d,*c = ligne+1;
    if (*c == '#')
    {
      if (*(c+1) == '!') //full line profil
      {
        config.global_ip_file = TRUE;

        //next step, pass #!";
        c+=4;
        //IP
        if (*c == '"')
        {
          c++; //pass the "
          snprintf(ip,ip_sz_max,"%s",c);
          d = ip;

          while (*d && *d != '"')d++;
          if (*d != '"')ip[0]  = 0;
          else
          {
            *d = 0;
          }
          c+= (strlen(ip) + strlen("\";"));

          //DSC
          if (*c == '"')
          {
            c++; //pass the "
            char tmp_line[MAX_LINE_SIZE]="";
            snprintf(tmp_line,MAX_LINE_SIZE,"%s",c);
            d = tmp_line;

            while (*d && *d != '"')d++;
            if (*d == '"')
            {
              *d = 0;
              if (dsc)snprintf(dsc,dsc_sz_max,"%s",tmp_line);
              c+= (strlen(tmp_line) + strlen("\";"));

              //DOMAIN
              if (*c == '"')
              {
                c++; //pass the "
                tmp_line[0] = 0;
                snprintf(tmp_line,MAX_LINE_SIZE,"%s",c);
                d = tmp_line;

                while (*d && *d != '"')d++;
                if (*d == '"')
                {
                  *d = 0;
                  if (domain)snprintf(domain,domain_sz_max,"%s",tmp_line);
                  c+= (strlen(tmp_line) + strlen("\";"));

                  //LOGIN
                  if (*c == '"')
                  {
                    c++; //pass the "
                    tmp_line[0] = 0;
                    snprintf(tmp_line,MAX_LINE_SIZE,"%s",c);
                    d = tmp_line;

                    while (*d && *d != '"')d++;
                    if (*d == '"')
                    {
                      *d = 0;
                      if (login)snprintf(login,login_sz_max,"%s",tmp_line);
                      c+= (strlen(tmp_line) + strlen("\";"));

                      //PASSWORD
                      if (*c == '"')
                      {
                        c++; //pass the "
                        tmp_line[0] = 0;
                        snprintf(tmp_line,MAX_LINE_SIZE,"%s",c);
                        d = tmp_line;

                        while (*d && *d != '"')d++;
                        if (*d == '"')
                        {
                          *d = 0;
                          if (password)snprintf(password,password_sz_max,"%s",tmp_line);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }else //only ip +desc
    {
      config.global_ip_file = FALSE;
      snprintf(ip,ip_sz_max,"%s",c);
      d = ip;

      while (*d && *d != '"')d++;

      if (*d != '"')ip[0]  = 0;
      else
      {
        *d = 0;
        if (dsc)
        {
          c+= (strlen(ip) + strlen("\"\";"));
          //pass ";" and the last ";
          snprintf(dsc,dsc_sz_max,"%s",c);
          dsc[(unsigned int)(strlen(dsc)-2)]=0;
        }
      }
    }
  }
}
//------------------------------------------------------------------------------
void loadFileIp(char *file)
{
  //load file
  BOOL first_line = TRUE;
  config.global_ip_file = FALSE;
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

      char ip[MAX_PATH], dsc[MAX_PATH], domain[MAX_PATH], login[MAX_PATH], password[MAX_PATH];

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
          if (tmp[0] == '"')
          {
            getInfosFromLine(tmp, ip, MAX_PATH, dsc, MAX_PATH, domain, MAX_PATH, login, MAX_PATH, password, MAX_PATH);
            //getIPFromLine(tmp, ip, MAX_PATH, dsc, MAX_PATH);
            if(addIPTest(ip,dsc) && config.global_ip_file)
            {
              snprintf(config.accounts[config.nb_accounts].domain,MAX_PATH,"%s",domain);
              snprintf(config.accounts[config.nb_accounts].login,MAX_PATH,"%s",login);
              snprintf(config.accounts[config.nb_accounts].password,MAX_PATH,"%s",password);
              if (config.nb_accounts+1 >= MAX_ACCOUNTS)break;
              else config.nb_accounts++;

              if (first_line)config.nb_accounts = 1; //init count !
            }
          }else addIPTest(tmp,"");
          first_line = FALSE;
        }
      }

      snprintf(tmp,LINE_SIZE,"Loaded file with %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
      AddMsg(h_main,(char*)"INFORMATION",tmp,file);
      free(buffer);
    }
  }
  CloseHandle(hfile);
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
  SendDlgItemMessage(h_main,CB_DSC,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

  //load file
  char file[LINE_SIZE]="";
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize   = sizeof(OPENFILENAME);
  ofn.hwndOwner     = h_main;
  ofn.lpstrFile     = file;
  ofn.nMaxFile      = LINE_SIZE;
  ofn.lpstrFilter   = "*.csv \0*.csv\0*.txt \0*.txt\0*.* \0*.*\0";
  ofn.nFilterIndex  = 1;
  ofn.Flags         = OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT|OFN_EXPLORER|OFN_SHOWHELP;
  ofn.lpstrDefExt   = "*.*";
  if (GetOpenFileName(&ofn)==TRUE)
  {
    loadFileIp(file);
  }

  if (SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL) < 1)
  {
    CheckDlgButton(h_main,CHK_LOAD_IP_FILE,BST_UNCHECKED);

    EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),TRUE);
    EnableWindow(GetDlgItem(h_main,IP1),TRUE);
    EnableWindow(GetDlgItem(h_main,BT_IP_CP),TRUE);
    EnableWindow(GetDlgItem(h_main,IP2),TRUE);
  }

  //reinit GUI
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
  return 0;
}
//------------------------------------------------------------------------------
void LoadAuthFile(char *file)
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
                strncpy(config.accounts[config.nb_accounts].password,bf,MAX_PATH);

                if (config.accounts[config.nb_accounts].domain[0] != 0)
                  snprintf(bf,MAX_PATH,"%s\\%s",config.accounts[config.nb_accounts].domain,config.accounts[config.nb_accounts].login);
                else
                  snprintf(bf,MAX_PATH,"(local account)\\%s",config.accounts[config.nb_accounts].login);

                AddMsg(h_main, (char*)"INFORMATION",(char*)"Loading account",bf);

                //next
                if (config.nb_accounts+1 >= MAX_ACCOUNTS)break;
                else config.nb_accounts++;
              }
            }
          }
        }
      }

      snprintf(tmp,LINE_SIZE,"Loaded file with %u accounts",config.nb_accounts);
      AddMsg(h_main,(char*)"INFORMATION",tmp,file);
      free(buffer);
    }
  }
  CloseHandle(hfile);
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
    LoadAuthFile(file);
  }else config.nb_accounts = 0;

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

  char path[LINE_SIZE]="";
  strncat(GetLocalPath(path, LINE_SIZE),file,LINE_SIZE);

  //load file with on item by line to lstv
  HANDLE hfile = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    DWORD size      = GetFileSize(hfile,NULL);
    char *buffer    = (char *) malloc(size+1);
    if (buffer != NULL)
    {
      DWORD copiee =0;
      memset(buffer,0,size+1);
      ReadFile(hfile, buffer, size,&copiee,0);
      if (size != copiee)AddMsg(h_main, (char*)"ERROR",(char*)"In loading file",path);

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
      AddMsg(h_main,(char*)"INFORMATION",tmp,path);
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
/*HANDLE NetConnexionAuthenticateTest(char *ip, DWORD id_ip, char*remote_name, PSCANNE_ST config, DWORD iitem, BOOL message, long int *idm_ok)
{
  HANDLE htoken = NULL;
  char msg[LINE_SIZE];
  char user_netbios[LINE_SIZE] = "";

  if (LogonUser((LPTSTR)"NETWORK SERVICE", (LPTSTR)"NT AUTHORITY", NULL, /*LOGON32_LOGON_NEW_CREDENTIALS*//*9, /*LOGON32_PROVIDER_WINNT50*//*3, &htoken))
  {
    if (htoken != 0)
    {
      ImpersonateLoggedOnUser(htoken);
    }
  }

  if (config->nb_accounts == 0)
  {
    snprintf(msg,LINE_SIZE,"NET ACCOUNT TEST:%s",config->login);
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

    if (config->domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config->domain,config->login);
    else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config->login);

    snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
    NETRESOURCE NetRes;
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;
    if (WNetAddConnection2(&NetRes,config->password,user_netbios,CONNECT_PROMPT)==NO_ERROR)
    {
      if (message)
      {
        snprintf(msg,LINE_SIZE,"%s\\IPC$ with %s account.",ip,user_netbios);
        AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");

        snprintf(msg,LINE_SIZE,"Login NET %s\\IPC$ with %s account",ip,user_netbios);
        AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
      }
      if (htoken == 0) htoken = (HANDLE)0;
    }else if (htoken != NULL)
    {
      CloseHandle(htoken);
      htoken = FALSE;
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
      #endif
    }

  }else if (config->global_ip_file)
  {
    snprintf(msg,LINE_SIZE,"NET ACCOUNT TEST:%s (%02d)",config->accounts[id_ip].login,id_ip);
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

    if (config->accounts[id_ip].domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config->accounts[id_ip].domain,config->accounts[id_ip].login);
    else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config->accounts[id_ip].login);

    snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
    NETRESOURCE NetRes;
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;
    if (WNetAddConnection2(&NetRes,config->accounts[id_ip].password,user_netbios,CONNECT_PROMPT)==NO_ERROR)
    {
      if (message)
      {
        snprintf(msg,LINE_SIZE,"%s\\IPC$ with %s (%02d) account.",ip,config->accounts[id_ip].login,id_ip);
        AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");

        snprintf(msg,LINE_SIZE,"Login NET %s with (%02d) account",user_netbios,id_ip);
        AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
      }
      if (htoken == 0) htoken = (HANDLE)0;
    }else if (htoken != NULL)
    {
      CloseHandle(htoken);
      htoken = FALSE;
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
      #endif
    }
  }else
  {
    unsigned int i = 0;
    if (id_ok != NULL && *id_ok > ID_ERROR) i = *id_ok;

    for (; i<config->nb_accounts ;i++)
    {
      snprintf(msg,LINE_SIZE,"NET ACCOUNT TEST:%s (%02d)",config->accounts[i].login,i);
      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

      if (config->accounts[i].domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config->accounts[i].domain,config->accounts[i].login);
      else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config->accounts[i].login);

      snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
      NETRESOURCE NetRes;
      NetRes.dwScope      = RESOURCE_GLOBALNET;
      NetRes.dwType	      = RESOURCETYPE_ANY;
      NetRes.lpLocalName  = (LPSTR)"";
      NetRes.lpProvider   = (LPSTR)"";
      NetRes.lpRemoteName	= remote_name;
      if (WNetAddConnection2(&NetRes,config->accounts[i].password,user_netbios,CONNECT_PROMPT)==NO_ERROR)
      {
        if (message)
        {
          snprintf(msg,LINE_SIZE,"%s\\IPC$ with %s (%02d) account.",ip,config->accounts[i].login,i);
          AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");

          snprintf(msg,LINE_SIZE,"Login NET %s with (%02d) account",user_netbios,i);
          AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
        }
        if (id_ok != NULL) *id_ok = i;
        if (htoken == 0) return (HANDLE)1;
        else return htoken;

      }else if (htoken != NULL)
      {
        CloseHandle(htoken);
        htoken = NULL;
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
        #endif
      }
    }
  }

  return htoken;
}*/
//----------------------------------------------------------------
HANDLE NetConnexionAuthenticateTest(char *ip, DWORD id_ip, char*remote_name, PSCANNE_ST config, DWORD iitem, BOOL message, long int *id_ok)
{
  HANDLE htoken = NULL;
  char msg[LINE_SIZE];
  char user_netbios[LINE_SIZE] = "";
  char user_netbios_n[LINE_SIZE] = "";

  snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
  NETRESOURCE NetRes;

  memset(&NetRes,0,sizeof(NETRESOURCE));
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpRemoteName	= remote_name;

  if (config->domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config->domain,config->login);
  else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config->login);

  snprintf(user_netbios_n,LINE_SIZE,"%s\\%s",ip,config->login);

 /* if (WNetAddConnection2(&NetRes,config->password,user_netbios,CONNECT_UPDATE_PROFILE)==NO_ERROR)
  {
    AddMsg(h_main,"DEBUG","WNetAddConnection2:OK",ip);

    if (LogonUser((LPTSTR)user_netbios, NULL, (LPTSTR)config->password, 9,3, &htoken))
    {
      AddMsg(h_main,"DEBUG","LogonUser:OK",ip);
      if (htoken != 0)
      {
        ImpersonateLoggedOnUser(htoken);
      }
    }
  }*/

  if (LogonUser((LPTSTR)user_netbios, NULL, (LPTSTR)config->password,9,3, &htoken))
  {
    //AddMsg(h_main,"DEBUG","LogonUser:OK0",ip);
    if (htoken != 0)
    {
      ImpersonateLoggedOnUser(htoken);
    }else
    {
      if (LogonUser((LPTSTR)user_netbios_n, (LPTSTR)config->domain, (LPTSTR)config->password, 9,3, &htoken))
      {
        //AddMsg(h_main,"DEBUG","LogonUser:OK1",ip);
        if (htoken != 0)
        {
          ImpersonateLoggedOnUser(htoken);
        }else
        {
          if (LogonUser((LPTSTR)"NETWORK SERVICE", (LPTSTR)"NT AUTHORITY", NULL, /*LOGON32_LOGON_NEW_CREDENTIALS*/9, /*LOGON32_PROVIDER_WINNT50*/3, &htoken))
          {
            //AddMsg(h_main,"DEBUG","LogonUser:OK2",ip);
            if (htoken != 0)
            {
              ImpersonateLoggedOnUser(htoken);
            }
          }
        }
      }
    }
  }
/*
  if (WNetAddConnection2(&NetRes,config->password,user_netbios,CONNECT_UPDATE_PROFILE)==NO_ERROR)
  {
    //AddMsg(h_main,"DEBUG","WNetAddConnection2:OK",ip);
  }*/

/*
  if (LogonUser((LPTSTR)"NETWORK SERVICE", (LPTSTR)"NT AUTHORITY", NULL, /*LOGON32_LOGON_NEW_CREDENTIALS*//*9, /*LOGON32_PROVIDER_WINNT50*//*3, &htoken))
  {
    if (htoken != 0)
    {
      ImpersonateLoggedOnUser(htoken);
    }
  }*/

  if (config->nb_accounts == 0)
  {
    snprintf(msg,LINE_SIZE,"NET ACCOUNT TEST:%s",config->login);
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

    if (config->domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config->domain,config->login);
    else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config->login);

    snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
    NETRESOURCE NetRes;
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;
    if (WNetAddConnection2(&NetRes,config->password,user_netbios,CONNECT_PROMPT)==NO_ERROR)
    {
      if (message)
      {
        snprintf(msg,LINE_SIZE,"%s\\IPC$ with %s account.",ip,user_netbios);
        AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");

        snprintf(msg,LINE_SIZE,"Login NET %s\\IPC$ with %s account",ip,user_netbios);
        AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
      }
      if (htoken == 0) htoken = (HANDLE)0;
    }else if (htoken != NULL)
    {
      CloseHandle(htoken);
      htoken = FALSE;
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
      #endif
    }

  }else if (config->global_ip_file)
  {
    snprintf(msg,LINE_SIZE,"NET ACCOUNT TEST:%s (%02d)",config->accounts[id_ip].login,id_ip);
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

    if (config->accounts[id_ip].domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config->accounts[id_ip].domain,config->accounts[id_ip].login);
    else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config->accounts[id_ip].login);

    snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
    NETRESOURCE NetRes;
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;
    if (WNetAddConnection2(&NetRes,config->accounts[id_ip].password,user_netbios,CONNECT_PROMPT)==NO_ERROR)
    {
      if (message)
      {
        snprintf(msg,LINE_SIZE,"%s\\IPC$ with %s (%02d) account.",ip,config->accounts[id_ip].login,id_ip);
        AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");

        snprintf(msg,LINE_SIZE,"Login NET %s with (%02d) account",user_netbios,id_ip);
        AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
      }
      if (htoken == 0) htoken = (HANDLE)0;
    }else if (htoken != NULL)
    {
      CloseHandle(htoken);
      htoken = FALSE;
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
      #endif
    }
  }else
  {
    unsigned int i = 0;
    if (id_ok != NULL && *id_ok > ID_ERROR) i = *id_ok;

    for (; i<config->nb_accounts ;i++)
    {
      snprintf(msg,LINE_SIZE,"NET ACCOUNT TEST:%s (%02d)",config->accounts[i].login,i);
      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

      if (config->accounts[i].domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config->accounts[i].domain,config->accounts[i].login);
      else snprintf(user_netbios,LINE_SIZE,"%s\\%s",ip,config->accounts[i].login);

      snprintf(remote_name,LINE_SIZE,"\\\\%s\\ipc$",ip);
      NETRESOURCE NetRes;
      NetRes.dwScope      = RESOURCE_GLOBALNET;
      NetRes.dwType	      = RESOURCETYPE_ANY;
      NetRes.lpLocalName  = (LPSTR)"";
      NetRes.lpProvider   = (LPSTR)"";
      NetRes.lpRemoteName	= remote_name;
      if (WNetAddConnection2(&NetRes,config->accounts[i].password,user_netbios,CONNECT_PROMPT)==NO_ERROR)
      {
        if (message)
        {
          snprintf(msg,LINE_SIZE,"%s\\IPC$ with %s (%02d) account.",ip,config->accounts[i].login,i);
          AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");

          snprintf(msg,LINE_SIZE,"Login NET %s with (%02d) account",user_netbios,i);
          AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
        }
        if (id_ok != NULL) *id_ok = i;
        if (htoken == 0) return (HANDLE)1;
        else return htoken;

      }else if (htoken != NULL)
      {
        CloseHandle(htoken);
        htoken = NULL;
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
        #endif
      }
    }
  }

  return htoken;
}
//----------------------------------------------------------------
BOOL ipIsLoclahost(char *ip)
{
  if (!strcmp("127.0.0.1",ip))return TRUE;
  return FALSE;
}
//----------------------------------------------------------------
DWORD WINAPI ScanIp(LPVOID lParam)
{
  DWORD index = (DWORD)lParam;
  char ip[MAX_PATH]="", dsc[MAX_PATH]="", dns[MAX_PATH]="", ttl_s[MAX_PATH]="", cfg[MAX_LINE_SIZE]="",test_title[MAX_PATH],msg[MAX_PATH];
  long long int iitem = ID_ERROR;
  int ttl = -1;
  BOOL exist  = FALSE, dnsok = FALSE, netBIOS = FALSE;
  BOOL windows_OS = FALSE;
  long int id_ok = ID_ERROR;

  if (SendDlgItemMessage(h_main, CB_IP, LB_GETTEXTLEN, (WPARAM)index,(LPARAM)NULL) > MAX_PATH)
  {
    //EnterCriticalSection(&Sync_threads_end);
    ReleaseSemaphore(hs_threads,1,NULL);
    hs_c_threads--;
    //LeaveCriticalSection(&Sync_threads_end);

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
    }else nb_test_ip++;
    return 0;
  }
  SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)index,(LPARAM)ip);

  if (SendDlgItemMessage(h_main, CB_DSC, LB_GETTEXTLEN, (WPARAM)index,(LPARAM)NULL) < MAX_PATH)
  {
    SendDlgItemMessage(h_main, CB_DSC, LB_GETTEXT, (WPARAM)index,(LPARAM)dsc);
  }

  if (ip[0]!=0)
  {
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_threads",ip);
    AddMsg(h_main,"DEBUG","SCAN:BEGIN",ip);
    #endif
    //check if exist + NetBIOS
    if (config.disco_icmp||config.disco_dns||config.disco_netbios)
    {
      //EnterCriticalSection(&Sync_threads);
      WaitForSingleObject(hs_disco,INFINITE);
      hs_c_disco++;
      //LeaveCriticalSection(&Sync_threads);

      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_disco",ip);
      #endif

      if (ip[0]> '9' || ip[0]< '0' || ((ip[1]> '9' || ip[1]< '0') && ip[1] != '.'))
      {
        //resolution inverse
        strncpy(dns,ip,MAX_PATH);

        struct in_addr **a;
        struct hostent *host;

        if ((host=gethostbyname(ip)))
        {
          a = (struct in_addr **)host->h_addr_list;
          snprintf(ip,16,"%s",inet_ntoa(**a));
          if (auto_scan_config.DNS_DISCOVERY)
          {
            exist = TRUE;
            dnsok = TRUE;
            iitem = AddLSTVItem(ip, dsc, dns, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
          }
        }else
        {
          iitem = AddLSTVItem((char*)"[ERROR DNS]", ip, dsc, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (char*)"OK");
          //EnterCriticalSection(&Sync_threads_end);
          ReleaseSemaphore(hs_disco,1,NULL);
          hs_c_disco--;
          //LeaveCriticalSection(&Sync_threads_end);

          //EnterCriticalSection(&Sync_threads_end);
          ReleaseSemaphore(hs_threads,1,NULL);
          hs_c_threads--;
          //LeaveCriticalSection(&Sync_threads_end);

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
            if (ttl <= MACH_LINUX)iitem = AddLSTVItem(ip, dsc, NULL, ttl_s, (char*)"Linux",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            else if (ttl <= MACH_WINDOWS)iitem = AddLSTVItem(ip, dsc, NULL, ttl_s, (char*)"Windows",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            else if (ttl <= MACH_WINDOWS)iitem = AddLSTVItem(ip, dsc, NULL, ttl_s, (char*)"Router",NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
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
            if (auto_scan_config.DNS_DISCOVERY)
            {
              iitem = AddLSTVItem(ip, dsc, dns, NULL, (char*)"Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
              exist = TRUE;
              dnsok = TRUE;
            }
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_DNS,dns);
            if (auto_scan_config.DNS_DISCOVERY)dnsok = TRUE;
          }
        }
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","DNS:END",ip);
        #endif
      }
      //EnterCriticalSection(&Sync_threads_end);
      ReleaseSemaphore(hs_disco,1,NULL);
      hs_c_disco--;
      //LeaveCriticalSection(&Sync_threads_end);

      //NetBIOS
      if (exist && (iitem > ID_ERROR) && (dnsok || !config.disco_dns) && config.disco_netbios && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","NetBIOS:BEGIN",ip);
        #endif
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"NetBIOS");
        //EnterCriticalSection(&Sync_threads);
        WaitForSingleObject(hs_netbios,INFINITE);
        hs_c_netbios++;
        //LeaveCriticalSection(&Sync_threads);

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
          snprintf(cfg,MAX_LINE_SIZE,"Domain:%s",domain);
          AddMsg(h_main, (char*)"FOUND (Config)",ip,cfg);
          AddLSTVUpdateItem(cfg, COL_CONFIG, iitem);
          netBIOS = TRUE;
        }

        //NULL session
        if (scan_start)
        {
          BOOL null_session = Netbios_NULLSessionStart(ip, "IPC$");
          if(null_session)
          {
            netBIOS     = TRUE;
            snprintf(cfg,MAX_LINE_SIZE,"NULL Session:Enable");
            AddMsg(h_main, (char*)"FOUND (Config)",ip,cfg);
            AddLSTVUpdateItem(cfg, COL_CONFIG, iitem);

            cfg[0] = 0;
            CheckReversSID(ip, cfg, MAX_LINE_SIZE);

            if (cfg[0] != 0)
            {
              AddMsg(h_main, (char*)"FOUND (Config)",ip,cfg);
              AddLSTVUpdateItem(cfg, COL_CONFIG, iitem);
            }
          }

          if (scan_start)
          {
            char c_time[MAX_PATH]="";
            wchar_t server[MAX_PATH];
            snprintf(tmp,MAX_PATH,"\\\\%s",ip);
            mbstowcs(server, tmp, MAX_PATH);
            Netbios_Time(server, c_time, MAX_PATH);
            if (c_time[0] != 0)
            {
              snprintf(cfg,MAX_LINE_SIZE,"Time:%s",c_time);
              AddMsg(h_main, (char*)"FOUND (Config)",ip,cfg);
              AddLSTVUpdateItem(cfg, COL_CONFIG, iitem);
            }

            //Share
            if (scan_start)
            {
              Netbios_Share(server, iitem, COL_SHARE, ip, TRUE);
            }
          }

          if(null_session)Netbios_NULLSessionStop(ip, "IPC$");
        }

        //EnterCriticalSection(&Sync_threads_end);
        ReleaseSemaphore(hs_netbios,1,NULL);
        hs_c_netbios--;
        //LeaveCriticalSection(&Sync_threads_end);

        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_netbios",ip);
        AddMsg(h_main,"DEBUG","NetBIOS:END",ip);
        #endif
      }

      //check if TCP 445 for Registry and file is open:
      if(((!config.disco_netbios)||(config.disco_netbios && netBIOS) || (ttl > 64) || (dnsok && ttl < 1)) && scan_start && exist)
      {
        if (TCP_port_open(iitem, ip, RPC_DEFAULT_PORT, FALSE))
        {
          //registry
          BOOL registry_remote = FALSE;
          if (config.check_registry || config.check_services || config.check_software || config.check_USB || config.write_key || config.disco_netbios_policy)
          {
            #ifdef DEBUG_MODE
            AddMsg(h_main,"DEBUG","registry:BEGIN",ip);
            #endif
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Registry");
            //EnterCriticalSection(&Sync_threads);
            WaitForSingleObject(hs_registry,INFINITE);
            hs_c_registry++;
            //LeaveCriticalSection(&Sync_threads);

            #ifdef DEBUG_MODE
            AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_registry",ip);
            #endif
            registry_remote = RemoteConnexionScan(iitem, ip, index, &config, windows_OS, &id_ok);
            if (registry_remote)nb_registry++;
            //EnterCriticalSection(&Sync_threads_end);
            ReleaseSemaphore(hs_registry,1,NULL);
            hs_c_registry--;
            //LeaveCriticalSection(&Sync_threads_end);
            #ifdef DEBUG_MODE
            AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_registry",ip);
            AddMsg(h_main,"DEBUG","registry:END",ip);
            #endif
          }

          //files
          if (config.check_files && scan_start)
          {
            #ifdef DEBUG_MODE
            AddMsg(h_main,"DEBUG","files:BEGIN",ip);
            #endif

            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files");
            //EnterCriticalSection(&Sync_threads);
            WaitForSingleObject(hs_file,INFINITE);
            hs_c_file++;
            //LeaveCriticalSection(&Sync_threads);
            #ifdef DEBUG_MODE
            AddMsg(h_main,"DEBUG","WaitForSingleObject-hs_file",ip);
            #endif

            RemoteConnexionFilesScan(iitem, ip, index, &config, &id_ok);

            //EnterCriticalSection(&Sync_threads_end);
            ReleaseSemaphore(hs_file,1,NULL);
            hs_c_file--;
            //LeaveCriticalSection(&Sync_threads_end);
            #ifdef DEBUG_MODE
            AddMsg(h_main,"DEBUG","ReleaseSemaphore-hs_file",ip);
            AddMsg(h_main,"DEBUG","files:END",ip);
            #endif
          }
        }else if (exist)
        {
          #ifndef DEBUG_NOERROR
          if (config.check_files)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_FILES, iitem);
          if (config.check_registry)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_REG, iitem);
          if (config.check_services)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_SERVICE, iitem);
          if (config.check_software)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_SOFTWARE, iitem);
          if (config.check_USB)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_USB, iitem);
          #endif
        }
      }else if(exist)
      {
        #ifndef DEBUG_NOERROR
        if (config.check_files)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_FILES, iitem);
        if (config.check_registry)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_REG, iitem);
        if (config.check_services)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_SERVICE, iitem);
        if (config.check_software)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_SOFTWARE, iitem);
        if (config.check_USB)AddLSTVUpdateItem((char*)"NOT TESTED!", COL_USB, iitem);
        #endif
      }

      //SSH
      if(exist && (config.check_ssh || config.check_ssh_os) && scan_start)
      {
        char tmp_os[MAX_MSG_SIZE]="";
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"SSH");

        //check if the port is open

        if (TCP_port_open(iitem, ip, SSH_DEFAULT_PORT, FALSE))
        {
          //EnterCriticalSection(&Sync_threads);
          WaitForSingleObject(hs_ssh,INFINITE);
          hs_c_ssh++;
          //LeaveCriticalSection(&Sync_threads);

          if (config.nb_accounts == 0)
          {
            snprintf(msg,MAX_PATH,"SSH ACCOUNT TEST:%s",config.login);
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

            int ret_ssh = ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.login, config.password, -1,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,TRUE,TRUE);
            if (ret_ssh == SSH_ERROR_OK)
            {
              if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "head -n 1 /etc/issue"))
              {
                ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                if (config.check_ssh)ssh_exec(iitem,ip, SSH_DEFAULT_PORT, config.login, config.password);
              }else  if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.login, config.password, -1,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE,FALSE) == SSH_ERROR_OK)
              {
                if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
                {
                  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                  if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.login, config.password);
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
          }else if(config.global_ip_file)
          {
            snprintf(msg,MAX_PATH,"SSH ACCOUNT TEST:%s (%02d)",config.accounts[index].login,index);
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

            int ret_ssh = ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[index].login, config.accounts[index].password, -1,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,TRUE,TRUE);
            if (ret_ssh == SSH_ERROR_OK)
            {
              if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "head -n 1 /etc/issue"))
              {
                ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                if (config.check_ssh)ssh_exec(iitem,ip, SSH_DEFAULT_PORT, config.accounts[index].login, config.accounts[index].password);
              }else  if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[index].login, config.accounts[index].password, -1,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE,FALSE) == SSH_ERROR_OK)
              {
                if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
                {
                  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                  if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[index].login, config.accounts[index].password);
                }
              }
            }else
            {
              #ifdef DEBUG_MODE_SSH
              char msg[MAX_LINE_SIZE];
              snprintf(msg,MAX_LINE_SIZE,"%s:22 account %s (%d)",ip,config.accounts[index].login,ret_ssh);
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

              snprintf(msg,MAX_PATH,"SSH ACCOUNT TEST:%s (%02d)",config.accounts[j].login,j);
              ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

              ret_ssh = ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].password, j,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,first_msg,msg_auth);
              if (ret_ssh == SSH_ERROR_OK)
              {
                msg_auth = FALSE;
                if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "head -n 1 /etc/issue"))
                {
                  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                  if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].password);
                  break;
                }else if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].password, j,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE, FALSE) == SSH_ERROR_OK)
                {
                  if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
                  {
                    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                    if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].password);
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
          //EnterCriticalSection(&Sync_threads_end);
          ReleaseSemaphore(hs_ssh,1,NULL);
          hs_c_ssh--;
          //LeaveCriticalSection(&Sync_threads_end);
        #ifndef DEBUG_NOERROR
        }else AddLSTVUpdateItem((char*)"NOT TESTED! (port 22/TCP not open)", COL_SSH, iitem);
        #else
        }
        #endif
      }

      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","SCAN:END",ip);
      #endif

      if (exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"OK");
    }
  }
  //EnterCriticalSection(&Sync_threads_end);
  ReleaseSemaphore(hs_threads,1,NULL);
  hs_c_threads--;
  //LeaveCriticalSection(&Sync_threads_end);

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
  }else nb_test_ip++;
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI threads_count(LPVOID lParam)
{
  char tmp[MAX_PATH];
  while (hs_count != 0)
  {
    snprintf(tmp,MAX_PATH,"threads:%d, disco:%d, netbios:%d, files:%d, registry:%d, ssh:%d, tcp:%d",hs_c_threads, hs_c_disco, hs_c_netbios, hs_c_file, hs_c_registry, hs_c_ssh, hs_c_tcp);
    AddMsg(h_main,(char*)"DEBUG",(char*)"THREADS",(char*)tmp);
    Sleep(1000);
  }
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI scan(LPVOID lParam)
{
  BOOL auto_save = (BOOL)lParam;

  time_t exec_time_start, exec_time_end;
  time(&exec_time_start);

  #ifdef DEBUG_MODE
  AddMsg(h_main,(char*)"DEBUG",(char*)"scan",(char*)"time:OK");
  #endif

  //load IP
  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    //load IP interval
    SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);
    SendDlgItemMessage(h_main,CB_DSC,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

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

    if ((L21 | L22 | L23 | L24 | L11 | L12 | L13 | L14) == 0){}
    if ((L21 | L22 | L23 | L24) == 0)
    {
      char sip1[IP_SIZE];
      snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      addIPTest(sip1,"");
    }else if ((L11 | L12 | L13 | L14) == 0)
    {
      char sip2[IP_SIZE];
      snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);
      addIPTest(sip2,"");
    }else
    {
      char sip1[IP_SIZE],sip2[IP_SIZE];
      snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
      snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);
      addIPInterval(sip1, sip2,"");
    }
  }
  #ifdef DEBUG_MODE
  AddMsg(h_main,(char*)"DEBUG",(char*)"scan",(char*)"load-IP first:OK");
  #endif

  char tmp[MAX_PATH];
  snprintf(tmp,LINE_SIZE,"Loaded %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
  AddMsg(h_main,(char*)"INFORMATION",tmp,(char*)"");

  //load config
  unsigned int ref  = 0;
  nb_files          = 0;
  nb_registry       = 0;
  nb_windows        = 0;

  //check if no tests enable
  if (SendDlgItemMessage(h_main,CB_tests,LB_GETSELCOUNT,(WPARAM)NULL,(LPARAM)NULL) == 0)
  {
    AddMsg(h_main,(char*)"ERROR",(char*)"No test select from the left panel!",(char*)"");
  }

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
  if (config.check_files)
  {
    config.check_files    = (BOOL)load_file_list(CB_T_FILES,     (char*)DEFAULT_LIST_FILES);
    config.check_files    += (BOOL)load_file_list(CB_T_MULFILES, (char*)DEFAULT_LIST_MULFILES);
  }

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
    GetWindowText(GetDlgItem(h_main,ED_NET_PASSWORD),config.password,MAX_PATH);
    GetWindowText(GetDlgItem(h_main,ED_NET_DOMAIN),config.domain,MAX_PATH);
  }else
  {
    config.local_account = TRUE;
    config.login[0]      = 0;
    config.domain[0]     = 0;
    config.password[0]   = 0;
  }

  //---------------------------------------------
  //scan_start
  DWORD i;
  nb_i = SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

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

  if (nb_i == 1)
  {
    char tmp_IP[MAX_PATH]="";
    SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)0,(LPARAM)tmp_IP);
    if (tmp_IP[0] == 0 || (tmp_IP[0] == '0' && tmp_IP[1] == '.'))
      nb_i = 0;
  }

  //check for SHA1
  char ini_path[LINE_SIZE]="";
  char tmp_check[LINE_SIZE]="";
  strncat(GetLocalPath(ini_path, LINE_SIZE),AUTO_SCAN_FILE_INI,LINE_SIZE);

  if(GetPrivateProfileString("SCAN","SHA1_ONLY","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y')SHA1_enable = TRUE;
    else SHA1_enable = FALSE;

  }

  hs_c_threads  = 0;
  hs_c_disco    = 0;
  hs_c_netbios  = 0;
  hs_c_file     = 0;
  hs_c_registry = 0;
  hs_c_ssh      = 0;
  hs_c_tcp      = 0;

  for (i=0;(i<nb_i) && scan_start;i++)
  {
    //ScanIp((LPVOID)i);
    //EnterCriticalSection(&Sync_threads);
    WaitForSingleObject(hs_threads,INFINITE);
    hs_c_threads++;
    //LeaveCriticalSection(&Sync_threads);

    CreateThread(NULL,0,ScanIp,(PVOID)i,0,0);
  }

  //wait
  AddMsg(h_main,(char*)"INFORMATION",(char*)"Start waiting threads.",(char*)"");

  if (!scan_start)
  {
    DWORD end = 0;
    while ((nb_test_ip < i) && (end < THE_END_THREAD_WAIT)){Sleep(100);end++;}
  }else
  {
    //EnterCriticalSection(&Sync_threads);
    for(i=0;i<NB_MAX_THREAD;i++){WaitForSingleObject(hs_threads,INFINITE);hs_c_threads++;}
    //for(i=0;i<NB_MAX_THREAD;i++)WaitForSingleObject(hs_threads,THREAD_MAX_TIMEOUT);
    //LeaveCriticalSection(&Sync_threads);

    //EnterCriticalSection(&Sync_threads);
    WaitForSingleObject(hs_netbios,INFINITE);
    hs_c_netbios++;
    //LeaveCriticalSection(&Sync_threads);

    //EnterCriticalSection(&Sync_threads);
    WaitForSingleObject(hs_file,INFINITE);
    hs_c_file++;
    //LeaveCriticalSection(&Sync_threads);

    //EnterCriticalSection(&Sync_threads);
    WaitForSingleObject(hs_registry,INFINITE);
    hs_c_registry++;
    //LeaveCriticalSection(&Sync_threads);

    //EnterCriticalSection(&Sync_threads);
    WaitForSingleObject(hs_tcp,INFINITE);
    hs_c_tcp++;
    //LeaveCriticalSection(&Sync_threads);

    //EnterCriticalSection(&Sync_threads);
    WaitForSingleObject(hs_ssh,INFINITE);
    hs_c_ssh++;
    //LeaveCriticalSection(&Sync_threads);
  }

  WSACleanup();

  //calcul run time
  time(&exec_time_end);

  AddMsg(h_main,(char*)"INFORMATION",(char*)"End of scan!",(char*)"");
  snprintf(tmp,MAX_PATH,"Ip view:%lu/%lu in %d.%0d minutes",ListView_GetItemCount(GetDlgItem(h_main,LV_results)),nb_i,(exec_time_end - exec_time_start)/60,(exec_time_end - exec_time_start)%60);
  AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");

  if (config.check_files)
  {
    snprintf(tmp,MAX_PATH,"Remote file authentication OK:%lu/%lu",nb_files,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");
  }

  if (config.check_registry || config.check_services || config.check_software || config.check_USB || config.write_key)
  {
    snprintf(tmp,MAX_PATH,"Remote registry authentication OK:%lu/%lu",nb_registry,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");
    snprintf(tmp,MAX_PATH,"Computer in Microsoft Windows OS:%lu/%lu",nb_windows,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"");
  }

  //close
  CloseHandle(hs_threads);
  CloseHandle(hs_disco);
  CloseHandle(hs_netbios);
  CloseHandle(hs_file);
  CloseHandle(hs_registry);
  CloseHandle(hs_tcp);
  CloseHandle(hs_ssh);

  //---------------------------------------------
  //for mode simple
  if (auto_save)
  {
    if (ListView_GetItemCount(GetDlgItem(h_main,LV_results)) >0)
    {
      char file2[LINE_SIZE];
      time_t dateEtHMs;
      time(&dateEtHMs);
      struct tm *today = localtime(&dateEtHMs);
      char date[DATE_SIZE];
      strftime(date, DATE_SIZE,"%Y.%m.%d-%H.%M.%S",today);

      char cpath[LINE_SIZE]="";
      GetLocalPath(cpath, LINE_SIZE);

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SAVE","CSV","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'o' || tmp_check[0] == 'O')
        {
          snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.csv",cpath,date);
          if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_CSV, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
          else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);

          save_done = TRUE;
        }
        tmp_check[0] = 0;
      }

      if(GetPrivateProfileString("SAVE","XML","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'o' || tmp_check[0] == 'O')
        {
          snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.xml",cpath,date);
          if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_XML, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
          else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);

          save_done = TRUE;
        }
        tmp_check[0] = 0;
      }

      if(GetPrivateProfileString("SAVE","HTML","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'o' || tmp_check[0] == 'O')
        {
          snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.html",cpath,date);
          if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_HTML, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
          else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);

          save_done = TRUE;
        }
        tmp_check[0] = 0;
      }
    }
  }

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
  EnableWindow(GetDlgItem(h_main,BT_RE),TRUE);
  scan_start = FALSE;

  SetWindowText(GetDlgItem(h_main,BT_START),"Start");
  SetWindowText(h_main,TITLE);
  h_thread_scan = 0;
  hs_count = 0;
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI SaveWorld(LPVOID lParam)
{
  char file[MAX_PATH]= "";
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize    = sizeof(OPENFILENAME);
  ofn.hwndOwner      = h_main;
  ofn.lpstrFile      = file;
  ofn.nMaxFile       = MAX_PATH;
  ofn.nFilterIndex   = 1;
  ofn.Flags          = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  ofn.lpstrFilter    = "*.xml \0*.xml\0*.csv \0*.csv\0*.html \0*.html\0All formats\0*.*\0Log file\0*.txt\0";
  ofn.lpstrDefExt    = "\0";
  if (GetSaveFileName(&ofn)==TRUE)
  {
    if (ofn.nFilterIndex == SAVE_TYPE_ALL)
    {
      char file2[MAX_PATH];
      snprintf(file2,MAX_PATH,"%s.xml",file);
      if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_XML, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
      else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);

      snprintf(file2,MAX_PATH,"%s.csv",file);
      if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_CSV, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
      else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);

      snprintf(file2,MAX_PATH,"%s.html",file);
      if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_HTML, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2);
      else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2);
    }else if (ofn.nFilterIndex == SAVE_TYPE_LOG)
    {
      char file2[MAX_PATH];
      snprintf(file2,MAX_PATH,"%s.txt",file);
      SaveLV(GetDlgItem(h_main,CB_infos), file2);
    }else
    {
      if(SaveLSTV(GetDlgItem(h_main,LV_results), file, ofn.nFilterIndex, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file);
      else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file);
    }

    save_done = TRUE;
    save_current = FALSE;
  }
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
        DeleteCriticalSection(&Sync_item);
        DeleteCriticalSection(&Sync_threads);
        DeleteCriticalSection(&Sync_threads_end);

        if (h_log != INVALID_HANDLE_VALUE)CloseHandle(h_log);
        if (hndlIcmp != 0)FreeLibrary((HMODULE)hndlIcmp);
        if (hDLL_kernel32 != 0)FreeLibrary((HMODULE)hDLL_kernel32);
        FreeLibrary(richDll);
        EndDialog(hwnd,0);
        return 0;
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
              if (!save_current)
              {
                save_current = TRUE;
                CreateThread(NULL,0,SaveWorld,0,0,0);
              }
            }
            break;
            //------------------------------
            case BT_RE:
              if (scan_start == FALSE)
              {
                scan_start = TRUE;
                EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),FALSE);
                EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),FALSE);
                EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),FALSE);
                EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),FALSE);
                EnableWindow(GetDlgItem(h_main,IP1),FALSE);
                EnableWindow(GetDlgItem(h_main,BT_IP_CP),FALSE);
                EnableWindow(GetDlgItem(h_main,IP2),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),FALSE);
                EnableWindow(GetDlgItem(h_main,CB_tests),FALSE);
                EnableWindow(GetDlgItem(h_main,BT_LOAD_MDP_FILES),FALSE);
                EnableWindow(GetDlgItem(h_main,BT_START),FALSE);

                ListView_DeleteAllItems(GetDlgItem(h_main,LV_results));
                SendDlgItemMessage(h_main,CB_infos,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

                SetWindowText(GetDlgItem(h_main,BT_RE),"Stop");
                AddMsg(h_main, (char*)"INFORMATION",(char*)"Start remote extract",(char*)"");

                //critical section
                InitializeCriticalSection(&Sync);
                InitializeCriticalSection(&Sync_item);

                h_thread_scan = CreateThread(NULL,0,remote_extract,0,0,0);
              }else
              {
                scan_start = FALSE;
                EnableWindow(GetDlgItem(h_main,BT_RE),FALSE);
              }
            break;
            case BT_START:
              if (scan_start == FALSE)
              {
                scan_start = TRUE;
                EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),FALSE);
                EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),FALSE);
                EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),FALSE);
                EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),FALSE);
                EnableWindow(GetDlgItem(h_main,IP1),FALSE);
                EnableWindow(GetDlgItem(h_main,BT_IP_CP),FALSE);
                EnableWindow(GetDlgItem(h_main,IP2),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),FALSE);
                EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),FALSE);
                EnableWindow(GetDlgItem(h_main,CB_tests),FALSE);
                EnableWindow(GetDlgItem(h_main,BT_LOAD_MDP_FILES),FALSE);
                EnableWindow(GetDlgItem(h_main,BT_RE),FALSE);

                ListView_DeleteAllItems(GetDlgItem(h_main,LV_results));
                SendDlgItemMessage(h_main,CB_infos,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

                SetWindowText(GetDlgItem(h_main,BT_START),"Stop");
                AddMsg(h_main, (char*)"INFORMATION",(char*)"Start scan",(char*)"");

                //critical section
                InitializeCriticalSection(&Sync);
                InitializeCriticalSection(&Sync_item);

                #ifdef DEBUG_MODE
                AddMsg(h_main,(char*)"DEBUG",(char*)"scan",(char*)"CreateThread:START");
                #endif
                h_thread_scan = CreateThread(NULL,0,scan,(PVOID)(BOOL)(auto_scan_config.save_CSV+auto_scan_config.save_HTML+auto_scan_config.save_XML),0,0);
                hs_count = 0;
                #ifdef DEBUG_THREADS
                hs_count = CreateThread(NULL,0,threads_count,0,0,0);
                #endif

              }else
              {
                scan_start = FALSE;
                EnableWindow(GetDlgItem(h_main,BT_START),FALSE);
              }
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
          if (!scan_start && (IsWindowEnabled(GetDlgItem(h_main,BT_START)) != 0))
          {
            tri_order = !tri_order;
            c_Tri(GetDlgItem(hwnd,LV_results),((LPNMLISTVIEW)lParam)->iSubItem,tri_order);
          }
        break;
        case NM_DBLCLK:
        {
          char tmp[MAX_MSG_SIZE+1], tmp2[MAX_MSG_SIZE+1];
          long int i, index = SendDlgItemMessage(hwnd,LV_results,LVM_GETNEXTITEM,(WPARAM)-1,(LPARAM)LVNI_FOCUSED);
          RichEditInit(hdbclk_info);

          if (index != -1)
          {
            LVCOLUMN lvc;
            lvc.mask        = LVCF_TEXT;
            lvc.cchTextMax  = MAX_MSG_SIZE;
            lvc.pszText     = tmp;

            for (i=0;i<NB_COLUMN;i++)
            {
              tmp[0] = 0;
              tmp2[0] = 0;
              if (SendMessage(GetDlgItem(hwnd,LV_results),LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) != 0)
              {
                if (strlen(tmp)>0)
                {
                  ListView_GetItemText(GetDlgItem(hwnd,LV_results),index,i,tmp2,MAX_MSG_SIZE);

                  if (strlen(tmp2)>0)
                  {
                    RichEditCouleur(hdbclk_info,NOIR,"\r\n[");
                    RichEditCouleurGras(hdbclk_info,NOIR,tmp);
                    RichEditCouleur(hdbclk_info,NOIR,"]\r\n");
                    RichEditCouleur(hdbclk_info,NOIR,tmp2);
                    RichEditCouleur(hdbclk_info,NOIR,"\r\n");
                  }
                }
              }
            }
            RichSetTopPos(hdbclk_info);
            ShowWindow (hdbclk_info, SW_SHOW);
          }
          RichSetTopPos(hdbclk_info);
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
        MoveWindow(GetDlgItem(hwnd,CB_tests)  ,0  ,90         ,200        ,mHeight-95 ,TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_results),202,90         ,mWidth-204 ,mHeight-232,TRUE);
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
