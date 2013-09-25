//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//------------------------------------------------------------------------------
DWORD ReadValue(HKEY hk,char *path,char *value,void *data, DWORD data_size)
{
  DWORD data_size_read = 0;
  HKEY CleTmp=0;

  //open key
  if (RegOpenKey(hk,path,&CleTmp)!=ERROR_SUCCESS)
     return FALSE;

  //size of data
  if (RegQueryValueEx(CleTmp, value, 0, 0, 0, &data_size_read)!=ERROR_SUCCESS)
     return FALSE;

  //alloc
  char *c = (char *)malloc(data_size_read+1);
  if (c == NULL)return FALSE;

  //read value
  if (RegQueryValueEx(CleTmp, value, 0, 0, (LPBYTE)c, &data_size_read)!=ERROR_SUCCESS)
    return FALSE;

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
void RegistryScan(DWORD iitem,char *ip, HKEY hkey)
{
  //get datas to check
  char buffer[LINE_SIZE];
  DWORD i, nb_i = SendDlgItemMessage(h_main,CB_T_REGISTRY,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  char msg[LINE_SIZE];
  DWORD key_data_dw;
  HKEY CleTmp;
  REG_LINE_ST reg_st;
  DWORD dw_datas, ok;
  char ch_datas[LINE_SIZE];
  unsigned int key_data_type, key_src_data_type;

  for (i=0;i<nb_i && scan_start;i++)
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
            snprintf(msg,LINE_SIZE,"%s\\%s",ip,reg_st.path);
            AddMsg(h_main,"FOUND (Registry)",msg,"Registry Path Exist");
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
              if (reg_st.data_dword)snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
              else snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);

              AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Exist");
              AddLSTVUpdateItem(msg, COL_REG, iitem);
            }else if (reg_st.check_equal)
            {
              if (reg_st.data_dword)
              {
                if (atol(reg_st.data) == dw_datas)
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                  AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }else if (reg_st.data_string)
              {
                if (!strcmp(reg_st.data,ch_datas))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);
                  AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }
            }else if (reg_st.check_diff)
            {
              if (reg_st.data_dword)
              {
                if (atol(reg_st.data) != dw_datas)
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                  AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }else if (reg_st.data_string)
              {
                if (strcmp(reg_st.data,ch_datas))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);
                  AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Data Exist");
                  AddLSTVUpdateItem(msg, COL_REG, iitem);
                }
              }
            }else if (reg_st.check_inf)
            {
              if (atol(reg_st.data) > dw_datas)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Data Exist");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }
            }else if (reg_st.check_sup)
            {
              if (atol(reg_st.data) < dw_datas)
              {
                snprintf(msg,LINE_SIZE,"%s\\%s%s=%lu",ip,reg_st.path,reg_st.value,dw_datas);
                AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Data Exist");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
              }
            }else if (reg_st.check_content)
            {
              if (Contient(ch_datas,reg_st.data))
              {
                snprintf(msg,LINE_SIZE,"%s\\%s%s=%s",ip,reg_st.path,reg_st.value,ch_datas);
                AddMsg(h_main,"FOUND (Registry)",msg,"Registry Value Data Exist");
                AddLSTVUpdateItem(msg, COL_REG, iitem);
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
BOOL LSBExist(DWORD lsb, char *st)
{
  char buffer[LINE_SIZE];
  if (st == NULL) return FALSE;
  if (st[0] == 0) return FALSE;
  DWORD i, nb_i = SendDlgItemMessage(h_main,lsb,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  for (i=0;i<nb_i;i++)
  {
    if (SendDlgItemMessage(h_main,lsb,LB_GETTEXT,(WPARAM)i,(LPARAM)buffer))
    {
      if (!strcmp(buffer,st))return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------
void RegistryServiceScan(DWORD iitem,char *ip, char *path, HKEY hkey)
{
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"Services");
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
          if (ReadValue(hkey,key_path,"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              if (ReadValue(hkey,key_path,"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                AddMsg(h_main,"FOUND (Service)",key_path,key);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }else if (LSBExist(CB_T_SERVICES, name))
            {
              if (ReadValue(hkey,key_path,"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Service)",msg,name);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                AddMsg(h_main,"FOUND (Service)",key_path,name);
                AddLSTVUpdateItem(key_path, COL_SERVICE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SERVICES, key))
            {
              if (ReadValue(hkey,key_path,"ImagePath",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sImagePath=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Service)",msg,key);
                AddLSTVUpdateItem(msg, COL_SERVICE, iitem);
              }else
              {
                AddMsg(h_main,"FOUND (Service)",key_path,key);
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
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"Softwares");
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
          AddMsg(h_main,"DEBUG","registry:RegistrySoftwareScan:RegEnumKeyEx2=OK",ip);
          #endif
          name[0]   = 0;
          snprintf(key_path,LINE_SIZE,"%s%s\\",path,key);
          if (ReadValue(hkey,key_path,"DisplayName",name, LINE_SIZE) != 0)
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              if (ReadValue(hkey,key_path,"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sinstalllocation=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sInstallSource=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sUninstallString=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                AddMsg(h_main,"FOUND (Software)",key_path,key);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }else if (LSBExist(CB_T_SOFTWARE, name))
            {
              if (ReadValue(hkey,key_path,"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sinstalllocation=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,"InstallSource",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sInstallSource=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else if (ReadValue(hkey,key_path,"UninstallString",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sUninstallString=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Software)",msg,name);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                AddMsg(h_main,"FOUND (Software)",key_path,name);
                AddLSTVUpdateItem(key_path, COL_SOFTWARE, iitem);
              }
            }
          }else
          {
            if (LSBExist(CB_T_SOFTWARE, key))
            {
              if (ReadValue(hkey,key_path,"installlocation",ImagePath, LINE_SIZE) != 0)
              {
                snprintf(msg,LINE_SIZE,"%s\\%sinstalllocation=%s",ip,key_path,ImagePath);
                AddMsg(h_main,"FOUND (Software)",msg,key);
                AddLSTVUpdateItem(msg, COL_SOFTWARE, iitem);
              }else
              {
                AddMsg(h_main,"FOUND (Software)",key_path,key);
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
  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"USB");
  HKEY CleTmp,CleTmp2;
  if (RegOpenKey(hkey,path,&CleTmp)==ERROR_SUCCESS)
  {
    DWORD i,j,nbSubKey = 0, nbSubKey2;
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      char key[LINE_SIZE],key2[LINE_SIZE],key_path[LINE_SIZE],key_path2[LINE_SIZE],msg[LINE_SIZE];
      DWORD key_size, key_size2;

      for (i=0;i<nbSubKey && scan_start;i++)
      {
        //init datas to read
        key_size  = LINE_SIZE;
        key[0]    = 0;

        if (RegEnumKeyEx (CleTmp,i,key,&key_size,0,0,0,0)==ERROR_SUCCESS)
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
              if (RegEnumKeyEx (CleTmp2,j,key2,&key_size2,0,0,0,0)==ERROR_SUCCESS)
              {
                snprintf(key_path2,LINE_SIZE,"%s%s",key_path,key2);
                if (LSBExist(CB_T_USB, key))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s",ip,key_path,key);
                  AddMsg(h_main,"FOUND (USB)",msg,key);
                  AddLSTVUpdateItem(msg, COL_USB, iitem);
                }else if (LSBExist(CB_T_USB, key2))
                {
                  snprintf(msg,LINE_SIZE,"%s\\%s%s",ip,key_path,key2);
                  AddMsg(h_main,"FOUND (USB)",msg,key2);
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
BOOL NetConnexionAuthenticate(char *ip, char*remote_name, SCANNE_ST config)
{
  char user_netbios[LINE_SIZE] = "";
  if (config.domain[0] != 0)snprintf(user_netbios,LINE_SIZE,"%s\\%s",config.domain,config.login);
  else snprintf(user_netbios,LINE_SIZE,"%s",config.login);

  char tmp_connect[LINE_SIZE];
  remote_name[0] = 0;
  snprintf(tmp_connect,LINE_SIZE,"\\\\%s\\ipc$",ip);

  NETRESOURCE NetRes;
  NetRes.dwScope      = RESOURCE_GLOBALNET;
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpLocalName  = "";
  NetRes.lpProvider   = "";
  NetRes.lpRemoteName	= remote_name;
  if (WNetAddConnection2(&NetRes,config.mdp,user_netbios,CONNECT_PROMPT)==NO_ERROR)
  {
    char msg[LINE_SIZE];
    snprintf(msg,LINE_SIZE,"Login in %s IP with %s account.",ip,user_netbios);
    AddMsg(h_main,"INFORMATION",msg,"");

    return TRUE;//CONNECT_UPDATE_PROFILE
  }else
  {
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","registry:NetConnexionAuthenticate=FAIL",ip);
    #endif
    return FALSE;
  }

  //for close : WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
  return FALSE;
}
//----------------------------------------------------------------
HANDLE UserConnect(char *ip,SCANNE_ST config)
{
  HANDLE htoken = NULL;
  if (LogonUser(config.login, config.domain, config.mdp, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &htoken))
  {
    ImpersonateLoggedOnUser(htoken);
    #ifdef DEBUG_MODE
    if (htoken == 0)AddMsg(h_main,"DEBUG","registry:Authent:UserConnect=FAIL",ip);
    #endif
    return htoken;
  }else return FALSE;
}
//----------------------------------------------------------------
void UserDisConnect(HANDLE htoken)
{
  CloseHandle(htoken);
}
//----------------------------------------------------------------
BOOL RemoteRegistryNetConnexion(DWORD iitem,char *name, char *ip, SCANNE_ST config)
{
  BOOL ret            = FALSE;
  char tmp[MAX_PATH]  = "", remote_name[MAX_PATH]  = "", msg[LINE_SIZE];
  BOOL connect = NetConnexionAuthenticate(ip, remote_name,config);

  //if(NetConnexionAuthenticate(ip, remote_name))
  //{
    //net
    HKEY hkey;
    snprintf(tmp,MAX_PATH,"\\\\%s",name);
    if (RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey)==ERROR_SUCCESS)
    {
      if (connect)
      {
        snprintf(msg,LINE_SIZE,"Login (NET) in %s IP with %s account.",ip,config.login);
        AddMsg(h_main,"INFORMATION",msg,"");
      }else
      {
        snprintf(msg,LINE_SIZE,"Login (NET) in %s IP with NULL session account.",ip);
        AddMsg(h_main,"INFORMATION",msg,"");
      }

      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:RemoteRegistryNetConnexion:RegConnectRegistry=OK",ip);
      #endif
      //work
      if (config.check_registry && scan_start)RegistryScan(iitem,ip,hkey);
      if (config.check_services && scan_start)RegistryServiceScan(iitem,ip,"SYSTEM\\CurrentControlSet\\Services\\",hkey);
      if (config.check_software && scan_start)
      {
        RegistrySoftwareScan(iitem,ip,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
        RegistrySoftwareScan(iitem,ip,"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      }
      if (config.check_USB && scan_start)RegistryUSBScan(iitem,ip,"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

      RegCloseKey(hkey);
      ret = TRUE;
    }
    if(connect)WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
  //}
  return ret;
}
//----------------------------------------------------------------
BOOL RemoteRegistryImpersonateConnexion(DWORD iitem, char *name, char *ip, SCANNE_ST config)
{
  BOOL ret            = FALSE;
  HANDLE htoken = UserConnect(ip,config);
  if (htoken != NULL)
  {
    char msg[LINE_SIZE];
    snprintf(msg,LINE_SIZE,"Login (LogonUser) in %s IP with %s\\%s account.",ip,config.domain,config.login);
    AddMsg(h_main,"INFORMATION",msg,"");

    HKEY hkey;
    char tmp[MAX_PATH] = "";
    snprintf(tmp,MAX_PATH,"\\\\%s",name);
    if (RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey)==ERROR_SUCCESS)
    {
      #ifdef DEBUG_MODE
      AddMsg(h_main,"DEBUG","registry:RemoteRegistryImpersonateConnexion:RegConnectRegistry=OK",ip);
      #endif
      //work
      if (config.check_registry && scan_start)RegistryScan(iitem,ip,hkey);
      if (config.check_services && scan_start)RegistryServiceScan(iitem,ip,"SYSTEM\\CurrentControlSet\\Services\\",hkey);
      if (config.check_software && scan_start)
      {
        RegistrySoftwareScan(iitem,ip,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
        RegistrySoftwareScan(iitem,ip,"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      }
      if (config.check_USB && scan_start)RegistryUSBScan(iitem,ip,"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

      RegCloseKey(hkey);
      ret = TRUE;
    }
    UserDisConnect(htoken);
  }
  return ret;
}
//----------------------------------------------------------------
BOOL RemoteRegistrySimpleConnexion(DWORD iitem, char *name, char *ip, SCANNE_ST config)
{
  BOOL ret            = FALSE;
  HKEY hkey;
  char tmp[MAX_PATH]  = "";
  snprintf(tmp,MAX_PATH,"\\\\%s",name);

  if (RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey)==ERROR_SUCCESS)
  {
    char msg[LINE_SIZE];
    snprintf(msg,LINE_SIZE,"Login (direct User) in %s IP with NULL session account.",ip);
    AddMsg(h_main,"INFORMATION",msg,"");

    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","registry:RemoteRegistrySimpleConnexion:RegConnectRegistry=OK",ip);
    #endif
    //work
    if (config.check_registry && scan_start)RegistryScan(iitem,ip,hkey);
    if (config.check_services && scan_start)RegistryServiceScan(iitem,ip,"SYSTEM\\CurrentControlSet\\Services\\",hkey);
    if (config.check_software && scan_start)
    {
      RegistrySoftwareScan(iitem,ip,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
      RegistrySoftwareScan(iitem,ip,"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\",hkey);
    }
    if (config.check_USB && scan_start)RegistryUSBScan(iitem,ip,"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\",hkey);

    RegCloseKey(hkey);
    ret = TRUE;
  }
  return ret;
}
//----------------------------------------------------------------
BOOL RemoteConnexionScan(DWORD iitem, char *name, char *ip, SCANNE_ST config)
{
  #ifdef DEBUG_MODE
  AddMsg(h_main,"DEBUG","registry:RemoteConnexionScan",ip);
  #endif
  if (RemoteRegistryNetConnexion(iitem, name, ip, config)) return TRUE;
  else if (RemoteRegistryImpersonateConnexion(iitem, name, ip, config)) return TRUE;
  else
  {
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_REG,"CONNEXION FAIL!");
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_SERVICE,"CONNEXION FAIL!");
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_SOFTWARE,"CONNEXION FAIL!");
    ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_USB,"CONNEXION FAIL!");
  }
  //else if (RemoteRegistrySimpleConnexion(iitem, name, ip, config)) return TRUE;
  return FALSE;
}
