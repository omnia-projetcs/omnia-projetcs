//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
void DeleteKey(HKEY hkey, char *parent_path, char* SubKey)
{
  BOOL ok = FALSE;
  HINSTANCE hDLL;

  //environnement 64bits !!!
  if((hDLL = LoadLibrary("ADVAPI32.DLL" ))!= NULL)
  {
    typedef LONG (WINAPI * P_RegOpenKeyExA)(HKEY hKey , LPCTSTR lpSubKey , DWORD ulOptions, REGSAM samDesired , PHKEY phkResult);
    typedef LONG (WINAPI * P_RegDeleteKeyExA)(HKEY hKey , LPCTSTR lpSubKey , REGSAM samDesired , DWORD Reserved);

    P_RegOpenKeyExA RegOpenKeyEx      = (P_RegOpenKeyExA)   GetProcAddress(hDLL , "RegOpenKeyExA");
    P_RegDeleteKeyExA RegDeleteKeyEx  = (P_RegDeleteKeyExA) GetProcAddress(hDLL , "RegDeleteKeyExA");
    if (RegOpenKeyEx && RegDeleteKeyEx)
    {
      //delete subkey on 32b and 64b systèmes
      // KEY_WOW64_32KEY = 0x0200
      // KEY_WOW64_64KEY = 0x0100

      // on ouvre la cle
      HKEY CleTmp=0;
      if (RegOpenKeyEx(hkey,parent_path,0,KEY_ALL_ACCESS,&CleTmp)==ERROR_SUCCESS)
      {
        if (RegDeleteKeyEx(CleTmp,SubKey,0x0300,0) == ERROR_SUCCESS) ok = TRUE;

        //On ferme la cle
        RegCloseKey(CleTmp);
      }
    }
    FreeLibrary(hDLL);
  }


  if(!ok) //environnement 32bits !!!
  {
    // on ouvre la cle
    HKEY CleTmp=0;
    if (RegOpenKey(hkey,parent_path,&CleTmp)!=ERROR_SUCCESS)
       return;

    RegDeleteKey(CleTmp,SubKey);

    //On ferme la cle
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
void DeleteSubKey(HKEY hkey, char *parent_path)
{
  // on ouvre la cle
  HKEY CleTmp=0;
  if (RegOpenKey(hkey,parent_path,&CleTmp)!=ERROR_SUCCESS)
    return;

  //nombre de subkey
  DWORD i,nbSubKey=0,TailleNomSubKey;
  if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
  {
    char SubKey[MAX_PATH];
    for (i=0;i<nbSubKey;i++)
    {
      TailleNomSubKey = MAX_PATH;
      if (RegEnumKeyEx (CleTmp,i,SubKey,(LPDWORD)&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
      {
        DeleteKey(hkey, parent_path, SubKey);
      }
    }
  }

  //On ferme la cle
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
void clean_registry()
{
  if (MessageBox(0,"All registry history on local computer will be delete!\nAre you sure ?","Clean registry",MB_YESNO|MB_TOPMOST)==IDYES)
  {
    //Userassist
    DeleteKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\{5E6AB780-7743-11CF-A12B-00AA004AE837}\\", "Count");
    DeleteKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\{75048700-EF1F-11D0-9888-006097DEACF9}\\", "Count");

    //USB can oblige to reinstall drivers for materials
    DeleteSubKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR\\");

    //MRU
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\", "RunMRU");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\", "RecentDocs");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\", "StreamMRU");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\", "ComputerDescriptions");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\", "Map Network Drive MRU");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\", "LastVisitedMRU");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\", "OpenSaveMRU");
    DeleteSubKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Search Assistant\\ACMru\\");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Internet Explorer\\", "TypedURLs");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\MediaPlayer\\Player\\", "RecentFileList");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\MediaPlayer\\Player\\", "RecentURLList");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\Paint\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\Wordpad\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\9.0\\Publisher\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\10.0\\Publisher\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\11.0\\Publisher\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\9.0\\PowerPoint\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\10.0\\PowerPoint\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\11.0\\PowerPoint\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\12.0\\PowerPoint\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\13.0\\PowerPoint\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\14.0\\PowerPoint\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\9.0\\Excel\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\10.0\\Excel\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\11.0\\Excel\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\12.0\\Excel\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\13.0\\Excel\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\14.0\\Excel\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\9.0\\Word\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\10.0\\Word\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\11.0\\Word\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\12.0\\Word\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\13.0\\Word\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\14.0\\Word\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\9.0\\FrontPage Explorer\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\10.0\\FrontPage Explorer\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Office\\11.0\\FrontPage Explorer\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\MiTeC\\HexEditor\\1.x\\wnd_hex_Main\\", "Twnd_hex_Main.MRUMan");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\MiTeC\\HexEditor\\2.x\\wnd_hex_Main\\", "Twnd_hex_Main.MRUMan");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\MiTeC\\HexEditor\\3.x\\wnd_hex_Main\\", "Twnd_hex_Main.MRUMan");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\MiTeC\\HexEditor\\4.x\\wnd_hex_Main\\", "Twnd_hex_Main.MRUMan");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\MiTeC\\HexEditor\\5.x\\wnd_hex_Main\\", "Twnd_hex_Main.MRUMan");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\MiTeC\\WRR\\1.x\\Main\\", "MRUHistory");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\MiTeC\\WRR\\2.x\\Main\\", "MRUHistory");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Foxit Software\\Foxit Reader 1.0\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Foxit Software\\Foxit Reader 2.0\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Foxit Software\\Foxit Reader 3.0\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Foxit Software\\Foxit Reader 4.0\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Foxit Software\\Foxit Reader 5.0\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Foxit Software\\Foxit Reader 6.0\\", "Recent File List");
    DeleteKey(HKEY_CURRENT_USER,"SOFTWARE\\Morpheus\\GUI\\", "SearchRecent");

    //MUI
    DeleteKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\ShellNoRoam\\", "MUICache");
    DeleteKey(HKEY_CURRENT_USER,"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\", "MUICache");
    DeleteKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\Shell\\", "LocalizedResourceName");
  }
}
//------------------------------------------------------------------------------
void CleanEventLog(char *eventname)
{
  HANDLE Heventlog = OpenEventLog(NULL,eventname);
  if (Heventlog!=NULL)
  {
    ClearEventLog(Heventlog,NULL);
    CloseEventLog(Heventlog);
  }
}
//------------------------------------------------------------------------------
void clean_logs()
{
  if (MessageBox(0,"All audits log entry on local computer will be delete!\nAre you sure ?","Clean audits log",MB_YESNO|MB_TOPMOST)==IDYES)
  {
    BOOL ok = FALSE;
    //énumération de tous les journaux existants
    // Dans : HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Eventlog
    char eventname[MAX_PATH];
    HKEY CleTmp=0;
    DWORD nbSubKey=0,i=0, TailleNomSubKey;
    if (RegOpenKey((HKEY)HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Eventlog",&CleTmp)==ERROR_SUCCESS)
    {
      if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
      {
        if (nbSubKey > 0)
        {
          for (i=0;i<nbSubKey;i++)
          {
            TailleNomSubKey = MAX_PATH;
            if (RegEnumKeyEx (CleTmp,i,eventname,(LPDWORD)&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
            {
              ok=TRUE;
              CleanEventLog(eventname);
            }
          }
        }
      }
      RegCloseKey(CleTmp);
    }

    if (!ok)
    {
      CleanEventLog("Application");       //journal application
      CleanEventLog("Security");          //journal sécurité
      CleanEventLog("System");            //journal système
      CleanEventLog("Internet Explorer"); //Internet Explorer
      CleanEventLog("OSession");          //session Office
    }
  }
}
