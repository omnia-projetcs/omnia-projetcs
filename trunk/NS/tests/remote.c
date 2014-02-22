//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
//get list of files with size and last modifcation date
//take directory with '\\'
void ListAllfiles(char *path, char*results, unsigned int max_size, BOOL resursiv)
{
  char tmp_path[MAX_PATH], tmp[MAX_PATH];
  snprintf(tmp_path,MAX_PATH,"%s*.*",path);
  results[0] = 0;

  FILETIME LocalFileTime;
  SYSTEMTIME SysTimeModification;
  LARGE_INTEGER filesize;
  WIN32_FIND_DATA data;
  HANDLE hfind = FindFirstFile(tmp_path, &data);
  if (hfind != INVALID_HANDLE_VALUE)
  {
    do
    {
      filesize.HighPart = data.nFileSizeHigh;
      filesize.LowPart  = data.nFileSizeLow;

      FileTimeToLocalFileTime(&(data.ftLastWriteTime), &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SysTimeModification);
      snprintf(tmp,MAX_PATH,"%s%s [Last_modification:%02d/%02d/%02d-%02d:%02d:%02d] %ld bytes\n"
                  ,path,data.cFileName
                   ,SysTimeModification.wYear,SysTimeModification.wMonth,SysTimeModification.wDay
                   ,SysTimeModification.wHour,SysTimeModification.wMinute,SysTimeModification.wSecond
                   ,filesize.QuadPart);
      strncat(results+strlen(results),tmp,max_size-strlen(results));
      strncat(results+strlen(results),"\0",max_size-strlen(results));

      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && resursiv)
      {
        snprintf(tmp,MAX_PATH,"%s%s\\",path,data.cFileName);
        ListAllfiles(tmp, results+strlen(results), max_size-strlen(results), resursiv);
      }
    }while(FindNextFile(hfind, &data) != 0);
    CloseHandle(hfind);
  }
}
//----------------------------------------------------------------
//copy a directory or files in a directory with extensions
//take directory with '\\'
void Copyfiles(char *pathsrc, char*pathdst, char *ext, BOOL resursiv)
{
  char tmp_path[MAX_PATH], tmp[MAX_PATH],tmp2[MAX_PATH];
  WIN32_FIND_DATA data;

  if (ext != NULL)
  {
    //all files
    snprintf(tmp_path,MAX_PATH,"%s*.%s",pathsrc,ext);
    HANDLE hfind = FindFirstFile(tmp_path, &data);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      do
      {
        snprintf(tmp,MAX_PATH,"%s%s",pathsrc,data.cFileName);
        snprintf(tmp2,MAX_PATH,"%s%s",pathdst,data.cFileName);
        CopyFile(tmp,tmp2,TRUE);
      }while(FindNextFile(hfind, &data) != 0);
      CloseHandle(hfind);
    }
    //all directories
    snprintf(tmp_path,MAX_PATH,"%s*.*",pathsrc);
    hfind = FindFirstFile(tmp_path, &data);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      do
      {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && resursiv)
        {
          snprintf(tmp,MAX_PATH,"%s%s\\",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s\\",pathdst,data.cFileName);
          CreateDirectory(tmp2,NULL);
          Copyfiles(tmp, tmp2, ext, resursiv);
        }
      }while(FindNextFile(hfind, &data) != 0);
      CloseHandle(hfind);
    }
  }else
  {
    snprintf(tmp_path,MAX_PATH,"%s*.*",pathsrc);
    HANDLE hfind = FindFirstFile(tmp_path, &data);
    if (hfind != INVALID_HANDLE_VALUE)
    {
      do
      {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && resursiv)
        {
          snprintf(tmp,MAX_PATH,"%s%s\\",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s\\",pathdst,data.cFileName);
          CreateDirectory(tmp2,NULL);
          Copyfiles(tmp, tmp2, ext, resursiv);
        }else
        {
          snprintf(tmp,MAX_PATH,"%s%s",pathsrc,data.cFileName);
          snprintf(tmp2,MAX_PATH,"%s%s",pathdst,data.cFileName);
          CopyFile(tmp,tmp2,TRUE);
        }
      }while(FindNextFile(hfind, &data) != 0);
      CloseHandle(hfind);
    }
  }
}
//----------------------------------------------------------------


