//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
  typedef struct
  {
    DWORD v_number;
    DWORD m_number;
    DWORD os_vtype;
    DWORD pf_size;
    char filename[60];
    DWORD file_hash;
    DWORD d_unknow;
    DWORD secA_off;
    DWORD secA_nb_sec;
    DWORD secB_off;
    DWORD secB_nb_sec;
    DWORD secC_off;
    DWORD secC_size;
    DWORD secD_off;
    DWORD secD_nb_sec;
    DWORD secD_size;
    FILETIME last_file_exec;    //usefull
    FILETIME f_unknow1;
    FILETIME f_unknow2;
    DWORD file_exec_counter;    //usefull
    DWORD nb_sections;
  }HDR_PREFETCH;
  #define HDR_PREFETCH_SIZE 0x98
//------------------------------------------------------------------------------
void addPrefetchtoDB(char *file, char *create_time, char *last_update, char*last_access, DWORD count, char *exec, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_prefetch (file,create_time,last_update,last_access,count,exec,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%08lu\",\"%s\",%d);",
           file,create_time,last_update,last_access,count,exec,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
void PfCheck(unsigned int session_id, sqlite3 *db, char *file)
{
  //open file and read first datas
  HANDLE hfile = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (hfile != INVALID_HANDLE_VALUE)
  {
    DWORD sz_prefetch_file = GetFileSize(hfile,NULL);
    if ( sz_prefetch_file!= INVALID_FILE_SIZE && sz_prefetch_file > HDR_PREFETCH_SIZE-5)
    {
      char CreationTime[DATE_SIZE_MAX]="",LastWriteTime[DATE_SIZE_MAX]="",LastAccessTime[DATE_SIZE_MAX]="";
      DWORD count = 0, copiee;
      char exec_time[DATE_SIZE_MAX];
      char buffer[HDR_PREFETCH_SIZE];
      HDR_PREFETCH *pf;

      if (sz_prefetch_file < HDR_PREFETCH_SIZE) ReadFile(hfile, buffer, sz_prefetch_file,&copiee,0);
      else ReadFile(hfile, buffer, HDR_PREFETCH_SIZE,&copiee,0);

      pf = (HDR_PREFETCH*)buffer;

      count = pf->file_exec_counter;
      filetimeToString_GMT(pf->last_file_exec, exec_time, DATE_SIZE_MAX);

      //file times
      BY_HANDLE_FILE_INFORMATION hfi;
      if (GetFileInformationByHandle(hfile,&hfi))
      {
        filetimeToString_GMT(hfi.ftCreationTime, CreationTime, DATE_SIZE_MAX);
        filetimeToString_GMT(hfi.ftLastWriteTime, LastWriteTime, DATE_SIZE_MAX);
        filetimeToString_GMT(hfi.ftLastAccessTime, LastAccessTime, DATE_SIZE_MAX);
      }

      addPrefetchtoDB(file, CreationTime, LastWriteTime, LastAccessTime, count, exec_time, session_id, db);
    }
    CloseHandle(hfile);
  }
}

//------------------------------------------------------------------------------
//format : http://www.forensicswiki.org/wiki/Windows_Prefetch_File_Format
DWORD WINAPI Scan_prefetch(LPVOID lParam)
{
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  //check if local or not :)
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
  if (hitem!=NULL || !LOCAL_SCAN || WINE_OS)
  {
    sqlite3_exec(db,"BEGIN TRANSACTION;", NULL, NULL, NULL);
    char tmp_file_pref[MAX_PATH],ext[MAX_PATH];
    while(hitem!=NULL)
    {
      tmp_file_pref[0] = 0;
      ext[0]           = 0;
      GetTextFromTrv(hitem, tmp_file_pref, MAX_PATH);
      if (!strcmp("pf",extractExtFromFile(charToLowChar(tmp_file_pref), ext, MAX_PATH)))
        PfCheck(session_id, db, tmp_file_pref);

      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }

    sqlite3_exec(db,"END TRANSACTION;", NULL, NULL, NULL);
    h_thread_test[(unsigned int)lParam] = 0;
    check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
    return 0;
  }

  //init
  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);

  //get system path
  char path[MAX_PATH] ="%WINDIR%\\Prefetch\\*.pf";
  ReplaceEnv("WINDIR",path,MAX_PATH);

  char path_f[MAX_PATH];
  HANDLE hfile;

  WIN32_FIND_DATA data;
  HANDLE hfic = FindFirstFile(path, &data);
  if (hfic != INVALID_HANDLE_VALUE)
  {
    char CreationTime[DATE_SIZE_MAX],LastWriteTime[DATE_SIZE_MAX],LastAccessTime[DATE_SIZE_MAX];
    DWORD count = 0, copiee;
    char exec_time[DATE_SIZE_MAX];
    char buffer[HDR_PREFETCH_SIZE];
    HDR_PREFETCH *pf;

    do
    {
      if(data.cFileName[0] == '.' && (data.cFileName[1] == 0 || data.cFileName[1] == '.') || data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){}
      else
      {
        //dates
        filetimeToString_GMT(data.ftCreationTime, CreationTime, DATE_SIZE_MAX);
        filetimeToString_GMT(data.ftLastWriteTime, LastWriteTime, DATE_SIZE_MAX);
        filetimeToString_GMT(data.ftLastAccessTime, LastAccessTime, DATE_SIZE_MAX);

        strncpy(path_f,path,MAX_PATH);
        path_f[strlen(path_f)-4]=0;
        strncat(path_f,data.cFileName,MAX_PATH);
        strncat(path_f,"\0",MAX_PATH);

        count         = 0;
        exec_time[0]  = 0;

        //open file and read first datas
        hfile = CreateFile(path_f,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
        if (hfile != INVALID_HANDLE_VALUE)
        {
          DWORD sz_prefetch_file = GetFileSize(hfile,NULL);
          if ( sz_prefetch_file!= INVALID_FILE_SIZE && sz_prefetch_file > HDR_PREFETCH_SIZE-5)
          {
            if (sz_prefetch_file < HDR_PREFETCH_SIZE) ReadFile(hfile, buffer, sz_prefetch_file,&copiee,0);
            else ReadFile(hfile, buffer, HDR_PREFETCH_SIZE,&copiee,0);

            pf = (HDR_PREFETCH*)buffer;

            count = pf->file_exec_counter;
            filetimeToString_GMT(pf->last_file_exec, exec_time, DATE_SIZE_MAX);
          }
          CloseHandle(hfile);
        }

        //HDR_PREFETCH
        //add :
        //0x0078 	8 	FTIME 	Latest execution time of executable (FILETIME)
        //0x0090 	4 	DWORD 	Execution counter
        addPrefetchtoDB(path_f, CreationTime, LastWriteTime, LastAccessTime, count, exec_time, session_id, db);
      }
    }while(FindNextFile (hfic,&data) && start_scan);
  }

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}
