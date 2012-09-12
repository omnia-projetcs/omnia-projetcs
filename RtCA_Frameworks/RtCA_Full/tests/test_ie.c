//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addIEdtoDB(char *file, char *parameter, char *data, char *date, DWORD id_language_description, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_IE (file,parameter,data,date,id_language_description,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%lu\",%d);",
           file,parameter,data,date,id_language_description,session_id);

  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
void ReadDATFile(char *file, DWORD id_description, unsigned int session_id, sqlite3 *db)
{
  //open file
  HANDLE Hfic = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0/*FILE_FLAG_SEQUENTIAL_SCAN*/,0);
  if (Hfic == INVALID_HANDLE_VALUE)return;

  DWORD taille_fic = GetFileSize(Hfic,NULL);
  //0x5068 = 0x5000 = start + 0x68 = 104 = min size
  if (taille_fic<0x5068 || taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(Hfic);
    return;
  }

  //alloc
  char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
  if (!buffer)
  {
    CloseHandle(Hfic);
    return;
  }

  //load file
  DWORD copiee, position = 0, increm = 0;
  if (taille_fic > DIXM)increm = DIXM;
  else increm = taille_fic;

  while (position<taille_fic && increm!=0)
  {
    copiee = 0;
    ReadFile(Hfic, buffer+position, increm,&copiee,0);
    position +=copiee;
    if (taille_fic-position < increm)increm = taille_fic-position ;
  }
  CloseHandle(Hfic);

  if (position>0 && taille_fic > 0x5068)
  {
    //DAT FILE or not ?
    if (buffer[0]!='C' || buffer[7]!='U' || buffer[10]!='C')
    {
      HeapFree(GetProcessHeap(), 0, buffer);
      return;
    }

    typedef struct      //URL + LEAK format
    {
      char type[4];     //URL, REDR, LEAK
      char size;        //size*128 = size of datas
      char padding[3];
      FILETIME time_create;
      FILETIME time_last_access;
      char unknow1[60];
      unsigned short count;
      char unknow2[18];
      char url[1];      //URL +2car = file + 2 car = header
    }URL_INDEX_DAT;
    URL_INDEX_DAT *url_dat;

    char parameter[5];
    char data[MAX_PATH];
    char date[DATE_SIZE_MAX];

    position = 0x5000;
    char *c = buffer+position;

    do
    {
      url_dat = (URL_INDEX_DAT*)c;

      if ((url_dat->type[0] == 'U' && url_dat->type[1] == 'R' && url_dat->type[2] == 'L') ||
          (url_dat->type[0] == 'L' && url_dat->type[1] == 'E' && url_dat->type[2] == 'A' && url_dat->type[3] == 'K'))
      {
        snprintf(parameter,5,"%s",url_dat->type);  //Parameter
        parameter[4]=0;
        snprintf(data,MAX_PATH,"%s",url_dat->url);        //Data

        //date
        if (url_dat->time_last_access.dwLowDateTime != 0 || url_dat->time_last_access.dwHighDateTime != 0)
        {
          filetimeToString_GMT(url_dat->time_last_access, date, DATE_SIZE_MAX);
        }else if (url_dat->time_create.dwLowDateTime != 0 || url_dat->time_create.dwHighDateTime != 0)
        {
          filetimeToString_GMT(url_dat->time_create, date, DATE_SIZE_MAX);
        }else date[0] = 0;

        //add
        convertStringToSQL(parameter, 5);
        convertStringToSQL(data, MAX_PATH);
        addIEdtoDB(file,parameter,data,date, id_description, session_id, db);

        //next
        position+= (url_dat->size * 128);
        c = c + (url_dat->size * 128);
      }else if (url_dat->type[0] == 'R' && url_dat->type[1] == 'E' && url_dat->type[2] == 'D' && url_dat->type[3] == 'R')
      {
        snprintf(parameter,5,"%s",url_dat->type);  //Parameter
        parameter[4]=0;
        snprintf(data,MAX_PATH,"%s",c+16);                //Data
        date[0] = 0;

        convertStringToSQL(parameter, 5);
        convertStringToSQL(data, MAX_PATH);
        addIEdtoDB(file,parameter,data,date, id_description, session_id, db);

        //next
        position+= (url_dat->size * 128);
        c = c + (url_dat->size * 128);
      }else {c++;position++;}
    }while(position<taille_fic && start_scan);
  }

  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_ie_history(LPVOID lParam)
{
  sqlite3 *db = (sqlite3 *)db_scan;

  char tmp_file[MAX_PATH];
  unsigned int session_id = current_session_id;

  //get child
  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
  if (hitem == NULL && LOCAL_SCAN) //local
  {
    //get path of all profils users
    //HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ProfileList
    HKEY CleTmp   = 0;
    if (RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\",&CleTmp)==ERROR_SUCCESS)
    {
      DWORD i, nbSubKey=0, key_size;
      char tmp_key[MAX_PATH], tmp_key_path[MAX_PATH];
      if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
      {
        //get subkey
        for(i=0;i<nbSubKey;i++)
        {
          key_size    = MAX_PATH;
          tmp_key[0]  = 0;
          if (RegEnumKeyEx (CleTmp,i,tmp_key,&key_size,0,0,0,0)==ERROR_SUCCESS)
          {
            //generate the key path
            snprintf(tmp_key_path,MAX_PATH,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\%s\\",tmp_key);
            //get profil path
            if (ReadValue(HKEY_LOCAL_MACHINE,tmp_key_path,"ProfileImagePath",tmp_key, MAX_PATH))
            {
              //verify the path if %systemdrive%
              ReplaceEnv("SYSTEMDRIVE",tmp_key,MAX_PATH);

              //cookies
              snprintf(tmp_key_path,MAX_PATH,"%s\\Cookies\\index.dat",tmp_key);
              ReadDATFile(tmp_key_path, 3, session_id, db);

              //search other files cache
              WIN32_FIND_DATA wfd0;
              snprintf(tmp_key_path,MAX_PATH,"%s\\Local Settings\\Historique\\*.*",tmp_key);
              HANDLE hfic = FindFirstFile(tmp_key_path, &wfd0);
              if (hfic != INVALID_HANDLE_VALUE)
              {
                char tmp_path[MAX_PATH],tmp_path2[MAX_PATH];
                do
                {
                  if (wfd0.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                  {
                    if(wfd0.cFileName[0] == '.' && (wfd0.cFileName[1] == 0 || wfd0.cFileName[1] == '.'))continue;

                    snprintf(tmp_path,MAX_PATH,"%s\\Local Settings\\Historique\\%s\\index.dat",tmp_key,wfd0.cFileName);
                    ReadDATFile(tmp_path, 15, session_id, db);
                    //get file and tests it
                    WIN32_FIND_DATA wfd1;
                    HANDLE hfic2 = FindFirstFile(tmp_path, &wfd1);
                    if (hfic2 == INVALID_HANDLE_VALUE)continue;
                    do
                    {
                      if (wfd1.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                      {
                        if(wfd1.cFileName[0] == '.' && (wfd1.cFileName[1] == 0 || wfd1.cFileName[1] == '.'))continue;

                        snprintf(tmp_path2,MAX_PATH,"%s\\Local Settings\\Historique\\%s\\%s\\index.dat",tmp_key,wfd0.cFileName,wfd1.cFileName);
                        ReadDATFile(tmp_path2, 15, session_id, db);
                      }
                    }while(FindNextFile (hfic,&wfd1) && start_scan);
                  }
                }while(FindNextFile (hfic,&wfd0));
              }
            }
          }
        }
      }
      RegCloseKey(CleTmp);
    }
  }else
  {
    while(hitem!=NULL && start_scan)
    {
      //get item txt
      GetTextFromTrv(hitem, tmp_file, MAX_PATH);
      ReadDATFile(tmp_file, 15, session_id, db);

      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }
  }

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}
