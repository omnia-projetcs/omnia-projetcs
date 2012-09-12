//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addRegistryUserassisttoDB(char *file, char *hk, char *key, char*raw_type,DWORD type_id,
                             char *path, char*use_count, char *session_number, char *time, char *last_use,
                             char *user, char *RID, char *SID, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_registry_userassist (file,hk,key,raw_type,type_id,path,use_count,user,RID,SID,session_number,time,last_use,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",%lu,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           file,hk,key,raw_type,type_id,path,use_count,user,RID,SID,session_number,time,last_use,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//---------------------------------------------------------------------------------------------------------------
void ROTTOASCII(char *donnee,char *resultat,int size)
{
 int i;
 resultat[0]=0;

 for (i=0;i<size;i++)
 {
    switch(donnee[i])
    {
     case 'a': strcat(resultat,"n"); break;
     case 'b': strcat(resultat,"o"); break;
     case 'c': strcat(resultat,"p"); break;
     case 'd': strcat(resultat,"q"); break;
     case 'e': strcat(resultat,"r"); break;
     case 'f': strcat(resultat,"s"); break;
     case 'g': strcat(resultat,"t"); break;
     case 'h': strcat(resultat,"u"); break;
     case 'i': strcat(resultat,"v"); break;
     case 'j': strcat(resultat,"w"); break;
     case 'k': strcat(resultat,"x"); break;
     case 'l': strcat(resultat,"y"); break;
     case 'm': strcat(resultat,"z"); break;
     case 'n': strcat(resultat,"a"); break;
     case 'o': strcat(resultat,"b"); break;
     case 'p': strcat(resultat,"c"); break;
     case 'q': strcat(resultat,"d"); break;
     case 'r': strcat(resultat,"e"); break;
     case 's': strcat(resultat,"f"); break;
     case 't': strcat(resultat,"g"); break;
     case 'u': strcat(resultat,"h"); break;
     case 'v': strcat(resultat,"i"); break;
     case 'w': strcat(resultat,"j"); break;
     case 'x': strcat(resultat,"k"); break;
     case 'y': strcat(resultat,"l"); break;
     case 'z': strcat(resultat,"m"); break;
     case 'A': strcat(resultat,"N"); break;
     case 'B': strcat(resultat,"O"); break;
     case 'C': strcat(resultat,"P"); break;
     case 'D': strcat(resultat,"Q"); break;
     case 'E': strcat(resultat,"R"); break;
     case 'F': strcat(resultat,"S"); break;
     case 'G': strcat(resultat,"T"); break;
     case 'H': strcat(resultat,"U"); break;
     case 'I': strcat(resultat,"V"); break;
     case 'J': strcat(resultat,"W"); break;
     case 'K': strcat(resultat,"X"); break;
     case 'L': strcat(resultat,"Y"); break;
     case 'M': strcat(resultat,"Z"); break;
     case 'N': strcat(resultat,"A"); break;
     case 'O': strcat(resultat,"B"); break;
     case 'P': strcat(resultat,"C"); break;
     case 'Q': strcat(resultat,"D"); break;
     case 'R': strcat(resultat,"E"); break;
     case 'S': strcat(resultat,"F"); break;
     case 'T': strcat(resultat,"G"); break;
     case 'U': strcat(resultat,"H"); break;
     case 'V': strcat(resultat,"I"); break;
     case 'W': strcat(resultat,"J"); break;
     case 'X': strcat(resultat,"K"); break;
     case 'Y': strcat(resultat,"L"); break;
     case 'Z': strcat(resultat,"M"); break;
     case '\\': strcat(resultat,"\\"); break;
     case ' ': strcat(resultat," "); break;
     case ':': strcat(resultat,":"); break;
     case '+': strcat(resultat,"+"); break;
     case '-': strcat(resultat,"-"); break;
     case '_': strcat(resultat,"_"); break;
     case '.': strcat(resultat,"."); break;
     case ']': strcat(resultat,"]"); break;
     case '[': strcat(resultat,"["); break;
     case '(': strcat(resultat,"("); break;
     case ')': strcat(resultat,")"); break;
     case '#': strcat(resultat,"#"); break;
     case '0': strcat(resultat,"0"); break;
     case '1': strcat(resultat,"1"); break;
     case '2': strcat(resultat,"2"); break;
     case '3': strcat(resultat,"3"); break;
     case '4': strcat(resultat,"4"); break;
     case '5': strcat(resultat,"5"); break;
     case '6': strcat(resultat,"6"); break;
     case '7': strcat(resultat,"7"); break;
     case '8': strcat(resultat,"8"); break;
     case '9': strcat(resultat,"9"); break;
     case 'é': strcat(resultat,"é"); break;
     case 'à': strcat(resultat,"à"); break;
     case 'è': strcat(resultat,"è"); break;
     case '~': strcat(resultat,"~"); break;
     case '&': strcat(resultat,"&"); break;
     case '"': strcat(resultat,"\""); break;
     case '|': strcat(resultat,"|"); break;
     case 'ç': strcat(resultat,"ç"); break;
     case '@': strcat(resultat,"@"); break;
     case '$': strcat(resultat,"$"); break;
     case '{': strcat(resultat,"{"); break;
     case '}': strcat(resultat,"}"); break;
     case '<': strcat(resultat,"<"); break;
     case '>': strcat(resultat,">"); break;
     case '°': strcat(resultat,"°"); break;
     case '%': strcat(resultat,"%"); break;
     case 'ù': strcat(resultat,"ù"); break;
     case '*': strcat(resultat,"*"); break;
     case 'µ': strcat(resultat,"µ"); break;
     case '§': strcat(resultat,"§"); break;
     case '^': strcat(resultat,"^"); break;
     case 'â': strcat(resultat,"â"); break;
     case 'ê': strcat(resultat,"ê"); break;
     case 'î': strcat(resultat,"î"); break;
     case 'ë': strcat(resultat,"ë"); break;
     case 'ï': strcat(resultat,"ï"); break;
     case '²': strcat(resultat,"²"); break;
     case '\'': strcat(resultat,"'"); break;
     case '\0': strcat(resultat,"\0");
     return;
     };
  };
}
//------------------------------------------------------------------------------
DWORD ReadUserassistDatas(char *value, DWORD value_size, char *data, DWORD data_size, char*raw_type, DWORD raw_type_size,
                          char *path, DWORD path_size, char*use_count, DWORD use_count_size,
                          char *session_number, DWORD session_number_size, char *time_use, DWORD time_use_size,char *last_use, DWORD last_use_size)
{
  //convert value
  char tmp[value_size+1];
  ROTTOASCII(value,tmp,value_size);
  strncpy(raw_type,tmp,raw_type_size);

  char *c = raw_type;
  while (*c && *c!=':')c++;
  if (*c == ':')
  {
    *c = 0;
    c++;
    strncpy(path,c,path_size);
  }

  DWORD type_id = 0;
       if (strcmp(raw_type,"UEME_RUNPIDL") == 0)    type_id = 310;
  else if (strcmp(raw_type,"UEME_RUNPATH") == 0)    type_id = 311;
  else if (strcmp(raw_type,"UEME_CTLSESSION") == 0) type_id = 312;
  else if (strcmp(raw_type,"UEME_CTLCUACount") == 0)type_id = 313;
  else if (strcmp(raw_type,"UEME_UISCUT") == 0)     type_id = 314;
  else if (strcmp(raw_type,"UEME_UIQCUT") == 0)     type_id = 315;
  else if (strcmp(raw_type,"UEME_UIHOTKEY") == 0)   type_id = 316;
  else if (strcmp(raw_type,"UEME_RUNWMCMD") == 0)   type_id = 317;
  else if (strcmp(raw_type,"UEME_RUNCPL") == 0)     type_id = 318;
  else if (strcmp(raw_type,"UEME_UITOOLBAR") == 0)
  {
    type_id = 319;
    strncpy(tmp,path,value_size);

          if (strcmp(path,"0x1120") == 0)snprintf(path,value_size,"%s (Back)",path);
    else  if (strcmp(path,"0x1121") == 0)snprintf(path,value_size,"%s (Forward)",path);
    else  if (strcmp(path,"0x1123") == 0)snprintf(path,value_size,"%s (Search)",path);
    else  if (strcmp(path,"0x1130") == 0)snprintf(path,value_size,"%s (Up)",path);
    else  if (strcmp(path,"0x1133") == 0)snprintf(path,value_size,"%s (Folders)",path);
    else  if (strcmp(path,"0x47031") == 0)snprintf(path,value_size,"%s (Views)",path);
    else  if (strcmp(path,"0x17011") == 0 || strcmp(path,"0x47011") == 0)snprintf(path,value_size,"%s (Delete)",path);
    else  if (strcmp(path,"0x1701b") == 0 || strcmp(path,"0x4701b") == 0)snprintf(path,value_size,"%s (Undo)",path);
    else  if (strcmp(path,"0x1701e") == 0 || strcmp(path,"0x4701e") == 0)snprintf(path,value_size,"%s (Copy To)",path);
    else  if (strcmp(path,"0x1701f") == 0 || strcmp(path,"0x4701f") == 0)snprintf(path,value_size,"%s (Move To)",path);
  }else type_id = 215;

  //get DATAS
  typedef struct
  {
    unsigned int  session_number;
    unsigned int  use_count;
    FILETIME      LastWriteTime;
  }USERASSIST_DATAS_FORMAT;
  USERASSIST_DATAS_FORMAT *s = (USERASSIST_DATAS_FORMAT *)data;

  typedef struct
  {
    unsigned int  unknow_0;
    unsigned int  use_count;
    unsigned int  focus_count;
    unsigned int  focus_total_time; //milliseconds
    BYTE          unknow_1[44];
    FILETIME      LastWriteTime;
  }USERASSIST_DATAS_FORMAT2;
  USERASSIST_DATAS_FORMAT2 *s2 = (USERASSIST_DATAS_FORMAT2 *)data;

  if (data_size == 8) //NT 4.x, 95, 98...
  {
    snprintf(session_number,session_number_size ,"%08d",s->session_number & 0xFFFFFFFF);
    snprintf(use_count,use_count_size           ,"%08d",s->use_count & 0xFFFFFFFF);
  }else if (data_size == 16) //2000,ME,XP
  {
    if ((s->use_count & 0xFFFFFFFF) > 5)
      snprintf(use_count,use_count_size           ,"%08d",(s->use_count & 0xFFFFFFFF)-5);
    else snprintf(use_count,use_count_size           ,"%08d",s->use_count & 0xFFFFFFFF);

    snprintf(session_number,session_number_size ,"%08d",s->session_number & 0xFFFFFFFF);

    if (s->LastWriteTime.dwLowDateTime != 0 || s->LastWriteTime.dwHighDateTime != 0)
      filetimeToString_GMT(s->LastWriteTime, last_use, last_use_size);
  }else if (data_size >= 68) //Vista,7,2008...
  {
    snprintf(use_count,use_count_size           ,"%08d",s2->use_count & 0xFFFFFFFF);

    if (s2->focus_total_time != 0xFFFFFFFF)
    {
      snprintf(time_use,time_use_size           ,"%d %02d:%02d:%02d.%d"
               ,(s2->focus_total_time)/(24*60*60*1000)
               ,((s2->focus_total_time)%(24*60*60*1000))/(60*60*1000)
               ,(((s2->focus_total_time)%(24*60*60*1000))%(60*60*1000))/(60*1000)
               ,((((s2->focus_total_time)%(24*60*60*1000))%(60*60*1000))%(60*1000))/1000
               ,(((((s2->focus_total_time)%(24*60*60*1000))%(60*60*1000))%(60*1000))%1000));
    }

    if (s2->LastWriteTime.dwLowDateTime != 0 || s2->LastWriteTime.dwHighDateTime != 0)
      filetimeToString_GMT(s2->LastWriteTime, last_use, last_use_size);
  }

  //case UEME_CTLSESSION
  if (type_id == 312)
  {
    strncpy(session_number,use_count,session_number_size);
    use_count[0] = 0;
  }

  return type_id;
}
//------------------------------------------------------------------------------
//local function part !!!
//------------------------------------------------------------------------------
void resgistry_userassist_local(unsigned int session_id, sqlite3 *db)
{
  HKEY CleTmp,CleTmp2,CleTmp3;
  if (RegOpenKey((HKEY)HKEY_USERS,"",&CleTmp)!=ERROR_SUCCESS)return;

  char key[MAX_PATH];
  DWORD nbSubKey=0,nbSubKey2,nbSubValue, i,j,k, key_size,key_size2, value_size,data_size,type;
  if (RegQueryInfoKey (CleTmp,NULL,NULL,NULL,&nbSubKey,NULL,NULL,NULL,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)
  {
    RegCloseKey(CleTmp);
    return;
  }

  //datas
  char user[MAX_PATH],RID[MAX_PATH],SID[MAX_PATH],raw_type[MAX_PATH],path[MAX_PATH],
  use_count[MAX_PATH],session_number[MAX_PATH],last_use[DATE_SIZE_MAX],time[MAX_PATH];
  DWORD type_id;
  char value[MAX_PATH], data[MAX_PATH];

  //get all key
  char key_path[MAX_PATH],key_path2[MAX_PATH];
  for (i=0;i<nbSubKey && start_scan;i++)
  {
    key_size  = MAX_PATH;
    SID[0]    = 0;
    RID[0]    = 0;
    user[0]   = 0;
    time[0]   = 0;

    if (RegEnumKeyEx (CleTmp,i,SID,(LPDWORD)&key_size,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
    {
      //only for reals users
      if (key_size < 10)continue;

      //next
      snprintf(key_path,MAX_PATH,"%s\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist",SID);
      if (RegOpenKey((HKEY)HKEY_USERS,key_path,&CleTmp2)!=ERROR_SUCCESS)continue;

      //get user
      GetRegistryKeyOwner(CleTmp, user, RID, SID, MAX_PATH);

      nbSubKey2 = 0;
      if (RegQueryInfoKey (CleTmp2,NULL,NULL,NULL,&nbSubKey2,NULL,NULL,NULL,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)
      {
        RegCloseKey(CleTmp2);
        continue;
      }
      for (j=0;j<nbSubKey2 && start_scan;j++)
      {
        key_size2 = MAX_PATH;
        key[0]    = 0;
        if (RegEnumKeyEx (CleTmp2,j,key,(LPDWORD)&key_size2,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
        {
          //get values and datas
          snprintf(key_path2,MAX_PATH,"%s\\%s\\Count",key_path,key);

          if (RegOpenKey((HKEY)HKEY_USERS,key_path2,&CleTmp3)!=ERROR_SUCCESS)continue;

          nbSubValue = 0;
          if (RegQueryInfoKey (CleTmp3,NULL,NULL,NULL,NULL,NULL,NULL,&nbSubValue,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)
          {
            RegCloseKey(CleTmp3);
            continue;
          }

          for (k=0;k<nbSubValue && start_scan;k++)
          {
            value_size  = MAX_PATH;
            data_size   = MAX_PATH;
            value[0]    = 0;
            data[0]     = 0;
            type        = 0;

            if (RegEnumValue (CleTmp3,k,value,&value_size,NULL,&type,(LPBYTE)data,&data_size)==ERROR_SUCCESS)
            {
              raw_type[0]       = 0;
              path[0]           = 0;
              use_count[0]      = 0;
              session_number[0] = 0;
              last_use[0]       = 0;
              type_id           = 0;
              type_id = ReadUserassistDatas(value, value_size, data, data_size, raw_type,MAX_PATH,
                                            path,MAX_PATH, use_count,MAX_PATH,
                                            session_number,MAX_PATH, time, MAX_PATH, last_use,DATE_SIZE_MAX);
              if (type_id != 0)
              {
                convertStringToSQL(path, MAX_PATH);
                addRegistryUserassisttoDB("", "HKEY_USERS", key_path2, raw_type,type_id,
                                          path, use_count, session_number, time, last_use,
                                          user, RID, SID, session_id, db);
              }
            }
          }
          RegCloseKey(CleTmp3);
        }
      }
      RegCloseKey(CleTmp2);
    }
  }
  RegCloseKey(CleTmp);
}
//------------------------------------------------------------------------------
//file registry part
//------------------------------------------------------------------------------
void resgistry_userassist_file(HK_F_OPEN *hks, char *ckey, unsigned int session_id, sqlite3 *db)
{
  //exist or not in the file ?
  HBIN_CELL_NK_HEADER *nk_h = GetRegistryNK(hks->buffer, hks->taille_fic, (hks->pos_fhbin)+HBIN_HEADER_SIZE, hks->position, ckey);
  if (nk_h == NULL)return;

  char RID[MAX_PATH],SID[MAX_PATH],raw_type[MAX_PATH],path[MAX_PATH],
  use_count[MAX_PATH],session_number[MAX_PATH],last_use[DATE_SIZE_MAX],time[MAX_PATH];
  DWORD type_id;
  char value[MAX_PATH], data[MAX_LINE_SIZE];
  char tmp_key[MAX_PATH], tmp_key2[MAX_PATH], key_path[MAX_PATH];

  HBIN_CELL_NK_HEADER *nk_h_tmp, *nk_h_tmp2;
  DWORD data_size,i,j,k, nbSubValue,nbSubKey2,nbSubKey = GetSubNK(hks->buffer, hks->taille_fic, nk_h, hks->position, 0, NULL, 0);
  for (i=0;i<nbSubKey && start_scan;i++)
  {
    //for each subkey
    if(GetSubNK(hks->buffer, hks->taille_fic, nk_h, hks->position, i, tmp_key, MAX_PATH))
    {
      //get nk of key :)
      nk_h_tmp = GetSubNKtonk(hks->buffer, hks->taille_fic, nk_h, hks->position, i);
      if (nk_h_tmp == NULL)continue;

      nbSubKey2 = GetSubNK(hks->buffer, hks->taille_fic, nk_h_tmp, hks->position, 0, NULL, 0);
      for (j=0;j<nbSubKey2 && start_scan;j++)
      {
        //for each subkey
        if(GetSubNK(hks->buffer, hks->taille_fic, nk_h_tmp, hks->position, j, tmp_key2, MAX_PATH))
        {
          //get nk of key :)
          nk_h_tmp2 = GetSubNKtonk(hks->buffer, hks->taille_fic, nk_h_tmp, hks->position, j);
          if (nk_h_tmp2 == NULL)continue;

          //get all values !!!!
          snprintf(key_path,MAX_PATH,"%s\\%s\\%s",ckey,tmp_key,tmp_key2);
          Readnk_Infos(hks->buffer, hks->taille_fic, (hks->pos_fhbin)+HBIN_HEADER_SIZE, hks->position, NULL, nk_h_tmp2,
                       NULL, 0, RID, MAX_PATH, SID, MAX_PATH);

          nbSubValue = GetValueData(hks->buffer,hks->taille_fic, nk_h_tmp2, (hks->pos_fhbin)+HBIN_HEADER_SIZE, 0, NULL, 0, NULL, 0);
          for (k=0;k<nbSubValue && start_scan;k++)
          {
            data_size = MAX_LINE_SIZE;
            if (GetBinaryValueData(hks->buffer,hks->taille_fic, nk_h_tmp2, (hks->pos_fhbin)+HBIN_HEADER_SIZE, k,value,MAX_PATH,data, &data_size))
            {
              raw_type[0]       = 0;
              path[0]           = 0;
              use_count[0]      = 0;
              session_number[0] = 0;
              last_use[0]       = 0;
              type_id           = 0;
              type_id = ReadUserassistDatas(value, strlen(value), data, data_size, raw_type,MAX_PATH,
                                            path,MAX_PATH, use_count,MAX_PATH,
                                            session_number,MAX_PATH, time, MAX_PATH, last_use,DATE_SIZE_MAX);
              if (type_id != 0)
              {
                convertStringToSQL(path, MAX_PATH);
                addRegistryUserassisttoDB(hks->file, "", key_path, raw_type,type_id,
                                          path, use_count, session_number, time, last_use,
                                          "", RID, SID, session_id, db);
              }
            }
          }
        }
      }
    }
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DWORD WINAPI Scan_registry_userassist(LPVOID lParam)
{
  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  char file[MAX_PATH];
  HK_F_OPEN hks;

  //files or local
  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
  if (hitem!=NULL || !LOCAL_SCAN) //files
  {
    while(hitem!=NULL && start_scan)
    {
      file[0] = 0;
      GetTextFromTrv(hitem, file, MAX_PATH);
      if (file[0] != 0)
      {

        //open file + verify
        if(OpenRegFiletoMem(&hks, file))
        {
          resgistry_userassist_file(&hks,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist",session_id,db);

          CloseRegFiletoMem(&hks);
        }
      }
      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }
  }else resgistry_userassist_local(session_id,db);

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}
