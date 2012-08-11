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
  SQLITE_SaveSession("RtCA.sqlite.back");

  BOOL ret = BackupSQLITESession(file, DEFAULT_SQLITE_FILE);

  //recharger la liste des sessions
  FORMAT_CALBAK_READ_INFO fcri;
  fcri.type = TYPE_SQLITE_FLAG_SESSIONS_INIT;
  SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);

  SendMessage(hCombo_session, CB_SETCURSEL,SendMessage(hCombo_session, CB_GETCOUNT,0,0)-1,0);
  return ret;
}
//------------------------------------------------------------------------------
BOOL SQLITE_SaveSession(char *file)
{
  BOOL ret = BackupSQLITESession(DEFAULT_SQLITE_FILE, file);
  return ret;
}
