//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addRegistryFirewalltoDB(char *file, char *hk,char *key,char *application,char *rule, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_firewall (file,hk,key,application,rule,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           file,hk,key,application,rule,session_id);
  if (!CONSOL_ONLY || DEBUG_CMD_MODE)AddDebugMessage("test_firewall", request, "-", MSG_INFO);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
//local function part !!!
//------------------------------------------------------------------------------
void EnumRegistryRules_local(HKEY hk, char *chk,char *path, unsigned int session_id, sqlite3 *db)
{
  HKEY CleTmp;
  if (RegOpenKey(hk,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD nbSubValue = 0,i;
    if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbSubValue,0,0,0,0)==ERROR_SUCCESS)
    {
      char value[MAX_PATH], data[MAX_PATH];
      DWORD value_size, data_size,type;

      for (i=0;i<nbSubValue;i++)
      {
        value_size  = MAX_PATH;
        data_size   = MAX_PATH;
        value[0]    = 0;
        data[0]     = 0;
        type        = 0;

        if (RegEnumValue (CleTmp,i,value,&value_size,0,(LPDWORD)&type,(LPBYTE)data,&data_size)==ERROR_SUCCESS)
        {
          if (type == REG_SZ || type == REG_EXPAND_SZ)
          {
            convertStringToSQL(value, MAX_PATH);
            convertStringToSQL(data, MAX_PATH);
            addRegistryFirewalltoDB("",chk,path,value,data, session_id, db);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//file registry part
//------------------------------------------------------------------------------
void EnumRegistryRules_file(HK_F_OPEN *hks,char *ckey, unsigned int session_id, sqlite3 *db)
{
  HBIN_CELL_NK_HEADER *nk_h = GetRegistryNK(hks->buffer,hks->taille_fic, (hks->pos_fhbin)+HBIN_HEADER_SIZE, hks->position,ckey);
  if (nk_h == NULL)return;

  char value[MAX_PATH], data[MAX_PATH];

  DWORD type, k,nbSubValue = GetValueData(hks->buffer,hks->taille_fic, nk_h, (hks->pos_fhbin)+HBIN_HEADER_SIZE, 0, NULL, 0, NULL, 0);
  for (k=0;k<nbSubValue;k++)
  {
    type = GetValueData(hks->buffer,hks->taille_fic, nk_h, (hks->pos_fhbin)+HBIN_HEADER_SIZE, k,value,MAX_PATH,data,MAX_PATH);
    if (type == REG_SZ || type == REG_EXPAND_SZ)
    {
      convertStringToSQL(value, MAX_PATH);
      convertStringToSQL(data, MAX_PATH);
      addRegistryFirewalltoDB(hks->file,"",ckey,value,data, session_id, db);
    }
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DWORD WINAPI Scan_firewall(LPVOID lParam)
{
  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;
  WaitForSingleObject(hsemaphore,INFINITE);
  AddDebugMessage("test_firewall", "Scan Firewall - START", "OK", MSG_INFO);

  char file[MAX_PATH];
  char tmp_msg[MAX_PATH];
  HK_F_OPEN hks;

  //files or local
  HTREEITEM hitem = (HTREEITEM)SendDlgItemMessage((HWND)h_conf,TRV_FILES, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
  if (hitem!=NULL) //files
  {
    while(hitem!=NULL)
    {
      file[0] = 0;
      GetTextFromTrv(hitem, file, MAX_PATH);
      if (file[0] != 0)
      {
        //info
        snprintf(tmp_msg,MAX_PATH,"Scan Registry file : %s",file);
        AddDebugMessage("test_firewall", tmp_msg, "OK", MSG_INFO);

        //open file + verify
        if(OpenRegFiletoMem(&hks, file))
        {
          //Enum all rules
          EnumRegistryRules_file(&hks,"ControlSet001\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\DomainProfile\\AuthorizedApplications\\List",session_id,db);
          EnumRegistryRules_file(&hks,"ControlSet001\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile\\AuthorizedApplications\\List",session_id,db);

          //Windows Defender
          EnumRegistryRules_file(&hks,"Microsoft\\Windows Defender\\Exclusions\\Extensions",session_id,db);
          EnumRegistryRules_file(&hks,"Microsoft\\Windows Defender\\Exclusions\\Paths",session_id,db);
          EnumRegistryRules_file(&hks,"Microsoft\\Windows Defender\\Exclusions\\Processes",session_id,db);
          CloseRegFiletoMem(&hks);
        }
      }
      hitem = (HTREEITEM)SendDlgItemMessage((HWND)h_conf,TRV_FILES, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }
  }else
  {
    //Enum all rules
    EnumRegistryRules_local(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\DomainProfile\\AuthorizedApplications\\List",session_id,db);
    EnumRegistryRules_local(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile\\AuthorizedApplications\\List",session_id,db);

    //Windows Defender
    EnumRegistryRules_local(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows Defender\\Exclusions\\Extensions",session_id,db);
    EnumRegistryRules_local(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows Defender\\Exclusions\\Paths",session_id,db);
    EnumRegistryRules_local(HKEY_LOCAL_MACHINE,"HKEY_LOCAL_MACHINE","SOFTWARE\\Microsoft\\Windows Defender\\Exclusions\\Processes",session_id,db);
  }

  AddDebugMessage("test_firewall", "Scan Firewall - DONE", "OK", MSG_INFO);
  check_treeview(GetDlgItem(h_conf,TRV_TEST), H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  ReleaseSemaphore(hsemaphore,1,NULL);
  return 0;
}
