//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
BOOL BackupSQLITESession(char *src, char *dst)
{
  sqlite3 *dst_file;
  if (sqlite3_open(dst, &dst_file) != SQLITE_OK)
  {
    return FALSE;
  }

  //backup
  BOOL ret = FALSE;
  sqlite3_backup *pBackup = sqlite3_backup_init(dst_file, "main", db_scan, "main");
  if (pBackup)
  {
    sqlite3_backup_step(pBackup, -1);
    sqlite3_backup_finish(pBackup);
    ret = TRUE;
  }

  //close
  sqlite3_close(dst_file);

  return ret;
}
//------------------------------------------------------------------------------
BOOL SQLITE_LoadSession(char *file)
{
  //backup file first
  if(!SQLITE_SaveSession("RtCA.sqlite.back"))AddDebugMessage("load_save", "Fail to save \"RtCA.sqlite.back\" file before load data", "NOK", MSG_ERROR);
  else AddDebugMessage("load_save", "File bakup to \"RtCA.sqlite.back\" before load data", "OK", MSG_INFO);

  BOOL ret = BackupSQLITESession(file, DEFAULT_SQLITE_FILE);
  AddDebugMessage("load_save", "Merge file to database", "OK", MSG_INFO);

  //recharger la liste des sessions
  FORMAT_CALBAK_READ_INFO fcri;
  fcri.type = TYPE_SQLITE_FLAG_SESSIONS_INIT;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
  AddDebugMessage("load_save", "Reload of all sessions", "OK", MSG_INFO);

  HANDLE htmp = GetDlgItem(h_conf,CB_SESSION);
  SendMessage(htmp, CB_SETCURSEL,SendMessage(htmp, CB_GETCOUNT,0,0)-1,0);
  return ret;
}
//------------------------------------------------------------------------------
BOOL SQLITE_SaveSession(char *file)
{
  BOOL ret = BackupSQLITESession(DEFAULT_SQLITE_FILE, file);
  AddDebugMessage("load_save", "Backup all database to file", "OK", MSG_INFO);
  return ret;
}
