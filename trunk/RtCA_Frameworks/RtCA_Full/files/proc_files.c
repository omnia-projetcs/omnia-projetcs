//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void AddItemFiletoTreeView(HANDLE htv, char *lowcase_file, char *path, char *global_path)
{
  //get extension
  char ext[MAX_PATH], tmp_path[MAX_PATH];
  if (extractExtFromFile(lowcase_file, ext, MAX_PATH))
  {
    if (strcmp(ext,"log")==0 ||
        strcmp(ext,"evt")==0 ||
        strcmp(ext,"evtx")==0) //logs
    {
      if (global_path != NULL)
        AddItemTreeView(htv,global_path, TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
      else
      {
        snprintf(tmp_path,MAX_PATH,"%s%s",path,lowcase_file);
        AddItemTreeView(htv,tmp_path, TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
      }
      check_treeview(htrv_test, H_tests[TEST_LOGS], TRV_STATE_CHECK);
    }else if (strcmp(ext,"reg")==0 ||
              strcmp(lowcase_file,"security.dat")==0 ||
              strcmp(lowcase_file,"ntuser.dat")==0   || (startWith(lowcase_file,"ntuser") && strcmp(ext,"dat")==0)    ||
              strcmp(lowcase_file,"usrclass.dat")==0 || (startWith(lowcase_file,"usrclass.") && strcmp(ext,"dat")==0) ||
              strcmp(lowcase_file,"classes.dat")==0  || (startWith(lowcase_file,"classes") && strcmp(ext,"dat")==0)) //registry
    {
      if (global_path != NULL)AddItemTreeView(htv,global_path, TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
      else
      {
        snprintf(tmp_path,MAX_PATH,"%s%s",path,lowcase_file);
        AddItemTreeView(htv,tmp_path, TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
      }

      check_treeview(htrv_test, H_tests[TEST_REG_NETWORK], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_REG_ANTIVIRUS], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_REG_FIREWALL], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_ENV], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_SHARE], TRV_STATE_CHECK);

      unsigned int i;
      for (i = TEST_REG_START;i<=TEST_REG_END;i++)
      {
        check_treeview(htrv_test, H_tests[i], TRV_STATE_CHECK);
      }

    }else if (strcmp(ext,"db")==0 ||              //android
              strcmp(ext,"sqlite")==0 ||          //firefox
              strcmp(ext,"dat")==0 ||             //ie
              strcmp(lowcase_file,"index.dat")==0 || (startWith(lowcase_file,"index") && strcmp(ext,"dat")==0) ||
              strcmp(lowcase_file,"ntds.dit")==0  || (startWith(lowcase_file,"ntds") && strcmp(ext,"dit")==0)) //applications
    {
      if (global_path != NULL)AddItemTreeView(htv,global_path, TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      else
      {
        snprintf(tmp_path,MAX_PATH,"%s%s",path,lowcase_file);
        AddItemTreeView(htv,tmp_path, TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      }
      check_treeview(htrv_test, H_tests[TEST_ANDROID], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_CHROME], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_FIREFOX], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_IE], TRV_STATE_CHECK);
    }
  }else
  {
    if (strcmp(lowcase_file,"sam")==0 ||
        strcmp(lowcase_file,"software")==0 ||
        strcmp(lowcase_file,"system")==0 ||
        strcmp(lowcase_file,"default")==0 ||
        strcmp(lowcase_file,"hardware")==0 ||
        strcmp(lowcase_file,"security")==0)//registry
    {
      if (global_path != NULL)AddItemTreeView(htv,global_path, TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
      else
      {
        snprintf(tmp_path,MAX_PATH,"%s%s",path,lowcase_file);
        AddItemTreeView(htv,tmp_path, TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
      }

      check_treeview(htrv_test, H_tests[TEST_REG_NETWORK], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_REG_ANTIVIRUS], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_REG_FIREWALL], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_ENV], TRV_STATE_CHECK);
      check_treeview(htrv_test, H_tests[TEST_SHARE], TRV_STATE_CHECK);

      unsigned int i;
      for (i = TEST_REG_START;i<=TEST_REG_END;i++)
      {
        check_treeview(htrv_test, H_tests[i], TRV_STATE_CHECK);
      }

    }else if (strcmp(lowcase_file,"archived history")==0 ||  //chrome
              strcmp(lowcase_file,"history")==0 ||
              strcmp(lowcase_file,"cookies")==0 ||
              strcmp(lowcase_file,"default")==0 ||
              strcmp(lowcase_file,"login data")==0 ||
              strcmp(lowcase_file,"web data")==0)
    {
      if (global_path != NULL)AddItemTreeView(htv,global_path, TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      else
      {
        snprintf(tmp_path,MAX_PATH,"%s%s",path,lowcase_file);
        AddItemTreeView(htv,tmp_path, TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      }
      check_treeview(htrv_test, H_tests[TEST_CHROME], TRV_STATE_CHECK);
    }
  }
}
//------------------------------------------------------------------------------
void scan_file(char *path, HANDLE htv)
{
  WIN32_FIND_DATA data;
  char tmp_path[MAX_PATH], file[MAX_PATH];

  snprintf(tmp_path,MAX_PATH,"%s*.*",path);
  HANDLE hfic = FindFirstFile(tmp_path, &data);
  if (hfic == INVALID_HANDLE_VALUE)return;
  do
  {
    // return
    if(data.cFileName[0] == '.' && (data.cFileName[1] == 0 || data.cFileName[1] == '.')){}
    else if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      snprintf(tmp_path,MAX_PATH,"%s%s\\",path,data.cFileName);
      scan_file(tmp_path, htv);
    }else //file
    {
      strncpy(file,data.cFileName,MAX_PATH);
      AddItemFiletoTreeView(htv, charToLowChar(file), path, NULL);
    }
  }while(FindNextFile (hfic,&data));
}
//------------------------------------------------------------------------------
DWORD  WINAPI AutoSearchFiles(LPVOID lParam)
{
  //list all
  char tmp[MAX_PATH];
  int i,nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);

  //search
  for (i=0;i<nblecteurs;i+=4)
  {
    switch(GetDriveType(&tmp[i]))
    {
      case DRIVE_FIXED:
      case DRIVE_REMOTE:
      case DRIVE_RAMDISK:
      case DRIVE_REMOVABLE:
        AddItemTreeView(htrv_files,&tmp[i], TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
        scan_file(&tmp[i], htrv_files);
      break;
    }
  }

  //tri and clean
  CleanTreeViewFiles(htrv_files);
  B_AUTOSEARCH = FALSE;
  return 0;
}
