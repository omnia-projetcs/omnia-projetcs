//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
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
BOOL parseLineToReg(char *line, REG_LINE_ST *reg_st)
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
    if (ch_os[0] != 0)
    {
      ret = TRUE;
      if (ReadValue(hkey,(char*)"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\",(char*)"CSDVersion",ch_sp, MAX_PATH))
      {
        if (ch_os[0] != 0)
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
  DWORD key_data_dw;
  HKEY CleTmp;
  REG_LINE_ST reg_st;
  DWORD dw_datas, ok;
  char ch_datas[LINE_SIZE];
  unsigned int key_data_type, key_src_data_type;

  for (i=0;i<_nb_i && scan_start;i++)
  {
    if (SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      //for title line add # after the "
      //line format :"SYSTEM\\CurrentControlSet\\Services\\";"value";"data";"format";"check";
      //data format : string/dword
      //check format: =<>!*
      if (parseLineToReg(buffer,&reg_st))
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
              if (Contient(charToLowChar(ch_datas),charToLowChar(reg_st.data)))
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
    FILETIME LastWriteTime;// derni�re mise a jour ou creation de la cle
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
      FILETIME LastWriteTime0,LastWriteTime;// derni�re mise a jour ou creation de la cle
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
void RegistryWriteKey(DWORD iitem,char *ip, HKEY hkey, char *chkey)
{
  //get datas to check
  char buffer[LINE_SIZE];
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_REGISTRY_W,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  REG_LINE_ST reg_st;
  HKEY CleTmp;
  DWORD ddatas;
  char msg[MAX_PATH];

  for (i=0;i<_nb_i && scan_start;i++)
  {
    if (SendDlgItemMessage(h_main,CB_T_REGISTRY_W,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      //format :
      //"SOFTWARE\Microsoft\Windows NT\CurrentVersion\";"toto";"datas";"STRING";"HKLM";"*";
      if (parseLineToReg(buffer,&reg_st))
      {
        if (!strcmp(chkey, reg_st.description))
        {
          if (RegOpenKey(hkey,reg_st.path,&CleTmp)==ERROR_SUCCESS)
          {
            if (reg_st.data_dword)
            {
              ddatas = atol(reg_st.data);
              if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s",ip,chkey,reg_st.path,reg_st.value,reg_st.data);
                AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
              }
            }else if (reg_st.data_string)
            {
              if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_SZ,(BYTE*)(reg_st.data),strlen(reg_st.data)))==ERROR_SUCCESS)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(STRING)=%s",ip,chkey,reg_st.path,reg_st.value,reg_st.data);
                AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"");
              }
            }
            RegCloseKey(CleTmp);
          }
        }
      }
    }
  }
}
//----------------------------------------------------------------
BOOL RemoteRegistryNetConnexion(DWORD iitem,char *name, char *ip, SCANNE_ST config, BOOL windows_OS)
{
  BOOL ret            = FALSE;
  HANDLE connect      = FALSE;
  char tmp[MAX_PATH]  = "", remote_name[MAX_PATH]  = "", msg[LINE_SIZE];

  connect = NetConnexionAuthenticateTest(ip, remote_name,config, iitem);

  //for testing account policy
  if (config.disco_netbios_policy && scan_start)
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

  if ((config.check_registry || config.check_services || config.check_software || config.check_USB || config.write_key) && scan_start)
  {
    //net
    HKEY hkey;
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);
    if (RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey)==ERROR_SUCCESS)
    {
      if (config.local_account)
      {
        snprintf(msg,LINE_SIZE,"Login (ScanReg:NET) in %s IP with current session account.",ip);
        AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
      }else if (!connect)
      {
        snprintf(msg,LINE_SIZE,"Login (ScanReg:NET) in %s IP with NULL session account.",ip);
        AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
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
      if (config.check_registry && scan_start)
      {
        RegistryScan(iitem,ip,hkey,(char*)"HKLM");

        //other check
        HKEY hkey2;
        if (RegConnectRegistry(tmp,HKEY_CLASSES_ROOT,&hkey2)==ERROR_SUCCESS)
        {
          RegistryScan(iitem,ip,hkey2,(char*)"HKCR");
          if (config.write_key && scan_start)RegistryWriteKey(iitem,ip,hkey2,(char*)"HKCR");
          RegCloseKey(hkey2);
        }
        if (RegConnectRegistry(tmp,HKEY_USERS,&hkey2)==ERROR_SUCCESS)
        {
          RegistryScan(iitem,ip,hkey2,(char*)"HKU");
          if (config.write_key && scan_start)RegistryWriteKey(iitem,ip,hkey2,(char*)"HKU");
          RegCloseKey(hkey2);
        }
      }

      if (config.check_services && scan_start)RegistryServiceScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Services\\",hkey);
      if (config.check_software && scan_start)
      {
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      }
      if (config.check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

      if (config.write_key && scan_start)RegistryWriteKey(iitem,ip,hkey,(char*)"HKLM");

      RegCloseKey(hkey);
      ret = TRUE;
    }
  }

  if(connect)
  {
    WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
    CloseHandle(connect);
  }

  return ret;
}
//----------------------------------------------------------------
BOOL RemoteConnexionScan(DWORD iitem, char *name, char *ip, SCANNE_ST config, BOOL windows_OS)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","registry:RemoteConnexionScan",ip);
  #endif
  if(RemoteRegistryNetConnexion(iitem, name, ip, config, windows_OS))return TRUE;
  else
  {
    AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_REG,iitem);
    AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_SERVICE,iitem);
    AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_SOFTWARE,iitem);
    AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_USB,iitem);
  }
  return FALSE;
}
