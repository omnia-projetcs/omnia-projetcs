//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void LoadPRocessList(HWND hlv)
{
  ListView_DeleteAllItems(hlv);

  PROCESSENTRY32 pe = {sizeof(PROCESSENTRY32)};
  HANDLE hCT = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS|TH32CS_SNAPTHREAD, 0);
  if (hCT==INVALID_HANDLE_VALUE)return;

  DWORD cbNeeded, k, j, nb_process=0, ref_item;
  HANDLE hProcess;
  HMODULE hMod[MAX_PATH];
  FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime;
  LINE_PROC_ITEM port_line[MAX_PATH];
  char process[DEFAULT_TMP_SIZE],
       pid[DEFAULT_TMP_SIZE],
       path[MAX_PATH],
       cmd[MAX_PATH],
       owner[DEFAULT_TMP_SIZE],
       rid[DEFAULT_TMP_SIZE],
       sid[DEFAULT_TMP_SIZE],
       start_date[DATE_SIZE_MAX];

  PROCESS_INFOS_ARGS process_infos[MAX_PATH];

  LVITEM lvi;
  lvi.mask = LVIF_TEXT|LVIF_PARAM;
  lvi.iSubItem = 0;
  lvi.lParam = LVM_SORTITEMS;
  lvi.pszText="";

  while(Process32Next(hCT, &pe))
  {
    //open process info
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,pe.th32ProcessID);
    if (hProcess == NULL)continue;

    //process
    process[0] = 0;
    strncpy(process,pe.szExeFile,DEFAULT_TMP_SIZE);

    //pid
    snprintf(pid,DEFAULT_TMP_SIZE,"%04lu",pe.th32ProcessID);

    //path
    path[0]=0;
    if (EnumProcessModules(hProcess,hMod, MAX_PATH,&cbNeeded))
    {
      if (GetModuleFileNameEx(hProcess,hMod[0],path,MAX_PATH) == 0)path[0] = 0;
    }

    //cmd
    cmd[0]=0;
    GetProcessArg(hProcess, cmd, MAX_PATH);

    //owner
    GetProcessOwner(pe.th32ProcessID, owner, rid, sid, DEFAULT_TMP_SIZE);

    //start date process
    start_date[0] = 0;
    if (GetProcessTimes(hProcess, &lpCreationTime,&lpExitTime, &lpKernelTime, &lpUserTime))
    {
      //traitement de la date
      if (lpCreationTime.dwHighDateTime != 0 && lpCreationTime.dwLowDateTime != 0)
      {
       filetimeToString_GMT(lpCreationTime, start_date, DATE_SIZE_MAX);
      }
    }

    //ports !
    j=GetPortsFromPID(pe.th32ProcessID, port_line, MAX_PATH, SIZE_ITEMS_PORT_MAX);

    //update list of process
    if (nb_process<MAX_PATH)
    {
      process_infos[nb_process].pid = pe.th32ProcessID;
      snprintf(process_infos[nb_process].args,MAX_PATH,"%s",cmd);
      nb_process++;
    }

    //add items !
    if (j == 0)
    {
      lvi.iItem = ListView_GetItemCount(hlv);
      ref_item = ListView_InsertItem(hlv, &lvi);

      ListView_SetItemText(hlv,ref_item,0,process);
      ListView_SetItemText(hlv,ref_item,1,pid);
      ListView_SetItemText(hlv,ref_item,2,path);
      ListView_SetItemText(hlv,ref_item,3,cmd);
      ListView_SetItemText(hlv,ref_item,4,owner);
      ListView_SetItemText(hlv,ref_item,5,rid);
      ListView_SetItemText(hlv,ref_item,6,sid);
      ListView_SetItemText(hlv,ref_item,7,start_date);
      ListView_SetItemText(hlv,ref_item,8,"");
      ListView_SetItemText(hlv,ref_item,9,"");
      ListView_SetItemText(hlv,ref_item,10,"");
      ListView_SetItemText(hlv,ref_item,11,"");
      ListView_SetItemText(hlv,ref_item,12,"");
      ListView_SetItemText(hlv,ref_item,13,"");
      ListView_SetItemText(hlv,ref_item,14,"");
      ListView_SetItemText(hlv,ref_item,15,"");
      ListView_SetItemText(hlv,ref_item,16,"");
    }else
    {
      for (k=0;k<j;k++)
      {
        lvi.iItem = ListView_GetItemCount(hlv);
        ref_item = ListView_InsertItem(hlv, &lvi);

        ListView_SetItemText(hlv,ref_item,0,process);
        ListView_SetItemText(hlv,ref_item,1,pid);
        ListView_SetItemText(hlv,ref_item,2,path);
        ListView_SetItemText(hlv,ref_item,3,cmd);
        ListView_SetItemText(hlv,ref_item,4,owner);
        ListView_SetItemText(hlv,ref_item,5,rid);
        ListView_SetItemText(hlv,ref_item,6,sid);
        ListView_SetItemText(hlv,ref_item,7,start_date);
        ListView_SetItemText(hlv,ref_item,8,port_line[k].protocol);
        ListView_SetItemText(hlv,ref_item,9,port_line[k].IP_src);
        ListView_SetItemText(hlv,ref_item,10,port_line[k].Port_src);
        ListView_SetItemText(hlv,ref_item,11,port_line[k].IP_dst);
        ListView_SetItemText(hlv,ref_item,12,port_line[k].Port_dst);
        ListView_SetItemText(hlv,ref_item,13,port_line[k].state);
        ListView_SetItemText(hlv,ref_item,14,"");
        ListView_SetItemText(hlv,ref_item,15,"");
        ListView_SetItemText(hlv,ref_item,16,"");
      }
    }
    CloseHandle(hProcess);
  }

  //add verify
  //add function for check virustotal file
  //add file info
  //add check signature

  //add popup menu ^^


  //verify shadow process !!!
  //EnumProcessAndThread(nb_process, process_infos,session_id,db);

  CloseHandle(hCT);
}
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_info(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
    case WM_SIZE:
    {
      unsigned int mWidth  = LOWORD(lParam);
      unsigned int mHeight = HIWORD(lParam);
      MoveWindow(hlstv_process,5,0,mWidth-10,mHeight-5,TRUE);

      //column resise
      unsigned int i;
      if(nb_column_process_view) for (i=0;i<nb_column_process_view;i++)redimColumnH(hlstv_process,i,(mWidth-40)/nb_column_process_view);
    }
    break;
    case WM_COMMAND:break;
    case WM_CONTEXTMENU:break;
    case WM_NOTIFY:break;
    case WM_CLOSE : ShowWindow(hwnd, SW_HIDE);break;
  }

 /* if (uMsg == WM_SIZE)
  {
      unsigned int mWidth = LOWORD(lParam);  // width of client area
      unsigned int mHeight = HIWORD(lParam);  // width of client area

      MoveWindow(GetDlgItem(hwnd,LV_VIEW),5,0,mWidth-10,mHeight-5,TRUE);
      //redimmensionnement des colonnes
      unsigned int col_size = (mWidth-70)/9;
      redimColumn(hwnd,LV_VIEW,0,col_size);
      redimColumn(hwnd,LV_VIEW,1,col_size*2);
      redimColumn(hwnd,LV_VIEW,2,col_size);
      redimColumn(hwnd,LV_VIEW,3,col_size);
      redimColumn(hwnd,LV_VIEW,4,col_size);
      redimColumn(hwnd,LV_VIEW,5,col_size);
      redimColumn(hwnd,LV_VIEW,6,col_size);
      redimColumn(hwnd,LV_VIEW,7,col_size);

      InvalidateRect(hwnd, NULL, TRUE);
  }
  else if (uMsg == WM_COMMAND)
  {
    if (HIWORD(wParam) == BN_CLICKED)
    {
      switch(LOWORD(wParam))
      {
        case POPUP_LV_S_SELECTION : LVSaveAll(TABL_INFO, LV_VIEW, NB_COLONNE_LV[LV_INFO_VIEW_NB_COL], TRUE, FALSE, FALSE);break;
        case POPUP_LV_S_VIEW : LVSaveAll(TABL_INFO, LV_VIEW, NB_COLONNE_LV[LV_INFO_VIEW_NB_COL], FALSE, FALSE, FALSE);break;
        case POPUP_LV_S_DELETE : LVDelete(TABL_INFO,LV_VIEW);break;

        case POPUP_LV_CP_COL1:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),0);break;
        case POPUP_LV_CP_COL2:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1);break;
        case POPUP_LV_CP_COL3:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),2);break;
        case POPUP_LV_CP_COL4:CopyData(GetDlgItem(hwnd,LV_VIEW), SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),3);break;
        case POPUP_TRV_CONF_OFP:
        {
          if (SendDlgItemMessage(hwnd,LV_VIEW,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0))
          {
            char tmp[MAX_PATH]="";
            ListView_GetItemText(GetDlgItem(hwnd,LV_VIEW),SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,tmp,MAX_PATH);

            unsigned int size = strlen(tmp);
            if (size)
            {
              //on récupère le path
              char *c = tmp+size-1;
              while(*c != '\\')c--;
              c++;
              *c=0;
              ShellExecute(Tabl[TABL_MAIN], "explore", tmp, NULL,NULL,SW_SHOW);
            }
          }
        }
        break;
        case POPUP_LV_PROPERTIES:
          {
            //lecture du path du fichier
            char file[MAX_PATH];
            ListView_GetItemText(GetDlgItem(hwnd,LV_VIEW),SendMessage(GetDlgItem(hwnd,LV_VIEW),LVM_GETNEXTITEM,-1,LVNI_FOCUSED),1,file,MAX_PATH);

            if (file[1]==':' || (file[1]=='\\' && file[1]=='\\'))
            {
              //properties
              SHELLEXECUTEINFO se;
              ZeroMemory(&se,sizeof(se));
              se.cbSize = sizeof(se);
              se.fMask = SEE_MASK_INVOKEIDLIST;
              se.lpFile = file;
              se.lpVerb = "properties";
              ShellExecuteEx(&se);
            }
          }
        break;
      }
    }
  }else if (uMsg == WM_CONTEXTMENU)
  {
    HMENU hmenu;
    if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(POPUP_LV_INFO)))!= NULL)
    {
      //vérification si un item est sélectionné ^^
      if (ListView_GetSelectedCount((HANDLE)wParam)<1)
      {
        RemoveMenu(hmenu,POPUP_LV_S_SELECTION,MF_BYCOMMAND|MF_GRAYED);

        RemoveMenu(hmenu,POPUP_LV_CP_COL1,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL2,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL3,MF_BYCOMMAND|MF_GRAYED);
        RemoveMenu(hmenu,POPUP_LV_CP_COL4,MF_BYCOMMAND|MF_GRAYED);
      }

      //affichage du popup menu
      TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
      DestroyMenu(hmenu);
    }
  }
  else if (uMsg == WM_NOTIFY)
  {
    if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)//click sur une entête de colonne
    {
      c_Tri(GetDlgItem(hwnd,LV_VIEW),((LPNMLISTVIEW)lParam)->iSubItem);
    }
  }else if (uMsg == WM_CLOSE)ShowWindow(hwnd, SW_HIDE);
*/
  return FALSE;
}
