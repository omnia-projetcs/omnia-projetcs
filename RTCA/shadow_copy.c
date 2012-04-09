//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//http://technet.microsoft.com/en-us/library/cc754968.aspx
//http://msdn.microsoft.com/en-us/library/windows/desktop/aa384644%28v=vs.85%29.aspx
//http://pauldotcom.com/2011/11/safely-dumping-hashes-from-liv.html
void ShadowCopyFileTo(char *path_src, char *dst)
{
  //démarrer le service de shadow copy
  ShellExecute(Tabl[TABL_MAIN], "open","net","start VSS",NULL,SW_HIDE);

  //effectuer un cliché
  char tmp[MAX_PATH]="create shadow /for=c:";
  tmp[19]=path_src[0];
  ShellExecute(Tabl[TABL_MAIN], "open","vssadmin",tmp,NULL,SW_HIDE);

  //accéder aux données de la shadow copy


  //énumérer les cliché et sélectioner le dernier
  //vssadmin list shadows ou VSSADMIN LIST WRITERS

  //effectuer une copie !!!
  /*
copy \\?\GLOBALROOT\Device\HarddiskVolumeShadowCopy[X]\windows\ntds\ntds.dit .
copy \\?\GLOBALROOT\Device\HarddiskVolumeShadowCopy[X]\windows\system32\config\SYSTEM .
copy \\?\GLOBALROOT\Device\HarddiskVolumeShadowCopy[X]\windows\system32\config\SAM .
  */

  //supprimer les shadow copy
  snprintf(tmp,MAX_PATH,"delete shadows /for=%c: /all",path_src[0]);
  ShellExecute(Tabl[TABL_MAIN], "open","vssadmin",tmp,NULL,SW_HIDE);

  //stopper le service
  ShellExecute(Tabl[TABL_MAIN], "open","net","stop VSS",NULL,SW_HIDE);
}
