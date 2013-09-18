//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "resources.h"
//----------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/aa394558%28v=vs.85%29.aspx
BOOL GetWMI(char *ip)
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

  //connexion à la base
  if(FAILED(pLoc->ConnectServer(
          BSTR(L"\\\\COMPUTERNAME\\ROOT\\CIMV2"),   //emplacement           : \\myserver\root\default
          BSTR(L"DOMAIN\\USER"),                  //login actuel          : domain\user
          BSTR(L"MDP"),                             //mots de passe actuel  : mdp
          NULL,        //langue
          0,                      //securityFlags (par défaut)
          0,                      //authorité (NTLM par défaiut)
          0,                      //context
          &pSvc)))
  {
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return FALSE;
  }


  return TRUE;
}
