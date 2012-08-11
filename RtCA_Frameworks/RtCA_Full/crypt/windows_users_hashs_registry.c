//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
//local function part !!!
//------------------------------------------------------------------------------
/*
Extrait en partie de Pwdump 1 de Jeremy Allison.
Pour modifier les droits et accéder aux données des utilisateurs dans le regitre.
*/
//------------------------------------------------------------------------------
//lecture du SID à partir d'un nom
BOOL get_sid(const char *name, SID **ppsid)
{
  SID_NAME_USE sid_use;
  DWORD sid_size = 0;
  DWORD dom_size = 0;
  char *domain;

  *ppsid = 0;
  if(LookupAccountName(0, name, 0, &sid_size, 0, &dom_size, &sid_use) == 0) {
    if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)return FALSE;
  }

  *ppsid = (SID *)LocalAlloc( LMEM_FIXED, sid_size);
  domain = (char *)LocalAlloc( LMEM_FIXED, dom_size);
  if( *ppsid == 0 || domain == 0)
  {
    if(*ppsid)
      LocalFree((HLOCAL)*ppsid);
    if(domain)
      LocalFree((HLOCAL)domain);
    *ppsid = 0;
    return FALSE;
  }

  if(LookupAccountName(0, name, *ppsid, &sid_size, domain, &dom_size, &sid_use) == 0)
  {
    LocalFree((HLOCAL)*ppsid);
    LocalFree((HLOCAL)domain);
    *ppsid = 0;
    return FALSE;
  }

  LocalFree((HLOCAL)domain);
  return TRUE;
}
//------------------------------------------------------------------------------
//génération de profil DACL pour modifier les droits !!
BOOL create_sd_from_list( SECURITY_DESCRIPTOR *sdout, int num, ...)
{
  va_list ap;
  SID **sids = 0;
  char *name;
  DWORD amask;
  DWORD acl_size;
  PACL pacl = 0;
  int i;
  if((sids = (SID **)calloc(1,sizeof(SID *)*num)) == 0)return FALSE;

  acl_size = num * (sizeof(ACL) +
             sizeof(ACCESS_ALLOWED_ACE) +
             sizeof(DWORD));

  /* Collect all the SID's */
  va_start( ap, num);
  for( i = 0; i < num; i++) {
    name = va_arg( ap, char *);
    amask = va_arg(ap, DWORD);
    if(get_sid( name, &sids[i]) == FALSE)goto cleanup;

    acl_size += GetLengthSid(sids[i]);
  }
  va_end(ap);

  if((pacl = (PACL)LocalAlloc( LMEM_FIXED, acl_size)) == 0)goto cleanup;
  if(InitializeSecurityDescriptor( sdout, SECURITY_DESCRIPTOR_REVISION) == FALSE)goto cleanup;
  if(InitializeAcl( pacl, acl_size, ACL_REVISION) == FALSE)goto cleanup;

  va_start(ap, num);
  for( i = 0; i < num; i++) {
    ACE_HEADER *ace_p;
    name = va_arg( ap, char *);
    amask = va_arg( ap, DWORD);
    if(AddAccessAllowedAce( pacl, ACL_REVISION, amask, sids[i]) == FALSE)goto cleanup;

    /* Make sure the ACE is inheritable */
    if(GetAce( pacl, 0, (LPVOID *)&ace_p) == FALSE)goto cleanup;

    ace_p->AceFlags |= ( CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE);
  }

  /* Add the ACL into the sd. */
  if(SetSecurityDescriptorDacl( sdout, TRUE, pacl, FALSE) == FALSE)goto cleanup;

  for( i = 0; i < num; i++)
    if(sids[i] != 0)
      LocalFree((HLOCAL)sids[i]);
  free(sids);

  return TRUE;

cleanup:

  if(sids != 0) {
    for( i = 0; i < num; i++)
      if(sids[i] != 0)
        LocalFree((HLOCAL)sids[i]);
    free(sids);
  }
  if(pacl != 0)
    LocalFree((HLOCAL)pacl);
  return FALSE;
}
//------------------------------------------------------------------------------
int set_userkeys_security( HKEY start, const char *path, SECURITY_DESCRIPTOR *psd,HKEY *return_key)
{
	HKEY key;
	DWORD err;
	char usersid[128];
	DWORD indx = 0;

	/* Open the path and enum all the user keys - setting
	   the same security on them. */
	if((err = RegOpenKeyEx( start, path, 0, KEY_ENUMERATE_SUB_KEYS, &key)) !=ERROR_SUCCESS)return -1;
	/* Now enumerate the subkeys, setting the security on them all. */
	do {
		DWORD size;
		FILETIME ft;

		size = sizeof(usersid);
		err = RegEnumKeyEx(	key, indx, usersid, &size, 0, 0, 0, &ft);
		if(err == ERROR_SUCCESS) {
			HKEY subkey;

			indx++;
			if((err = RegOpenKeyEx( key, usersid, 0, WRITE_DAC, &subkey)) !=ERROR_SUCCESS)
      {
				RegCloseKey(key);
				return -1;
			}
			if((err = RegSetKeySecurity( subkey, DACL_SECURITY_INFORMATION,psd)) != ERROR_SUCCESS)
      {
				RegCloseKey(subkey);
				RegCloseKey(key);
				return -1;
			}
			RegCloseKey(subkey);
		}
	} while(err == ERROR_SUCCESS);

	if(err != ERROR_NO_MORE_ITEMS)
  {
		RegCloseKey(key);
		return -1;
	}
	if(return_key == 0)
		RegCloseKey(key);
	else
		*return_key = key;
	return 0;
}
//------------------------------------------------------------------------------
/*int restore_sam_tree_access(HKEY start, char *pth)
{
	char path[MAX_PATH];
	char *p;
	HKEY key;
	DWORD err;
	SECURITY_DESCRIPTOR sd;
	DWORD admin_mask;

  strncpy(path,pth,MAX_PATH);
	admin_mask = WRITE_DAC | READ_CONTROL;

  char group_name[256];
  if (AdministratorGroupName(group_name, 255))
  {
      if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,group_name, admin_mask) == FALSE)       //local
        if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrators", admin_mask) == FALSE)       //english
          if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrateurs", admin_mask) == FALSE)    //french
            if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administradores", admin_mask) == FALSE)  //spanish
              if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administratoren", admin_mask) == FALSE)//dutch
            return -1;
  }else if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrators", admin_mask) == FALSE)       //english
          if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrateurs", admin_mask) == FALSE)    //french
            if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administradores", admin_mask) == FALSE)  //spanish
              if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administratoren", admin_mask) == FALSE)//dutch
            return -1;

	// Remove the security on the user keys first.
	if(set_userkeys_security( start, path, &sd, 0) != 0)return -1;

	// now go up the path, restoring security
	do {
		if((err = RegOpenKeyEx( start, path, 0, WRITE_DAC, &key)) !=ERROR_SUCCESS)return -1;
		if((err = RegSetKeySecurity( key, DACL_SECURITY_INFORMATION,&sd)) != ERROR_SUCCESS)
    {
			RegCloseKey(key);
			return  -1;
		}
		RegCloseKey(key);
		p = strrchr(path, '\\');
		if( p != 0)
			*p = 0;
	} while( p != 0 );

	return 0;
}*/
//------------------------------------------------------------------------------
/*int set_sam_tree_access( HKEY start, char *pth)
{
	char path[MAX_PATH];
	char *p;
	HKEY key;
	DWORD err;
	BOOL security_changed = FALSE;
	SECURITY_DESCRIPTOR sd;
	DWORD admin_mask;
	BOOL finished = FALSE;

	strncpy(path,pth,MAX_PATH);
	admin_mask = WRITE_DAC | READ_CONTROL | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS;

  //lecture du group d'administrateur
  char group_name[256];
  if (AdministratorGroupName(group_name, 255))
  {
      if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,group_name, admin_mask) == FALSE)       //local
        if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrators", admin_mask) == FALSE)       //english
          if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrateurs", admin_mask) == FALSE)    //french
            if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administradores", admin_mask) == FALSE)  //spanish
              if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administratoren", admin_mask) == FALSE)//dutch
                return -1;
  }else if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrators", admin_mask) == FALSE)       //english
          if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administrateurs", admin_mask) == FALSE)    //french
            if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administradores", admin_mask) == FALSE)  //spanish
              if(create_sd_from_list( &sd, 2, "SYSTEM", GENERIC_ALL,"Administratoren", admin_mask) == FALSE)//dutch
                return -1;

	p = strchr(path, '\\');

	do {
		if( p != 0)
			*p = 0;
		else
			finished = TRUE;

		if((err = RegOpenKeyEx( start, path, 0, WRITE_DAC, &key)) != ERROR_SUCCESS)return (security_changed ? -2: -1);
		if((err = RegSetKeySecurity( key, DACL_SECURITY_INFORMATION,&sd)) != ERROR_SUCCESS)
    {
			RegCloseKey(key);
			return (security_changed ? -2: -1);
		}
		security_changed = TRUE;
		RegCloseKey(key);
		if(p != 0) {
			*p++ = '\\';
			p = strchr(p, '\\');
		}
	} while( !finished );

	if(set_userkeys_security( start, path, &sd, &key) != 0)
		return -2;
	return 0;
}*/
//------------------------------------------------------------------------------
BOOL registry_users_extract()
{
  //import syskey
  /*char sk[MAX_PATH]="";
  registry_syskey_local(sk, MAX_PATH);

  HKEY CleTmp;
  DWORD nbSubKey = 0,i,j,k,l,TailleNomSubKey, size;
  char path[MAX_PATH],NomSubKey[MAX_PATH],tmp[MAX_LINE_SIZE];
  char buffer[MAX_LINE_SIZE],cbuffer[MAX_LINE_SIZE];

  char name[MAX_PATH],RID[MAX_PATH],SID[MAX_PATH],group[MAX_PATH],type[MAX_PATH],
  description[MAX_PATH],last_logon[DATE_SIZE_MAX],last_password_change[DATE_SIZE_MAX];
  DWORD nb_connexion,state_id;


  //modification des droits
  BOOL ok = FALSE;
  int ret = 0;set_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Account\\Users");
  if (ret == 0)//ok
  {
    //lecture des users
    if (RegOpenKey(HKEY_LOCAL_MACHINE,"SAM\\SAM\\Domains\\Account\\Users\\",&CleTmp)==ERROR_SUCCESS)
    {
      if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
      {
        if (nbSubKey >0)
        {
          int rid;
          for (i=0;i<nbSubKey;i++)
          {
            TailleNomSubKey=MAX_PATH;// on reinitialise la taille a chaque fois sinon il ne lit pas la valeur suivant
            if (RegEnumKeyEx (CleTmp,i,NomSubKey,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
            {
              //path user
              snprintf(path,MAX_LINE_SIZE,"SAM\\SAM\\Domains\\Account\\Users\\%s",NomSubKey);
              size = ReadValue(HKEY_LOCAL_MACHINE,path,"V",buffer, MAX_LINE_SIZE);
              if(size>0)
              {
                //Data to Hexa
                DataToHexaChar(buffer, size, cbuffer, MAX_LINE_SIZE);

                //extract datas
                rid = TraiterUserDataFromSAM_V(lv_line);

                //décodage du hash avec la clé ^^
                //lecture de la valeure F (account) pour la génération de la syskey
                BYTE b_f[MAX_LINE_SIZE];
                if (LireGValeur(HKEY_LOCAL_MACHINE,"SAM\\SAM\\Domains\\Account","F",(char*)b_f) > 0x80)
                {
                  DecodeSAMHash(sk,lv_line[10].c,rid,lv_line[2].c,b_f);
                }

                snprintf(tmp_add,MAX_PATH,"Last logon : %s,%s",lv_line[2].c,lv_line[4].c);
                snprintf(user,MAX_PATH,"%s SID :%s",lv_line[2].c,lv_line[3].c);
                l = AddToLV(hlv, lv_line, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);

                //traitement des données de connexion
                lv_line[4].c[0]=0;
                size = LireGValeur(HKEY_LOCAL_MACHINE,path,"F",lv_line[4].c);

                if(size>0 && lv_line[4].c[0]!=0)
                {
                  //transformation des données en hexa ^^
                  lv_line[3].c[0]=0;
                  for (j=0;j<size && j/2<MAX_LINE_SIZE;j++)
                  {
                    snprintf(tmp,10,"%02X",lv_line[4].c[j]&0xff);
                    strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
                  }
                  strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);

                  //application des données
                  TraiterUserDataFromSAM_F(lv_line);
                  ListView_SetItemText(hlv,l,6,lv_line[6].c);
                  ListView_SetItemText(hlv,l,7,lv_line[7].c);
                  ListView_SetItemText(hlv,l,8,lv_line[8].c);
                  ListView_SetItemText(hlv,l,9,lv_line[9].c);
                }

                //StateHC(lv_line, 6, user);

                //ajouter dans l'historique
                AddToLV_Registry2(lv_line[6].c, user, "Users & groups", tmp_add);
                AddToLV_RegistryCritical(lv_line[6].c, user, "Users & groups", tmp_add);

                snprintf(tmp_add,MAX_PATH,"Last password change : %s,%s",lv_line[2].c,lv_line[4].c);
                AddToLV_Registry2(lv_line[7].c, user, "Users & groups", tmp_add);
                AddToLV_RegistryCritical(lv_line[7].c, user, "Users & groups", tmp_add);
              }
            }
          }
          //on vérifie qu'il y a bien des utilisateurs qui ont été ajoutés ^^
          if(ListView_GetItemCount(hlv)>0) ok = TRUE;
        }
      }
      RegCloseKey(CleTmp);
    }
    restore_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Account\\Users");
  }else if (ret == -2)restore_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Account\\Users");


  ret = set_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Account\\Aliases");
  if (ret == 0)//ok
  {
    //lecture des groupes ^^
    CleTmp = 0;
    if (RegOpenKey(HKEY_LOCAL_MACHINE,"SAM\\SAM\\Domains\\Account\\Aliases\\",&CleTmp)==ERROR_SUCCESS)
    {
      if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
      {
        if (nbSubKey >0)
        {
          NomSubKey[0]=0;
          for (i=0;i<nbSubKey;i++)
          {
            TailleNomSubKey=MAX_PATH;// on reinitialise la taille a chaque fois sinon il ne lit pas la valeur suivant
            if (RegEnumKeyEx (CleTmp,i,NomSubKey,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
            {
              //génération du path group
              snprintf(path,MAX_LINE_SIZE,"SAM\\SAM\\Domains\\Account\\Aliases\\%s",NomSubKey);

              lv_line[4].c[0]=0;
              size = LireGValeur(HKEY_LOCAL_MACHINE,path,"C",lv_line[4].c);
              if(size>0)
              {
                //transformation des données en hexa ^^
                lv_line[3].c[0]=0;
                for (j=0;j<size && j/2<MAX_LINE_SIZE;j++)
                {
                  snprintf(tmp,10,"%02X",lv_line[4].c[j]&0xff);
                  strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
                }
                strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);

                //traitement des données user (hash, etcs ..)
                lv_line[4].c[0]=0;
                TraiterGroupDataFromSAM_C(lv_line,hlv);
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
    }
    restore_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Account\\Aliases");
  }else if (ret == -2)restore_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Account\\Aliases");

  ret = set_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Builtin\\Aliases");
  if (ret == 0)//ok
  {
    CleTmp = 0;
    if (RegOpenKey(HKEY_LOCAL_MACHINE,"SAM\\SAM\\Domains\\Builtin\\Aliases\\",&CleTmp)==ERROR_SUCCESS)
    {
      nbSubKey = 0;
      if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
      {
        if (nbSubKey >0)
        {

          NomSubKey[0]=0;
          for (i=0;i<nbSubKey;i++)
          {
            TailleNomSubKey=MAX_PATH;// on reinitialise la taille a chaque fois sinon il ne lit pas la valeur suivant
            if (RegEnumKeyEx (CleTmp,i,NomSubKey,&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
            {
              //génération du path group
              snprintf(path,MAX_LINE_SIZE,"SAM\\SAM\\Domains\\Builtin\\Aliases\\%s",NomSubKey);

              lv_line[4].c[0]=0;
              size = LireGValeur(HKEY_LOCAL_MACHINE,path,"C",lv_line[4].c);
              if(size>0)
              {
                //transformation des données en hexa ^^
                lv_line[3].c[0]=0;
                for (j=0;j<size && j/2<MAX_LINE_SIZE;j++)
                {
                  snprintf(tmp,10,"%02X",lv_line[4].c[j]&0xff);
                  strncat(lv_line[3].c,tmp,MAX_LINE_SIZE);
                }
                strncat(lv_line[3].c,"\0",MAX_LINE_SIZE);

                //traitement des données user (hash, etcs ..)
                lv_line[4].c[0]=0;
                TraiterGroupDataFromSAM_C(lv_line,hlv);
             }
           }
         }
       }
     }
     RegCloseKey(CleTmp);
    }
    restore_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Builtin\\Aliases");
  }else if (ret == -2)restore_sam_tree_access(HKEY_LOCAL_MACHINE,"SECURITY\\SAM\\Domains\\Builtin\\Aliases");

  return ok;*/

  return FALSE;
}

//------------------------------------------------------------------------------
//file registry part
//------------------------------------------------------------------------------
