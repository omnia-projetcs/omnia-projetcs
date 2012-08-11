//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addRegistryUsertoDB(char *name, char *RID, char *SID, char *group,
                         char *description, char *last_logon, char *last_password_change,
                         DWORD nb_connexion, char *type, DWORD state_id, DWORD session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_registry_user (name,RID,SID,grp,description,last_logon,last_password_change,nb_connexion,type,state_id,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%lu,\"%s\",%lu,%lu);",
           name,RID,SID,group,description,last_logon,last_password_change,nb_connexion,type,state_id,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
//local function part !!!
//------------------------------------------------------------------------------
void GetSIDFromUser(char *user, char* rid, char *sid, unsigned int max_size)
{
  BYTE BSid[MAX_PATH];
  PSID psid       = (PSID)BSid;
  DWORD sid_size  = MAX_PATH;
  SID_NAME_USE TypeSid;

  char domain[MAX_PATH];
  DWORD domain_size = MAX_PATH;

  if(LookupAccountName(NULL,(LPCTSTR)user,psid,(LPDWORD)&sid_size,(LPTSTR)domain,(LPDWORD)&domain_size,(PSID_NAME_USE)&TypeSid))
  {
    if (IsValidSid(psid))SidtoUser(psid, user, rid, sid, max_size);
  }
}
//------------------------------------------------------------------------------
void Scan_registry_user_local(sqlite3 *db, unsigned int session_id)
{
  if (registry_users_extract())return;

  //load all users informations
  HMODULE hDLL;
  typedef DWORD (WINAPI *NETAPIBUFFERFREE)(LPVOID Buffer);
  NETAPIBUFFERFREE NetApiBufferFree;
  typedef DWORD (WINAPI *NETUSERENUM)(LPCWSTR servername, DWORD level, DWORD filter, LPBYTE* bufptr, DWORD prefmaxlen, LPDWORD entriesread, LPDWORD totalentries, LPDWORD resume_handle);
  NETUSERENUM NetUserEnum;
  typedef DWORD (WINAPI *NETUSERGETINFO)( LPCWSTR servername, LPCWSTR username, DWORD level, LPBYTE* bufptr);
  NETUSERGETINFO NetUserGetInfo;
  typedef DWORD (WINAPI *NETUSERGETLOCALGROUPS)( LPCWSTR servername, LPCWSTR username, DWORD level, DWORD flags, LPBYTE* bufptr, DWORD prefmaxlen, LPDWORD entriesread, LPDWORD totalentries);
  NETUSERGETLOCALGROUPS NetUserGetLocalGroups;

  typedef struct _USER_INFO_0 {
    LPWSTR usri0_name;
  }*LPUSER_INFO_0;

  typedef struct _USER_INFO_2 {
    LPWSTR usri2_name;
    LPWSTR usri2_password;
    DWORD  usri2_password_age;
    DWORD  usri2_priv;
    LPWSTR usri2_home_dir;
    LPWSTR usri2_comment;
    DWORD  usri2_flags;
    LPWSTR usri2_script_path;
    DWORD  usri2_auth_flags;
    LPWSTR usri2_full_name;
    LPWSTR usri2_usr_comment;
    LPWSTR usri2_parms;
    LPWSTR usri2_workstations;
    DWORD  usri2_last_logon;
    DWORD  usri2_last_logoff;
    DWORD  usri2_acct_expires;
    DWORD  usri2_max_storage;
    DWORD  usri2_units_per_week;
    PBYTE  usri2_logon_hours;
    DWORD  usri2_bad_pw_count;
    DWORD  usri2_num_logons;
    LPWSTR usri2_logon_server;
    DWORD  usri2_country_code;
    DWORD  usri2_code_page;
  } USER_INFO_2, *PUSER_INFO_2, *LPUSER_INFO_2;

  typedef struct _GROUP_USERS_INFO_0 {
    LPWSTR grui0_name;
  } GROUP_USERS_INFO_0, *PGROUP_USERS_INFO_0, *LPGROUP_USERS_INFO_0;

  //load function
  if ((hDLL = LoadLibrary( "NETAPI32.dll"))!=NULL)
  {
    NetApiBufferFree = (NETAPIBUFFERFREE) GetProcAddress(hDLL,"NetApiBufferFree");
    NetUserEnum = (NETUSERENUM) GetProcAddress(hDLL,"NetUserEnum");
    NetUserGetInfo = (NETUSERGETINFO) GetProcAddress(hDLL,"NetUserGetInfo");
    NetUserGetLocalGroups = (NETUSERGETLOCALGROUPS) GetProcAddress(hDLL,"NetUserGetLocalGroups");

    if (NetApiBufferFree && NetUserEnum && NetUserGetInfo && NetUserGetLocalGroups)
    {
      //enumerate all accounts
      DWORD nStatus, i, nb, total;
      LPUSER_INFO_0 pBuf, Buffer;
      USER_INFO_2 * pBuf_info;

      char name[MAX_PATH],RID[MAX_PATH],SID[MAX_PATH],group[MAX_PATH],type[MAX_PATH],
      description[MAX_PATH],last_logon[DATE_SIZE_MAX],last_password_change[DATE_SIZE_MAX];
      DWORD nb_connexion,state_id;

      LPGROUP_USERS_INFO_0 LBuffer,pTmpBuf;
      DWORD dwEntriesRead, dwTotalEntries, j, s;

      do
      {
        nStatus = NetUserEnum(0,0,2,(LPBYTE*)&pBuf,-1,&nb,&total,0);
        if (((nStatus == 0) || (nStatus == 234)) && (Buffer = pBuf) != 0)
        {
          for (i = 0; i < nb; i++)
          {
            if (Buffer->usri0_name!=0)
            {
              //init
              name[0]                 = 0;
              RID[0]                  = 0;
              SID[0]                  = 0;
              group[0]                = 0;
              description[0]          = 0;
              last_logon[0]           = 0;
              last_password_change[0] = 0;
              nb_connexion            = 0;
              state_id                = 0;
              type[0]                 = 0;

              if (NetUserGetInfo(0,Buffer->usri0_name,2,(LPBYTE *)&pBuf_info) == 0)
              {
                //name
                snprintf(name,MAX_PATH,"%S",Buffer->usri0_name);

                //get account infos ^^ SID + RID
                GetSIDFromUser(name, RID, SID, MAX_PATH);

                //group
                if (NetUserGetLocalGroups(0,Buffer->usri0_name,0,1,(LPBYTE*)&LBuffer,-1,&dwEntriesRead,&dwTotalEntries) == 0)
                {
                  if ((pTmpBuf = LBuffer) != 0)
                  {
                    for (j = 0; j < dwEntriesRead; j++)
                    {
                      if(pTmpBuf != 0)
                      {
                        s = strlen(group);
                        if (s>=MAX_PATH)break;

                        snprintf(group+s,MAX_PATH-s,"%S,",pTmpBuf->grui0_name);
                      }
                      pTmpBuf++;
                    }
                  }
                }

                //description
                snprintf(description,MAX_PATH,"(%S) %S %S",pBuf_info->usri2_full_name,pBuf_info->usri2_usr_comment,pBuf_info->usri2_comment);

                //state
                if (pBuf_info->usri2_flags&0x2)state_id = 300;//Disable
                else state_id = 301;//Enable
                if (pBuf_info->usri2_flags&0x10)state_id+=2;//Lock

                //last_logon
                if (pBuf_info->usri2_last_logon != 0)
                {
                  timeToString(pBuf_info->usri2_last_logon, last_logon, DATE_SIZE_MAX);
                }

                //last_password_change
                if (pBuf_info->usri2_password_age != 0)
                {
                  snprintf(last_password_change,DATE_SIZE_MAX,"%lu:%lu:%lu",
                           pBuf_info->usri2_password_age/86400,
                           pBuf_info->usri2_password_age%86400/60/60,
                           pBuf_info->usri2_password_age%86400/60%60);
                }

                //nb_connexion			wprintf(L"\n\nPassword last set:		%dday %dhour %dmin Before",
                nb_connexion = pBuf_info->usri2_num_logons;

                //type
                switch(pBuf_info->usri2_priv)
                {
                  case 0:snprintf(type,MAX_PATH,"%lu : Guest",pBuf_info->usri2_priv);break;
                  case 1:snprintf(type,MAX_PATH,"%lu : User",pBuf_info->usri2_priv);break;
                  case 2:snprintf(type,MAX_PATH,"%lu : Administrator",pBuf_info->usri2_priv);break;
                  default:snprintf(type,MAX_PATH,"%lu : Unknow",pBuf_info->usri2_priv);break;
                }

                addRegistryUsertoDB(name, RID, SID, group, description,
                                    last_logon, last_password_change,
                                    nb_connexion, type, state_id, session_id, db);
              }
              Buffer++;
            }else break;
          }
        }
      }while (nStatus == ERROR_MORE_DATA && start_scan);
    }
    FreeLibrary(hDLL);
  }
}
//------------------------------------------------------------------------------
//file registry part
//------------------------------------------------------------------------------
void Scan_registry_user_file(HK_F_OPEN *hks, sqlite3 *db, unsigned int session_id)
{

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DWORD WINAPI Scan_registry_user(LPVOID lParam)
{
  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  char file[MAX_PATH];
  HK_F_OPEN hks;

  //files or local
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
  if (hitem!=NULL) //files
  {
    while(hitem!=NULL)
    {
      file[0] = 0;
      GetTextFromTrv(hitem, file, MAX_PATH);
      if (file[0] != 0)
      {
        //open file + verify
        if(OpenRegFiletoMem(&hks, file))
        {
          Scan_registry_user_file(&hks, db, session_id);

          CloseRegFiletoMem(&hks);
        }
      }
      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }
  }else Scan_registry_user_local(db, session_id);

  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  return 0;
}
