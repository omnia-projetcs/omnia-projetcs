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
  AddMsg(h_main,(char*)"DEBUG (OpenSCManager)",ip,(char*)"StartRemoteRegistryService START",FALSE);
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
      AddMsg(h_main,(char*)"DEBUG (OpenSCManager)",ip,(char*)"StartRemoteRegistryService START SERVICE OK",FALSE);
      #endif // DEBUG_MODE_REGISTRY
    }
  }else if (REG_REMOTE_SERVICE_STOP)
  {
    //close all dependances
    DWORD dwBytesNeeded;
    DWORD i, dwCount;
    LPENUM_SERVICE_STATUS   lpDependencies = NULL;

    if (!EnumDependentServices(hos, SERVICE_ACTIVE, lpDependencies, 0, &dwBytesNeeded, &dwCount))
    {
      lpDependencies = (LPENUM_SERVICE_STATUS) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);
      if (lpDependencies != NULL)
      {
        if (EnumDependentServices(hos, SERVICE_ACTIVE, lpDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount))
        {
          SC_HANDLE               hDepService;
          ENUM_SERVICE_STATUS     ess;
          SERVICE_STATUS_PROCESS  ssp;

          DWORD dwStartTime = GetTickCount();
          DWORD dwTimeout = 30000; // 30-second time-out

          for ( i = 0; i < dwCount; i++ )
          {
            ess = *(lpDependencies + i);
            hDepService = OpenService(hm, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS );
            if (hDepService == NULL)break;

            // Send a stop code.
            if (ControlService(hDepService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp))
            {
              while (ssp.dwCurrentState != SERVICE_STOPPED)
              {
                Sleep(ssp.dwWaitHint);
                if ( !QueryServiceStatusEx(hDepService,SC_STATUS_PROCESS_INFO,(LPBYTE)&ssp,sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))break;
                if (ssp.dwCurrentState == SERVICE_STOPPED)break;
                if (GetTickCount() - dwStartTime > dwTimeout)break;
              }
            }
            CloseServiceHandle(hDepService);
          }
        }
        HeapFree(GetProcessHeap(), 0, lpDependencies);
      }
    }

    //close service
    if (ControlService(hos, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)NULL) != FALSE)
    {
      ret = TRUE;
      #ifdef DEBUG_MODE_REGISTRY
      AddMsg(h_main,(char*)"DEBUG (OpenSCManager)",ip,(char*)"StartRemoteRegistryService STOP SERVICE OK",FALSE);
      #endif // DEBUG_MODE_REGISTRY
    }
  }

  //close
  CloseServiceHandle(hos);
  CloseServiceHandle(hm);
  return ret;
}
//----------------------------------------------------------------
BOOL CheckServiceOpenSC(DWORD iitem, char *ip)
{
  BOOL ret = FALSE;

  //Open RPC
  SC_HANDLE hm = OpenSCManager(ip, NULL, GENERIC_READ|SERVICE_QUERY_CONFIG);
  if (hm != NULL)
  {
    DWORD rd = 0, sz = 0;
    SC_HANDLE hos;
    QUERY_SERVICE_CONFIG *psc;
    char state[MAX_PATH],msg[LINE_SIZE];

    //get all service name to test one by one
    char service_name[LINE_SIZE];
    DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_SERVICES,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
    for (i=0;i<_nb_i;i++)
    {
      if (SendDlgItemMessage(h_main,CB_T_SERVICES,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) < LINE_SIZE)
      {
        if (SendDlgItemMessage(h_main,CB_T_SERVICES,LB_GETTEXT,(WPARAM)i,(LPARAM)service_name))
        {
          //Open the service
          hos = OpenService(hm, service_name, GENERIC_READ|SERVICE_QUERY_CONFIG);
          if(hos != NULL)
          {
            //get size for buffer
            QueryServiceConfig(hos, NULL, 0, &sz);
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
              if (sz > 0)
              {
                psc = malloc(sz+1);
                if (psc != NULL)
                {
                  //get informations
                  if (QueryServiceConfig(hos, psc, sz, &rd))
                  {
                    switch(psc->dwStartType)
                    {
                      case 0: strcpy(state,"Start in Boot");break;
                      case 1: strcpy(state,"Start by system");break;
                      case 2: strcpy(state,"Automatic start ");break;
                      case 3: strcpy(state,"Manual start ");break;
                      case 4: strcpy(state,"Disable");break;
                      default:strcpy(state,"Unknow");break;
                    }

                    snprintf(msg,LINE_SIZE,"%s\\RPC\\ImagePath=%s (state:%s)",ip,psc->lpBinaryPathName,state);
                    AddMsg(h_main,(char*)"FOUND (Service)",msg,service_name,FALSE);
                    AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
                    ret = TRUE;
                  }
                  free(psc);
                }
              }
            }
            CloseServiceHandle(hos);
          }
        }
      }
    }
    CloseServiceHandle(hm);
  }

  return ret;
}
//----------------------------------------------------------------
DWORD ReadValue(HKEY hk,char *path,char *value,void *data, DWORD data_size)
{
  DWORD data_size_read = 0;
  HKEY CleTmp=0;

  //open key
  if (RegOpenKey(hk,path,&CleTmp)!=ERROR_SUCCESS)
  {
    //AddMsg(h_main,(char*)"DEBUG (Registry:ReadValue)",(char*)"Bad Key",(char*)"");
    return FALSE;
  }

  //size of data
  long r = RegQueryValueEx(CleTmp, value, 0, 0, 0, &data_size_read);
  if (r!=ERROR_SUCCESS && r!=ERROR_MORE_DATA)
  {
    RegCloseKey(CleTmp);
//    char tmp[MAX_PATH];
//    snprintf(tmp,MAX_PATH,"%d/%d : r:%d, %s %s",data_size_read,data_size,r,path,value);
//    AddMsg(h_main,(char*)"DEBUG (Registry:ReadValue)",(char*)"Bad Data size",(char*)tmp,FALSE);
    return FALSE;
  }

  //alloc
  data_size_read = data_size_read+1;
  char *c = (char *)malloc(data_size_read);
  if (c == NULL)
  {
    //AddMsg(h_main,(char*)"DEBUG (Registry:ReadValue)",(char*)"Bad malloc",(char*)"",FALSE);
    RegCloseKey(CleTmp);
    return FALSE;
  }

  //read value
  if (RegQueryValueEx(CleTmp, value, 0, 0, (LPBYTE)c, &data_size_read)!=ERROR_SUCCESS)
  {
    //AddMsg(h_main,(char*)"DEBUG (Registry:ReadValue)",(char*)"Bad query value",(char*)"",FALSE);
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
  #ifdef DEBUG_MODE_REGISTRY
  AddMsg(h_main,(char*)"DEBUG (parseLineToReg)","",(char*)line,FALSE);
  #endif // DEBUG_MODE_REGISTRY

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
  while (*s && *s != '\"' || *(s+1)!= ';')s++;
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

  #ifdef DEBUG_MODE_REGISTRY
  char tmp_[MAX_PATH]="";
  snprintf(tmp_,MAX_PATH,"%s, %s, %s, %s, %s, %s",reg_st->data_dword?"DWORD":"STRING",reg_st->chkey,reg_st->description,reg_st->data,reg_st->value,reg_st->path);
  AddMsg(h_main,(char*)"DEBUG (parseLineToReg)","",(char*)tmp_,FALSE);
  #endif // DEBUG_MODE_REGISTRY

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
void RegistryScanCheck(DWORD iitem,char *ip, HKEY hkey, char* chkey, REG_LINE_ST *reg_st, char *newpath)
{
  char msg[LINE_SIZE];
  HKEY CleTmp;
  DWORD dw_datas, ok;
  char ch_datas[LINE_SIZE];

  if (RegOpenKey(hkey,newpath!=NULL?newpath:reg_st->path,&CleTmp)==ERROR_SUCCESS)
  {
    #ifdef DEBUG_MODE_REGISTRY
    AddMsg(h_main,(char*)"DEBUG (RegistryScanCheck:RegOpenKey)",newpath!=NULL?newpath:reg_st->path,(char*)reg_st->value,FALSE);
    #endif // DEBUG_MODE_REGISTRY

    if (reg_st->check_no_value)
    {
      snprintf(msg,LINE_SIZE,"%s\\%s\\%s (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->description);
      AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"Registry Key Exist",FALSE);
      AddLSTVUpdateItem(msg, COL_REG, iitem);
    }else
    {
      ok          = 0;
      dw_datas    = 0;
      ch_datas[0] = 0;

      if (reg_st->data_dword)ok = ReadValue(hkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,&dw_datas, sizeof(DWORD));
      else ok = ReadValue(hkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas, MAX_PATH);
      if (!ok)return;

      #ifdef DEBUG_MODE_REGISTRY
      AddMsg(h_main,(char*)"DEBUG (RegistryScanCheck)",newpath!=NULL?newpath:reg_st->path,ch_datas,FALSE);
      #endif // DEBUG_MODE_REGISTRY

      //check
      if (reg_st->check_no_data)
      {
        if (reg_st->data_dword)snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->description);
        else snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas,reg_st->description);

        AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
        AddLSTVUpdateItem(msg, COL_REG, iitem);
      }else if (reg_st->check_equal)
      {
        if (reg_st->data_dword)
        {
          if (atol(reg_st->data) == dw_datas)
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[=%s:OK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
          }else
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[=%s:NOK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
          }
          AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
          AddLSTVUpdateItem(msg, COL_REG, iitem);
        }else if (reg_st->data_string)
        {
          if (!strcmp(reg_st->data,ch_datas))
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[=%s:OK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas,reg_st->data,reg_st->description);
          }else
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[=%s:NOK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas,reg_st->data,reg_st->description);
          }
          AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
          AddLSTVUpdateItem(msg, COL_REG, iitem);
        }
      }else if (reg_st->check_diff)
      {
        if (reg_st->data_dword)
        {
          if (atol(reg_st->data) != dw_datas)
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[!=%s:OK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
          }else
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[!=%s:NOK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
          }
          AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
          AddLSTVUpdateItem(msg, COL_REG, iitem);
        }else if (reg_st->data_string)
        {
          if (strcmp(reg_st->data,ch_datas))
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[!=%s:OK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas,reg_st->data,reg_st->description);
          }else
          {
            snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[!=%s:NOK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas,reg_st->data,reg_st->description);
          }
          AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
          AddLSTVUpdateItem(msg, COL_REG, iitem);
        }
      }else if (reg_st->check_inf)
      {
        if (atol(reg_st->data) > dw_datas)
        {
          snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[<%s:OK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
        }else
        {
          snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[<%s:NOK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
        }
        AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
        AddLSTVUpdateItem(msg, COL_REG, iitem);
      }else if (reg_st->check_sup)
      {
        if (atol(reg_st->data) < dw_datas)
        {
          snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[>%s:OK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
        }else
        {
          snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%lu[>%s:NOK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,dw_datas,reg_st->data,reg_st->description);
        }
        AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
        AddLSTVUpdateItem(msg, COL_REG, iitem);
      }else if (reg_st->check_content)
      {
        if (Contient(charToLowChar(ch_datas),charToLowChar(reg_st->data))>-1)
        {
          snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[?%s:OK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas,reg_st->data,reg_st->description);
        }else
        {
          snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s=%s[?%s:NOK] (%s)",ip,chkey,newpath!=NULL?newpath:reg_st->path,reg_st->value,ch_datas,reg_st->data,reg_st->description);
        }
        AddMsg(h_main,(char*)"FOUND (Registry)",msg,(char*)"",FALSE);
        AddLSTVUpdateItem(msg, COL_REG, iitem);
      }
    }
    RegCloseKey(CleTmp);
  }
}
//----------------------------------------------------------------
void RegistryScan(DWORD iitem,char *ip, HKEY hkey, char* chkey, BOOL hkey_users)
{
  //get datas to check
  char buffer[LINE_SIZE];
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  char msg[LINE_SIZE];
  HKEY CleTmp;
  REG_LINE_ST reg_st;
  DWORD dw_datas, ok;
  char ch_datas[LINE_SIZE];
  BOOL hkey_users_ok = FALSE;

  DWORD j,nbSubKey = 0;
  DWORD key_size;
  char key[LINE_SIZE],key_path[LINE_SIZE], newpath[LINE_SIZE];

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
        #ifdef DEBUG_MODE_REGISTRY
        AddMsg(h_main,(char*)"DEBUG (parseLineToReg)",reg_st.path,(char*)reg_st.value,FALSE);
        #endif // DEBUG_MODE_REGISTRY

        //check if in ".DEFAULT" or not
        hkey_users_ok = FALSE;
        if (hkey_users)
        {
          if (reg_st.path[0] == '.' && strlen(reg_st.path) == 8 && reg_st.path[7] == 'T')
            hkey_users_ok = FALSE;
          else hkey_users_ok = TRUE;
        }

        if (hkey_users_ok)
        {
          nbSubKey = 0;
          if (RegQueryInfoKey (hkey,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
          {
            for (j=0;j<nbSubKey && scan_start;j++)
            {
              //init datas to read
              key_size  = LINE_SIZE;
              key[0]    = 0;
              if (RegEnumKeyEx (hkey,j,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
              {
                snprintf(newpath,LINE_SIZE,"%s\\%s",key,reg_st.path);

                //check !
                RegistryScanCheck(iitem,ip, hkey, chkey, &reg_st, newpath);
              }
            }
          }
        }else
        {
          //check !
          RegistryScanCheck(iitem,ip, hkey, chkey, &reg_st, NULL);
        }
      }
    }
  }
}
//------------------------------------------------------------------------------
int RegistryServiceScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Services");
  HKEY CleTmp;
  int ok = -1;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,nbSubKey = 0, d_tmp = 0;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      ok = 0;
      char key[LINE_SIZE],key_path[LINE_SIZE],name[LINE_SIZE],msg[LINE_SIZE],ImagePath[LINE_SIZE],state[MAX_PATH];
      DWORD key_size;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistryServiceScan:RegEnumKeyEx=OK",ip,FALSE);
          #endif
          name[0]   = 0;
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (ReadValue(hkey,key_path,(char*)"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              ok++;
              state[0] = 0;
              if(ReadValue(hkey,key_path,(char*)"Start",&d_tmp, sizeof(d_tmp))!= 0)
              {
                switch(d_tmp)
                {
                  case 0: strcpy(state,"Start in Boot");break;
                  case 1: strcpy(state,"Start by system");break;
                  case 2: strcpy(state,"Automatic start ");break;
                  case 3: strcpy(state,"Manual start ");break;
                  case 4: strcpy(state,"Disable");break;
                  default:strcpy(state,"Unknow");break;
                }
              }

              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sImagePath=%s (state:%s)",ip,key_path,ImagePath,state);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key,FALSE);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (state:%s)",ip,key_path,state);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key,FALSE);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }else if (LSBExist(CB_T_SERVICES, name))
            {
              ok++;
              state[0] = 0;
              if(ReadValue(hkey,key_path,(char*)"Start",&d_tmp, sizeof(d_tmp))!= 0)
              {
                switch(d_tmp)
                {
                  case 0: strcpy(state,"Start in Boot");break;
                  case 1: strcpy(state,"Start by system");break;
                  case 2: strcpy(state,"Automatic start ");break;
                  case 3: strcpy(state,"Manual start ");break;
                  case 4: strcpy(state,"Disable");break;
                  default:strcpy(state,"Unknow");break;
                }
              }

              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sImagePath=%s (state:%s)",ip,key_path,ImagePath,state);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,name,FALSE);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (state:%s)",ip,key_path,state);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,name,FALSE);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              state[0] = 0;
              if(ReadValue(hkey,key_path,(char*)"Start",&d_tmp, sizeof(d_tmp))!= 0)
              {
                switch(d_tmp)
                {
                  case 0: strcpy(state,"Start in Boot");break;
                  case 1: strcpy(state,"Start by system");break;
                  case 2: strcpy(state,"Automatic start ");break;
                  case 3: strcpy(state,"Manual start ");break;
                  case 4: strcpy(state,"Disable");break;
                  default:strcpy(state,"Unknow");break;
                }
              }

              ok++;
              if (ReadValue(hkey,key_path,(char*)"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sImagePath=%s (state:%s)",ip,key_path,ImagePath,state);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key,FALSE);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (state:%s)",ip,key_path,state);
                AddMsg(h_main,(char*)"FOUND (Service)",msg,key,FALSE);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
  return ok;
}
//----------------------------------------------------------------
int RegistrySoftwareScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Softwares");
  HKEY CleTmp;
  int ok = -1;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,nbSubKey = 0;
    FILETIME LastWriteTime;// dernière mise a jour ou creation de la cle
    SYSTEMTIME SysTime;

    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key_path[LINE_SIZE],name[LINE_SIZE],msg[LINE_SIZE],ImagePath[LINE_SIZE],lastWriteDate[MAX_PATH];
      DWORD key_size;
      ok =0;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistrySoftwareScan:RegEnumKeyEx2=OK",ip,FALSE);
          #endif
          name[0]   = 0;
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (ReadValue(hkey,key_path,(char*)"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              ok++;
              if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                snprintf(lastWriteDate,MAX_PATH," (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lastWriteDate[0] = 0;

              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sinstalllocation=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key,FALSE);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sInstallSource=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key,FALSE);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sUninstallString=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key,FALSE);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key,FALSE);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }else if (LSBExist(CB_T_SOFTWARE, name))
            {
              ok++;
              if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                snprintf(lastWriteDate,MAX_PATH," (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lastWriteDate[0] = 0;

              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sinstalllocation=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name,FALSE);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sInstallSource=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name,FALSE);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,(char*)"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sUninstallString=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name,FALSE);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,name,FALSE);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              ok++;
              if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                snprintf(lastWriteDate,MAX_PATH," (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
              else lastWriteDate[0] = 0;

              if (ReadValue(hkey,key_path,(char*)"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%sinstalllocation=%s %s",ip,key_path,ImagePath,lastWriteDate);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key,FALSE);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path);
                AddMsg(h_main,(char*)"FOUND (Software)",msg,key,FALSE);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
  return ok;
}
//----------------------------------------------------------------
//V2
int RegistryUSBScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"USB");
  HKEY CleTmp,CleTmp2;
  int ok = -1;

  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,j,nbSubKey = 0, nbSubKey2;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      ok = 0;
      char key[LINE_SIZE],key2[LINE_SIZE],key_path[LINE_SIZE],key_path2[LINE_SIZE],msg[LINE_SIZE];
      DWORD key_size, key_size2;
      FILETIME LastWriteTime;// dernière mise a jour ou creation de la cle
      SYSTEMTIME SysTime;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","registry:RegistryUSBScan:RegEnumKeyEx=OK",ip,FALSE);
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

                if (LSBExistC(CB_T_USB, key2))
                {
                  ok++;
                  if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (UTC? Last Write Time [Add/remove])%02d/%02d/%02d-%02d:%02d:%02d)",ip,key_path2,SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                  else
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path2);

                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key2,FALSE);
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
  return ok;
}
/*
//----------------------------------------------------------------
int RegistryUSBScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"USB");
  HKEY CleTmp,CleTmp2;
  int ok = -1;

  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,j,nbSubKey = 0, nbSubKey2;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      ok = 0;
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
          AddMsg(h_main,"DEBUG","registry:RegistryUSBScan:RegEnumKeyEx=OK",ip,FALSE);
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
                if (LSBExist(CB_T_USB, key2))
                {
                  ok++;
                  if (FileTimeToSystemTime(&LastWriteTime, &SysTime) != 0)
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (Last Write Time %02d/%02d/%02d-%02d:%02d:%02d)",ip,key_path2,SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                  else
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path2);

                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key2,FALSE);
                  AddLSTVUpdateItem(msg, COL_USB, iitem);
                }else if (LSBExist(CB_T_USB, key))
                {
                  ok++;
                  if (FileTimeToSystemTime(&LastWriteTime0, &SysTime) != 0)
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s (Last Write Time. %02d/%02d/%02d-%02d:%02d:%02d)",ip,key_path2,SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
                  else
                    snprintf(msg,LINE_SIZE,"%s\\HKLM\\%s",ip,key_path2);

                  AddMsg(h_main,(char*)"FOUND (USB)",msg,key,FALSE);
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
  return ok;
}
*/
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
                AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
              }else
              {
                snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
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
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
                  }
                }else if (reg_st.check_equal && (val == ddatas))
                {
                  ddatas = atol(reg_st.data);
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s[=%l:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
                  }
                }else if (reg_st.check_inf && (val < ddatas))
                {
                  ddatas = atol(reg_st.data);
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s[<%l:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
                  }
                }else if (reg_st.check_sup && (val > ddatas))
                {
                  ddatas = atol(reg_st.data);
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_DWORD,(BYTE*)&ddatas,sizeof(ddatas)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(DWORD)=%s[>%l:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
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
                AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
              }else
              {
                snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
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
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
                  }
                }else if (reg_st.check_equal && !strcmp(val_s,reg_st.description))
                {
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_SZ,(BYTE*)(reg_st.data),strlen(reg_st.data)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(STRING)=%s[=%s:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val_s);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
                  }
                }else if (reg_st.check_content && Contient(charToLowChar(val_s),charToLowChar(reg_st.description))>-1)
                {
                  if ((RegSetValueEx(CleTmp,reg_st.value,0,REG_SZ,(BYTE*)(reg_st.data),strlen(reg_st.data)))==ERROR_SUCCESS)
                  {
                    snprintf(msg,LINE_SIZE,"%s\\%s\\%s%s(STRING)=%s[=%s:OK]",ip,chkey,reg_st.path,reg_st.value,reg_st.data,val_s);
                    AddMsg(h_main,(char*)"WRITE (Registry)",msg,(char*)"",FALSE);
                  }else
                  {
                    snprintf(msg,LINE_SIZE,"NO RIGHT TO WRITE IN %s\\%s\\%s%s",ip,chkey,reg_st.path,reg_st.value);
                    AddMsg(h_main,(char*)"WRITE ERROR (Registry)",msg,(char*)"",FALSE);
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
BOOL Registry_List_users(DWORD iitem, char *ip, HKEY hkey)
{
  BOOL ret = FALSE;
 /* char  msg[LINE_SIZE] = "",
        user[MAX_PATH] = "",
        key[MAX_PATH] = "",
        tmp[MAX_PATH] = "";

  DWORD i,nbSubKey = 0, key_size = 0;
  HKEY CleTmp = NULL;

  //read list of local users !
  if (RegOpenKey(hkey,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI\\SessionData\\",&CleTmp)!=ERROR_SUCCESS)
  {
    nbSubKey = 0;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      for (i=0;i<nbSubKey && scan_start;i++)
      {
        key[0]   = 0;
        key_size = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          snprintf(tmp,LINE_SIZE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI\\SessionData\\%s\\",key);

          user[0] = 0;
          if (ReadValue(hkey,tmp,(char*)"LoggedOnUsername",user, MAX_PATH))
          {
            if (strlen(user)>2)
            {
              ret = TRUE;
              snprintf(msg,LINE_SIZE,"%s\\%s\\LoggedOnUsername=%s",ip,tmp,user);
              AddMsg(h_main,(char*)"FOUND (Registry:LoggedOnUsername)",msg,(char*)"",FALSE);
              AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
            }
          }else
          {
            user[0] = 0;
            if (ReadValue(hkey,tmp,(char*)"LoggedOnSAMUser",user, MAX_PATH))
            {
              if (strlen(user)>2)
              {
                ret = TRUE;
                snprintf(msg,LINE_SIZE,"%s\\%s\\LoggedOnSAMUser=%s",ip,tmp,user);
                AddMsg(h_main,(char*)"FOUND (Registry:LoggedOnSAMUser)",msg,(char*)"",FALSE);
                AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
              }
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }

  CleTmp = NULL;
  //read list of local users !
  if (RegOpenKey(hkey,"SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\",&CleTmp)!=ERROR_SUCCESS)
  {
    nbSubKey = 0;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      for (i=0;i<nbSubKey && scan_start;i++)
      {
        key[0]   = 0;
        key_size = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          snprintf(tmp,LINE_SIZE,"SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\%s\\",key);
          user[0] = 0;
          if (ReadValue(hkey,tmp,(char*)"ProfileImagePath",user, MAX_PATH))
          {
            if (strlen(user)>2)
            {
              ret = TRUE;
              snprintf(msg,LINE_SIZE,"%s\\%s\\ProfileImagePath=%s",ip,tmp,user);
              AddMsg(h_main,(char*)"FOUND (Registry:ProfileImagePath)",msg,(char*)"",FALSE);
              AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
            }
          }else
          {
            ret = TRUE;
            snprintf(msg,LINE_SIZE,"%s\\%s",ip,tmp);
            AddMsg(h_main,(char*)"FOUND (Registry:User Full SID)",msg,(char*)"",FALSE);
            AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }

  if (RegOpenKey(hkey,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\",&CleTmp)!=ERROR_SUCCESS)
  {
    nbSubKey = 0;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      for (i=0;i<nbSubKey && scan_start;i++)
      {
        key[0]   = 0;
        key_size = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          snprintf(tmp,LINE_SIZE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\%s\\",key);
          user[0] = 0;
          if (ReadValue(hkey,tmp,(char*)"ProfileImagePath",user, MAX_PATH))
          {
            if (strlen(user)>2)
            {
              ret = TRUE;
              snprintf(msg,LINE_SIZE,"%s\\%s\\ProfileImagePath=%s",ip,tmp,user);
              AddMsg(h_main,(char*)"FOUND (Registry:ProfileImagePath)",msg,(char*)"",FALSE);
              AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
            }
          }else
          {
            ret = TRUE;
            snprintf(msg,LINE_SIZE,"%s\\%s",ip,tmp);
            AddMsg(h_main,(char*)"FOUND (Registry:User Full SID)",msg,(char*)"",FALSE);
            AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }

  DWORD nbValues = 0, value_size = LINE_SIZE, data_size = LINE_SIZE, type;
  char value[LINE_SIZE], data[LINE_SIZE];

  if (RegOpenKey(hkey,"SYSTEM\\CurrentControlSet\\Control\\hivelist\\",&CleTmp)!=ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbValues,0,0,0,0)==ERROR_SUCCESS)
    {
      for (i=0;i<nbValues && scan_start;i++)
      {
        value_size  = LINE_SIZE;
        value[0]    = 0;
        data_size   = LINE_SIZE;
        data[0]     = 0;
        type        = 0;

        if (RegEnumValue (CleTmp,i,value,&value_size,0,&type,(LPBYTE)data,&data_size)==ERROR_SUCCESS)
        {
          ret = TRUE;
          snprintf(msg,LINE_SIZE,"%s\\SYSTEM\\CurrentControlSet\\Control\\hivelist\\%s=%s",ip,value,data);
          AddMsg(h_main,(char*)"FOUND (Registry:Hive list)",msg,(char*)"",FALSE);
          AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
        }

       /*
        key[0]   = 0;
        key_size = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          snprintf(tmp,LINE_SIZE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\%s\\",key);
          user[0] = 0;
          if (ReadValue(hkey,tmp,(char*)"ProfileImagePath",user, MAX_PATH))
          {
            if (strlen(user)>2)
            {
              ret = TRUE;
              snprintf(msg,LINE_SIZE,"%s\\%s\\ProfileImagePath=%s",ip,tmp,user);
              AddMsg(h_main,(char*)"FOUND (Config:ProfileImagePath)",msg,(char*)"",FALSE);
              AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
            }
          }else
          {
            ret = TRUE;
            snprintf(msg,LINE_SIZE,"%s\\%s",ip,tmp);
            AddMsg(h_main,(char*)"FOUND (Config:User Full SID)",msg,(char*)"",FALSE);
            AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          }
        }*//*
      }
    }
    RegCloseKey(CleTmp);
  }
*/
  return ret;
}
//----------------------------------------------------------------
BOOL RemoteRegistryNetConnexion(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, BOOL windows_OS, long int *id_ok, BOOL users_check, BOOL os_check)
{
  BOOL ret            = FALSE;
  HANDLE connect      = 0;
  char tmp[MAX_PATH]  = "", remote_name[MAX_PATH]  = "", msg[LINE_SIZE];

  if (config->local_account)
  {
    connect = (HANDLE)1;
  }else
  {
    connect = NetConnexionAuthenticateTest(ip, ip_id, remote_name,config, iitem, TRUE, id_ok);
  }

  //for testing account policy
  if (config->disco_netbios_policy && scan_start)
  {
    char pol[MAX_PATH]="";
    wchar_t server[MAX_PATH];
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);

    //WaitForSingleObject(hs_netbios,INFINITE);
    mbstowcs(server, tmp, MAX_PATH);

    if (Netbios_Policy(server, pol, MAX_PATH))
    {
      AddLSTVUpdateItem(pol, COL_POLICY, iitem);
    }
    //ReleaseSemaphore(hs_netbios,1,NULL);
  }

  if (config->disco_netbios_users && scan_start && users_check)
  {
    Netbios_List_users(iitem, ip);
  }

  if ((config->check_registry || config->check_services || config->check_software || config->check_USB || config->write_key || os_check || (users_check && config->disco_users)) && scan_start)
  {
    //net
    HKEY hkey;
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);

    LONG reg_access = RegConnectRegistry(tmp, HKEY_LOCAL_MACHINE, &hkey);
    BOOL start_remote_registry = FALSE;

    /*if(reg_access!=ERROR_SUCCESS && config->local_account)
    {
      if (LogonUser((LPTSTR)"NETWORK SERVICE", (LPTSTR)"NT AUTHORITY", NULL, /*LOGON32_LOGON_NEW_CREDENTIALS*//*9, /*LOGON32_PROVIDER_WINNT50*//*3, &connect))
      {
        if (connect != 0)
        {
          ImpersonateLoggedOnUser(connect);
        }
      }
      if (connect == 0) connect = (HANDLE)1;
    }*/
	
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
        //if(!LOG_LOGIN_DISABLE)AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"",FALSE);
      }else if (!connect)
      {
        snprintf(msg,LINE_SIZE,"%s with NULL session account.",ip);
        if(!LOG_LOGIN_DISABLE)AddMsg(h_main,(char*)"LOGIN (Registry:NET)",msg,(char*)"",FALSE);
      }

      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:RemoteRegistryNetConnexion:RegConnectRegistry=OK",ip,FALSE);
      #endif

      //get Exact OS !!!
      if (RegistryOS(iitem,hkey))
      {
        if (!windows_OS)nb_windows++;
      }

      //get user list + curent user
      if (config->disco_users && scan_start && users_check)
      {
        Registry_List_users(iitem, ip, hkey);
      }

      //work
      if (config->check_registry && scan_start)
      {
        RegistryScan(iitem,ip,hkey,(char*)"HKLM", FALSE);

        //other check
        HKEY hkey2;
        /*if (RegConnectRegistry(tmp,HKEY_CLASSES_ROOT,&hkey2)==ERROR_SUCCESS)
        {
          RegistryScan(iitem,ip,hkey2,(char*)"HKCR", FALSE);
          if (config->write_key && scan_start)RegistryWriteKey(ip,hkey2,(char*)"HKCR");
          RegCloseKey(hkey2);
        }*/
        if (RegConnectRegistry(tmp,HKEY_USERS,&hkey2)==ERROR_SUCCESS)
        {
          RegistryScan(iitem,ip,hkey2,(char*)"HKU", TRUE);
          if (config->write_key && scan_start)RegistryWriteKey(ip,hkey2,(char*)"HKU");
          RegCloseKey(hkey2);
        }
      }

      if (config->check_services && scan_start)
      {
        if (RegistryServiceScan(iitem, ip,(char*)"SYSTEM\\CurrentControlSet\\Services\\",hkey) == -1)
          if(Netbios_List_service(iitem, ip, TRUE) < 1)
            CheckServiceOpenSC(iitem, ip);
      }
      if (config->check_software && scan_start)
      {
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
        RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      }
      //if (config->check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);
      if (config->check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\",hkey);

      if (config->write_key && scan_start)RegistryWriteKey(ip,hkey,(char*)"HKLM");

      RegCloseKey(hkey);
      ret = TRUE;
    }else if (config->check_services && scan_start)
    {
      if(Netbios_List_service(iitem, ip, TRUE) < 1)
        CheckServiceOpenSC(iitem, ip);
    }

    if (start_remote_registry)
    {
      StartRemoteRegistryService(ip, FALSE);
    }
  }

  if(connect)
  {
    if (config->local_account)
    {
    }else
    {
      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
    }
    if (connect != (HANDLE)1)CloseHandle(connect);
  }

  return ret;
}
//----------------------------------------------------------------
BOOL LocalRegistryNetConnexion(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, long int *id_ok)
{
  //for testing account policy
  if (config->disco_netbios_policy && scan_start)
  {
    char pol[MAX_PATH]="";
    char tmp[MAX_PATH]="";
    wchar_t server[MAX_PATH];
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);

    //WaitForSingleObject(hs_netbios,INFINITE);
    mbstowcs(server, tmp, MAX_PATH);

    if (Netbios_Policy(server, pol, MAX_PATH))
    {
      AddLSTVUpdateItem(pol, COL_POLICY, iitem);
    }
    //ReleaseSemaphore(hs_netbios,1,NULL);
  }

  if (config->disco_netbios_users && scan_start)
  {
    Netbios_List_users(iitem, ip);
  }

  HKEY hkey;
  if (RegOpenKey(HKEY_LOCAL_MACHINE,"",&hkey)==ERROR_SUCCESS)
  {
    //get Exact OS !!!
    if (RegistryOS(iitem,hkey))
    {
      nb_windows++;
    }

    if (config->disco_users && scan_start)
    {
      Registry_List_users(iitem, ip, hkey);
    }

    if (config->check_registry && scan_start)
    {
      RegistryScan(iitem,ip,hkey,(char*)"HKLM", FALSE);

      //other check
      HKEY hkey2;
      if (RegOpenKey(HKEY_CLASSES_ROOT,"",&hkey2)==ERROR_SUCCESS)
      {
        RegistryScan(iitem,ip,hkey2,(char*)"HKCR", FALSE);
        if (config->write_key && scan_start)RegistryWriteKey(ip,hkey2,(char*)"HKCR");
        RegCloseKey(hkey2);
      }
      if (RegOpenKey(HKEY_CURRENT_USER,"",&hkey2)==ERROR_SUCCESS)
      {
        RegistryScan(iitem,ip,hkey2,(char*)"HKCU", FALSE);
        if (config->write_key && scan_start)RegistryWriteKey(ip,hkey2,(char*)"HKU");
        RegCloseKey(hkey2);
      }
      if (RegOpenKey(HKEY_USERS,"",&hkey2)==ERROR_SUCCESS)
      {
        RegistryScan(iitem,ip,hkey2,(char*)"HKU", TRUE);
        if (config->write_key && scan_start)RegistryWriteKey(ip,hkey2,(char*)"HKU");
        RegCloseKey(hkey2);
      }
    }

    if (config->check_services && scan_start)
    {
      if (RegistryServiceScan(iitem, ip,(char*)"SYSTEM\\CurrentControlSet\\Services\\",hkey) == -1)
        if(Netbios_List_service(iitem, ip, TRUE) < 1)
          CheckServiceOpenSC(iitem, ip);
    }

    if (config->check_software && scan_start)
    {
      RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      RegistrySoftwareScan(iitem,ip,(char*)"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
    }
    //if (config->check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);
    if (config->check_USB && scan_start)RegistryUSBScan(iitem,ip,(char*)"SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\",hkey);

    if (config->write_key && scan_start)RegistryWriteKey(ip,hkey,(char*)"HKLM");

    RegCloseKey(hkey);
  }else if (config->check_services && scan_start)
  {
    if(Netbios_List_service(iitem, ip, TRUE) < 1)
      CheckServiceOpenSC(iitem, ip);
  }
  return TRUE;
}
//----------------------------------------------------------------
BOOL RemoteConnexionScan(DWORD iitem, char *ip, DWORD ip_id, PSCANNE_ST config, BOOL windows_OS, long int *id_ok, BOOL users_check, BOOL os_check)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","registry:RemoteConnexionScan",ip,FALSE);
  #endif
  if (ipIsLoclahost(ip))
  {
    if (LocalRegistryNetConnexion(iitem, ip, ip_id, config, id_ok))return TRUE;
    else AddLSTVUpdateItem((char*)"LOCAL REG SCAN FAIL!",COL_FILES,iitem);

  }else if(RemoteRegistryNetConnexion(iitem, ip, ip_id, config, windows_OS, id_ok, users_check, os_check))return TRUE;
  else
  {
    #ifndef DEBUG_NOERROR
    if (config->check_registry)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_REG,iitem);
    if (config->check_services)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_SERVICE,iitem);
    if (config->check_software)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_SOFTWARE,iitem);
    if (config->check_USB)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"CONNEXION FAIL!",COL_USB,iitem);
    #endif
  }
  return FALSE;
}
