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
  sqlite3 *dst_file, *src_file;
  if (sqlite3_open(dst, &dst_file) != SQLITE_OK)
  {
    return FALSE;
  }

  if (sqlite3_open(src, &src_file) != SQLITE_OK)
  {
    sqlite3_close(dst_file);
    return FALSE;
  }

  //backup
  BOOL ret = FALSE;
  sqlite3_backup *pBackup = sqlite3_backup_init(dst_file, "main", src_file, "main");
  if (pBackup)
  {
    sqlite3_backup_step(pBackup, -1);
    sqlite3_backup_finish(pBackup);
    ret = TRUE;
  }

  //close
  sqlite3_close(dst_file);
  sqlite3_close(src_file);

  return ret;
}
//------------------------------------------------------------------------------
BOOL SQLITE_LoadSession(char *file)
{
  //backup file first
  SQLITE_SaveSession("RtCA.sqlite.back");

  //BOOL ret = BackupSQLITESession(file, SQLITE_LOCAL_BDD);

  char request[MAX_LINE_SIZE];
  snprintf(request,MAX_LINE_SIZE,"ATTACH '%s' AS toMerge;",file);
  sqlite3_exec(db_scan,request, NULL, NULL, NULL);

  sqlite3_exec(db_scan,"INSERT INTO session SELECT * FROM toMerge.session;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_IE SELECT * FROM toMerge.extract_IE;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_android SELECT * FROM toMerge.extract_android;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_antivirus SELECT * FROM toMerge.extract_antivirus;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_arp SELECT * FROM toMerge.extract_arp;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_chrome SELECT * FROM toMerge.extract_chrome;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_clipboard SELECT * FROM toMerge.extract_clipboard;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_disk SELECT * FROM toMerge.extract_disk;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_env SELECT * FROM toMerge.extract_env;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_file SELECT * FROM toMerge.extract_file;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_firefox SELECT * FROM toMerge.extract_firefox;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_firewall SELECT * FROM toMerge.extract_firewall;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_guide SELECT * FROM toMerge.extract_guide;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_host SELECT * FROM toMerge.extract_host;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_log SELECT * FROM toMerge.extract_log;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_network SELECT * FROM toMerge.extract_network;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_pipe SELECT * FROM toMerge.extract_pipe;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_policy SELECT * FROM toMerge.extract_policy;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_process SELECT * FROM toMerge.extract_process;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_route SELECT * FROM toMerge.extract_route;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_share SELECT * FROM toMerge.extract_share;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_tache SELECT * FROM toMerge.extract_tache;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_account_password SELECT * FROM toMerge.extract_registry_account_password;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_mru SELECT * FROM toMerge.extract_registry_mru;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_shell_bags SELECT * FROM toMerge.extract_registry_shell_bags;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_path SELECT * FROM toMerge.extract_registry_path;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_service_driver SELECT * FROM toMerge.extract_registry_service_driver;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_software SELECT * FROM toMerge.extract_registry_software;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_settings SELECT * FROM toMerge.extract_registry_settings;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_start SELECT * FROM toMerge.extract_registry_start;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_update SELECT * FROM toMerge.extract_registry_update;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_usb SELECT * FROM toMerge.extract_registry_usb;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_user SELECT * FROM toMerge.extract_registry_user;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_userassist SELECT * FROM toMerge.extract_registry_userassist;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_prefetch SELECT * FROM toMerge.extract_prefetch;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_registry_deleted_keys SELECT * FROM toMerge.extract_registry_deleted_keys;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_file_nk SELECT * FROM toMerge.extract_file_nk;", NULL, NULL, NULL);
  sqlite3_exec(db_scan,"INSERT INTO extract_ldap SELECT * FROM toMerge.extract_ldap;", NULL, NULL, NULL);

  sqlite3_exec(db_scan,"DETACH DATABASE toMerge;", NULL, NULL, NULL);

  //recharger la liste des sessions
  FORMAT_CALBAK_READ_INFO fcri;
  fcri.type = TYPE_SQLITE_FLAG_SESSIONS_INIT;
  SQLITE_LireData(&fcri, SQLITE_LOCAL_BDD);

  SendMessage(hCombo_session, CB_SETCURSEL,SendMessage(hCombo_session, CB_GETCOUNT,0,0)-1,0);
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL SQLITE_SaveSession(char *file)
{
  BOOL ret = BackupSQLITESession(SQLITE_LOCAL_BDD, file);
  return ret;
}
