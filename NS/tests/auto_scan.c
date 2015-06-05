//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//------------------------------------------------------------------------------
char* GetLocalPath(char *path, unsigned int sizeMax)
{
  char *c = path+GetModuleFileName(0, path,sizeMax);
  while(*c != '\\') c--;
  *c = 0;

  /*
  char tmp[MAX_PATH]="";
  snprintf(tmp,MAX_PATH,"%s",GetCommandLine()+1);
  c = tmp+strlen(tmp)-2; //bypass "
  while(*c != '\\') c--;
  *c = 0;
  AddMsg(h_main, (char*)"DEBUG (actual->future)",path,tmp,FALSE);
  */

  return path;
}
//----------------------------------------------------------------
void RemoteConnexionRegistryScan_auto_scan(DWORD iitem, char*ip, DWORD ip_id, BOOL windows_OS, long int *id_ok)
{
  char remote_name[MAX_PATH]="", tmp[MAX_PATH]="";
  HANDLE connect = 0;

  WaitForSingleObject(hs_netbios,INFINITE);
  EnterCriticalSection(&Sync_threads_netbios);
  hs_c_netbios++;
  LeaveCriticalSection(&Sync_threads_netbios);

  if (scan_start)connect = NetConnexionAuthenticateTest(ip, ip_id, remote_name,&config, iitem, FALSE, id_ok);

  if (scan_start)
  {
    HKEY hkey;
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);

    BOOL start_remote_registry = FALSE;
    LONG reg_access = RegConnectRegistry(tmp,HKEY_LOCAL_MACHINE,&hkey);

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

    if (reg_access == ERROR_SUCCESS && scan_start)
    {
      DWORD DWORD_tmp=0;
      char value[MAX_PATH], tmp_value[MAX_PATH]="",  msg[MAX_PATH];

      //get Exact OS !!!
      if (RegistryOS(iitem,hkey))
      {
        if (!windows_OS)nb_windows++;
      }

      if (auto_scan_config.PASSWORD_POLICY)
      {
        DWORD_tmp=0;
        if(ReadValue(hkey,"System\\CurrentControlSet\\Services\\Netlogon\\Parameters\\","RequireStrongKey",&DWORD_tmp, sizeof(DWORD)) != 0)
        {
          snprintf(value,MAX_PATH,"%s\\HKLM\\System\\CurrentControlSet\\Services\\Netlogon\\Parameters\\RequireStrongKey=%lu",ip,DWORD_tmp);

          if (DWORD_tmp == 1)
          {
            AddLSTVUpdateItem("Password complexity:OK (Enable)", COL_CONFIG, iitem);
            AddMsg(h_main, (char*)"FOUND (Account policy)",(char*)"Password complexity:OK (Enable)",value,FALSE);
          }else
          {
            AddLSTVUpdateItem("Password complexity:NOK (Disable)", COL_CONFIG, iitem);
            AddMsg(h_main, (char*)"FOUND (Account policy)",(char*)"Password complexity:NOK (Disable)",value,FALSE);
          }
        }
      }

      if (auto_scan_config.AUTORUN)
      {
        DWORD_tmp=0;
        if(ReadValue(hkey,"Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer\\","NoDriveTypeAutorun",&DWORD_tmp, sizeof(DWORD)) != 0)
        {
          snprintf(value,MAX_PATH,"%s\\HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer\\NoDriveTypeAutorun=%lu",ip,DWORD_tmp);
          if (DWORD_tmp == 0xFF)
          {
            AddLSTVUpdateItem("AUTORUN:OK (Disable)", COL_CONFIG, iitem);
            AddMsg(h_main, (char*)"FOUND (Registry)",(char*)"AUTORUN:OK (Disable)",value,FALSE);
          }else
          {
            AddLSTVUpdateItem("AUTORUN:NOK (Enable)", COL_CONFIG, iitem);
            AddMsg(h_main, (char*)"FOUND (Registry)",(char*)"AUTORUN:NOK (Enable)",value,FALSE);
          }
        }
      }

      if (auto_scan_config.M_SEC)
      {
        tmp_value[0]=0;
        if(ReadValue(hkey,auto_scan_config.MSEC_REG_PATH,auto_scan_config.MSEC_REG_VALUE,tmp_value, MAX_PATH) != 0)
        {
          snprintf(value,MAX_PATH,"%s\\HKLM\\%s%s=%s",ip,auto_scan_config.MSEC_REG_PATH,auto_scan_config.MSEC_REG_VALUE,tmp_value);

          snprintf(tmp,MAX_PATH,"MSEC:OK (installed:%s)",tmp_value);
          AddLSTVUpdateItem(tmp, COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Registry)",(char*)tmp,value,FALSE);
        }
      }

      //current time
      time_t current_time;
      time(&current_time);
      struct tm *today = localtime(&current_time);

      if (auto_scan_config.MCAFEE_INSTALLED)
      {
        tmp_value[0]=0;
        if(ReadValue(hkey,"SOFTWARE\\Mcafee\\AVEngine\\","AvDatDate",tmp_value, MAX_PATH) != 0)
        {
          snprintf(value,MAX_PATH,"%s\\HKLM\\SOFTWARE\\Mcafee\\AVEngine\\AvDatDate=%s",ip,tmp_value);
          AddLSTVUpdateItem("McAfee:OK (installed)", COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Registry)",(char*)"McAfee:OK (installed)",value,FALSE);

          //check version
          tmp[0] = tmp_value[0];
          tmp[1] = tmp_value[1];
          tmp[2] = tmp_value[2];
          tmp[3] = tmp_value[3];
          tmp[4] = 0;
          unsigned int year = atoi(tmp);

          tmp[0] = tmp_value[5];
          tmp[1] = tmp_value[6];
          tmp[2] = 0;
          unsigned int month = atoi(tmp);

          tmp[0] = tmp_value[8];
          tmp[1] = tmp_value[9];
          tmp[2] = 0;
          unsigned int day = atoi(tmp);
          if (today->tm_mday - day >= 0 && (today->tm_mon+1) == month && today->tm_year+1900 == year)
          {
            if (today->tm_mday - day <= auto_scan_config.MCAFEE_UPDATE_DAYS_INTERVAL)
            {
              snprintf(msg,MAX_PATH,"McAfee:OK (last update:%s)",tmp_value);

            }else
            {
              snprintf(msg,MAX_PATH,"McAfee:NOK (last update:%s)",tmp_value);
            }
          }else if (((today->tm_mon+1) - month == 1) && (27-day + today->tm_mday)<= auto_scan_config.MCAFEE_UPDATE_DAYS_INTERVAL)
          {
            //exact calcul
            unsigned int ref = 31;
            switch(today->tm_mon+1)
            {
              case 2:ref=27;break;

              case 4:
              case 6:
              case 9:
              case 11:ref=30;break;
            }

            if ((ref-day + today->tm_mday)<= auto_scan_config.MCAFEE_UPDATE_DAYS_INTERVAL)
            {
              snprintf(msg,MAX_PATH,"McAfee:OK (last update:%s)",tmp_value);

            }else
            {
              snprintf(msg,MAX_PATH,"McAfee:NOK (last update:%s)",tmp_value);
            }
          }else if (((today->tm_year+1900) - year == 1) && (today->tm_mon == 0 && month == 12))
          {
            if ((31-day + today->tm_mday) <= auto_scan_config.MCAFEE_UPDATE_DAYS_INTERVAL)
            {
              snprintf(msg,MAX_PATH,"McAfee:OK (last update:%s)",tmp_value);

            }else
            {
              snprintf(msg,MAX_PATH,"McAfee:NOK (last update:%s)",tmp_value);
            }
          }else
          {
            snprintf(msg,MAX_PATH,"McAfee:NOK (last update:%s)",tmp_value);
          }
          AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Registry)",ip,(char*)msg,FALSE);
        }
      }

      if (auto_scan_config.WSUS_WORKS)
      {
        tmp_value[0]=0;
        if(ReadValue(hkey,"SOFTWARE\\Policies\\Microsoft\\Windows\\WindowsUpdate\\","WUServer",tmp_value, MAX_PATH) != 0)
        {
          snprintf(msg,MAX_PATH,"WSUS:OK (server:%s)",tmp_value);

          //check if work
          DWORD_tmp=0;
          if(ReadValue(hkey,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\Detect\\","LastError",&DWORD_tmp, sizeof(DWORD)) != 0)
          {
            if (DWORD_tmp != 0)
            {
              snprintf(msg,MAX_PATH,"WSUS:NOK (error server:%s)",tmp_value);
            }
          }else snprintf(msg,MAX_PATH,"WSUS:NOK (not used, server:%s)",tmp_value);
          AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Registry)",ip,(char*)msg,FALSE);

          //patch update
          tmp_value[0]=0;
          if(ReadValue(hkey,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\Install\\","LastSuccessTime",tmp_value, MAX_PATH) != 0)
          {
            tmp[0] = tmp_value[0];
            tmp[1] = tmp_value[1];
            tmp[2] = tmp_value[2];
            tmp[3] = tmp_value[3];
            tmp[4] = 0;
            unsigned int year = atoi(tmp);

            tmp[0] = tmp_value[5];
            tmp[1] = tmp_value[6];
            tmp[2] = 0;
            unsigned int month = atoi(tmp);

            tmp[0] = tmp_value[8];
            tmp[1] = tmp_value[9];
            tmp[2] = 0;
            unsigned int day = atoi(tmp);

            if ((today->tm_mday - day) >= 0 && (today->tm_mon+1) == month && (today->tm_year+1900) == year)
            {
              if (today->tm_mday - day <= 30)
              {
                snprintf(msg,MAX_PATH,"WSUS UPDATE:OK (last update:%s)",tmp_value);

              }else
              {
                snprintf(msg,MAX_PATH,"WSUS UPDATE:NOK (last update:%s)",tmp_value);
              }
            }else if (((today->tm_mon+1) - month == 1) && (27-day + today->tm_mday)<= 30)
            {
              //exact calcul
              unsigned int ref = 31;
              switch(today->tm_mon+1)
              {
                case 2:ref=27;break;

                case 4:
                case 6:
                case 9:
                case 11:ref=30;break;
              }

              if ((ref-day + today->tm_mday)<= 30)
              {
                snprintf(msg,MAX_PATH,"WSUS UPDATE:OK (last update:%s)",tmp_value);

              }else
              {
                snprintf(msg,MAX_PATH,"WSUS UPDATE:NOK (last update:%s)",tmp_value);
              }
            }else if (((today->tm_year+1900) - year == 1) && (today->tm_mon == 0 && month == 12))
            {
              if ((31-day + today->tm_mday) <= 30)
              {
                snprintf(msg,MAX_PATH,"WSUS UPDATE:OK (last update:%s)",tmp_value);

              }else
              {
                snprintf(msg,MAX_PATH,"WSUS UPDATE:NOK (last update:%s)",tmp_value);
              }
            }else
            {
              snprintf(msg,MAX_PATH,"WSUS UPDATE:NOK (last update:%s)",tmp_value);
            }
          }else snprintf(msg,MAX_PATH,"WSUS UPDATE:NOK (never updated)");
          AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Registry)",ip,(char*)msg,FALSE);
        }
      }
      RegCloseKey(hkey);
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

  ReleaseSemaphore(hs_netbios,1,NULL);
  EnterCriticalSection(&Sync_threads_netbios);
  hs_c_netbios--;
  LeaveCriticalSection(&Sync_threads_netbios);
}
//----------------------------------------------------------------
void RemoteConnexionScan_auto_scan(DWORD iitem, char*ip, DWORD ip_id, long int *id_ok)
{
  char remote_name[MAX_PATH], tmp[MAX_PATH];
  HANDLE connect = 0;
  if (scan_start)  connect = NetConnexionAuthenticateTest(ip, ip_id, remote_name,&config, iitem, TRUE, id_ok);

  //account policy
  if (auto_scan_config.PASSWORD_POLICY && scan_start)
  {
    wchar_t server[MAX_PATH];
    snprintf(tmp,MAX_PATH,"\\\\%s",ip);
    mbstowcs(server, tmp, MAX_PATH);

    char tmp_pUmI_0[MAX_PATH]="", tmp_pUmI_3[MAX_PATH]="";
    USER_MODALS_INFO_0 *pUmI_0 = NULL;
    USER_MODALS_INFO_3 *pUmI_3 = NULL;
    if (NERR_Success == NetUserModalsGet(server,0,(LPBYTE *)&pUmI_0))
    {
      if (pUmI_0 != NULL)
      {
        snprintf(tmp_pUmI_0,MAX_PATH,
                                     "min_passwd_age:%s(%lu days)\r\n"
                                     "max_passwd_age:%s(%lu days)\r\n"
                                     "min_passwd_len:%s(%lu)\r\n"
                                     "passord_history:%s(%lu)"
                                    ,(pUmI_0->usrmod0_min_passwd_age/86400)>=auto_scan_config.PASSWORD_POLICY_MIN_AGE?"OK":"NOK",pUmI_0->usrmod0_min_passwd_age/86400
                                    ,(pUmI_0->usrmod0_max_passwd_age/86400)<=auto_scan_config.PASSWORD_POLICY_MAX_AGE?"OK":"NOK",pUmI_0->usrmod0_max_passwd_age/86400
                                    ,pUmI_0->usrmod0_min_passwd_len>=auto_scan_config.PASSWORD_POLICY_MIN_LEN?"OK":"NOK",pUmI_0->usrmod0_min_passwd_len
                                    ,pUmI_0->usrmod0_password_hist_len>=auto_scan_config.PASSWORD_POLICY_HISTORY?"OK":"NOK",pUmI_0->usrmod0_password_hist_len);
        NetApiBufferFree(pUmI_0);
        AddLSTVUpdateItem(tmp_pUmI_0, COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,tmp_pUmI_0,FALSE);
      }
    }
    if (NERR_Success == NetUserModalsGet(server,3,(LPBYTE *)&pUmI_3))
    {
      if (pUmI_3 != NULL)
      {
        snprintf(tmp_pUmI_3,MAX_PATH,"bad_password_count_to_lock_account:%s(%lu)"
                                    ,pUmI_3->usrmod3_lockout_threshold<=auto_scan_config.PASSWORD_POLICY_LOCKOUT_COUNT?"OK":"NOK",pUmI_3->usrmod3_lockout_threshold);
        NetApiBufferFree(pUmI_3);
        AddLSTVUpdateItem(tmp_pUmI_3, COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,tmp_pUmI_3,FALSE);
      }
    }
  }

  if (auto_scan_config.ADMIN_ACCOUNT && scan_start)
  {
    if (*id_ok != ID_ERROR)
    {
      if (!strcmp(auto_scan_config.C_ADMIN_ACCOUNT, config.accounts[*id_ok].login))
      {
        AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Available)", COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Available)",FALSE);
      }else
      {
        if (TestReversSID(ip, auto_scan_config.C_ADMIN_ACCOUNT))
        {
          AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but bad state or bad password)", COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but bad state or bad password)",FALSE);
        }
      }
    }else
    {
      if (TestReversSID(ip, auto_scan_config.C_ADMIN_ACCOUNT))
      {
        AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but bad state or bad password)", COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but bad state or bad password)",FALSE);
      }
    }




    /*if (!Netbios_check_user(iitem, ip, auto_scan_config.C_ADMIN_ACCOUNT))
    {
      if (TestReversSID(ip, auto_scan_config.C_ADMIN_ACCOUNT))
      {
        AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Available untested)", COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Available untested)",FALSE);
      }else
      {
        if (connect != FALSE)
        {
          AddLSTVUpdateItem("ADMIN_ACCOUNT:NOK (Not available)", COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:NOK (Not available),FALSE");
        }
      }
    }*/
  }

  if(connect)
  {
    WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
    if (connect != (HANDLE)1)CloseHandle(connect);
  }
}
//----------------------------------------------------------------
void RemoteConnexionScanFiles_auto_scan(DWORD iitem, char*ip, long int *id_ok)
{
  unsigned int i = 0;
  char msg[MAX_PATH];
  char remote_name[LINE_SIZE],tmp_path[LINE_SIZE];
  char tmp_login[MAX_PATH];
  snprintf(remote_name,LINE_SIZE,"\\\\%s\\C$",ip);

  if (id_ok != NULL && *id_ok > ID_ERROR) i = *id_ok;

  WaitForSingleObject(hs_netbios,INFINITE);
  EnterCriticalSection(&Sync_threads);
  hs_c_netbios++;
  LeaveCriticalSection(&Sync_threads);

  //MCAFEE_UPDATE_DAYS_INTERVAL
  for (; i<config.nb_accounts && scan_start;i++)
  {
    NETRESOURCE NetRes  = {0};
    NetRes.dwScope      = RESOURCE_GLOBALNET;
    NetRes.dwType	      = RESOURCETYPE_ANY;
    NetRes.lpLocalName  = (LPSTR)"";
    NetRes.lpProvider   = (LPSTR)"";
    NetRes.lpRemoteName	= remote_name;

    if (config.accounts[i].domain[0] != 0)
    {
      snprintf(tmp_login,MAX_PATH,"%s\\%s",config.accounts[i].domain,config.accounts[i].login);
    }else
    {
      snprintf(tmp_login,MAX_PATH,"%s\\%s",ip,config.accounts[i].login);
    }

    if (WNetAddConnection2(&NetRes,config.accounts[i].password,tmp_login,CONNECT_PROMPT)==NO_ERROR)
    {
      snprintf(msg,LINE_SIZE,"%s with %s (%02d) account.",remote_name,config.accounts[i].login,i);
      if(!LOG_LOGIN_DISABLE)AddMsg(h_main,(char*)"LOGIN (Files:NET)",msg,(char*)"",FALSE);

      if (id_ok != NULL)*id_ok = i;

      HANDLE hfic;
      WIN32_FIND_DATA data;
      FILETIME LocalFileTime;
      SYSTEMTIME SysTimeModification;
      snprintf(tmp_path,LINE_SIZE,"%s\\ProgramData\\McAfee\\DesktopProtection\\OnDemandScanLog.txt",remote_name);

      if (GetFileAttributes(tmp_path) != INVALID_FILE_ATTRIBUTES)
      {
        hfic = FindFirstFile(tmp_path, &data);
        if (hfic != INVALID_HANDLE_VALUE)
        {
          //current time
          time_t current_time;
          time(&current_time);
          struct tm *today = localtime(&current_time);

          FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
          FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);

          if (today->tm_mday - SysTimeModification.wDay >= 0 && (today->tm_mon+1) == SysTimeModification.wMonth && today->tm_year+1900 == SysTimeModification.wYear)
          {
            if (today->tm_mday - SysTimeModification.wDay <= auto_scan_config.MCAFEE_SCAN_DAYS_INTERVAL)
            {
              snprintf(msg,MAX_PATH,"McAfee last scan:OK (last update:%02d/%02d/%02d)",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay);

            }else
            {
              snprintf(msg,MAX_PATH,"McAfee last scan:NOK (last update:%02d/%02d/%02d)",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay);
            }
          }else if (((today->tm_mon+1) - SysTimeModification.wMonth == 1) && (27-SysTimeModification.wDay + today->tm_mday)<= auto_scan_config.MCAFEE_SCAN_DAYS_INTERVAL)
          {
            //exact calcul
            unsigned int ref = 31;
            switch(today->tm_mon+1)
            {
              case 2:ref=27;break;

              case 4:
              case 6:
              case 9:
              case 11:ref=30;break;
            }

            if ((ref-SysTimeModification.wDay + today->tm_mday)<= auto_scan_config.MCAFEE_SCAN_DAYS_INTERVAL)
            {
              snprintf(msg,MAX_PATH,"McAfee last scan:OK (last update:%02d/%02d/%02d)",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay);

            }else
            {
              snprintf(msg,MAX_PATH,"McAfee last scan:NOK (last update:%02d/%02d/%02d)",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay);
            }
          }else if (((today->tm_year+1900) - SysTimeModification.wYear == 1) && (today->tm_mon == 0 && SysTimeModification.wMonth == 12))
          {
            if ((31-SysTimeModification.wDay + today->tm_mday) <= auto_scan_config.MCAFEE_SCAN_DAYS_INTERVAL)
            {
              snprintf(msg,MAX_PATH,"McAfee last scan:OK (last update:%02d/%02d/%02d)",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay);

            }else
            {
              snprintf(msg,MAX_PATH,"McAfee last scan:NOK (last update:%02d/%02d/%02d)",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay);
            }
          }else
          {
            snprintf(msg,MAX_PATH,"McAfee last scan:NOK (last update:%02d/%02d/%02d)",SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay);
          }
          AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (File)",ip,msg,FALSE);
          FindClose(hfic);
        }
      }

      WNetCancelConnection2(remote_name,CONNECT_UPDATE_PROFILE,1);
      break;
    }
  }

  ReleaseSemaphore(hs_netbios,1,NULL);
  EnterCriticalSection(&Sync_threads_netbios);
  hs_c_netbios--;
  LeaveCriticalSection(&Sync_threads_netbios);
}
//----------------------------------------------------------------
DWORD WINAPI ScanIp_auto_scan(LPVOID lParam)
{
  if (!scan_start)return 0;
  DWORD index = (DWORD)lParam;
  BOOL exist = FALSE;
  long int iitem = ID_ERROR;
  char ip[MAX_PATH]="", dsc[MAX_PATH], dns[MAX_PATH]="", ttl_s[MAX_PATH]="", os_s[MAX_PATH]="",cfg[MAX_LINE_SIZE]="",test_title[MAX_PATH],msg[MAX_PATH];
  if (SendDlgItemMessage(h_main, CB_IP, LB_GETTEXTLEN, (WPARAM)index,(LPARAM)NULL) > MAX_PATH)
  {

    ReleaseSemaphore(hs_threads,1,NULL);
    EnterCriticalSection(&Sync_threads);
    hs_c_threads--;
    LeaveCriticalSection(&Sync_threads);

    //tracking
    SetMainTitle(NULL, TRUE);
    return 0;
  }
  SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)index,(LPARAM)ip);

  dsc[0] = 0;
  if (SendDlgItemMessage(h_main, CB_DSC, LB_GETTEXTLEN, (WPARAM)index,(LPARAM)NULL) < MAX_PATH)
  {
    SendDlgItemMessage(h_main, CB_DSC, LB_GETTEXT, (WPARAM)index,(LPARAM)dsc);
  }

  if (ip[0]!=0 && scan_start)
  {
    WaitForSingleObject(hs_disco,INFINITE);
    EnterCriticalSection(&Sync_threads_disco);
    hs_c_disco++;
    LeaveCriticalSection(&Sync_threads_disco);

    if ((ip[0]> '9' || ip[0]< '0' || ((ip[1]> '9' || ip[1]< '0') && ip[1] != '.')) && scan_start)
    {
      //resolution inverse
      strncpy(dns,ip,MAX_PATH);

      struct in_addr **a;
      struct hostent *host;

      if ((host=gethostbyname(ip)))
      {
        a = (struct in_addr **)host->h_addr_list;
        snprintf(ip,16,"%s",inet_ntoa(**a));
        if(auto_scan_config.DNS_DISCOVERY)
        {
          exist = TRUE;
          iitem = AddLSTVItem(ip, dsc, dns, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
          if (!LOG_DNS_DISABLE)AddMsg(h_main, (char*)"DNS (IP->Name)",ip,dns,FALSE);
        }
      }else
      {
        iitem = AddLSTVItem((char*)"[ERROR DNS]", ip, dsc, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (char*)"OK");

        ReleaseSemaphore(hs_disco,1,NULL);
        EnterCriticalSection(&Sync_threads);
        hs_c_disco--;
        LeaveCriticalSection(&Sync_threads);

        ReleaseSemaphore(hs_threads,1,NULL);
        EnterCriticalSection(&Sync_threads);
        hs_c_threads--;
        LeaveCriticalSection(&Sync_threads);

        //tracking
        SetMainTitle(NULL,TRUE);
        return 0;
      }
    }

    if (iitem == ID_ERROR)exist = FALSE;
    //ICMP
    if (scan_start)
    {
      int ttl = Ping(ip);
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
    }

    if (iitem == ID_ERROR)exist = FALSE;

    //DNS
    if (scan_start && dns[0] == 0)
    {
      if(exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"DNS");
      if(ResDNS(ip, dns, MAX_PATH))
      {
        if (!exist)
        {
          if (!LOG_DNS_DISABLE)AddMsg(h_main, (char*)"DNS (IP->Name)",ip,dns,FALSE);
          if (auto_scan_config.DNS_DISCOVERY)
          {
            iitem = AddLSTVItem(ip, dsc, dns, NULL, (char*)"Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            exist = TRUE;
          }
        }else
        {
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_DNS,dns);
          if (auto_scan_config.DNS_DISCOVERY)exist = TRUE;
        }
      }
    }

    if (iitem == ID_ERROR)exist = FALSE;

    ReleaseSemaphore(hs_disco,1,NULL);
    EnterCriticalSection(&Sync_threads);
    hs_c_disco--;
    LeaveCriticalSection(&Sync_threads);

    BOOL windows_OS = FALSE;
    //tests !!!
    char tmp[MAX_LINE_SIZE] = "";
    long int id_ok = ID_ERROR;
    if (exist && scan_start)
    {
      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"NetBIOS");
      WaitForSingleObject(hs_netbios,INFINITE);
      EnterCriticalSection(&Sync_threads_netbios);
      hs_c_netbios++;
      LeaveCriticalSection(&Sync_threads_netbios);

      //get OS
      if (dns[0] == 0 && scan_start)
      {
        windows_OS = Netbios_OS(ip, os_s, dns, NULL, MAX_PATH);
        if (windows_OS)
        {
          if (dns[0] != 0) ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_DNS,dns);
          if (os_s[0] != 0)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,os_s);
          nb_windows++;
        }
      }else
      {
        windows_OS = Netbios_OS(ip, os_s, NULL, NULL, MAX_PATH);
        if (windows_OS)
        {
          if (os_s[0] != 0)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,os_s);
          nb_windows++;
        }
      }

      if (auto_scan_config.NULL_SESSION || auto_scan_config.REVERS_SID)
      {
        BOOL null_session = Netbios_NULLSessionStart(ip, "IPC$");
        if(null_session)
        {
          snprintf(cfg,MAX_LINE_SIZE,"NULL Session:Enable");
          AddMsg(h_main, (char*)"FOUND (Config)",ip,cfg,FALSE);
          AddLSTVUpdateItem(cfg, COL_CONFIG, iitem);

          if (auto_scan_config.REVERS_SID)
          {
            //cfg[0] = 0;
            //CheckReversSID(ip, cfg, MAX_LINE_SIZE);
            Netbios_List_users_reversSID(iitem, ip, TRUE, cfg, MAX_LINE_SIZE);

            if (cfg[0] != 0)
            {
              AddMsg(h_main, (char*)"FOUND (Config)",ip,cfg,FALSE);
              AddLSTVUpdateItem(cfg, COL_CONFIG, iitem);
            }
          }
        }

        //Share
        if (scan_start)
        {
          wchar_t server[MAX_PATH];
          snprintf(tmp,MAX_PATH,"\\\\%s",ip);
          mbstowcs(server, tmp, MAX_PATH);

          Netbios_Share(server, iitem, COL_CONFIG, ip, TRUE);
        }

        if(null_session)Netbios_NULLSessionStop(ip, "IPC$");
      }
      RemoteConnexionScan_auto_scan(iitem, ip, index, &id_ok);

      ReleaseSemaphore(hs_netbios,1,NULL);
      EnterCriticalSection(&Sync_threads_netbios);
      hs_c_netbios--;
      LeaveCriticalSection(&Sync_threads_netbios);
    }

    if (exist && scan_start)
    {
      if (TCP_port_open(iitem, ip, RPC_DEFAULT_PORT, FALSE))
      {
        //registry
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"Registry");
        WaitForSingleObject(hs_registry,INFINITE);
        EnterCriticalSection(&Sync_threads);
        hs_c_registry++;
        LeaveCriticalSection(&Sync_threads);

        RemoteConnexionRegistryScan_auto_scan(iitem, ip, index, windows_OS, &id_ok);
        ReleaseSemaphore(hs_registry,1,NULL);
        EnterCriticalSection(&Sync_threads);
        hs_c_registry--;
        LeaveCriticalSection(&Sync_threads);

        //add file test !!!
        if (scan_start && auto_scan_config.MCAFEE_SCAN)
        {
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"Files");
          WaitForSingleObject(hs_file,INFINITE);
          EnterCriticalSection(&Sync_threads);
          hs_c_file++;
          LeaveCriticalSection(&Sync_threads);

          RemoteConnexionScanFiles_auto_scan(iitem, ip, &id_ok);
          ReleaseSemaphore(hs_file,1,NULL);
          EnterCriticalSection(&Sync_threads);
          hs_c_file--;
          LeaveCriticalSection(&Sync_threads);
        }
      }else
      {
        #ifndef DEBUG_NOERROR
        if (config.check_files)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_FILES, iitem);
        if (config.check_registry)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_REG, iitem);
        if (config.check_services)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_SERVICE, iitem);
        if (config.check_software)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_SOFTWARE, iitem);
        if (config.check_USB)if(!LOG_ERROR_VIEW_DISABLE)AddLSTVUpdateItem((char*)"NOT TESTED! (port 445/TCP not open)", COL_USB, iitem);
        #endif
      }
    }

    if (exist && scan_start)
    {
      char tmp_os[MAX_MSG_SIZE]="";
      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)"SSH");

      if (TCP_port_open(iitem, ip, SSH_DEFAULT_PORT, FALSE))
      {
        WaitForSingleObject(hs_ssh,INFINITE);
        EnterCriticalSection(&Sync_threads_ssh);
        hs_c_ssh++;
        LeaveCriticalSection(&Sync_threads_ssh);

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
            }else  if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.login, config.password, -1,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE,FALSE) == SSH_ERROR_OK)
            {
              if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
              {
                ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
              }
            }

            if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.login, config.password);
          }
        }else if(config.global_ip_file)
        {
          snprintf(msg,MAX_PATH,"SSH ACCOUNT TEST:%s (%02d)",config.accounts[index].login, index);
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)(LPSTR)msg);

          int ret_ssh = ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[index].login, config.accounts[index].password, -1,"head -n 1 /etc/issue",tmp_os,MAX_MSG_SIZE,TRUE,TRUE);
          if (ret_ssh == SSH_ERROR_OK)
          {
            if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "head -n 1 /etc/issue"))
            {
              ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
            }else  if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[index].login, config.accounts[index].password, -1,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE,FALSE) == SSH_ERROR_OK)
            {
              if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
              {
                ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
              }
            }

            if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[index].login, config.accounts[index].password);
          }
        }else
        {
          DWORD j = 0;
          int ret_ssh = 0;
          BOOL first_msg = TRUE;
          BOOL msg_auth  = TRUE;
          char msg[MAX_LINE_SIZE];
          for (j=0;j<config.nb_accounts&& scan_start;j++)
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
                break;
              }else if (ssh_exec_cmd(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].password, j,"uname -a",tmp_os,MAX_MSG_SIZE,FALSE, FALSE) == SSH_ERROR_OK)
              {
                if (tmp_os[0] != 0 && LinuxStart_msgOK(tmp_os, "uname -a"))
                {
                  ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,tmp_os);
                  break;
                }
              }
              if (config.check_ssh)ssh_exec(iitem, ip, SSH_DEFAULT_PORT, config.accounts[j].login, config.accounts[j].password);
            }
            first_msg = FALSE;
          }
        }
        ReleaseSemaphore(hs_ssh,1,NULL);
        EnterCriticalSection(&Sync_threads_ssh);
        hs_c_ssh--;
        LeaveCriticalSection(&Sync_threads_ssh);
      }
    }

    if (exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"OK");
  }

  ReleaseSemaphore(hs_threads,1,NULL);
  EnterCriticalSection(&Sync_threads);
  hs_c_threads--;
  LeaveCriticalSection(&Sync_threads);

  if (exist)
  {
    //check if Computer OK
    ttl_s[0] = 0;
    ListView_GetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,ttl_s,MAX_PATH);
    if (ttl_s != 0)
    {
      if (!strcmp(ttl_s, "Firewall"))
      {
        //check all datas if no data proved : Firewall?
        ListView_GetItemText(GetDlgItem(h_main,LV_results),iitem,COL_TTL,ttl_s,MAX_PATH);
        if (ttl_s[0] == 0)
        {
          ListView_GetItemText(GetDlgItem(h_main,LV_results),iitem,COL_CONFIG,ttl_s,MAX_PATH);
          if (ttl_s[0] == 0)
          {
            ListView_GetItemText(GetDlgItem(h_main,LV_results),iitem,COL_SSH,ttl_s,MAX_PATH);
            if (ttl_s[0] == 0 || !strcmp(ttl_s,"NOT TESTED! (port 22/TCP not open)"))
            {
              ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_OS,(LPSTR)"FW?");
              nb_unknow++;
            }
          }
        }
      }
    }
  }

  //tracking
  SetMainTitle(NULL,TRUE);
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI auto_scan(LPVOID lParam)
{
  //check if NS.ini exist !!!
  char ini_path[LINE_SIZE]="";
  scan_start = TRUE;
  strncat(GetLocalPath(ini_path, LINE_SIZE),AUTO_SCAN_FILE_INI,LINE_SIZE);
  ListView_DeleteAllItems(GetDlgItem(h_main,LV_results));
  SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);
  SendDlgItemMessage(h_main,CB_DSC,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);
  SendDlgItemMessage(h_main,CB_infos,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

  char tmp_check[LINE_SIZE]="";

  //specials tests
  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","DNS_DISCOVERY","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.DNS_DISCOVERY = TRUE;
    else auto_scan_config.DNS_DISCOVERY = FALSE;
  }else auto_scan_config.DNS_DISCOVERY = TRUE;

  //disable extract of hash
  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","NO_HASH_CHECK","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')config.no_hash_check = TRUE;
  }else config.no_hash_check = FALSE;

  //no GUI
  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","NO_GUI","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.NO_GUI = TRUE;
  }else auto_scan_config.NO_GUI = FALSE;

  //LOG
  tmp_check[0] = 0;
  if(GetPrivateProfileString("LOG","LOG","ENABLE",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'D' || tmp_check[0] == 'd'||tmp_check[0] == 'N' || tmp_check[0] == 'n')LOG_DISABLE = TRUE;
  }
  tmp_check[0] = 0;
  if(GetPrivateProfileString("LOG","LOG_DNS","ENABLE",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'D' || tmp_check[0] == 'd'||tmp_check[0] == 'N' || tmp_check[0] == 'n')LOG_DNS_DISABLE = TRUE;
  }
  tmp_check[0] = 0;
  if(GetPrivateProfileString("LOG","LOG_LOGIN","ENABLE",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'D' || tmp_check[0] == 'd'||tmp_check[0] == 'N' || tmp_check[0] == 'n')LOG_LOGIN_DISABLE = TRUE;
  }
  tmp_check[0] = 0;
  if(GetPrivateProfileString("LOG","LOG_ERROR_VIEW","ENABLE",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'D' || tmp_check[0] == 'd'||tmp_check[0] == 'N' || tmp_check[0] == 'n')LOG_ERROR_VIEW_DISABLE = TRUE;
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","REG_REMOTE_SERVICE_STOP","YES",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'D' || tmp_check[0] == 'd'||tmp_check[0] == 'N' || tmp_check[0] == 'n')REG_REMOTE_SERVICE_STOP = FALSE;
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","EMP_MIN","0",tmp_check,LINE_SIZE,ini_path))
  {
    emp_MIN_SZ = atol(tmp_check);
  }
  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","EMP_MAX","0",tmp_check,LINE_SIZE,ini_path))
  {
    emp_MAX_SZ = atol(tmp_check);

    if (emp_MIN_SZ > emp_MAX_SZ)
    {
      DWORD t = emp_MAX_SZ;
      emp_MAX_SZ = emp_MIN_SZ;
      emp_MIN_SZ = t;
    }
  }

  //get all parameters
  //SAVE
  tmp_check[0] = 0;
  if(GetPrivateProfileString("SAVE","CSV","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
    {
      auto_scan_config.save_CSV = TRUE;
    }else auto_scan_config.save_CSV = FALSE;
  }else auto_scan_config.save_CSV = FALSE;
  tmp_check[0] = 0;

  tmp_check[0] = 0;
  if(GetPrivateProfileString("SAVE","XML","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
    {
      auto_scan_config.save_XML = TRUE;
    }
    else auto_scan_config.save_XML = FALSE;
  }else auto_scan_config.save_XML = FALSE;

  tmp_check[0] = 0;
  if(GetPrivateProfileString("SAVE","HTML","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
    {
      auto_scan_config.save_HTML = TRUE;
    }
    else auto_scan_config.save_HTML = FALSE;
  }else auto_scan_config.save_HTML = FALSE;

  tmp_check[0] = 0;
  if(GetPrivateProfileString("SAVE","AUTO_CLOSE_AFTER_SAVE","NO",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
    {
      auto_scan_config.auto_close_after_save = TRUE;
    }else auto_scan_config.auto_close_after_save = FALSE;
  }else auto_scan_config.auto_close_after_save = FALSE;


  tmp_check[0] = 0;
  if(GetPrivateProfileString("CHECK","REVERS_SID","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
    {
      auto_scan_config.REVERS_SID = TRUE;
      config.config_revers_SID    = TRUE;
    }else
    {
      auto_scan_config.REVERS_SID = FALSE;
      config.config_revers_SID    = FALSE;
    }
  }else
  {
    auto_scan_config.REVERS_SID = FALSE;
    config.config_revers_SID    = FALSE;
  }
  tmp_check[0] = 0;
  config.disco_netbios_users  = FALSE;
  if(GetPrivateProfileString("CHECK","DISCO_NETBIOS_USERS","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
    {
      config.disco_netbios_users  = TRUE;
    }
  }
  tmp_check[0] = 0;
  config.disco_users  = FALSE;
  if(GetPrivateProfileString("CHECK","DISCO_USERS","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
    {
      config.disco_users  = TRUE;
    }
  }

  //global threads vars
  tmp_check[0] = 0;
  if(GetPrivateProfileString("THREAD","NB_MAX_THREADS","400",tmp_check,LINE_SIZE,ini_path))
  {
    NB_MAX_THREAD = atol(tmp_check);
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("THREAD","NB_MAX_TH_DISCO","400",tmp_check,LINE_SIZE,ini_path))
  {
    NB_MAX_DISCO_THREADS = atol(tmp_check);
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("THREAD","NB_MAX_TH_TCP_SCAN","100",tmp_check,LINE_SIZE,ini_path))
  {
    NB_MAX_TCP_TEST_THREADS = atol(tmp_check);
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("THREAD","NB_MAX_TH_NETBIOS","10",tmp_check,LINE_SIZE,ini_path))
  {
    NB_MAX_NETBIOS_THREADS = atol(tmp_check);
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("THREAD","NB_MAX_TH_FILES","5",tmp_check,LINE_SIZE,ini_path))
  {
    NB_MAX_FILE_THREADS = atol(tmp_check);
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("THREAD","NB_MAX_TH_REGISTRY","5",tmp_check,LINE_SIZE,ini_path))
  {
    NB_MAX_REGISTRY_THREADS = atol(tmp_check);
  }

  /*tmp_check[0] = 0;
  if(GetPrivateProfileString("THREAD","NB_MAX_TH_SSH","1",tmp_check,LINE_SIZE,ini_path))
  {
    NB_MAX_SSH_THREADS = atol(tmp_check);
  }*/
  NB_MAX_SSH_THREADS      = 1;

  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","TYPE","",tmp_check,LINE_SIZE,ini_path))
  {
    if (tmp_check[0] == 'D' || tmp_check[0] == 'd')//DISABLE
    {
      EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
      scan_start = FALSE;
      h_thread_scan = 0;
      if (auto_scan_config.NO_GUI) auto_scan_config.NO_GUI = FALSE;

      return 0;
    }else if (tmp_check[0] == 'M' || tmp_check[0] == 'm') //MANUAL mode
    {
      if (auto_scan_config.NO_GUI) auto_scan_config.NO_GUI = FALSE;

      //check test if enable !!
      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_ICMP","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_ICMP);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_DNS","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_DNS);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_NETBIOS","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_NETBIOS);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_NETBIOS_POLICY","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_NETBIOS_POLICY);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_NETBIOS_USERS","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_NETBIOS_USERS);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_CHECK_FILES","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_FILES);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_CHECK_REGISTRY","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_REGISTRY);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_CHECK_SERVICES","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_SERVICES);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_CHECK_SOFTWARE","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_SOFTWARE);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_CHECK_USB","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_USB);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_CHECK_SSH","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_SSH);
      }

      tmp_check[0] = 0;
      if(GetPrivateProfileString("SCAN","DISCO_WRITE_KEY","",tmp_check,LINE_SIZE,ini_path))
      {
        if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_WRITE_KEY);
      }
      EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
      scan_start = FALSE;
      h_thread_scan = 0;
      return 0;
    }
  }else
  {
    EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
    scan_start = FALSE;
    h_thread_scan = 0;
    return 0;
  }

  tmp_check[0] = 0;
  if(GetPrivateProfileString("SCAN","IP_FILE","",tmp_check,LINE_SIZE,ini_path))
  {
    //load ips
    EnableWindow(GetDlgItem(h_main,BT_START),FALSE);
    loadFileIp(tmp_check);

    CheckDlgButton(h_main,CHK_LOAD_IP_FILE,BST_CHECKED);

    if (config.global_ip_file == FALSE)
    {
      //loads accounts
      tmp_check[0] = 0;
      config.nb_accounts = 0;
      if(GetPrivateProfileString("SCAN","ACCOUNT_FILE","",tmp_check,LINE_SIZE,ini_path))
      {
        LoadAuthFile(tmp_check);
      }
    }

    //check scan type :
    tmp_check[0] = 0;
    if(GetPrivateProfileString("SCAN","TYPE","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'A' || tmp_check[0] == 'a') //AUTO mode
      {


      }else if (tmp_check[0] == 'S' || tmp_check[0] == 's') //SIMPLE mode
      {
        //check test if enable !!
        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_ICMP","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_ICMP);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_DNS","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_DNS);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_NETBIOS","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_NETBIOS);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_NETBIOS_POLICY","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_NETBIOS_POLICY);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_NETBIOS_USERS","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_NETBIOS_USERS);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_CHECK_FILES","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_FILES);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_CHECK_REGISTRY","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_REGISTRY);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_CHECK_SERVICES","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_SERVICES);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_CHECK_SOFTWARE","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_SOFTWARE);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_CHECK_USB","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_USB);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_CHECK_SSH","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_SSH);
        }

        tmp_check[0] = 0;
        if(GetPrivateProfileString("SCAN","DISCO_WRITE_KEY","",tmp_check,LINE_SIZE,ini_path))
        {
          if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')SendDlgItemMessage(h_main,CB_tests,LB_SETSEL,(WPARAM)TRUE,(LPARAM)CHK_TEST_WRITE_KEY);
        }

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

        EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
        SetWindowText(GetDlgItem(h_main,BT_START),"Stop");
        AddMsg(h_main, (char*)"INFORMATION",(char*)"Start scan",(char*)"",FALSE);
        h_thread_scan = CreateThread(NULL,0,scan,(LPVOID)TRUE,0,0);
        return 0;
      }
    }

    //disable interface + init
    EnableWindow(GetDlgItem(h_main,BT_START),TRUE);

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

    SetWindowText(GetDlgItem(h_main,BT_START),"Stop");
    AddMsg(h_main, (char*)"INFORMATION",(char*)"Start scan",(char*)"",FALSE);

    //check test minimals
    config.disco_icmp           = TRUE;
    config.disco_dns            = TRUE;
    config.disco_netbios        = TRUE;
    config.disco_netbios_policy = TRUE;
    config.check_files          = TRUE;
    config.check_registry       = TRUE;
    config.check_ssh            = FALSE;

    //CHECK
    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","DISCO_CHECK_SSH","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
      {
        config.check_ssh      = (BOOL)load_file_list(CB_T_SSH,(char*)DEFAULT_LIST_SSH, TRUE);
      }
    }

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","M_SEC","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.M_SEC = TRUE;
      else auto_scan_config.M_SEC = FALSE;
    }else auto_scan_config.M_SEC = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","PATCH_UPDATED","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.PATCH_UPDATED = TRUE;
      else auto_scan_config.PATCH_UPDATED = FALSE;
    }else auto_scan_config.PATCH_UPDATED = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","WSUS_WORKS","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.WSUS_WORKS = TRUE;
      else auto_scan_config.WSUS_WORKS = FALSE;
    }else auto_scan_config.WSUS_WORKS = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","MCAFEE_INSTALLED","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.MCAFEE_INSTALLED = TRUE;
      else auto_scan_config.MCAFEE_INSTALLED = FALSE;
    }else auto_scan_config.MCAFEE_INSTALLED = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","MCAFEE_UPDATED","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.MCAFEE_UPDATED = TRUE;
      else auto_scan_config.MCAFEE_UPDATED = FALSE;
    }else auto_scan_config.MCAFEE_UPDATED = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","MCAFEE_SCAN","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.MCAFEE_SCAN = TRUE;
      else auto_scan_config.MCAFEE_SCAN = FALSE;
    }else auto_scan_config.MCAFEE_SCAN = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","PASSWORD_POLICY","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.PASSWORD_POLICY = TRUE;
      else auto_scan_config.PASSWORD_POLICY = FALSE;
    }else auto_scan_config.PASSWORD_POLICY = FALSE;

    tmp_check[0]                       = 0;
    auto_scan_config.C_ADMIN_ACCOUNT[0] = 0;
    if(GetPrivateProfileString("CHECK","ADMIN_ACCOUNT","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')
      {
        auto_scan_config.ADMIN_ACCOUNT = TRUE;
        GetPrivateProfileString("CHECK_OPTIONS","ADMIN_ACCOUNT","",auto_scan_config.C_ADMIN_ACCOUNT,MAX_PATH,ini_path);

        if (auto_scan_config.C_ADMIN_ACCOUNT[0] == 0)
          auto_scan_config.ADMIN_ACCOUNT = FALSE;
      }else auto_scan_config.ADMIN_ACCOUNT = FALSE;
    }else auto_scan_config.ADMIN_ACCOUNT = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","NULL_SESSION","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.NULL_SESSION = TRUE;
      else auto_scan_config.NULL_SESSION = FALSE;
    }else auto_scan_config.NULL_SESSION = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","AUTORUN","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.AUTORUN = TRUE;
      else auto_scan_config.AUTORUN = FALSE;
    }else auto_scan_config.AUTORUN = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK","SHARE_ACCESS","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.SHARE_ACCESS = TRUE;
      else auto_scan_config.SHARE_ACCESS = FALSE;
    }else auto_scan_config.SHARE_ACCESS = FALSE;

    //Options
    auto_scan_config.MSEC_REG_PATH[0] = 0;
    GetPrivateProfileString("CHECK_OPTIONS","MSEC_REG_PATH","",auto_scan_config.MSEC_REG_PATH,MAX_PATH,ini_path);
    auto_scan_config.MSEC_REG_VALUE[0] = 0;
    GetPrivateProfileString("CHECK_OPTIONS","MSEC_REG_VALUE","",auto_scan_config.MSEC_REG_VALUE,MAX_PATH,ini_path);

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","PASSWORD_POLICY_COMPLEXITY_ENABLE","",tmp_check,LINE_SIZE,ini_path))
    {
      if (tmp_check[0] == 'Y' || tmp_check[0] == 'y' || tmp_check[0] == 'O' || tmp_check[0] == 'o')auto_scan_config.PASSWORD_POLICY_COMPLEXITY_ENABLE = TRUE;
      else auto_scan_config.PASSWORD_POLICY_COMPLEXITY_ENABLE = FALSE;
    }else auto_scan_config.PASSWORD_POLICY_COMPLEXITY_ENABLE = FALSE;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","MCAFEE_SCAN_DAYS_INTERVAL","",tmp_check,LINE_SIZE,ini_path))
    {
      auto_scan_config.MCAFEE_SCAN_DAYS_INTERVAL = atoi(tmp_check);
    }else auto_scan_config.MCAFEE_SCAN_DAYS_INTERVAL = 0;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","MCAFEE_UPDATE_DAYS_INTERVAL","",tmp_check,LINE_SIZE,ini_path))
    {
      auto_scan_config.MCAFEE_UPDATE_DAYS_INTERVAL = atoi(tmp_check);
    }else auto_scan_config.MCAFEE_UPDATE_DAYS_INTERVAL = 0;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","PASSWORD_POLICY_MIN_AGE","",tmp_check,LINE_SIZE,ini_path))
    {
      auto_scan_config.PASSWORD_POLICY_MIN_AGE = atoi(tmp_check);
    }else auto_scan_config.PASSWORD_POLICY_MIN_AGE = 0;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","PASSWORD_POLICY_MAX_AGE","",tmp_check,LINE_SIZE,ini_path))
    {
      auto_scan_config.PASSWORD_POLICY_MAX_AGE = atoi(tmp_check);
    }else auto_scan_config.PASSWORD_POLICY_MAX_AGE = 0;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","PASSWORD_POLICY_MIN_LEN","",tmp_check,LINE_SIZE,ini_path))
    {
      auto_scan_config.PASSWORD_POLICY_MIN_LEN = atoi(tmp_check);
    }else auto_scan_config.PASSWORD_POLICY_MIN_LEN = 0;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","PASSWORD_POLICY_LOCKOUT_COUNT","",tmp_check,LINE_SIZE,ini_path))
    {
      auto_scan_config.PASSWORD_POLICY_LOCKOUT_COUNT = atoi(tmp_check);
    }else auto_scan_config.PASSWORD_POLICY_LOCKOUT_COUNT = 0;

    tmp_check[0] = 0;
    if(GetPrivateProfileString("CHECK_OPTIONS","PASSWORD_POLICY_HISTORY","",tmp_check,LINE_SIZE,ini_path))
    {
      auto_scan_config.PASSWORD_POLICY_HISTORY = atoi(tmp_check);
    }else auto_scan_config.PASSWORD_POLICY_HISTORY = 0;

    //start init scan
    time_t exec_time_start, exec_time_end;
    time(&exec_time_start);

    char tmp[MAX_PATH];
    nb_windows        = 0;

    //---------------------------------------------
    //scan_start
    DWORD i;
    nb_i = SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

    //init all criticals sections !!!
    initthreadings();

    hs_threads  = CreateSemaphore(NULL,NB_MAX_THREAD,NB_MAX_THREAD,NULL);
    hs_disco    = CreateSemaphore(NULL,NB_MAX_DISCO_THREADS,NB_MAX_DISCO_THREADS,NULL);
    hs_netbios  = CreateSemaphore(NULL,NB_MAX_NETBIOS_THREADS,NB_MAX_NETBIOS_THREADS,NULL);
    hs_file     = CreateSemaphore(NULL,NB_MAX_FILE_THREADS,NB_MAX_FILE_THREADS,NULL);
    hs_registry = CreateSemaphore(NULL,NB_MAX_REGISTRY_THREADS,NB_MAX_REGISTRY_THREADS,NULL);
    hs_tcp      = CreateSemaphore(NULL,NB_MAX_TCP_TEST_THREADS,NB_MAX_TCP_TEST_THREADS,NULL);
    hs_ssh      = CreateSemaphore(NULL,NB_MAX_SSH_THREADS,NB_MAX_SSH_THREADS,NULL);

    //wsa init
    WSADATA WSAData;
    WSAStartup(0x02, &WSAData );
    nb_test_ip = 0;
    nb_unknow  = 0;

    if (nb_i == 1)
    {
      char tmp_IP[MAX_PATH]="";
      SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)0,(LPARAM)tmp_IP);
      if (tmp_IP[0] == 0 || (tmp_IP[0] == '0' && tmp_IP[1] == '.'))
        nb_i = 0;
    }

    for (i=0;(i<nb_i) && scan_start;i++)
    {
      WaitForSingleObject(hs_threads,INFINITE);
      EnterCriticalSection(&Sync_threads);
      hs_c_threads++;
      LeaveCriticalSection(&Sync_threads);

      CreateThread(NULL,0,ScanIp_auto_scan,(PVOID)i,0,0);
    }

    //wait
    AddMsg(h_main,(char*)"INFORMATION",(char*)"Start waiting threads.",(char*)"",FALSE);

    if (!scan_start)
    {
      DWORD end = 0;
      while (nb_test_ip < i && end < THE_END_THREAD_WAIT){Sleep(100);end++;}
    }else
    {
      for(i=0;i<NB_MAX_THREAD;i++)WaitForSingleObject(hs_threads,INFINITE);

      WaitForSingleObject(hs_netbios,INFINITE);
      EnterCriticalSection(&Sync_threads_netbios);
      hs_c_netbios++;
      LeaveCriticalSection(&Sync_threads_netbios);

      WaitForSingleObject(hs_file,INFINITE);
      EnterCriticalSection(&Sync_threads_files);
      hs_c_file++;
      LeaveCriticalSection(&Sync_threads_files);

      WaitForSingleObject(hs_registry,INFINITE);
      EnterCriticalSection(&Sync_threads_registry);
      hs_c_registry++;
      LeaveCriticalSection(&Sync_threads_registry);

      WaitForSingleObject(hs_tcp,INFINITE);
      EnterCriticalSection(&Sync_threads_tcp);
      hs_c_tcp++;
      LeaveCriticalSection(&Sync_threads_tcp);

      WaitForSingleObject(hs_ssh,INFINITE);
      EnterCriticalSection(&Sync_threads_ssh);
      hs_c_ssh++;
      LeaveCriticalSection(&Sync_threads_ssh);
    }

    WSACleanup();

    //calcul run time
    time(&exec_time_end);

    AddMsg(h_main,(char*)"INFORMATION",(char*)"End of scan!",(char*)"",FALSE);
    snprintf(tmp,MAX_PATH,"Ip view:%lu/%lu in %d.%0d minutes",ListView_GetItemCount(GetDlgItem(h_main,LV_results)),nb_i,(exec_time_end - exec_time_start)/60,(exec_time_end - exec_time_start)%60);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"",FALSE);
    snprintf(tmp,MAX_PATH,"Computer in Microsoft Windows OS:%lu/%lu",nb_windows,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"",FALSE);
    snprintf(tmp,MAX_PATH,"Computer Unknow (valide?):%lu/%lu",nb_unknow,nb_i);
    AddMsg(h_main,(char*)"INFORMATION",(char*)tmp,(char*)"",FALSE);


    //close
    CloseHandle(hs_threads);
    CloseHandle(hs_disco);
    CloseHandle(hs_netbios);
    CloseHandle(hs_file);
    CloseHandle(hs_registry);
    CloseHandle(hs_tcp);
    CloseHandle(hs_ssh);

    //chek if results
    if (ListView_GetItemCount(GetDlgItem(h_main,LV_results)) >0)
    {
      //save
      char file2[LINE_SIZE];
      time_t dateEtHMs;
      time(&dateEtHMs);
      struct tm *today = localtime(&dateEtHMs);

      char date[DATE_SIZE];
      strftime(date, DATE_SIZE,"%Y.%m.%d-%H.%M.%S",today);

      char cpath[LINE_SIZE]="";
      GetLocalPath(cpath, LINE_SIZE);

      if (auto_scan_config.save_CSV)
      {
        snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.csv",cpath,date);
        if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_CSV, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2,FALSE);
        else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2,FALSE);

        save_done = TRUE;
      }

      if (auto_scan_config.save_XML)
      {
        snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.xml",cpath,date);
        if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_XML, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2,FALSE);
        else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2,FALSE);

        save_done = TRUE;
      }

      if (auto_scan_config.save_HTML)
      {
        snprintf(file2,LINE_SIZE,"%s\\[%s]_auto_scan_NS.html",cpath,date);
        if(SaveLSTV(GetDlgItem(h_main,LV_results), file2, SAVE_TYPE_HTML, NB_COLUMN)) AddMsg(h_main, (char*)"INFORMATION",(char*)"Recorded data",file2,FALSE);
        else AddMsg(h_main, (char*)"ERROR",(char*)"No data saved to!",file2,FALSE);

        save_done = TRUE;
      }
    }

    if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
    {
      EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),TRUE);
      EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),TRUE);
      EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),TRUE);
    }

    //if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
    {
      EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),TRUE);
      EnableWindow(GetDlgItem(h_main,IP1),TRUE);
      EnableWindow(GetDlgItem(h_main,BT_IP_CP),TRUE);
      EnableWindow(GetDlgItem(h_main,IP2),TRUE);
    }

    CheckDlgButton(h_main,CHK_LOAD_IP_FILE,BST_UNCHECKED);
    EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),TRUE);
    EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
    EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),TRUE);
    EnableWindow(GetDlgItem(h_main,CB_tests),TRUE);
    EnableWindow(GetDlgItem(h_main,BT_LOAD_MDP_FILES),TRUE);
    EnableWindow(GetDlgItem(h_main,BT_RE),TRUE);
    scan_start = FALSE;

    EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
    SetWindowText(GetDlgItem(h_main,BT_START),"Start");
    SetWindowText(h_main,TITLE);
    h_thread_scan = 0;

    if (auto_scan_config.auto_close_after_save && save_done)
    {
      SendMessage(h_main, WM_CLOSE, 0, 0);
    }

    return 0;
  }

  h_thread_scan = 0;
  if (auto_scan_config.auto_close_after_save && save_done)
  {
    SendMessage(h_main, WM_CLOSE, 0, 0);
  }
  return 0;
}
