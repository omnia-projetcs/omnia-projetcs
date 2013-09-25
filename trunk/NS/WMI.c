//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "resources.h"
//----------------------------------------------------------------

//----------------------------------------------------------------
BOOL mLSBExist(DWORD lsb, char *st)
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
//------------------------------------------------------------------------------
void mAddLSTVUpdateItem(char *add, DWORD column, DWORD iitem)
{
  HWND hlstv  = GetDlgItem(h_main,LV_results);
  char buffer[MAX_LINE_SIZE] = "";
  ListView_GetItemText(hlstv,iitem,column,buffer,MAX_LINE_SIZE);
  if (buffer[0] != 0)
  {
    strncat(buffer,"\r\n",MAX_LINE_SIZE);
    strncat(buffer,add,MAX_LINE_SIZE);
    strncat(buffer,"\0",MAX_LINE_SIZE);
  }else
  {
    strncpy(buffer,add,MAX_LINE_SIZE);
  }
  ListView_SetItemText(hlstv,iitem,column,buffer);
}
//----------------------------------------------------------------
void mAddMsg(HWND hwnd, char *type, char *txt, char *info)
{
  char msg[MAX_PATH],date[DATE_SIZE];
  time_t dateEtHMs;
  time(&dateEtHMs);
  DWORD copiee = 0;

  snprintf(date,20,"%s",(char *)ctime(&dateEtHMs));
  if (info != NULL) snprintf(msg,MAX_PATH,"[%s] %s - %s %s",date+11,type,txt,info);
  else snprintf(msg,MAX_PATH,"[%s] %s - %s",date+11,type,txt);
  SendDlgItemMessage(hwnd,CB_infos,LB_INSERTSTRING,(WPARAM)-1,(LPARAM)msg);
  if (h_log != INVALID_HANDLE_VALUE)
  {
    strncat(msg,"\r\n\0",MAX_PATH);
    WriteFile(h_log,msg,strlen(msg),&copiee,0);
  }
}
//----------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/aa394558%28v=vs.85%29.aspx
BOOL GetWMITests(DWORD iitem, char *ip, SCANNE_ST config)
{
  //init
  //objet de connexion WMI
  IWbemLocator *pLoc = NULL;
  HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hres))return FALSE;

  if(FAILED(CoInitializeSecurity( NULL,-1,NULL,NULL,
     RPC_C_AUTHN_LEVEL_DEFAULT,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE,NULL)))
  {
    CoUninitialize();
    return FALSE;
  };

  //init objet
  if(FAILED(CoCreateInstance(CLSID_WbemLocator,NULL,CLSCTX_INPROC_SERVER,
     IID_IWbemLocator, (LPVOID *) &pLoc)))
  {
    pLoc->Release();
    CoUninitialize();
    return FALSE;
  };

  //connexion
  IWbemServices *pSvc = NULL;
  char tmp[MAX_LINE_SIZE];

  //login
  WCHAR user[MAX_LINE_SIZE];
  if (config.domain[0] != 0)
  {
    snprintf(tmp,MAX_LINE_SIZE,"%s\\%s",config.domain,config.login);
    MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)tmp,(int)-1,(LPWSTR)user,(int)MAX_LINE_SIZE);
  }else MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.login,(int)-1,(LPWSTR)user,(int)MAX_LINE_SIZE);

  //password
  WCHAR wpassword[MAX_LINE_SIZE];
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.mdp,(int)-1,(LPWSTR)wpassword,(int)MAX_LINE_SIZE);

  //connexion
  WCHAR connexion[MAX_LINE_SIZE];
  snprintf(tmp,MAX_LINE_SIZE,"\\\\%s\\ROOT\\CIMV2",ip);
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)tmp,(int)-1,(LPWSTR)connexion,(int)MAX_LINE_SIZE);

  //connexion à la base
  if(FAILED(pLoc->ConnectServer(
          BSTR(connexion),      //emplacement           : \\myserver\root\default
          BSTR(user),           //login actuel          : domain\user
          BSTR(wpassword),       //mots de passe actuel  : mdp
          NULL,                 //langue
          0,                    //securityFlags (par défaut)
          0,                    //authorité (NTLM par défaiut)
          0,                    //context
          &pSvc)))
  {
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return FALSE;
  }

  //authenticiation + impersonate lvl
  typedef struct _COAUTHIDENTITY
  {
    USHORT *User;
    ULONG  UserLength;
    USHORT *Domain;
    ULONG  DomainLength;
    USHORT *Password;
    ULONG  PasswordLength;
    ULONG  Flags;
  } COAUTHIDENTITY;

  COAUTHIDENTITY *userAcct  =  NULL ;
  COAUTHIDENTITY authIdent;

  memset(&authIdent, 0, sizeof(COAUTHIDENTITY));
  authIdent.PasswordLength  = wcslen(wpassword);
  authIdent.Password        = (USHORT*)wpassword;

  WCHAR wuser[MAX_LINE_SIZE], wdomain[MAX_LINE_SIZE];
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.login,(int)-1,(LPWSTR)wuser,(int)MAX_LINE_SIZE);
  MultiByteToWideChar((UINT)CP_ACP,(DWORD)MB_PRECOMPOSED,(LPSTR)config.domain,(int)-1,(LPWSTR)wdomain,(int)MAX_LINE_SIZE);
  authIdent.UserLength      = wcslen(wuser);
  authIdent.User            = (USHORT*)wuser;
  authIdent.DomainLength    = wcslen(wdomain);
  authIdent.Domain          = (USHORT*)wdomain;
  authIdent.Flags           = SEC_WINNT_AUTH_IDENTITY_UNICODE;
  userAcct                  = &authIdent;

  if(FAILED(CoSetProxyBlanket(
     pSvc,                         // the proxy to set
     RPC_C_AUTHN_WINNT,            // authentication service
     RPC_C_AUTHZ_NONE,             // authorization service
     COLE_DEFAULT_PRINCIPAL,       // Server principal name
     RPC_C_AUTHN_LEVEL_PKT_PRIVACY,// authentication level
     RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
     userAcct,                         // client identity
     EOAC_NONE                     // proxy capabilities
  )))
  {
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return FALSE;
  }

  //get datas !!!
  IEnumWbemClassObject* pEnumerator = NULL;

  //services
  if (config.check_services)
  {
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_Service"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      IWbemClassObject *pclsObj;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
        if (mLSBExist(CB_T_SERVICES, ctmp))
        {
          pclsObj->Get(L"PathName", 0, &vtProp2, 0, 0);
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\PathName=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (Service)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
          VariantClear(&vtProp2);
        }else
        {
          pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
          snprintf(ctmp2,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
          if (mLSBExist(CB_T_SERVICES, ctmp2))
          {
            pclsObj->Get(L"PathName", 0, &vtProp2, 0, 0);
            snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\PathName=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
            mAddMsg(h_main,(char*)"FOUND (Service)",msg,ctmp2);
            mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
            VariantClear(&vtProp2);
          }
        }
        VariantClear(&vtProp);

        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        if(uReturn == 0)break;
      }
      pEnumerator->Release();
    }
  }

  //software
  if (config.check_software)
  {
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_Product"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      IWbemClassObject *pclsObj;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
        if (mLSBExist(CB_T_SOFTWARE, ctmp))
        {
          pclsObj->Get(L"InstallLocation", 0, &vtProp2, 0, 0);
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\InstallLocation=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (Software)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
          VariantClear(&vtProp2);
        }else
        {
          pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
          snprintf(ctmp2,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
          if (mLSBExist(CB_T_SOFTWARE, ctmp2))
          {
            pclsObj->Get(L"InstallLocation", 0, &vtProp2, 0, 0);
            snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\InstallLocation=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
            mAddMsg(h_main,(char*)"FOUND (Software)",msg,ctmp2);
            mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
            VariantClear(&vtProp2);
          }
        }
        VariantClear(&vtProp);

        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        if(uReturn == 0)break;
      }
      pEnumerator->Release();
    }

    //updates
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_QuickFixEngineering"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      IWbemClassObject *pclsObj;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        pclsObj->Get(L"Description", 0, &vtProp, 0, 0);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
        if (mLSBExist(CB_T_SOFTWARE, ctmp))
        {
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S",ip,vtProp.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (Software)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
        }
        VariantClear(&vtProp);

        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        if(uReturn == 0)break;
      }
      pEnumerator->Release();
    }
  }

  //USB
  if (config.check_USB)
  {
    if (!FAILED(pSvc->ExecQuery(
        BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_USBController"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator)))
    {
      IWbemClassObject *pclsObj;
      ULONG uReturn = 0;
      VARIANT vtProp,vtProp2;
      char ctmp[MAX_LINE_SIZE],ctmp2[MAX_LINE_SIZE],msg[MAX_LINE_SIZE];
      while (pEnumerator)
      {
        pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
        snprintf(ctmp,MAX_LINE_SIZE,"%S",vtProp.bstrVal);
        if (mLSBExist(CB_T_USB, ctmp))
        {
          pclsObj->Get(L"DeviceID", 0, &vtProp2, 0, 0);
          snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\DeviceID=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
          mAddMsg(h_main,(char*)"FOUND (USB)",msg,ctmp);
          mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
          VariantClear(&vtProp2);
        }else
        {
          pclsObj->Get(L"DeviceID", 0, &vtProp2, 0, 0);
          snprintf(ctmp2,MAX_LINE_SIZE,"%S",vtProp2.bstrVal);
          if (mLSBExist(CB_T_USB, ctmp2))
          {
            snprintf(msg,MAX_LINE_SIZE,"%s\\WMI\\%S\\DeviceID=%S",ip,vtProp.bstrVal,vtProp2.bstrVal);
            mAddMsg(h_main,(char*)"FOUND (USB)",msg,ctmp2);
            mAddLSTVUpdateItem(msg, COL_SERVICE, iitem);
            VariantClear(&vtProp2);
          }
        }
        VariantClear(&vtProp);

        pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
        if(uReturn == 0)break;
      }
      pEnumerator->Release();
    }
  }

  //clean
  pSvc->Release();
  pLoc->Release();
  CoUninitialize();
  return TRUE;
}
