//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
HANDLE h_filter_th, h_load_th;
//------------------------------------------------------------------------------
void ShowOnglet(DWORD id)
{
  ShowWindow(GetDlgItem(h_state,DLG_STATE_LV_ALL), SW_HIDE);
  ShowWindow(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL), SW_HIDE);
  ShowWindow(GetDlgItem(h_state,DLG_STATE_LV_TIME_ZONE), SW_HIDE);
  ShowWindow(GetDlgItem(h_state,DLG_STATE_LV_FILTER), SW_HIDE);

  ShowWindow(GetDlgItem(h_state,id), SW_SHOW);
}
//------------------------------------------------------------------------------
void InitGuiState()
{
  //tri
  TRI_STATE_ALL       = FALSE;
  TRI_STATE_DATE      = FALSE;
  TRI_STATE_CRITICAL  = FALSE;
  TRI_STATE_FILTER    = FALSE;
  h_filter_th         = 0;
  h_load_th           = 0;
  SELECT_SESSION      = TRUE;
  SELECT_TEST         = TRUE;

  //init list of sessions
  char tmp[DEFAULT_TMP_SIZE];
  int i,nb = SendMessage(hCombo_session,CB_GETCOUNT,0,(LPARAM)0);
  SendDlgItemMessage(h_state,DLG_STATE_LB_SESSION,LB_RESETCONTENT,0,0);
  for (i=nb;i>0;i--)
  {
    if (SendMessage(hCombo_session, CB_GETLBTEXTLEN,i-1,0) < DEFAULT_TMP_SIZE)
    {
      tmp[0] = 0;
      if(SendMessage(hCombo_session, CB_GETLBTEXT,i-1,(LPARAM)tmp) > 0)
      {
        SendDlgItemMessage(h_state,DLG_STATE_LB_SESSION,LB_ADDSTRING,0,tmp);
      }
    }
  }

  //init list of tests
  nb = SendMessage(hlstbox, LB_GETCOUNT,0,(LPARAM)0);
  SendDlgItemMessage(h_state,DLG_STATE_LB_TEST,LB_RESETCONTENT,0,0);
  for (i=0;i<nb;i++)
  {
    if (SendMessage(hlstbox, LB_GETTEXTLEN,i,0) < DEFAULT_TMP_SIZE)
    {
      tmp[0] = 0;
      if(SendMessage(hlstbox, LB_GETTEXT,i,(LPARAM)tmp) > 0)
      {
        SendDlgItemMessage(h_state,DLG_STATE_LB_TEST,LB_ADDSTRING,0,tmp);
      }
    }
  }

  //init text
  SetWindowText(GetDlgItem(h_state,DLG_STATE_ED_TIME_1),"2012/11/18 11:11:11");
  SetWindowText(GetDlgItem(h_state,DLG_STATE_ED_TIME_2),"2012/11/18 11:11:12");

  //status bar
  SendDlgItemMessage(h_state,DLG_STATE_SB,SB_SETTEXT,0, (LPARAM)"");

  ShowOnglet(DLG_STATE_LV_ALL);
}
//------------------------------------------------------------------------------
DWORD WINAPI Filter_state(LPVOID lParam)
{
  //get if date or note
  char tmp1[DEFAULT_TMP_SIZE]="", tmp2[DEFAULT_TMP_SIZE]="";
  GetWindowText(GetDlgItem(h_state,DLG_STATE_ED_TIME_1),tmp1,DEFAULT_TMP_SIZE);
  GetWindowText(GetDlgItem(h_state,DLG_STATE_ED_TIME_2),tmp2,DEFAULT_TMP_SIZE);

  LVITEM lvi;
  lvi.mask = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem = 0;
  lvi.lParam = LVM_SORTITEMS;
  lvi.pszText="";

  HANDLE hlv_src = GetDlgItem(h_state,DLG_STATE_LV_ALL), hlv_dst = GetDlgItem(h_state,DLG_STATE_LV_FILTER);

  if (tmp2[0] == 0 || tmp1[4]!='/' || tmp1[7]!='/' || tmp1[10]!=' ' || tmp1[13]!=':' || tmp1[16]!=':')
  {
    //string search
    DWORD i, j,k, ref_item, nb_item = ListView_GetItemCount(hlv_src);
    char tmp[MAX_LINE_SIZE];
    for (i=0;i<nb_item;i++)
    {
      for (j=0;j<8;j++)
      {
        //récupération du text
        tmp[0]=0;
        ListView_GetItemText(hlv_src,i,j,tmp,MAX_LINE_SIZE);

        //test si la recherche est présente dedans
        if (ContientNoCass(tmp,tmp1))
        {
          //add item
          lvi.iItem = ListView_GetItemCount(hlv_dst);
          ref_item = ListView_InsertItem(hlv_dst, &lvi);
          for(k=0;k<8;k++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv_src,i,k,tmp,MAX_LINE_SIZE);
            ListView_SetItemText(hlv_dst,ref_item,k,tmp);
          }
          break;
        }
      }
    }
  }else //intervalls
  {
    //string search
    DWORD i,k, ref_item, nb_item = ListView_GetItemCount(hlv_src);
    int a,b;
    char tmp[MAX_LINE_SIZE];
    for (i=0;i<nb_item;i++)
    {
      tmp[0]=0;
      ListView_GetItemText(hlv_src,i,0,tmp,MAX_LINE_SIZE);
      if (tmp[0] != 0)
      {
        a = strcmp(tmp1, tmp);
        b = strcmp(tmp2, tmp);

        if (a>=0 && b<=0)
        {
          //add item
          lvi.iItem = ListView_GetItemCount(hlv_dst);
          ref_item = ListView_InsertItem(hlv_dst, &lvi);
          for(k=0;k<8;k++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv_src,i,k,tmp,MAX_LINE_SIZE);
            ListView_SetItemText(hlv_dst,ref_item,k,tmp);
          }
        }
      }
    }
  }

  SetWindowText(GetDlgItem(h_state,DLG_STATE_BT_FILTER),"Filter");
  h_filter_th = 0;
  return 0;
}
//------------------------------------------------------------------------------
char current_global_request[MAX_LINE_SIZE];
unsigned int type_format_current_global_request;
char src_name[MAX_PATH];
char session_state[MAX_PATH];
int callback_sqlite_state(void *datas, int argc, char **argv, char **azColName)
{
  if (argv[0] == 0)return 0;
  FORMAT_CALBAK_TYPE *type = datas;
  switch(type->type)
  {
    //----------------------------
    //get request
    case TYPE_SQLITE_FLAG_GET_ITEMS_INFO:
      type_format_current_global_request = atoi(argv[0]);
      snprintf(current_global_request,MAX_LINE_SIZE,"%s",argv[1]);
    break;
    //----------------------------
    //tests
    case 0://file
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item2= ListView_InsertItem(hlv, &lvi);
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item3= ListView_InsertItem(hlv, &lvi);

      //set text
      char file[MAX_LINE_SIZE];
      snprintf(file,MAX_LINE_SIZE,"%s%s",argv[0],argv[1]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"Size:%s%s%s%s%s%s,ADS:[%s],SAH256:%s,VirusTotal:%s,Description:%s",
               argv[6],strlen(argv[11])?",Hidden":"",strlen(argv[12])?",System":"",strlen(argv[13])?",Archive":"",strlen(argv[14])?",Encrypted":"",strlen(argv[15])?",Tempory":"",argv[16],argv[17],argv[18],argv[19]);

      ListView_SetItemText(hlv,ref_item,0,argv[3]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[3]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src_name);    //Source
      ListView_SetItemText(hlv,ref_item,3,description); //Description
      ListView_SetItemText(hlv,ref_item,4,file);        //Path+File
      ListView_SetItemText(hlv,ref_item,5,argv[7]);     //Owner
      ListView_SetItemText(hlv,ref_item,6,argv[9]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session

      ListView_SetItemText(hlv,ref_item2,0,argv[4]);    //date
      ListView_SetItemText(hlv,ref_item2,1,azColName[4]);//Origine
      ListView_SetItemText(hlv,ref_item2,2,src_name);   //Source
      ListView_SetItemText(hlv,ref_item2,3,description);//Description
      ListView_SetItemText(hlv,ref_item2,4,file);       //Path+File
      ListView_SetItemText(hlv,ref_item2,5,argv[7]);    //Owner
      ListView_SetItemText(hlv,ref_item2,6,argv[9]);    //SID
      ListView_SetItemText(hlv,ref_item2,7,session_state);//Session

      ListView_SetItemText(hlv,ref_item3,0,argv[5]);    //date
      ListView_SetItemText(hlv,ref_item3,1,azColName[5]);//Origine
      ListView_SetItemText(hlv,ref_item3,2,src_name);   //Source
      ListView_SetItemText(hlv,ref_item3,3,description);//Description
      ListView_SetItemText(hlv,ref_item3,4,file);       //Path+File
      ListView_SetItemText(hlv,ref_item3,5,argv[7]);    //Owner
      ListView_SetItemText(hlv,ref_item3,6,argv[9]);    //SID
      ListView_SetItemText(hlv,ref_item3,7,session_state);//Session
    }
    break;
    case 1://log
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item2= ListView_InsertItem(hlv, &lvi);

      //set text
      char source[MAX_LINE_SIZE];
      snprintf(source,MAX_LINE_SIZE,"%s\\%s:%s",src_name,argv[0],argv[4]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"%s,%s",argv[11],argv[6]);

      ListView_SetItemText(hlv,ref_item,0,argv[2]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[2]);//Origine
      ListView_SetItemText(hlv,ref_item,2,source);      //Source
      ListView_SetItemText(hlv,ref_item,3,argv[7]);     //state
      ListView_SetItemText(hlv,ref_item,4,description); //Description
      ListView_SetItemText(hlv,ref_item,5,argv[8]);     //Owner
      ListView_SetItemText(hlv,ref_item,6,argv[10]);    //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session

      ListView_SetItemText(hlv,ref_item2,0,argv[3]);    //date
      ListView_SetItemText(hlv,ref_item2,1,azColName[3]);//Origine
      ListView_SetItemText(hlv,ref_item2,2,source);     //Source
      ListView_SetItemText(hlv,ref_item2,3,argv[7]);    //state
      ListView_SetItemText(hlv,ref_item2,4,description);//Description
      ListView_SetItemText(hlv,ref_item2,5,argv[8]);    //Owner
      ListView_SetItemText(hlv,ref_item2,6,argv[10]);   //SID
      ListView_SetItemText(hlv,ref_item2,7,session_state);//Session

      if (strlen(argv[12])) //critical
      {
        hlv       = GetDlgItem(h_state,DLG_STATE_LV_CRITICAL);
        lvi.iItem = ListView_GetItemCount(hlv);
        ref_item  = ListView_InsertItem(hlv, &lvi);

        ListView_SetItemText(hlv,ref_item,0,argv[2]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[2]);//Origine
        ListView_SetItemText(hlv,ref_item,2,source);      //Source
        ListView_SetItemText(hlv,ref_item,3,argv[7]);     //state
        ListView_SetItemText(hlv,ref_item,4,description); //Description
        ListView_SetItemText(hlv,ref_item,5,argv[8]);     //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[10]);    //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session
      }
    }
    break;
    /*
    Disables :
    2:diques
    3:clipboard
    4:env
    */
    case 5: //AT
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item2= ListView_InsertItem(hlv, &lvi);
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item3= ListView_InsertItem(hlv, &lvi);

      //set text
      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"%s:%s,%s:%s,%s:%s,%s:%s",
               azColName[1],argv[1],azColName[0],argv[0],azColName[2],argv[2],azColName[3],argv[3]);

      ListView_SetItemText(hlv,ref_item,0,argv[4]);     //date create
      ListView_SetItemText(hlv,ref_item,1,azColName[4]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src_name);    //Source
      ListView_SetItemText(hlv,ref_item,3,description); //Description
      ListView_SetItemText(hlv,ref_item,4,argv[8]);     //details
      ListView_SetItemText(hlv,ref_item,5,argv[7]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[9]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session

      ListView_SetItemText(hlv,ref_item2,0,argv[5]);    //date update
      ListView_SetItemText(hlv,ref_item2,1,azColName[5]);//Origine
      ListView_SetItemText(hlv,ref_item2,2,src_name);   //Source
      ListView_SetItemText(hlv,ref_item2,3,description);//Description
      ListView_SetItemText(hlv,ref_item2,4,argv[8]);    //details
      ListView_SetItemText(hlv,ref_item2,5,argv[7]);    //Owner
      //ListView_SetItemText(hlv,ref_item2,6,argv[9]);    //SID
      ListView_SetItemText(hlv,ref_item2,7,session_state);//Session

      ListView_SetItemText(hlv,ref_item3,0,argv[6]);    //date access
      ListView_SetItemText(hlv,ref_item3,1,azColName[6]);//Origine
      ListView_SetItemText(hlv,ref_item3,2,src_name);   //Source
      ListView_SetItemText(hlv,ref_item3,3,description);//Description
      ListView_SetItemText(hlv,ref_item3,4,argv[8]);    //details
      ListView_SetItemText(hlv,ref_item3,5,argv[7]);    //Owner
      //ListView_SetItemText(hlv,ref_item3,6,argv[9]);    //SID
      ListView_SetItemText(hlv,ref_item3,7,session_state);//Session
    }
    break;
    case 6: //Process
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char description[MAX_LINE_SIZE];
      if (strlen(argv[1]))snprintf(description,MAX_LINE_SIZE,"[%s:%s,%s:%s] %s %s:%s->%s:%s", azColName[1],argv[1],azColName[16],argv[16],argv[8],argv[9],argv[10],argv[11],argv[12]);
      else snprintf(description,MAX_LINE_SIZE,"[%s:%s,%s:%s]", azColName[1],argv[1],azColName[16],argv[16]);

      ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src_name);    //Source
      ListView_SetItemText(hlv,ref_item,3,description); //description
      ListView_SetItemText(hlv,ref_item,4,strlen(argv[3])?argv[3]:argv[0]); //Process
      ListView_SetItemText(hlv,ref_item,5,argv[4]);     //Owner
      ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session

      if (strlen(argv[14])) //hidden
      {
        hlv       = GetDlgItem(h_state,DLG_STATE_LV_CRITICAL);
        lvi.iItem = ListView_GetItemCount(hlv);
        ref_item  = ListView_InsertItem(hlv, &lvi);

        ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
        ListView_SetItemText(hlv,ref_item,2,src_name);    //Source
        ListView_SetItemText(hlv,ref_item,3,description); //description
        ListView_SetItemText(hlv,ref_item,4,strlen(argv[3])?argv[3]:argv[0]); //Process
        ListView_SetItemText(hlv,ref_item,5,argv[4]);     //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session
      }
    }
    break;
    /*
    Disable:
    7:pipe
    */
    case 8://network
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s",src_name,argv[0]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"%s/%s gw:%s DNS:%s WiFi:%s",argv[4],argv[5],argv[6],argv[7],argv[11]);

      ListView_SetItemText(hlv,ref_item,0,argv[12]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[12]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);          //Source
      ListView_SetItemText(hlv,ref_item,3,description);  //desc
      ListView_SetItemText(hlv,ref_item,4,argv[2]);      //description
      //ListView_SetItemText(hlv,ref_item,5,argv[4]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    /*
    Disable:
    9:route
    10:DNS cache
    11:ARP
    12:share
    */
    case 13: //configuration registre
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"%s=%s",argv[3],argv[4]);

      ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,argv[6]);     //desc
      ListView_SetItemText(hlv,ref_item,4,description); //description
      //ListView_SetItemText(hlv,ref_item,5,argv[4]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    /*
    Disable:
    14:services
    */
    case 15: //USB
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"[%s]%s(%s)",argv[8],argv[3],argv[7]);

      ListView_SetItemText(hlv,ref_item,0,argv[9]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[9]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,argv[6]);     //desc
      ListView_SetItemText(hlv,ref_item,4,description);      //description
      //ListView_SetItemText(hlv,ref_item,5,argv[4]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    case 16: //software
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"%s=%s",azColName[5],argv[5]);

      ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,description);     //desc
      ListView_SetItemText(hlv,ref_item,4,argv[3]);     //description
      ListView_SetItemText(hlv,ref_item,5,argv[8]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    case 17: //update
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"%s, %s",argv[3],argv[5]);

      ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,description);     //desc
      ListView_SetItemText(hlv,ref_item,4,argv[4]);     //description
      ListView_SetItemText(hlv,ref_item,5,argv[5]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    case 18: //runs
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

      ListView_SetItemText(hlv,ref_item,0,argv[5]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[5]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,argv[3]);     //desc
      ListView_SetItemText(hlv,ref_item,4,argv[4]);     //description
      //ListView_SetItemText(hlv,ref_item,5,argv[5]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session

      //criticals
      hlv       = GetDlgItem(h_state,DLG_STATE_LV_CRITICAL);
      lvi.iItem = ListView_GetItemCount(hlv);
      ref_item  = ListView_InsertItem(hlv, &lvi);

      ListView_SetItemText(hlv,ref_item,0,argv[5]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[5]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,argv[3]);     //desc
      ListView_SetItemText(hlv,ref_item,4,argv[4]);     //description
      //ListView_SetItemText(hlv,ref_item,5,argv[5]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[6]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    case 19: //users
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      DWORD ref_item;

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s",src_name,argv[0]);
      char user[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s (%s)",argv[1],argv[4]);

      if (argv[6][4] == '\\')
      {
        lvi.iItem= ListView_GetItemCount(hlv);
        ref_item = ListView_InsertItem(hlv, &lvi);
        ListView_SetItemText(hlv,ref_item,0,argv[6]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[6]);//Origine
        ListView_SetItemText(hlv,ref_item,2,src);         //Source
        ListView_SetItemText(hlv,ref_item,3,argv[5]);     //desc
        ListView_SetItemText(hlv,ref_item,4,argv[9]);     //description
        ListView_SetItemText(hlv,ref_item,5,user);        //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[3]);     //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session

        //critical
        hlv      = GetDlgItem(h_state,DLG_STATE_LV_CRITICAL);
        lvi.iItem= ListView_GetItemCount(hlv);
        ref_item = ListView_InsertItem(hlv, &lvi);
        ListView_SetItemText(hlv,ref_item,0,argv[6]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[6]);//Origine
        ListView_SetItemText(hlv,ref_item,2,src);         //Source
        ListView_SetItemText(hlv,ref_item,3,argv[5]);     //desc
        ListView_SetItemText(hlv,ref_item,4,argv[9]);     //description
        ListView_SetItemText(hlv,ref_item,5,user);        //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[3]);     //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session
      }

      if (argv[7][4] == '\\')
      {
        lvi.iItem= ListView_GetItemCount(hlv);
        ref_item = ListView_InsertItem(hlv, &lvi);
        ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
        ListView_SetItemText(hlv,ref_item,2,src);         //Source
        ListView_SetItemText(hlv,ref_item,3,argv[5]);     //desc
        ListView_SetItemText(hlv,ref_item,4,argv[9]);     //description
        ListView_SetItemText(hlv,ref_item,5,user);        //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[3]);     //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session

        //critical
        hlv      = GetDlgItem(h_state,DLG_STATE_LV_CRITICAL);
        lvi.iItem= ListView_GetItemCount(hlv);
        ref_item = ListView_InsertItem(hlv, &lvi);
        ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
        ListView_SetItemText(hlv,ref_item,2,src);         //Source
        ListView_SetItemText(hlv,ref_item,3,argv[5]);     //desc
        ListView_SetItemText(hlv,ref_item,4,argv[9]);     //description
        ListView_SetItemText(hlv,ref_item,5,user);        //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[3]);     //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session
      }
    }
    break;
    case 20: //userassist
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";

      if (argv[9][4] == '\\')
      {
        lvi.iItem      = ListView_GetItemCount(hlv);
        DWORD ref_item = ListView_InsertItem(hlv, &lvi);

        //set text
        char src[MAX_LINE_SIZE];
        snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

        ListView_SetItemText(hlv,ref_item,0,argv[9]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[9]);//Origine
        ListView_SetItemText(hlv,ref_item,2,src);         //Source
        ListView_SetItemText(hlv,ref_item,3,argv[4]);     //desc
        ListView_SetItemText(hlv,ref_item,4,argv[5]);     //description
        ListView_SetItemText(hlv,ref_item,5,argv[10]);    //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[12]);    //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session

        hlv      = GetDlgItem(h_state,DLG_STATE_LV_CRITICAL);
        lvi.iItem= ListView_GetItemCount(hlv);
        ref_item = ListView_InsertItem(hlv, &lvi);
        ListView_SetItemText(hlv,ref_item,0,argv[9]);     //date
        ListView_SetItemText(hlv,ref_item,1,azColName[9]);//Origine
        ListView_SetItemText(hlv,ref_item,2,src);         //Source
        ListView_SetItemText(hlv,ref_item,3,argv[4]);     //desc
        ListView_SetItemText(hlv,ref_item,4,argv[5]);     //description
        ListView_SetItemText(hlv,ref_item,5,argv[10]);    //Owner
        ListView_SetItemText(hlv,ref_item,6,argv[12]);    //SID
        ListView_SetItemText(hlv,ref_item,7,session_state);//Session
      }
    }
    break;
    case 21: //mru & mui & history
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";

      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

      ListView_SetItemText(hlv,ref_item,0,argv[6]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[6]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,argv[5]);     //desc
      ListView_SetItemText(hlv,ref_item,4,argv[4]);     //description
      ListView_SetItemText(hlv,ref_item,5,argv[7]);     //Owner
      ListView_SetItemText(hlv,ref_item,6,argv[9]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    /*
    Disable:
    22:password
    */
    case 23: //path
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";

      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s\\%s",src_name,strlen(argv[1])?argv[1]:argv[0],argv[2]);

      ListView_SetItemText(hlv,ref_item,0,argv[8]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[8]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      //ListView_SetItemText(hlv,ref_item,3,argv[5]);     //desc
      ListView_SetItemText(hlv,ref_item,4,argv[4]);     //description
      ListView_SetItemText(hlv,ref_item,5,argv[5]);     //Owner
      ListView_SetItemText(hlv,ref_item,6,argv[7]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    /*
    Disable:
    24:security guide
    */
    case 25: //Antivirus
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";

      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s",src_name,argv[0]);

      char description[MAX_LINE_SIZE];
      snprintf(description,MAX_LINE_SIZE,"%s,%s",argv[3],argv[4]);

      ListView_SetItemText(hlv,ref_item,0,argv[7]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[7]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,description); //desc
      ListView_SetItemText(hlv,ref_item,4,argv[1]);     //description
      //ListView_SetItemText(hlv,ref_item,5,argv[5]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[7]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    /*
    Disable:
    26:firewall
    */
    case 27:
    case 28:
    case 29:
    case 30: //navigators + Android
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char src[MAX_LINE_SIZE];
      snprintf(src,MAX_LINE_SIZE,"%s:%s",src_name,argv[0]);

      ListView_SetItemText(hlv,ref_item,0,argv[4]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[4]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src);         //Source
      ListView_SetItemText(hlv,ref_item,3,argv[3]);     //desc
      ListView_SetItemText(hlv,ref_item,4,argv[2]);     //description
      //ListView_SetItemText(hlv,ref_item,5,argv[5]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[7]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
    case 31: //prefetc
    {
      HANDLE hlv     = GetDlgItem(h_state,DLG_STATE_LV_ALL);
      LVITEM lvi;
      lvi.mask       = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem   = 0;
      lvi.lParam     = LVM_SORTITEMS;
      lvi.pszText    = "";
      lvi.iItem      = ListView_GetItemCount(hlv);
      DWORD ref_item = ListView_InsertItem(hlv, &lvi);

      //set text
      char exec[MAX_LINE_SIZE];
      snprintf(exec,MAX_LINE_SIZE,"%s:%s",azColName[4],argv[4]);

      ListView_SetItemText(hlv,ref_item,0,argv[5]);     //date
      ListView_SetItemText(hlv,ref_item,1,azColName[5]);//Origine
      ListView_SetItemText(hlv,ref_item,2,src_name);    //Source
      ListView_SetItemText(hlv,ref_item,3,exec);        //desc
      ListView_SetItemText(hlv,ref_item,4,argv[0]);     //description
      //ListView_SetItemText(hlv,ref_item,5,argv[5]);     //Owner
      //ListView_SetItemText(hlv,ref_item,6,argv[7]);     //SID
      ListView_SetItemText(hlv,ref_item,7,session_state);//Session
    }
    break;
  }

  if (ListView_GetItemCount(GetDlgItem(h_state,DLG_STATE_LV_ALL))%1000 == 0)
  {
    char request[MAX_LINE_SIZE];
    snprintf(request,MAX_LINE_SIZE,"Loading %lu items...",ListView_GetItemCount(GetDlgItem(h_state,DLG_STATE_LV_ALL)));
    SendDlgItemMessage(h_state,DLG_STATE_SB,SB_SETTEXT,0, (LPARAM)request);
  }

  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI Load_state(LPVOID lParam)
{
  long int nb_items     = SendDlgItemMessage(h_state,DLG_STATE_LB_TEST,LB_GETCOUNT,0,(LPARAM)0);
  long int nb_sessions  = SendDlgItemMessage(h_state,DLG_STATE_LB_SESSION,LB_GETCOUNT,0,(LPARAM)0);

  DWORD i, s;
  FORMAT_CALBAK_READ_INFO fcri;
  char request[MAX_LINE_SIZE];

  for (i=0;i<nb_items;i++)
  {
    if (SendDlgItemMessage(h_state,DLG_STATE_LB_TEST,LB_GETSEL,i,(LPARAM)0)>0)
    {
      //init
      current_global_request[0]          = 0;
      type_format_current_global_request = 0;
      src_name[0]                        = 0;
      session_state[0]                   = 0;
      fcri.type = TYPE_SQLITE_FLAG_GET_ITEMS_INFO;
      SendDlgItemMessage(h_state,DLG_STATE_LB_TEST,LB_GETTEXT,i,(LPARAM)src_name);

      //get request format
      snprintf(request, MAX_LINE_SIZE,"SELECT mode,request FROM language_strings_columns_settings WHERE ord = %d;",i);
      sqlite3_exec(db_scan, request, callback_sqlite_state, &fcri, NULL);

      //load datas for each selected session
      for (s=0;s<nb_sessions;s++)
      {
        if (SendDlgItemMessage(h_state,DLG_STATE_LB_SESSION,LB_GETSEL,s,(LPARAM)0)>0)
        {
          fcri.type = i;
          SendDlgItemMessage(h_state,DLG_STATE_LB_SESSION,LB_GETTEXT,s,(LPARAM)session_state);

          switch(type_format_current_global_request)
          {
            case MODE_SQL_NONE:break;
            case MODE_SQL_FULL:
              snprintf(request, MAX_LINE_SIZE,"%s%d AND session_id=%lu;",current_global_request,(int)current_lang_id,session[s]);
              sqlite3_exec(db_scan, request, callback_sqlite_state, &fcri, NULL);
            break;
            case MODE_SQL_LITE:
              snprintf(request, MAX_LINE_SIZE,"%s session_id=%lu;",current_global_request,session[s]);
              sqlite3_exec(db_scan, request, callback_sqlite_state, &fcri, NULL);
            break;
          }
        }
      }
    }
  }
  SetWindowText(GetDlgItem(h_state,DLG_STATE_BT_LOAD),"Load");
  snprintf(request,MAX_LINE_SIZE,"Load %lu item(s)",ListView_GetItemCount(GetDlgItem(h_state,DLG_STATE_LV_ALL)));
  SendDlgItemMessage(h_state,DLG_STATE_SB,SB_SETTEXT,0, (LPARAM)request);

  h_load_th = 0;
  return 0;
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_state(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
    case WM_COMMAND:
      switch (HIWORD(wParam))
      {
        case BN_CLICKED:
          switch(LOWORD(wParam))
          {
            case DLG_STATE_BT_LOAD:
              if (h_load_th == 0)
              {
                ListView_DeleteAllItems(GetDlgItem(h_state,DLG_STATE_LV_ALL));
                ListView_DeleteAllItems(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL));
                ListView_DeleteAllItems(GetDlgItem(h_state,DLG_STATE_LV_CRITICAL));
                ListView_DeleteAllItems(GetDlgItem(h_state,DLG_STATE_LV_FILTER));
                SendMessage(GetDlgItem(h_state,DLG_STATE_SB),SB_SETTEXT,0, (LPARAM)"");
                SetWindowText(GetDlgItem(h_state,DLG_STATE_BT_LOAD),"STOP");
                h_load_th = CreateThread(NULL,0,Load_state,0,0,0);
              }else //stop
              {
                DWORD IDThread;
                GetExitCodeThread(h_load_th,&IDThread);
                TerminateThread(h_load_th,IDThread);
                SetWindowText(GetDlgItem(h_state,DLG_STATE_BT_LOAD),"Load");
                h_load_th = 0;
              }
            break;
            //tabl
            case DLG_STATE_BT_ALL:      ShowOnglet(DLG_STATE_LV_ALL);break;
            case DLG_STATE_BT_CRITICAL: ShowOnglet(DLG_STATE_LV_CRITICAL);break;
            case DLG_STATE_BT_TIME_ZONE:ShowOnglet(DLG_STATE_LV_TIME_ZONE);break;
            //filter
            case DLG_STATE_BT_FILTER:
              if(h_filter_th == 0)
              {
                //filter in each datas
                ListView_DeleteAllItems(GetDlgItem(h_state,DLG_STATE_LV_FILTER));
                SetWindowText(GetDlgItem(h_state,DLG_STATE_BT_FILTER),"STOP");
                h_filter_th = CreateThread(NULL,0,Filter_state,0,0,0);
                ShowOnglet(DLG_STATE_LV_FILTER);
              }else //stop
              {
                DWORD IDThread;
                GetExitCodeThread(h_filter_th,&IDThread);
                TerminateThread(h_filter_th,IDThread);
                SetWindowText(GetDlgItem(h_state,DLG_STATE_BT_FILTER),"Filter");
                h_filter_th = 0;
              }
            break;
              case POPUP_S_VIEW:
              {
                char file[MAX_PATH]="";
                OPENFILENAME ofn;
                ZeroMemory(&ofn, sizeof(OPENFILENAME));
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = file;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter ="*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0";
                ofn.nFilterIndex = 1;
                ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
                ofn.lpstrDefExt =".csv\0";
                if (GetSaveFileName(&ofn)==TRUE)
                {
                  if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_ALL)))SaveLSTV(GetDlgItem(hwnd,DLG_STATE_LV_ALL), file, ofn.nFilterIndex, 7);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL)))SaveLSTV(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL), file, ofn.nFilterIndex, 7);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_FILTER)))SaveLSTV(GetDlgItem(hwnd,DLG_STATE_LV_FILTER), file, ofn.nFilterIndex, 7);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE)))SaveLSTV(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE), file, ofn.nFilterIndex, 4);
                }
              }
              break;
              //-----------------------------------------------------
              case POPUP_S_SELECTION:
              {
                char file[MAX_PATH]="";
                OPENFILENAME ofn;
                ZeroMemory(&ofn, sizeof(OPENFILENAME));
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = file;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter ="*.csv \0*.csv\0*.xml \0*.xml\0*.html \0*.html\0";
                ofn.nFilterIndex = 1;
                ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
                ofn.lpstrDefExt =".csv\0";
                if (GetSaveFileName(&ofn)==TRUE)
                {
                  if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_ALL)))SaveLSTVSelectedItems(GetDlgItem(hwnd,DLG_STATE_LV_ALL), file, ofn.nFilterIndex, 7);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL)))SaveLSTVSelectedItems(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL), file, ofn.nFilterIndex, 7);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_FILTER)))SaveLSTVSelectedItems(GetDlgItem(hwnd,DLG_STATE_LV_FILTER), file, ofn.nFilterIndex, 7);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE)))SaveLSTVSelectedItems(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE), file, ofn.nFilterIndex, 4);
                }
              }
              break;
              case POPUP_I_00:
              case POPUP_I_01:
              case POPUP_I_02:
              case POPUP_I_03:
              case POPUP_I_04:
              case POPUP_I_05:
              case POPUP_I_06:
              case POPUP_I_07:
                  if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_ALL)))CopyDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_ALL), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_ALL),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), LOWORD(wParam)-POPUP_I_00 );
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL)))CopyDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), LOWORD(wParam)-POPUP_I_00 );
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_FILTER)))CopyDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_FILTER), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_FILTER),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), LOWORD(wParam)-POPUP_I_00 );
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE)))CopyDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), LOWORD(wParam)-POPUP_I_00 );
              break;
              case POPUP_CP_LINE:
                  if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_ALL)))CopyAllDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_ALL), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_ALL),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 8);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL)))CopyAllDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 8);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_FILTER)))CopyAllDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_FILTER), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_FILTER),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 8);
                  else if (IsWindowVisible(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE)))CopyAllDataToClipboard(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE), SendMessage(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE),LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 4);
              break;
              case SELECT_ALL_SESSION:SendDlgItemMessage(h_state,DLG_STATE_LB_SESSION,LB_SETSEL,SELECT_SESSION,(LPARAM)-1);SELECT_SESSION=!SELECT_SESSION;break;
              case SELECT_ALL_TEST:SendDlgItemMessage(h_state,DLG_STATE_LB_TEST,LB_SETSEL,SELECT_TEST,(LPARAM)-1);SELECT_TEST=!SELECT_TEST;break;
          }
        break;
      }
    break;
    case WM_CONTEXTMENU:
    {
      HANDLE hlv = (HANDLE)wParam;

      if (hlv == GetDlgItem(hwnd,DLG_STATE_LB_SESSION))
      {
        HMENU hmenu;
        if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_SELECT_ALL_SESSION)))!= NULL)
        {
          //affichage du popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
          DestroyMenu(hmenu);
        }
      }else if (hlv == GetDlgItem(hwnd,DLG_STATE_LB_TEST))
      {
        HMENU hmenu;
        if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_SELECT_ALL_TEST)))!= NULL)
        {
          //affichage du popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
          DestroyMenu(hmenu);
        }
      }else if (ListView_GetItemCount(hlv) > 0)
      {
        HMENU hmenu;
        if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(POPUP_LSTV)))!= NULL)
        {
          RemoveMenu(GetSubMenu(hmenu,0),4,MF_BYPOSITION);
          RemoveMenu(GetSubMenu(hmenu,0),2,MF_BYPOSITION);
          RemoveMenu(hmenu,POPUP_A_SEARCH,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_O_PATH,MF_BYCOMMAND);

          RemoveMenu(hmenu,POPUP_I_08,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_09,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_10,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_11,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_12,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_13,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_14,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_15,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_16,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_17,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_18,MF_BYCOMMAND);
          RemoveMenu(hmenu,POPUP_I_19,MF_BYCOMMAND);

          //load column text
          char buffer[DEFAULT_TMP_SIZE]="";
          LVCOLUMN lvc;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = DEFAULT_TMP_SIZE;
          lvc.pszText = buffer;

          unsigned int i=0;
          while (SendMessage(hlv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))
          {
            ModifyMenu(hmenu,POPUP_I_00+i,MF_BYCOMMAND|MF_STRING,POPUP_I_00+i,buffer);

            //reinit
            buffer[0] = 0;
            lvc.mask = LVCF_TEXT;
            lvc.cchTextMax = DEFAULT_TMP_SIZE;
            lvc.pszText = buffer;
            i++;
          }

          //set text !!!
          ModifyMenu(hmenu,POPUP_S_VIEW           ,MF_BYCOMMAND|MF_STRING ,POPUP_S_VIEW           ,cps[TXT_POPUP_S_VIEW].c);
          ModifyMenu(hmenu,POPUP_S_SELECTION      ,MF_BYCOMMAND|MF_STRING ,POPUP_S_SELECTION      ,cps[TXT_POPUP_S_SELECTION].c);
          ModifyMenu(hmenu,POPUP_CP_LINE          ,MF_BYCOMMAND|MF_STRING ,POPUP_CP_LINE          ,cps[TXT_POPUP_CP_LINE].c);

          if (hlv == GetDlgItem(hwnd,DLG_STATE_LV_ALL) ||
              hlv == GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL) ||
              hlv == GetDlgItem(hwnd,DLG_STATE_LV_FILTER))
          {}else if (hlv == GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE))
          {
            RemoveMenu(hmenu,POPUP_I_04,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_I_05,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_I_06,MF_BYCOMMAND);
            RemoveMenu(hmenu,POPUP_I_07,MF_BYCOMMAND);
          }

          //affichage du popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
          DestroyMenu(hmenu);
        }
      }
    }
    break;
    case WM_CLOSE : ShowWindow(hwnd, SW_HIDE);break;
    case WM_SIZE:
    {
      unsigned int mWidth  = LOWORD(lParam);
      unsigned int mHeight = HIWORD(lParam);

      //controle de la taille minimum
      if (mWidth<610 ||mHeight<300)
      {
        RECT Rect;
        GetWindowRect(hwnd, &Rect);
        MoveWindow(hwnd,Rect.left,Rect.top,600+20,300+64,TRUE);
      }else
      {
        MoveWindow(GetDlgItem(hwnd,DLG_STATE_LV_ALL)      ,200,42,mWidth-205,mHeight-64,TRUE);
        MoveWindow(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL) ,200,42,mWidth-205,mHeight-64,TRUE);
        MoveWindow(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE),200,42,mWidth-205,mHeight-64,TRUE);
        MoveWindow(GetDlgItem(hwnd,DLG_STATE_LV_FILTER)   ,200,42,mWidth-205,mHeight-64,TRUE);

        MoveWindow(GetDlgItem(hwnd,DLG_STATE_LB_SESSION)  ,2,1,192,mHeight/2-5,TRUE);
        MoveWindow(GetDlgItem(hwnd,DLG_STATE_LB_TEST)     ,2,mHeight/2,192,mHeight/2-68,TRUE);

        MoveWindow(GetDlgItem(hwnd,DLG_STATE_BT_LOAD)     ,2,mHeight-55,192,30,TRUE);

        MoveWindow(GetDlgItem(hwnd,DLG_STATE_SB),0,mHeight-22,mWidth,22,TRUE);

        //column resise
        unsigned int i;
        DWORD column_sz = (mWidth-220)/8;
        for (i=0;i<8;i++)
        {
          redimColumnH(GetDlgItem(hwnd,DLG_STATE_LV_ALL),i,column_sz);
          redimColumnH(GetDlgItem(hwnd,DLG_STATE_LV_CRITICAL),i,column_sz);
          redimColumnH(GetDlgItem(hwnd,DLG_STATE_LV_FILTER),i,column_sz);
        }

        column_sz = (mWidth-220)/4;
        for (i=0;i<4;i++)
        {
          redimColumnH(GetDlgItem(hwnd,DLG_STATE_LV_TIME_ZONE),i,column_sz);
        }
      }
      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
    case WM_NOTIFY:
      switch(((LPNMHDR)lParam)->code)
      {
        case LVN_COLUMNCLICK:
          switch(((LPNMHDR)lParam)->idFrom)
          {
            case DLG_STATE_LV_ALL:
              TRI_STATE_ALL = !TRI_STATE_ALL;
              c_Tri(GetDlgItem(hwnd,((LPNMHDR)lParam)->idFrom),((LPNMLISTVIEW)lParam)->iSubItem,TRI_STATE_ALL);
            break;
            case DLG_STATE_LV_CRITICAL:
              TRI_STATE_CRITICAL = !TRI_STATE_CRITICAL;
              c_Tri(GetDlgItem(hwnd,((LPNMHDR)lParam)->idFrom),((LPNMLISTVIEW)lParam)->iSubItem,TRI_STATE_CRITICAL);
            break;
            case DLG_STATE_LV_TIME_ZONE:
              TRI_STATE_DATE = !TRI_STATE_DATE;
              c_Tri(GetDlgItem(hwnd,((LPNMHDR)lParam)->idFrom),((LPNMLISTVIEW)lParam)->iSubItem,TRI_STATE_DATE);
            break;
            case DLG_STATE_LV_FILTER:
              TRI_STATE_FILTER = !TRI_STATE_FILTER;
              c_Tri(GetDlgItem(hwnd,((LPNMHDR)lParam)->idFrom),((LPNMLISTVIEW)lParam)->iSubItem,TRI_STATE_FILTER);
            break;
          }
        break;
      }
    break;
  }
  return 0;
}
