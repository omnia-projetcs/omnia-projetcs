//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addShellBags(char *source, char*key, char*value, char*data, char *sid, char *last_update,unsigned int session_id, sqlite3 *db)
{
  #ifndef CMD_LINE_ONLY_NO_DB
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_registry_shell_bags (source,key,value,data,sid,last_update,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           source,key,value,data,sid,last_update,session_id);

  sqlite3_exec(db,request, NULL, NULL, NULL);
  #else
  printf("\"Shell_Bags\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%d\";\r\n",
         source,key,value,data,sid,last_update,session_id);
  #endif
}
//------------------------------------------------------------------------------
//http://www.williballenthin.com/forensics/shellbags/index.html
void Read_ShellBags_Datas(char *ckey, char *SID, char *last_update, char *value, char*data, DWORD dataSize, unsigned int session_id, sqlite3 *db)
{
  //read date
  typedef struct
  {
    unsigned short struct_size;
    unsigned short type;
    DWORD file_size;
    DWORD create_time;
    unsigned short type2;
  }START_SHELLBAG;

  START_SHELLBAG *ss = (START_SHELLBAG *)data;
  char file[MAX_PATH]="";

  if (ss->type == 0x31 || ss->type == 0x32) //0x31 = directory, 0x32 = file
  {
    //read full file name
    char *c = data + sizeof(START_SHELLBAG);
    c = c+(strlen(c)+22);
    if (*c == 0x00)c--;
    snprintf(file,MAX_PATH,"%S",c);
    if (strlen(file) < 2)
    {
      c = data + sizeof(START_SHELLBAG);
      if (*(c-1) != 0)
      {
        c=c-2;
        snprintf(file,MAX_PATH,"%s",c);
      }else snprintf(file,MAX_PATH,"%s",c);
    }
  }else
  {
    //direct share path
    snprintf(file,MAX_PATH,"%s",&data[5]);
  }

  addShellBags("HKEY_USERS", ckey, value, file, SID, last_update,session_id, db);
}
//------------------------------------------------------------------------------
void Scan_registry_ShellBags_file(HK_F_OPEN *hks, char *ckey, unsigned int session_id, sqlite3 *db)
{
  //exist or not in the file ?
  HBIN_CELL_NK_HEADER *nk_h = GetRegistryNK(hks->buffer, hks->taille_fic, (hks->pos_fhbin)+HBIN_HEADER_SIZE, hks->position, ckey);
  if (nk_h == NULL)return;

  //get key infos
  char SID[MAX_PATH], last_update[DATE_SIZE_MAX];
  Readnk_Infos(hks->buffer, hks->taille_fic, (hks->pos_fhbin), hks->position, NULL, nk_h,
               last_update, DATE_SIZE_MAX, NULL, 0, SID, MAX_PATH);

  //enumerate values
  DWORD i, dataSize, nbSubValue = GetValueData(hks->buffer,hks->taille_fic, nk_h, (hks->pos_fhbin)+HBIN_HEADER_SIZE, 0, NULL, 0, NULL, 0);
  char value[MAX_PATH], data[MAX_LINE_SIZE];
  for (i=0;i<nbSubValue && start_scan;i++)
  {
    dataSize  = MAX_LINE_SIZE;
    value[0]  = 0;
    data[0]   = 0;
    if(GetBinaryValueData(hks->buffer,hks->taille_fic, nk_h, (hks->pos_fhbin)+HBIN_HEADER_SIZE, i,value,MAX_PATH,data, &dataSize))
    {
      //read datas :)
      if (value[0] != 0 && data[0] != 0 && dataSize > 40)
      {
        if (strcmp(value,"MRUListEx")!=0 && strcmp(value,"NodeSlot")!=0 && strcmp(value,"NodeSlots")!=0)
          Read_ShellBags_Datas(ckey, SID, last_update, value, data, dataSize, session_id, db);
      }
    }
  }

  //subkeys
  DWORD j,nbSubKey = GetSubNK(hks->buffer, hks->taille_fic, nk_h, hks->position, 0, NULL, 0);
  char tmp_key[MAX_PATH], tmp_ckey[MAX_PATH];
  for (j=0;j<nbSubKey && start_scan;j++)
  {
    if(GetSubNK(hks->buffer, hks->taille_fic, nk_h, hks->position, j, tmp_key, MAX_PATH))
    {
      if (tmp_key[0] != 0)
      {
        snprintf(tmp_ckey,MAX_PATH,"%s\\%s",ckey,tmp_key);
        Scan_registry_ShellBags_file(hks, tmp_ckey, session_id, db);
      }
    }
  }
}
//------------------------------------------------------------------------------
void Enum_ShellBags(char *ckey, char *SID, unsigned int session_id, sqlite3 *db)
{
  HKEY CleTmp;
  if (RegOpenKey((HKEY)HKEY_USERS,ckey,&CleTmp)!=ERROR_SUCCESS)return;

  DWORD nbSubKey=0, nbSubValue=0, i, key_size;
  FILETIME lastupdate;
  char date[DATE_SIZE_MAX]="";
  char key_path[MAX_PATH], tmp[MAX_PATH];

  if (RegQueryInfoKey (CleTmp,NULL,NULL,NULL,&nbSubKey,NULL,NULL,&nbSubValue,NULL,NULL,NULL,&lastupdate)!=ERROR_SUCCESS)
  {
    RegCloseKey(CleTmp);
    return;
  }
  //date
  filetimeToString_GMT(lastupdate, date, DATE_SIZE_MAX);

  //read values
  DWORD valueSize,dataSize;
  char value[MAX_LINE_SIZE], data[MAX_LINE_SIZE];
  for (i=0;i<nbSubValue && start_scan;i++)
  {
    valueSize = MAX_LINE_SIZE;
    dataSize  = MAX_LINE_SIZE;
    value[0]  = 0;
    data[0]  = 0;

    if (RegEnumValue (CleTmp,i,(LPTSTR)value,(LPDWORD)&valueSize,0,NULL,(LPBYTE)data,(LPDWORD)&dataSize)==ERROR_SUCCESS)
    {
      //read datas :)
      if (value[0] != 0 && data[0] != 0 && dataSize > 40 && valueSize)
      {
        if (strcmp(value,"MRUListEx")!=0 && strcmp(value,"NodeSlot")!=0 && strcmp(value,"NodeSlots")!=0)
          Read_ShellBags_Datas(ckey, SID, date, value, data, dataSize, session_id, db);
      }
    }
  }

  //sub key
  for (i=0;i<nbSubKey && start_scan;i++)
  {
    key_size  = MAX_PATH;
    tmp[0]    = 0;
    if (RegEnumKeyEx (CleTmp,i,tmp,(LPDWORD)&key_size,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
    {
      //next
      snprintf(key_path,MAX_PATH,"%s\\%s",ckey,tmp);
      Enum_ShellBags(key_path, SID,session_id, db);
    }
  }
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
void Scan_registry_ShellBags_direct(char *ckey, unsigned int session_id, sqlite3 *db)
{
  HKEY CleTmp;
  if (RegOpenKey((HKEY)HKEY_USERS,"",&CleTmp)!=ERROR_SUCCESS)return;

  DWORD nbSubKey=0, i,key_size;
  if (RegQueryInfoKey (CleTmp,NULL,NULL,NULL,&nbSubKey,NULL,NULL,NULL,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)
  {
    RegCloseKey(CleTmp);
    return;
  }

  //datas
  char SID[MAX_PATH];

  //get all key
  char key_path[MAX_PATH];
  for (i=0;i<nbSubKey && start_scan;i++)
  {
    key_size  = MAX_PATH;
    SID[0]    = 0;
    if (RegEnumKeyEx (CleTmp,i,SID,(LPDWORD)&key_size,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
    {
      //only for reals users
      if (key_size < 10)continue;

      //enumerate datas
      snprintf(key_path,MAX_PATH,"%s\\%s",SID,ckey);
      Enum_ShellBags(key_path, SID,session_id, db);
    }
  }
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
// source : http://support.microsoft.com/kb/813711
DWORD WINAPI Scan_registry_ShellBags(LPVOID lParam)
{
  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  char file[MAX_PATH];
  HK_F_OPEN hks;

  #ifdef CMD_LINE_ONLY_NO_DB
  printf("\"Shell_Bags\";\"source\";\"key\";\"value\";\"data\";\"sid\";\"last_update\";\"session_id\";\r\n");
  #endif
  //files or local
  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
  if (hitem!=NULL || !LOCAL_SCAN) //files
  {
    while(hitem!=NULL && start_scan)
    {
      file[0] = 0;
      GetTextFromTrv(hitem, file, MAX_PATH);
      if (file[0] != 0)
      {
        //verify
        if(OpenRegFiletoMem(&hks, file))
        {
          Scan_registry_ShellBags_file(&hks,"Software\\Microsoft\\Windows\\Shell\\BagMRU",session_id,db);
          Scan_registry_ShellBags_file(&hks,"Software\\Microsoft\\Windows\\ShellNoRoam\\BagMRU",session_id,db);
          Scan_registry_ShellBags_file(&hks,"Local Settings\\Software\\Microsoft\\Windows\\Shell\\BagMRU",session_id,db);
          Scan_registry_ShellBags_file(&hks,"Local Settings\\Software\\Microsoft\\Windows\\ShellNoRoam\\BagMRU",session_id,db);
          Scan_registry_ShellBags_file(&hks,"Wow6432Node\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\BagMRU",session_id,db);
          Scan_registry_ShellBags_file(&hks,"Wow6432Node\\Local Settings\\Software\\Microsoft\\Windows\\ShellNoRoam\\BagMRU",session_id,db);
          CloseRegFiletoMem(&hks);

        }
      }
      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }
  }else
  {
    Scan_registry_ShellBags_direct("Software\\Microsoft\\Windows\\Shell\\BagMRU",session_id,db);
    Scan_registry_ShellBags_direct("Software\\Microsoft\\Windows\\ShellNoRoam\\BagMRU",session_id,db);
    Scan_registry_ShellBags_direct("Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\BagMRU",session_id,db);
    Scan_registry_ShellBags_direct("Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\ShellNoRoam\\BagMRU",session_id,db);
    Scan_registry_ShellBags_direct("Software\\Classes\\Wow6432Node\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\BagMRU",session_id,db);
    Scan_registry_ShellBags_direct("Software\\Classes\\Wow6432Node\\Local Settings\\Software\\Microsoft\\Windows\\ShellNoRoam\\BagMRU",session_id,db);
  }

  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan);
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}
