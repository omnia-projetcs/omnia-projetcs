//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DWORD WINAPI Scan_configuration(LPVOID lParam)
{
  //on vide les listeview
  BOOL local        = (BOOL)lParam;
  HANDLE hlv        = GetDlgItem(Tabl[TABL_CONFIGURATION],LV_VIEW);
  ListView_DeleteAllItems(hlv);
  char tmp[MAX_PATH];

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_CONFIGURATION,TRUE);

  /*a coder*/
/*
Applications a prendre en compte :
  - IIS
  - Apache
  - Sharepoint
  - firefox
  - thunderbird
  - lotusnotes
  - outlook
  - outlook express
  - Tomcat
  - Citrix
  - Wifi
  - Opera
  - Safari
  - IE
  - MySQL
  - MSSQL
  - Oracle
  - LDAP
  - OpenVPN
  - AD
  - Bind/DNS
  - Bluetooth
  - exchange
  - fichiers temporaire
*/

  h_scan_configuration = NULL;
  if (!h_scan_logs && !h_scan_files && !h_scan_registry && !h_scan_configuration)
  {
    ScanStart = FALSE;
    SetWindowText(GetDlgItem(Tabl[TABL_CONF],BT_CONF_START),"Start");
    SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_INFO, (LPARAM)"");
  }

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_CONFIGURATION,FALSE);
  return 0;
}
