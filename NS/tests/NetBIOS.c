//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
//source : http://msdn.microsoft.com/en-us/library/windows/desktop/ms724832%28v=vs.85%29.aspx
BOOL Netbios_OS(char *ip, char*txtOS, char *name, char *domain, unsigned int sz_max)
{
  wchar_t serveur[MAX_PATH];
  char tmp[MAX_PATH];
  BOOL ret = FALSE;
  _snprintf(tmp,MAX_PATH,"\\\\%s",ip);
  //init de la chaine (pour connexion à la machine)
  mbstowcs( serveur,tmp,MAX_PATH);

  //lecture des informations NETBIOS
  WKSTA_INFO_100 *mybuff;
  NET_API_STATUS res = NetWkstaGetInfo(serveur, 100,(BYTE**)&mybuff);

  //OS
  if((res == ERROR_SUCCESS || res == ERROR_MORE_DATA) && mybuff)
  {
    ret = TRUE;
    if (name!=NULL)
    {
      snprintf(name,sz_max,"%S",serveur);
      if (name[0] == '\\' && name[1] == '\\')
        name[0] = 0;
    }

    if (domain!=NULL) snprintf(domain,sz_max,"%S",mybuff->wki100_langroup);

    //on test le type d'os et on met a jour
    switch(mybuff->wki100_ver_major)
    {
      case 4:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:snprintf(txtOS,sz_max,"Microsoft Windows 95/NT4*");break;
            case 10:snprintf(txtOS,sz_max,"Microsoft Windows 98*");break;//
            case 90:snprintf(txtOS,sz_max,"Microsoft Windows ME*");break;
        }
      break;
      case 5:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:snprintf(txtOS,sz_max,"Microsoft Windows 2K*");break;
            case 1:snprintf(txtOS,sz_max,"Microsoft Windows XP*");break;//
            case 2:snprintf(txtOS,sz_max,"Microsoft Windows 2003/XP-64b*");break;
        }
      break;
      case 6:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:snprintf(txtOS,sz_max,"Microsoft Windows Vista/2008*");break;
            case 1:snprintf(txtOS,sz_max,"Microsoft Windows 7/2008 R2-64b*");break;
            case 2:snprintf(txtOS,sz_max,"Microsoft Windows 8/2012*");break;
        }
      break;
      case 7:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:snprintf(txtOS,sz_max,"Microsoft Windows 7*");break;
        }
      break;
      default:
            snprintf(txtOS,sz_max,"Microsoft Windows [major:%lu;minor:%lu;pid:%lu]*",mybuff->wki100_ver_major,mybuff->wki100_ver_minor,mybuff->wki100_platform_id);
      break;
    }
  }
  NetApiBufferFree(mybuff);
  return ret;
}
//----------------------------------------------------------------
BOOL Netbios_NULLSessionStart(char *ip, char*share)
{
  char tmp[MAX_PATH];
  snprintf(tmp,MAX_PATH,"\\\\%s\\%s",ip,share);

  NETRESOURCE NetRes;
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpLocalName  = (LPSTR)"";
  NetRes.lpRemoteName	= tmp;
  NetRes.lpProvider   = (LPSTR)"";
  if (WNetAddConnection2(&NetRes,"","",0) == NO_ERROR)return TRUE;
  return FALSE;
}
//----------------------------------------------------------------
void Netbios_NULLSessionStop(char *ip, char*share)
{
  char tmp[MAX_PATH];
  snprintf(tmp,MAX_PATH,"\\\\%s\\%s",ip,share);
  WNetCancelConnection2(tmp,CONNECT_UPDATE_PROFILE,1);
}
//----------------------------------------------------------------
BOOL Netbios_NULLSession(char *ip, char*share)
{
  if (Netbios_NULLSessionStart(ip, share))
  {
    Netbios_NULLSessionStop(ip, share);
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------
BOOL Netbios_Time(wchar_t *server, char *time, unsigned int sz_max)
{
  TIME_OF_DAY_INFO *timep;
  BOOL ret = FALSE;

  if (NetRemoteTOD(server,(BYTE**)&timep) == NERR_Success)
  {
    if (timep)
    {
      snprintf(time,sz_max, "%d/%02d/%02d %d:%02d:%02d",timep->tod_year,timep->tod_month,timep->tod_day,timep->tod_hours, timep->tod_mins, timep->tod_secs);
      ret = TRUE;
    }
    NetApiBufferFree(timep);
  }
  return ret;
}
//----------------------------------------------------------------
BOOL Netbios_Share(wchar_t *server, DWORD iitem, DWORD col, char*ip, BOOL IPC_null_session)
{
  BOOL ret = FALSE;
  NET_API_STATUS res;
  PSHARE_INFO_1 BufPtr,p;
  DWORD i, er=0,tr=0,resume=0;
  char tmp_share[MAX_PATH], msg[MAX_PATH];

  do
  {
    res = NetShareEnum (server, 1, (LPBYTE *) &BufPtr, -1, &er, &tr, &resume);
    if((res == ERROR_SUCCESS || res == ERROR_MORE_DATA) && BufPtr!=NULL)
    {
      ret = TRUE;
      p   = BufPtr;

      for(i=1;i<=er&& scan_start;i++)
      {
        //check if we can connect in null session
        snprintf(tmp_share,MAX_PATH,"%S",p->shi1_netname);

        if (!strcmp(tmp_share,"IPC$"))
        {
         //pour éviter l'ouverture et fermeture du partage et créer un bug
          if (IPC_null_session)
          {
            snprintf(msg,MAX_PATH,"\\\\%s\\%S (%S)[NULL SESSION]",ip,p->shi1_netname,p->shi1_remark);
          }else
          {
            snprintf(msg,MAX_PATH,"\\\\%s\\%S (%S)",ip,p->shi1_netname,p->shi1_remark);
          }
        }else
        {
          if (Netbios_NULLSession(ip, tmp_share))
          {
            snprintf(msg,MAX_PATH,"\\\\%s\\%S (%S)[NULL SESSION READ ACCESS]",ip,p->shi1_netname,p->shi1_remark);
          }else
          {
            snprintf(msg,MAX_PATH,"\\\\%s\\%S (%S)",ip,p->shi1_netname,p->shi1_remark);
          }
        }

        AddMsg(h_main,"FOUND (Share)",msg,"");
        AddLSTVUpdateItem(msg, col, iitem);
        p++;
      }
     NetApiBufferFree(BufPtr);
    }
  }while(res==ERROR_MORE_DATA&& scan_start);
  return ret;
}
//----------------------------------------------------------------
BOOL TestReversSID(char *ip, char* user)
{
  UCHAR domain[MAX_PATH];
  UCHAR BSid[MAX_PATH];
  SID_NAME_USE peUse;

  PSID Sid        = (PSID) BSid;
  DWORD sz_Sid    = MAX_PATH;
  DWORD sz_domain = MAX_PATH;

  if (LookupAccountName((LPSTR)ip,(LPSTR)user, (PSID)Sid, &sz_Sid,(LPSTR)domain, &sz_domain,&peUse) != 0)return TRUE;
  else return FALSE;
}
//----------------------------------------------------------------
void CheckReversSID(char *ip, char *results, DWORD max_size_results)
{
    results[0] = 0;
    if(TestReversSID(ip,(char*)"invité"))             snprintf(results,max_size_results,"Revers SID:Enable (OK with \"invité\" account)");
    else if(TestReversSID(ip,(char*)"guest"))         snprintf(results,max_size_results,"Revers SID:Enable (OK with \"guest\" account)");
    else if(TestReversSID(ip,(char*)"gast"))          snprintf(results,max_size_results,"Revers SID:Enable (OK with \"gast\" account)");
    else if(TestReversSID(ip,(char*)"invitado"))      snprintf(results,max_size_results,"Revers SID:Enable (OK with \"invitado\" account)");
    else if(TestReversSID(ip,(char*)"HelpAssistant")) snprintf(results,max_size_results,"Revers SID:Enable (OK with \"HelpAssistant\" account)");
    else if(TestReversSID(ip,(char*)"ASPNET"))        snprintf(results,max_size_results,"Revers SID:Enable (OK with \"ASPNET\" account)");
    else if(TestReversSID(ip,(char*)"administrateur"))snprintf(results,max_size_results,"Revers SID:Enable (OK with \"administrateur\" account)");
    else if(TestReversSID(ip,(char*)"administrator")) snprintf(results,max_size_results,"Revers SID:Enable (OK with \"administrator\" account)");
}
//----------------------------------------------------------------
BOOL Netbios_Policy(wchar_t *server, char *pol, unsigned int sz_max)
{
  BOOL ret0 = FALSE, ret1 = FALSE;
  USER_MODALS_INFO_0 *pUmI_0 = NULL;
  USER_MODALS_INFO_3 *pUmI_3 = NULL;

  char tmp_pUmI_0[MAX_PATH]="", tmp_pUmI_3[MAX_PATH]="";

  pol[0] = 0;

  if (NERR_Success == NetUserModalsGet(server,0,(LPBYTE *)&pUmI_0))
  {
    if (pUmI_0 != NULL)
    {
      ret0 = TRUE;
      snprintf(tmp_pUmI_0,MAX_PATH,"min_passwd_len:%lu\r\n"
                                   "max_passwd_age:%lu(days)\r\n"
                                   "min_passwd_age:%lu(days)\r\n"
                                   "passord_history:%lu\r\n"
                                   //"force_logoff:%d(s)\n"
                                  ,pUmI_0->usrmod0_min_passwd_len
                                  ,pUmI_0->usrmod0_max_passwd_age/86400
                                  ,pUmI_0->usrmod0_min_passwd_age/86400
                                  ,pUmI_0->usrmod0_password_hist_len
                                  /*,pUmI_0->usrmod0_force_logoff*/);
      NetApiBufferFree(pUmI_0);
    }
  }

  if (NERR_Success == NetUserModalsGet(server,3,(LPBYTE *)&pUmI_3))
  {
    if (pUmI_3 != NULL)
    {
      ret1 = TRUE;
      snprintf(tmp_pUmI_3,MAX_PATH,"lockout_duration_before_auto_unlock:%lu(s)\r\n"
                                   "wait_time_to_reset_fail_logon_count:%lu(s)\r\n"
                                   "bad_password_count_to_lock_account:%lu"
                                  ,pUmI_3->usrmod3_lockout_duration
                                  ,pUmI_3->usrmod3_lockout_observation_window
                                  ,pUmI_3->usrmod3_lockout_threshold);
      NetApiBufferFree(pUmI_3);
    }
  }

  if (ret0 && ret1) snprintf(pol,sz_max,"%s%s",tmp_pUmI_0,tmp_pUmI_3);
  else if (ret0) strncpy(pol,tmp_pUmI_0,sz_max);
  else strncpy(pol,tmp_pUmI_3,sz_max);

  return (ret0|ret1);
}
//------------------------------------------------------------------------------
BOOL Netbios_check_user(DWORD iitem, char *ip, char*username)
{
  if (!scan_start)return FALSE;

  char tmp[MAX_PATH];
  wchar_t serveur[MAX_PATH];
  wchar_t user[MAX_PATH];
  snprintf(tmp,MAX_PATH,"\\\\%s",ip);
  mbstowcs(serveur,tmp,MAX_PATH);
  mbstowcs(user,username,MAX_PATH);

  LPUSER_INFO_1 p1Buf =NULL;
  LPUSER_INFO_2 p2Buf =NULL;
  LPUSER_INFO_3 p3Buf =NULL;
  NET_API_STATUS net_res;

  net_res = NetUserGetInfo(serveur,user,1,(LPBYTE*)&p1Buf);
  if (net_res == NERR_UserNotFound)
  {
    AddLSTVUpdateItem("ADMIN_ACCOUNT:NOK (Not found)", COL_CONFIG, iitem);
    AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:NOK (Not found)");
    return TRUE;
  }else if (net_res==NERR_Success)
  {
    if (p1Buf->usri1_flags&UF_ACCOUNTDISABLE)
    {
      if (p1Buf->usri1_flags&UF_PASSWORD_EXPIRED)
      {
        AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but disable, password expired)", COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but disable, password expired)");
      }else
      {
        AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but disable)", COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but disable)");
      }
    }else
    {
      AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Available)", COL_CONFIG, iitem);
      AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Available)");
    }
    NetApiBufferFree(p1Buf);
    return TRUE;
  }else if (NetUserGetInfo(serveur,user,2,(LPBYTE*)&p2Buf)==NERR_Success)
  {
    net_res = NetUserGetInfo(serveur,user,2,(LPBYTE*)&p2Buf);
    if (net_res == NERR_UserNotFound)
    {
      AddLSTVUpdateItem("ADMIN_ACCOUNT:NOK (Not found)", COL_CONFIG, iitem);
      AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:NOK (Not found)");
      return TRUE;
    }else if (net_res==NERR_Success)
    {
      if (p2Buf->usri2_flags&UF_ACCOUNTDISABLE)
      {
        if (p2Buf->usri2_flags&UF_PASSWORD_EXPIRED)
        {
          AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but disable, password expired)", COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but disable, password expired)");
        }else
        {
          AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but disable)", COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but disable)");
        }
      }else
      {
        AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Available)", COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Available)");
      }
      NetApiBufferFree(p2Buf);
      return TRUE;
    }else
    {
      net_res = NetUserGetInfo(serveur,user,3,(LPBYTE*)&p3Buf);
      if (net_res == NERR_UserNotFound)
      {
        AddLSTVUpdateItem("ADMIN_ACCOUNT:NOK (Not found)", COL_CONFIG, iitem);
        AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:NOK (Not found)");
        return TRUE;
      }else if (net_res==NERR_Success)
      {
        if (p3Buf->usri3_flags&UF_ACCOUNTDISABLE)
        {
          if (p3Buf->usri3_flags&UF_PASSWORD_EXPIRED || p3Buf->usri3_password_expired)
          {
            AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but disable, password expired)", COL_CONFIG, iitem);
            AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but disable, password expired)");
          }else
          {
            AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Exist but disable)", COL_CONFIG, iitem);
            AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Exist but disable)");
          }
        }else
        {
          AddLSTVUpdateItem("ADMIN_ACCOUNT:OK (Available)", COL_CONFIG, iitem);
          AddMsg(h_main, (char*)"FOUND (Account policy)",ip,(char*)"ADMIN_ACCOUNT:OK (Available)");
        }
        NetApiBufferFree(p3Buf);
        return TRUE;
      }
    }
  }

  return FALSE;
}
//------------------------------------------------------------------------------
int Netbios_List_service(DWORD iitem, char *ip, BOOL check)
{
  //check with current rights
  SC_HANDLE hm = OpenSCManager(ip, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE|SC_MANAGER_CONNECT);
  int ok = -1;
  if (hm != NULL)
  {
    char tmp[MAX_PATH], tmp2[MAX_PATH];
    ENUM_SERVICE_STATUS service_data, *lpservice = NULL;
    BOOL retVal;
    DWORD bytesNeeded,srvCount,resumeHandle = 0;

    //Call EnumServicesStatus using the handle returned by OpenSCManager
    retVal = EnumServicesStatus(hm,SERVICE_WIN32|SERVICE_DRIVER,SERVICE_STATE_ALL,&service_data,sizeof(service_data),&bytesNeeded,&srvCount,&resumeHandle);
    DWORD err = GetLastError();

    //Check if EnumServicesStatus needs more memory space
    if ((retVal == FALSE) || err == ERROR_MORE_DATA)
    {
        DWORD dwBytes = bytesNeeded + sizeof(ENUM_SERVICE_STATUS)+1;

        lpservice = (ENUM_SERVICE_STATUS*) malloc(dwBytes);
        if (lpservice != NULL)
        {
          if(EnumServicesStatus (hm,SERVICE_WIN32,SERVICE_STATE_ALL,lpservice,dwBytes,&bytesNeeded,&srvCount,&resumeHandle)==FALSE)
            srvCount = 0;
        }
    }

    //get datas
    int i;
    if (srvCount)
    {
      ok = 0;
      if (check)
      {
        for(i=0;i<srvCount && scan_start;i++)
        {
          if (LSBExist(CB_T_SERVICES, lpservice[i].lpDisplayName) || LSBExist(CB_T_SERVICES, lpservice[i].lpServiceName))
          {
            ok++;
            switch(lpservice[i].ServiceStatus.dwCurrentState)
            {
              case SERVICE_CONTINUE_PENDING:  snprintf(tmp,MAX_PATH,"%s %s (%s;State:CONTINUE_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
              case SERVICE_START_PENDING:     snprintf(tmp,MAX_PATH,"%s %s (%s;State:START_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
              case SERVICE_RUNNING:           snprintf(tmp,MAX_PATH,"%s %s (%s;State:RUNNING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
              case SERVICE_PAUSE_PENDING:     snprintf(tmp,MAX_PATH,"%s %s (%s;State:PAUSE_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
              case SERVICE_PAUSED:            snprintf(tmp,MAX_PATH,"%s %s (%s;State:PAUSED)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
              case SERVICE_STOP_PENDING:      snprintf(tmp,MAX_PATH,"%s %s (%s;State:STOP_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
              case SERVICE_STOPPED:           snprintf(tmp,MAX_PATH,"%s %s (%s;State:STOPPED)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
              default:                        snprintf(tmp,MAX_PATH,"%s %s (%s;State:UNKNOW)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            }
            AddMsg(h_main,(char*)"FOUND (Service)",tmp,"");
            AddLSTVUpdateItem(tmp, COL_SERVICE, iitem);
          }
        }
      }else
      {
        for(i=0;i<srvCount && scan_start;i++)
        {
          ok++;
          switch(lpservice[i].ServiceStatus.dwCurrentState)
          {
            case SERVICE_CONTINUE_PENDING:  snprintf(tmp,MAX_PATH,"%s %s (%s;State:CONTINUE_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            case SERVICE_START_PENDING:     snprintf(tmp,MAX_PATH,"%s %s (%s;State:START_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            case SERVICE_RUNNING:           snprintf(tmp,MAX_PATH,"%s %s (%s;State:RUNNING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            case SERVICE_PAUSE_PENDING:     snprintf(tmp,MAX_PATH,"%s %s (%s;State:PAUSE_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            case SERVICE_PAUSED:            snprintf(tmp,MAX_PATH,"%s %s (%s;State:PAUSED)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            case SERVICE_STOP_PENDING:      snprintf(tmp,MAX_PATH,"%s %s (%s;State:STOP_PENDING)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            case SERVICE_STOPPED:           snprintf(tmp,MAX_PATH,"%s %s (%s;State:STOPPED)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
            default:                        snprintf(tmp,MAX_PATH,"%s %s (%s;State:UNKNOW)\n",ip,lpservice[i].lpDisplayName,lpservice[i].lpServiceName);break;
          }
          AddMsg(h_main,(char*)"FOUND (Service)",tmp,"");
          AddLSTVUpdateItem(tmp, COL_SERVICE, iitem);
        }
      }
    }
    CloseServiceHandle(hm);
  }
  return ok;
}
//----------------------------------------------------------------
BOOL EnumTestReversSID(DWORD iitem, char *ip, BOOL check_only, char* user, char* results, unsigned int sz_results)
{
  #define MAX_SID_ERROR_COUNT   100

  UCHAR domain[MAX_PATH];
  UCHAR BSid[MAX_PATH];
  SID_NAME_USE peUse;

  int MySid[8]   = {0,0,0,0,0,0,0,0};
  PSID Sid        = (PSID) BSid;
  DWORD sz_Sid    = MAX_PATH;
  DWORD sz_domain = MAX_PATH;

  if (LookupAccountName((LPSTR)ip,(LPSTR)user, (PSID)Sid, &sz_Sid,(LPSTR)domain, &sz_domain,&peUse) != 0)
  {
    //check if valid SID!
    DWORD i, Dtmp=0;
    PSID_IDENTIFIER_AUTHORITY SidIdentAuthority = GetSidIdentifierAuthority(Sid);

    if (SidIdentAuthority->Value[0] == 0 && SidIdentAuthority->Value[1] == 0) //alors valeurs en hexa
      Dtmp = (int)(unsigned long)SidIdentAuthority->Value[5]+ (SidIdentAuthority->Value[4]<<8) + (SidIdentAuthority->Value[3]<<16) + (SidIdentAuthority->Value[2]<<24);//nombre d'authorités
    else return TRUE;

    //nb values of the SID!
    unsigned char ucMax = *GetSidSubAuthorityCount(Sid);
    if (ucMax < 1 || ucMax > 8) return TRUE;
    for (i=0;i<ucMax;++i)MySid[i] = *GetSidSubAuthority(Sid,i);

    if (check_only)
    {
      if (results != NULL)
      {
        if (Dtmp == 5)snprintf(results,sz_results     ,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4]);
        else if (Dtmp == 6)snprintf(results,sz_results,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5]);
        else if (Dtmp == 7)snprintf(results,sz_results,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6]);
        else if (Dtmp == 8)snprintf(results,sz_results,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6],MySid[7]);
      }
    }else //enum all
    {
      char tmp[MAX_PATH]="";

      if (Dtmp == 5)snprintf(tmp,MAX_PATH     ,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4]);
      else if (Dtmp == 6)snprintf(tmp,MAX_PATH,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5]);
      else if (Dtmp == 7)snprintf(tmp,MAX_PATH,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6]);
      else if (Dtmp == 8)snprintf(tmp,MAX_PATH,"Revers SID:Enable (OK with %s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d-%d)",domain,user,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6],MySid[7]);

      if (tmp[0] != 0)
      {
        AddMsg(h_main, (char*)"FOUND (Config)",ip,tmp);
        AddLSTVUpdateItem(tmp, COL_CONFIG, iitem);
      }

      //enums users accounts
      unsigned int err = 0;
      for (i=500; err < MAX_SID_ERROR_COUNT && scan_start && i < (520); i++)
      {
        MySid[ucMax-1] = i;
        SID_IDENTIFIER_AUTHORITY sia = {0,0,0,0,0,Dtmp};
        PSID pSid;
        if (AllocateAndInitializeSid(&sia,ucMax,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6],MySid[7],&pSid)!=0)
        {
          sz_Sid = sz_domain = MAX_PATH;
          if(LookupAccountSid(ip,pSid,BSid,&sz_Sid,domain,&sz_domain,&peUse))
          {
            err = 0;

            //account type
            /*
            typedef enum _SID_NAME_USE {
            SidTypeUser            = 1,
            SidTypeGroup,
            SidTypeDomain,
            SidTypeAlias,
            SidTypeWellKnownGroup,
            SidTypeDeletedAccount,
            SidTypeInvalid,
            SidTypeUnknown,
            SidTypeComputer,
            SidTypeLabel
          } SID_NAME_USE, *PSID_NAME_USE;
            */
            if (peUse == 1) //user enable only
            {
              tmp[0] = 0;

              if (Dtmp == 5)snprintf(tmp,MAX_PATH     ,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4]);
              else if (Dtmp == 6)snprintf(tmp,MAX_PATH,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5]);
              else if (Dtmp == 7)snprintf(tmp,MAX_PATH,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6]);
              else if (Dtmp == 8)snprintf(tmp,MAX_PATH,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6],MySid[7]);

              if (tmp[0] != 0)
              {
                AddMsg(h_main, (char*)"FOUND (Config:ReversSID)",ip,tmp);
                AddLSTVUpdateItem(tmp, COL_CONFIG, iitem);
              }
            }
          }else err++;
        }else err++;
      }

      //enum all users :)
      err = 0;
      for (i=1000; err < MAX_SID_ERROR_COUNT && scan_start && i < (1020); i++)
      {
        MySid[ucMax-1] = i;
        SID_IDENTIFIER_AUTHORITY sia = {0,0,0,0,0,Dtmp};
        PSID pSid;
        if (AllocateAndInitializeSid(&sia,ucMax,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6],MySid[7],&pSid)!=0)
        {
          sz_Sid = sz_domain = MAX_PATH;
          if(LookupAccountSid(ip,pSid,BSid,&sz_Sid,domain,&sz_domain,&peUse))
          {
            err = 0;

            //account type
            if (peUse == 1) //user enable only
            {
              tmp[0] = 0;

              if (Dtmp == 5)snprintf(tmp,MAX_PATH     ,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4]);
              else if (Dtmp == 6)snprintf(tmp,MAX_PATH,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5]);
              else if (Dtmp == 7)snprintf(tmp,MAX_PATH,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6]);
              else if (Dtmp == 8)snprintf(tmp,MAX_PATH,"%s\\%s SID:S-1-%d-%d-%d-%d-%d-%d-%d-%d-%d",domain,BSid,Dtmp,MySid[0],MySid[1],MySid[2],MySid[3],MySid[4],MySid[5],MySid[6],MySid[7]);

              if (tmp[0] != 0)
              {
                AddMsg(h_main, (char*)"FOUND (Config:ReversSID)",ip,tmp);
                AddLSTVUpdateItem(tmp, COL_CONFIG, iitem);
              }
            }
          }else err++;
        }else err++;
      }
    }
    return TRUE;
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL Netbios_List_users_reversSID(DWORD iitem, char *ip, BOOL check_only, char*results, unsigned int max_size_results)
{
  char users_test[][256]={"invité","guest","HelpAssistant","ASPNET","krbtgt","administrateur","administrator","admin","gast","invitado"};
  if (results != NULL)results[0] = 0;

  int i;
  for (i=0;i<10 && scan_start;i++)
  {
    if(EnumTestReversSID(iitem,ip,check_only,users_test[i],results,max_size_results)) return TRUE;
  }

  return FALSE;
}
//------------------------------------------------------------------------------
BOOL Netbios_List_users_Netbios(DWORD iitem, char *ip, DWORD limit)
{
  #define UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED      0x000080
  wchar_t serveur[MAX_PATH];
  char tmp[MAX_PATH],tmp2[MAX_PATH],tmp3[MAX_PATH];

  //init
  snprintf(tmp, MAX_PATH,"\\\\%s",ip);
  if (mbstowcs(serveur, tmp, MAX_PATH) >0)
  {
    NET_API_STATUS res;
    PNET_DISPLAY_USER  uBuff    = 0, u;
    DWORD i=0,nb                = 0,
                             er = 0,
                              j = 0;

    LPGROUP_USERS_INFO_0 gBuff  = NULL, g;
    DWORD dwPrefMaxLen          = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead         = 0;
    DWORD dwTotalEntries        = 0;

    do
    {
      res = NetQueryDisplayInformation(serveur, 1, i++, 100, MAX_PREFERRED_LENGTH, &er,(PVOID*)&uBuff);
      if(((res==ERROR_SUCCESS) || (res==ERROR_MORE_DATA))&& uBuff!=NULL && er>0)
      {
        u = uBuff;
        for (;er>0;er--,u++)
        {
          tmp2[0] = 0;
          if (u->usri1_flags & UF_ACCOUNTDISABLE)strncat(tmp2,"UF_ACCOUNTDISABLE,\0",MAX_PATH);
          if (u->usri1_flags & UF_PASSWD_NOTREQD)strncat(tmp2,"UF_PASSWD_NOTREQD,\0",MAX_PATH);
          if (u->usri1_flags & UF_PASSWD_CANT_CHANGE)strncat(tmp2,"UF_PASSWD_CANT_CHANGE,\0",MAX_PATH);
          if (u->usri1_flags & UF_LOCKOUT)strncat(tmp2,"UF_LOCKOUT,\0",MAX_PATH);
          if (u->usri1_flags & UF_DONT_EXPIRE_PASSWD)strncat(tmp2,"UF_DONT_EXPIRE_PASSWD,\0",MAX_PATH);
          if (u->usri1_flags & UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED)strncat(tmp2,"UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED,\0",MAX_PATH);
          if (u->usri1_flags & UF_PASSWORD_EXPIRED)strncat(tmp2,"UF_PASSWORD_EXPIRED,\0",MAX_PATH);

          strncat(tmp2,"\0",MAX_PATH);

          memset(tmp3,0,MAX_PATH);
          WideCharToMultiByte(CP_ACP,0,u->usri1_comment,lstrlenW(u->usri1_comment),tmp3,MAX_PATH,NULL,NULL);

          snprintf(tmp,MAX_PATH,"%S (%S:ID:%u,%s) [%s]",u->usri1_name,u->usri1_full_name,u->usri1_user_id,tmp3,tmp2);
          AddMsg(h_main, (char*)"FOUND (Config:Users)",ip,tmp);
          AddLSTVUpdateItem(tmp, COL_CONFIG, iitem);
          nb++;

          //get user group ?
          if(NetUserGetGroups(serveur,u->usri1_name,0,(LPBYTE*)&gBuff,dwPrefMaxLen,&dwEntriesRead,&dwTotalEntries) == NERR_Success)
          {
            g = gBuff;
            tmp[0] = 0;
            for (j=0; j<dwEntriesRead && g != NULL && scan_start;j++)
            {
              sprintf(tmp2,"%S,",g->grui0_name);
              if (!strcmp(tmp2,"None,") || !strcmp(tmp2,"Aucun,")){}
              else strncat(tmp,tmp2,MAX_PATH);
              g++;
            }

            if (tmp[0] != 0)
            {
              strncat(tmp,"\0",MAX_PATH);
              AddMsg(h_main, (char*)"FOUND (Config:Users groups)",ip,tmp);
              AddLSTVUpdateItem(tmp, COL_CONFIG, iitem);
            }

            if (gBuff != NULL)NetApiBufferFree(gBuff);
          }
        }
      }

      if (uBuff != NULL)NetApiBufferFree(uBuff);
    }while(res==ERROR_MORE_DATA && scan_start && nb<limit);
  }

  return FALSE;
}
//------------------------------------------------------------------------------
BOOL Netbios_List_users(DWORD iitem, char *ip)
{
  //check user (null-session and authenticate)
  if(Netbios_List_users_Netbios(iitem, ip, 50)) return TRUE;

  if (config.config_revers_SID == TRUE)
  {
    //check user (null-session + revers SID)
    if(Netbios_List_users_reversSID(iitem, ip, FALSE, NULL, 0)) return TRUE;
  }

  return FALSE;
}
