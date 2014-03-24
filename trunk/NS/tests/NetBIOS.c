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

      for(i=1;i<=er;i++)
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
            snprintf(msg,MAX_PATH,"\\\\%s\\%S (%S)[NULL SESSION]",ip,p->shi1_netname,p->shi1_remark);
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
  }while(res==ERROR_MORE_DATA);
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
