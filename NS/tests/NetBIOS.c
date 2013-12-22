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
    if (name!=NULL)   snprintf(name,sz_max,"%S",serveur);
    if (domain!=NULL) snprintf(domain,sz_max,"%S",mybuff->wki100_langroup);

    //on test le type d'os et on met a jour
    switch(mybuff->wki100_ver_major)
    {
      case 4:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Microsoft Windows 95/NT4*",sz_max);break;
            case 10:strncpy(txtOS,"Microsoft Windows 98*",sz_max);break;//
            case 90:strncpy(txtOS,"Microsoft Windows ME*",sz_max);break;
        }
      break;
      case 5:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Microsoft Windows 2K*",sz_max);break;
            case 1:strncpy(txtOS,"Microsoft Windows XP*",sz_max);break;//
            case 2:strncpy(txtOS,"Microsoft Windows 2003/XP-64b*",sz_max);break;
        }
      break;
      case 6:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Microsoft Windows Vista/2008*",sz_max);break;
            case 1:strncpy(txtOS,"Microsoft Windows 7/2008 R2-64b*",sz_max);break;
            case 2:strncpy(txtOS,"Microsoft Windows 8/2012*",sz_max);break;
        }
      break;
      case 7:
        switch (mybuff->wki100_ver_minor)
        {
            case 0:strncpy(txtOS,"Microsoft Windows 7*",sz_max);break;
        }
      break;
      default:
            _snprintf(txtOS,sz_max,"Microsoft Windows [major:%d;minor:%d]*",mybuff->wki100_ver_major,mybuff->wki100_ver_minor);
      break;
    }
  }
  NetApiBufferFree(mybuff);
  return ret;
}
//----------------------------------------------------------------
BOOL Netbios_NULLSession(char *ip)
{
  char tmp[MAX_PATH];
  _snprintf(tmp,MAX_PATH,"\\\\%s\\ipc$",ip);

  BOOL ret            = FALSE;
  NETRESOURCE NetRes;
  NetRes.dwType	      = RESOURCETYPE_ANY;
  NetRes.lpLocalName  = (LPSTR)"";
  NetRes.lpRemoteName	= tmp;
  NetRes.lpProvider   = (LPSTR)"";
  if (WNetAddConnection2(&NetRes,"","",0) == NO_ERROR)ret = TRUE;

  WNetCancelConnection2(tmp,CONNECT_UPDATE_PROFILE,1);
  return ret;
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
BOOL Netbios_Share(wchar_t *server, char *share, unsigned int sz_max)
{
  BOOL ret = FALSE;
  NET_API_STATUS res;
  PSHARE_INFO_1 BufPtr,p;
  DWORD i, er=0,tr=0,resume=0;
  char tmp[MAX_PATH];
  share[0] = 0;

  do
  {
    res = NetShareEnum (server, 1, (LPBYTE *) &BufPtr, -1, &er, &tr, &resume);
    if((res == ERROR_SUCCESS || res == ERROR_MORE_DATA) && BufPtr!=NULL)
    {
      ret = TRUE;
      p   = BufPtr;

      for(i=1;i<=er;i++)
      {
        //snprintf(tmp,MAX_PATH,"%S (%S)\r\n",p->shi1_netname,p->shi1_remark);
        //strncat(share,tmp,sz_max);
        snprintf(share+strlen(share),sz_max-strlen(share),"%S (%S)\r\n",p->shi1_netname,p->shi1_remark);
        p++;
      }
     NetApiBufferFree(BufPtr);
    }
  }while(res==ERROR_MORE_DATA);
  //strncat(share,"\0",sz_max);

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

  if (LookupAccountName((LPSTR)ip,(LPSTR)user, (PSID)Sid, &sz_Sid,(LPSTR)domain, &sz_domain,&peUse))return TRUE;
  else return FALSE;
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
      snprintf(tmp_pUmI_0,MAX_PATH,"min_passwd_len:%d\r\n"
                                   "max_passwd_age:%d(days)\r\n"
                                   "min_passwd_age:%d(days)\r\n"
                                   "passord_history:%d\r\n"
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
      snprintf(tmp_pUmI_3,MAX_PATH,"lockout_duration_before_auto_unlock:%d(s)\r\n"
                                   "wait_time_to_reset_fail_logon_count:%d(s)\r\n"
                                   "bad_password_count_to_lock_account:%d"
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
