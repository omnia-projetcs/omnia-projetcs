//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/windows/desktop/ms686335%28v=vs.85%29.aspx
//http://support.microsoft.com/kb/166819
#define REGISTRY_SERVICE_NAME "RemoteRegistry" //regsvc
BOOL StartRemoteRegistryService(char *ip, BOOL start)
{
  BOOL ret = FALSE;
  #ifdef DEBUG_MODE_REGISTRY
  AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,(char*)"StartRemoteRegistryService START");
  #endif // DEBUG_MODE_REGISTRY

  //get remote OpenSCManager
  SC_HANDLE hm = OpenSCManager(ip, NULL, SC_MANAGER_ALL_ACCESS);
  if (hm == NULL) return FALSE;

  //Open remote service
  SC_HANDLE hos = OpenService(hm, REGISTRY_SERVICE_NAME, SERVICE_ALL_ACCESS);
  if (hos == NULL)
  {
    CloseServiceHandle(hm);
    return FALSE;
  }

  //start - stop the service
  if (start)
  {
    if (StartService(hos, 0, NULL)!= FALSE)
    {
      ret = TRUE;
      #ifdef DEBUG_MODE_REGISTRY
      AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,(char*)"StartRemoteRegistryService START SERVICE OK");
      #endif // DEBUG_MODE_REGISTRY
    }
  }else
  {
    //close all dependances

    //close service
    if (ControlService(hos, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)NULL) != FALSE)
    {
      ret = TRUE;
      #ifdef DEBUG_MODE_REGISTRY
      AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,(char*)"StartRemoteRegistryService STOP SERVICE OK");
      #endif // DEBUG_MODE_REGISTRY
    }
  }

  //close
  CloseServiceHandle(hos);
  CloseServiceHandle(hm);
  return ret;
}
//----------------------------------------------------------------
DWORD ReadValue(HKEY hk,char *path,char *value,void *data, DWORD data_size)
{
  DWORD data_size_read = 0;
  HKEY CleTmp=0;

  //open key
  if (RegOpenKey(hk,path,&CleTmp)!=ERROR_SUCCESS)
     return FALSE;

  //size of data
  if (RegQueryValueEx(CleTmp, value, 0, 0, 0, &data_size_read)!=ERROR_SUCCESS)
  {
    RegCloseKey(CleTmp);
    return FALSE;
  }

  //alloc
  char *c = (char *)malloc(data_size_read+1);
  if (c == NULL)
  {
    RegCloseKey(CleTmp);
    return FALSE;
  }

  //read value
  if (RegQueryValueEx(CleTmp, value, 0, 0, (LPBYTE)c, &data_size_read)!=ERROR_SUCCESS)
  {
    RegCloseKey(CleTmp);
    return FALSE;
  }

  if (data_size_read<data_size) memcpy(data,c,data_size_read);
  else memcpy(data,c,data_size);

  //free + close
  free(c);
  RegCloseKey(CleTmp);
  return data_size_read;
}
//----------------------------------------------------------------
BOOL parseLineToReg(char *line, REG_LINE_ST *reg_st, BOOL reg_write)
{
  //line format :"SYSTEM\\CurrentControlSet\\Services\\";"value";"data";"format";"check";
  //get path
  strncpy(reg_st->path,line+1,LINE_SIZE);
  char *s = reg_st->path;
  while (*s && *s != '\"')s++;
  if (*s != '\"')return FALSE;

  strncpy(reg_st->value,s+3,LINE_SIZE);
  *s = 0;
  if (strlen(reg_st->path) == 0) return FALSE;

  //get value
  s = reg_st->value;
  while (*s && *s != '\"' && *(s+1)!= ';')s++;
  if (*s != '\"')return FALSE;

  strncpy(reg_st->data,s+3,LINE_SIZE);
  *s = 0;

  //get data
  s = reg_st->data;
  while (*s && *s != '\"' && *(s+1)!= ';')s++;
  if (*s != '\"')return FALSE;

  char tmp_format[LINE_SIZE];
  strncpy(tmp_format,s+3,LINE_SIZE);
  *s = 0;

  //get format
  s = tmp_format;
  while (*s && *s != '\"' && *(s+1)!= ';')s++;
  if (*s != '\"')return FALSE;

  strncpy(reg_st->description,s+3,LINE_SIZE);
  *s = 0;

  //CHKEY in write mode !
  if (reg_write)
  {
    strncpy(reg_st->chkey,reg_st->description,LINE_SIZE);
    s = reg_st->chkey;
    while (*s && *s != '\"' && *(s+1)!= ';')s++;
    if (*s != '\"')return FALSE;

    strncpy(reg_st->description,s+3,LINE_SIZE);
    *s = 0;
  }

  //get description
  s = reg_st->description;
  while (*s && *s != '\"' && *(s+1)!= ';')s++;
  if (*s != '\"')return FALSE;

  char tmp_check[LINE_SIZE];
  strncpy(tmp_check,s+3,LINE_SIZE);
  *s = 0;

  //word or dword
  if (tmp_format[0] == 'D' || tmp_format[0] == 'd' || tmp_format[0] == 'W' || tmp_format[0] == 'w')
  {
    reg_st->data_dword  = TRUE;
    reg_st->data_string = FALSE;
  }else
  {
    reg_st->data_dword  = FALSE;
    reg_st->data_string = TRUE;
  }

  //check
  reg_st->check_equal     = FALSE;
  reg_st->check_inf       = FALSE;
  reg_st->check_sup       = FALSE;
  reg_st->check_diff      = FALSE;
  reg_st->check_content   = FALSE;
  reg_st->check_no_data   = FALSE;
  reg_st->check_no_value  = FALSE;

  if (reg_st->data_dword)
  {
    switch(tmp_check[0])
    {
      case '=':reg_st->check_equal    = TRUE;break;
      case '<':reg_st->check_inf      = TRUE;break;
      case '>':reg_st->check_sup      = TRUE;break;
      case '!':reg_st->check_diff     = TRUE;break;
      case '*':reg_st->check_no_data  = TRUE;break;
      case '\"':
      case ' ':
      default:reg_st->check_no_value  = TRUE;break;
    }
  }else
  {
    switch(tmp_check[0])
    {
      case '=':reg_st->check_equal    = TRUE;break;
      case '!':reg_st->check_diff     = TRUE;break;
      case '?':reg_st->check_content  = TRUE;break;
      case '*':reg_st->check_no_data  = TRUE;break;
      case '\"':
      case ' ':
      default:reg_st->check_no_value  = TRUE;break;
    }
  }

  return TRUE;
}
//----------------------------------------------------------------
BOOL RegistryOS(DWORD iitem,HKEY hkey)
{
  BOOL ret = FALSE;
  char ch_datas[LINE_SIZE]="";
  char ch_os[MAX_PATH]="";
  char ch_sp[MAX_PATH]="";
  if (ReadValue(hkey,(char*)"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\",(char*)"ProductName",ch_os, MAX_PATH))
  {
    if (strlen(ch_os)>2)
    {
      ret = TRUE;
      if (ReadValue(hkey,(char*)"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\",(char*)"CSDVersion",ch_sp, MAX_PATH))
      {
        if (strlen(ch_sp)>2)
        {
          snprintf(ch_datas,LINE_SIZE,"%s %s",ch_os,ch_sp);
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,ch_datas);
        }else ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,ch_os);
      }else ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,ch_os);
    }
  }
  return ret;
}
//----------------------------------------------------------------
void RegistryScan(DWORD iitem,char *ip, HKEY hkey, char* chkey)
{
  //get datas to check
  char buffer[LINE_SIZE];
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  char msg[LINE_SIZE];
  HKEY CleTmp;
  REG_LINE_ST reg_st;
  DWORD dw_datas, ok;
  char ch_datas[LINE_SIZE];

  for (i=0;i<_nb_i && scan_start;i++)
  {
    if (SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) > LINE_SIZE)continue;
    if (SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      //for title line add # after the "
      //line format :"SYSTEM\\CurrentControlSet\\Services\\";"value";"data";"format";"check";
      //data format : string/dword
      //check format: =<>!*
      if (parseLineToReg(buffer,&reg_st,FALSE))
      {
        if (RegOpenKey(hkey,reg_st.path,&CleTmp)==ERROR_SUCCESS)
        {
          if (reg_st.check_no_value)
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s (%s)",ip,chkey,reg_st.path,reg_st.description);
            AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Key Exist");
            AddLSTVUpdateItem(msg, COL_REG, iitem);
          }else
          {
            ok          = 0;
            dw_datas    = 0;
            ch_datas[0] = 0;

            if (reg_st.data_dword)ok = ReadValue(hkey,reg_st.path,reg_st.value,&dw_datas, sizeof(DWORD));
            else ok = ReadValue(hkey,reg_st.path,reg_st.value,ch_datas, LINE_SIZE);
            if (!ok)continue;

            //check
            if (reg_st.check_no_data)
            {
              if (reg_st.data_dword)snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.description);
              else snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s (%s)",ip,chkey,reg_st.path,reg_st.value,ch_datas,reg_st.description);

              AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
              AddLSTVUpdateItem(msg, COL_REG, iitem);
            }else if (reg_st.check_equal)
            {
              if (reg_st.data_dword)
              {
                if (atol(reg_st.data) == dw_datas)
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[=%s:OK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
                }else
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[=%s:NOK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
                }
                AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }else if (reg_st.data_string)
              {
                if (!strcmp(reg_st.data,ch_datas))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[=%s:OK] (%s)",ip,chkey,reg_st.path,reg_st.value,ch_datas,reg_st.data,reg_st.description);
                }else
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[=%s:NOK] (%s)",ip,chkey,reg_st.path,reg_st.value,ch_datas,reg_st.data,reg_st.description);
                }
                AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }
            }else if (reg_st.check_diff)
            {
              if (reg_st.data_dword)
              {
                if (atol(reg_st.data) != dw_datas)
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[!=%s:OK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
                }else
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[!=%s:NOK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
                }
                AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }else if (reg_st.data_string)
              {
                if (strcmp(reg_st.data,ch_datas))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[!=%s:OK] (%s)",ip,chkey,reg_st.path,reg_st.value,ch_datas,reg_st.data,reg_st.description);
                }else
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[!=%s:NOK] (%s)",ip,chkey,reg_st.path,reg_st.value,ch_datas,reg_st.data,reg_st.description);
                }
                AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }
            }else if (reg_st.check_inf)
            {
              if (atol(reg_st.data) > dw_datas)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[<%s:OK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[<%s:NOK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
              }
              AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
              AddLSTVUpdateItem(msg, COL_REG, iitem);
            }else if (reg_st.check_sup)
            {
              if (atol(reg_st.data) < dw_datas)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[>%s:OK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[>%s:NOK] (%s)",ip,chkey,reg_st.path,reg_st.value,dw_datas,reg_st.data,reg_st.description);
              }
              AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
              AddLSTVUpdateItem(msg, COL_REG, iitem);
            }else if (reg_st.check_content)
            {
              if (Contient(charToLowChar(ch_datas),charToLowChar(reg_st.data))>-1)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[?%s:OK] (%s)",ip,chkey,reg_st.path,reg_st.value,ch_datas,reg_st.data,reg_st.description);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[?%s:NOK] (%s)",ip,chkey,reg_st.path,reg_st.value,ch_datas,reg_st.data,reg_st.description);
              }
              AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"");
              AddLSTVUpdateItem(msg, COL_REG, iitem);
            }
          }
          RegCloseKey(CleTmp);
        }
      }
    }
  }
}
//------------------------------------------------------------------------------
void RegistryServiceScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Services");
  HKEY CleTmp;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,nbSubKey = 0;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key_path[LINE_SIZE],name[LINE_SIZE],msg[LINE_SIZE],ImagePath[LINE_SIZE];
      DWORD key_size;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistryServiceScan:RegEnumKeyEx=OK",ip);
          #endif
          name[0]   = 0;
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (ReadValue(hkey,key_path,(char*)"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }else if (LSBExist(CB_T_SERVICES, name))
            {
              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,name);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,name);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//----------------------------------------------------------------
void RegistrySoftwareScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Softwares");
  HKEY CleTmp;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,nbSubKey = 0;
    FILETIME LastWriteTime;// dernière mise a jour ou creation de la cle
    SYSTEMTIME SysTime;

    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key_path[LINE_SIZE],name[LINE_SIZE],msg[LINE_SIZE],ImagePath[LINE_SIZE],lastWriteDate[MAX_PATH];
      DWORD key_size;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistrySoftwareScan:RegEnumKeyEx2=OK",ip);
          #endif
          name[0]   = 0;
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (ReadValue(hkey,key_path,(char*)"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                snprintf(lastWriteDate,MAX_PATH," (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lastWriteDate[0] = 0;

              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sinstalllocation=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sInstallSource=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sUninstallString=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }else if (LSBExist(CB_T_SOFTWARE, name))
            {
              if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                snprintf(lastWriteDate,MAX_PATH," (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lastWriteDate[0] = 0;

              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sinstalllocation=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sInstallSource=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sUninstallString=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                snprintf(lastWriteDate,MAX_PATH," (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lastWriteDate[0] = 0;

              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sinstalllocation=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//----------------------------------------------------------------
void RegistryUSBScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"USB");
  HKEY CleTmp,CleTmp2;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,j,nbSubKey = 0, nbSubKey2;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key2[LINE_SIZE],key_path[LINE_SIZE],key_path2[LINE_SIZE],msg[LINE_SIZE];
      DWORD key_size, key_size2;
      FILETIME LastWriteTime0,LastWriteTime;// dernière mise a jour ou creation de la cle
      SYSTEMTIME SysTime;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,&LastWriteTime0)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistryUSBScan:RegEnumKeyEx=OK",ip);
          #endif
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (RegOpenKey(hkey,key_path,&CleTmp2)!=ERROR_SUCCESS)continue;

          nbSubKey2 = 0;
          if (RegQueryInfoKey (CleTmp2,0,0,0,&nbSubKey2,0,0,0,0,0,0,0)==ERROR_SUCCESS)
          {
            for (j=0;j<nbSubKey2 && scan_start;j++)
            {
              key_size2 = LINE_SIZE;
              key2[0]   = 0;
              if (RegEnumKeyEx (CleTmp2,j,key2,&key_size2,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
              {
                snprintf(key_path2,LINE_SIZE,"%s%s",key_path,key2);
                if (LSBExist(CB_T_USB, key))
                {
                  if (FileTimeToSystemTime(&LastWriteTime0, &SysTime) != 0)
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",ip,key_path2,SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                  else
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path2);

                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key);
                  AddLSTVUpdateItem(msg, COL_USB, iitem);
                }else if (LSBExist(CB_T_USB, key2))
                {
                  if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",ip,key_path2,SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                  else
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path2);

                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key2);
                  AddLSTVUpdateItem(msg, COL_USB, iitem);
                }
              }
            }
          }
          RegCloseKey(CleTmp2);
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//----------------------------------------------------------------
void RegistryWriteKey(char *ip, HKEY hkey, char *chkey)
{
  //get datas to check
  char buffer[LINE_SIZE], val_s[LINE_SIZE];
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_REGISTRY_W,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  REG_LINE_ST reg_st;
  HKEY CleTmp;
  DWORD ddatas;
  char msg[MAX_PATH];
  DWORD val;

  for (i=0;i<_nb_i && scan_start;i++)
  {
    if (SendDlgItemMessage(h_main,CB_T_REGISTRY_W,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) > LINE_SIZE)continue;
    if (SendDlgItemMessage(h_main,CB_T_REGISTRY_W,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      //format :
      //"SOFTWARE\Microsoft\Windows NT\CurrentVersion\";"toto";"datas";"STRING";"HKLM";"value to check";"*";
      if (parseLineToReg(buffer,&reg_st,TRUE))
      {
        if (!strcmp(chkey, reg_st.chkey))
        {
          if (RegOpenKey(hkey,reg_st.path,&CleTmp)!=ERROR_SUCCESS)
          {
            if (RegCreateKeyEx(hkey,reg_st.path,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE ,NULL,&CleTmp,NULL)!=ERROR_SUCCESS)
              continue;
          }

          //check value
          if (reg_st.data_dword)
          {
            if (!reg_st.check_no_data && !reg_st.check_no_value)
            {
              //write value only
              ddatas = atol(reg_st.data);
              if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s",ip,chkey,reg_st.path,reg_st.value,reg_st.data);
                AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
              }else
              {
                snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
              }
            }else
            {
              //get value and verify
              ddatas = atol(reg_st.description);
              if (ReadValue(hkey,reg_st.path,reg_st.value,&val, sizeof(val)) != 0)
              {
                if (reg_st.check_diff && (val != ddatas))
                {
                  ddatas = atol(reg_st.data);
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s[!=%l:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
                  }
                }else if (reg_st.check_equal && (val == ddatas))
                {
                  ddatas = atol(reg_st.data);
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s[=%l:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
                  }
                }else if (reg_st.check_inf && (val < ddatas))
                {
                  ddatas = atol(reg_st.data);
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s[<%l:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
                  }
                }else if (reg_st.check_sup && (val > ddatas))
                {
                  ddatas = atol(reg_st.data);
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s[>%l:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
                  }
                }
              }
            }
          }else if (reg_st.data_string)
          {
            if (!reg_st.check_no_data && !reg_st.check_no_value)
            {
              //write value only
              if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_SZ,(BYTE*)(reg_st.data),strlen(reg_st.data)))==ERROR_SUCCESS)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(STRING)=%s",ip,chkey,reg_st.path,reg_st.value,reg_st.data);
                AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
              }else
              {
                snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
              }
            }else
            {
              //get value and verify
              if (ReadValue(hkey,reg_st.path,reg_st.value,val_s, sizeof(val_s)) != 0)
              {
                if (reg_st.check_diff && strcmp(val_s,reg_st.description)!= 0)
                {
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_SZ,(BYTE*)(reg_st.data),strlen(reg_st.data)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(STRING)=%s[!=%s:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val_s);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
                  }
                }else if (reg_st.check_equal && !strcmp(val_s,reg_st.description))
                {
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_SZ,(BYTE*)(reg_st.data),strlen(reg_st.data)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(STRING)=%s[=%s:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val_s);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
                  }
                }else if (reg_st.check_content && Contient(charToLowChar(val_s),charToLowChar(reg_st.description))>-1)
                {
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_SZ,(BYTE*)(reg_st.data),strlen(reg_st.data)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(STRING)=%s[=%s:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val_s);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"");
                  }
                }
              }
            }
          }
          RegCloseKey(CleTmp);
        }
      }
    }
  }
}
//----------------------------------------------------------------
BOOL RemoteRegistryNetConnexion(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, BOOL windows_OS, long int *id_ok)
{
  BOOL ret            = FALSE;
  HANDLE connect      = 0;
  char tmp[MAX_PATH]  = "", remote_name[MAX_PATH]  = "", msg[LINE_SIZE];

  connect = NetConnexionAuthenticateTest(ip, ip_id, remote_name,config, iitem, TRUE, id_ok);

  //for testing account policy
  if (config->disco_netbios_policy && scan_start)
  {
    char pol[MAX_PATH]="";
    wchar_t server[MAX_PATH];
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);

    WaitForSingleObject(hs_netbios,INFINITE);
    mbstowcs(server, tmp, MAX_PATH);

    if (Netbios_Policy(server, pol, MAX_PATH))
    {
      AddLSTVUpdateItem(pol, COL_POLICY, iitem);
    }
    ReleaseSemaphore(hs_netbios,1,NULL);
  }

  if ((config->check_registry || config->check_services || config->check_software || config->check_USB || config->write_key) && scan_start)
  {
    //net
    HKEY hkey;
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);

    LONG reg_access = RegConnectRegistry(tmp, HKEY_LOCAL_MACHINE, &hkey);
    BOOL start_remote_registry = FALSE;

    if (reg_access!=ERROR_SUCCESS && connect != 0)
    {
      if (StartRemoteRegistryService(ip, TRUE))
      {
        //wait 10 secondes
        Sleep(10000);
        start_remote_registry = TRUE;
        reg_access = RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey);
      }
    }

    if (reg_access==ERROR_SUCCESS)
    {
      if (config->local_account)
      {
        //snprintf(msg,LINE_SIZE,"%s with current session account.",ip);
        //AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");
      }else if (!connect)
      {
        snprintf(msg,LINE_SIZE,"%s with NULL session account.",ip);
        AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"");
      }

      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:RemoteRegistryNetConnexion:RegConnectRegistry=OK",ip);
      #endif

      //get Exact OS !!!
      if (RegistryOS(iitem,hkey))
      {
        if (!windows_OS)nb_windows++;
      }

      //work
      if (config->check_registry && scan_start)
      {
        RegistryScan(iitem,ip,hkey,(char*)"HKLM");

        //other check
        HKEY hkey2;
        if (RegConnectRegistry(tmp,HKEY_CLASSES_ROOT,&hkey2)==ERROR_SUCCESS)
        {
          RegistryScan(iitem,ip,hkey2,(char*)"HKCR");
          if (config->write_key && scan_start)RegistryWriteKey(ip,hkey2,(char*)"HKCR");
          RegCloseKey(hkey2);
        }
        if (RegConnectRegistry(tmp,HKEY_USERS,&hkey2)==ERROR_SUCCESS)
        {
          RegistryScan(iitem,ip,hkey2,(char*)"HKU");
          if (config->write_key && scan_start)RegistryWriteKey(ip,hkey2,(char*)"HKU");
          RegCloseKey(hkey2);
        }
      }

      if (config->check_services && scan_start)RegistryServiceScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Services\\",hkey);
      if (config->check_software && scan_start)
      {
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      }
      if (config->check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

      if (config->write_key && scan_start)RegistryWriteKey(ip,hkey,(char*)"HKLM");

      RegCloseKey(hkey);
      ret = TRUE;
    }

    if (start_remote_registry)
    {
      StartRemoteRegistryService(ip, FALSE);
    }
  }

  if(connect)
  {
    WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
    if (connect != (HANDLE)1)CloseHandle(connect);
  }

  return ret;
}
//----------------------------------------------------------------
BOOL RemoteConnexionScan(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, BOOL windows_OS, long int *id_ok)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","registry:RemoteConnexionScan",ip);
  #endif
  if(RemoteRegistryNetConnexion(iitem, ip, ip_id, config, windows_OS, id_ok))return TRUE;
  else
  {
    if (config->check_registry)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_REG,iitem);
    if (config->check_services)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_SERVICE,iitem);
    if (config->check_software)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_SOFTWARE,iitem);
    if (config->check_USB)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_USB,iitem);
  }
  return FALSE;
}
