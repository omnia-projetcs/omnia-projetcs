//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
#include "rc4.h"
#include "md5.h"
// inspiré de bkhive et samdump2 Nicola Cuomo - ncuomo@studenti.unina.it
//------------------------------------------------------------------------------
//décodage afin d'obtenir le hash des mots de passe
void DecodeSAMHash(char *sk,char *hsc, int rid, char *user, BYTE *b_f)
{
  if (b_f != NULL)
  {
    //chargement des fonctions pour l'exploitation DES
    typedef int (WINAPI *SF)(unsigned char*, int*, unsigned char*);

    HINSTANCE hDLL;
    SF sf27;
    SF sf25;

    if((hDLL = LoadLibrary("ADVAPI32.DLL" ))== NULL) return;
    sf27 = (SF) GetProcAddress( hDLL, "SystemFunction027" );
    sf25 = (SF) GetProcAddress( hDLL, "SystemFunction025" );

    //transcodage en byte de la sysley
    BYTE b_sk[16];
    unsigned int i, ref;
    for (i=0;i<16;i++)
    {
      b_sk[i] = (BYTE)HexaToDecS(&sk[i*2]);
    }

    //découpage codage en byte des hash
    BYTE b_LM[16]="",b_NT[16]="";

    //LM
    if (hsc[0] == '<')ref =18;
    else
    {
      for (i=0;i<16;i++)
      {
        b_LM[i] = (BYTE)HexaToDecS(&hsc[i*2+1]); // +1 = on passe le : de début ^^
      }
      ref = 34;
    }

    //NT
    if (hsc[ref] != '<')
    {
      for (i=0;i<16;i++)
      {
        b_NT[i] = (BYTE)HexaToDecS(&hsc[i*2+ref]);
      }
    }

    //init pour le déchiffrement ^^
    unsigned char aqwerty[] = "!@#$%^&*()qwertyUIOPAzxcvbnmQQQQQQQQQQQQ)(*@&%";
    unsigned char anum[] = "0123456789012345678901234567890123456789";
    unsigned char almpassword[] = "LMPASSWORD";
    unsigned char antpassword[] = "NTPASSWORD";

    MD5_CTX md5c;
    unsigned char md5hash[0x10];
    RC4_KEY rc4k;
    unsigned char hbootkey[0x20];
    unsigned char obfkey[0x10];
    unsigned char fb[0x10];
    int j;
    char tmp[11],result[MAX_PATH];

    //init de la clé pour exploitation
    MD5_Init( &md5c );
    MD5_Update( &md5c, &b_f[0x70], 0x10 );
    MD5_Update( &md5c, aqwerty, 0x2f );
    MD5_Update( &md5c, b_sk, 0x10 );
    MD5_Update( &md5c, anum, 0x29 );
    MD5_Final( md5hash, &md5c );
    RC4_set_key( &rc4k, 0x10, md5hash );
    RC4( &rc4k, 0x20, &b_f[0x80], hbootkey );

    //traitement des données ^^
    sprintf(hsc,"%s:%d:",user,rid);
    if (b_LM[0]!= 0)
    {
      //décodage ^^
      /* LANMAN */
      /* hash the hbootkey and decode lanman password hashes */
      MD5_Init( &md5c );
      MD5_Update( &md5c, hbootkey, 0x10 );
      MD5_Update( &md5c, &rid, 0x4 );
      MD5_Update( &md5c, almpassword, 0xb );
      MD5_Final( md5hash, &md5c );

      RC4_set_key( &rc4k, 0x10, md5hash );
      RC4( &rc4k, 0x10, b_LM, obfkey );

      sf25( obfkey, (int*)&rid, fb );

      //transformation des données en hexa ^^
      for (j=0;j<0x10;j++)
      {
        snprintf(tmp,10,"%.2X",fb[j]);
        strncat(hsc,tmp,MAX_LINE_SIZE);
      }
      strncat(hsc,":\0",MAX_LINE_SIZE);
    }else strncat(hsc,"NO LM PASSWORD******************:\0",MAX_LINE_SIZE);

    if (b_NT[0]!= 0)
    {
      /* NT */
      /* hash the hbootkey and decode the nt password hashes */
      MD5_Init( &md5c );
      MD5_Update( &md5c, hbootkey, 0x10 );
      MD5_Update( &md5c, &rid, 0x4 );
      MD5_Update( &md5c, antpassword, 0xb );
      MD5_Final( md5hash, &md5c );

      RC4_set_key( &rc4k, 0x10, md5hash );
      RC4( &rc4k, 0x10,b_NT, obfkey );

      /* sf27 wrap to sf25 */
      sf27( obfkey, (int*)&rid, fb );

      result[0]=0;
      for (j=0;j<0x10;j++)
      {
        snprintf(tmp,10,"%.2X",fb[j]);
        strncat(result,tmp,MAX_PATH);
      }
      strncat(result,"\0",MAX_PATH);

      if (!strcmp(result,"31D6CFE0D16AE931B73C59D7E0C089C0"))
        strncat(hsc,"NO NT PASSWORD******************\0",MAX_LINE_SIZE);
      else strncat(hsc,result,MAX_LINE_SIZE);

      strncat(hsc,":::\0",MAX_LINE_SIZE);
    }else strncat(hsc,"NO NT PASSWORD******************:::\0",MAX_LINE_SIZE);

    FreeLibrary(hDLL);
  }
}
//------------------------------------------------------------------------------
DWORD SearchNK(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *NKname, DWORD *nb_nk)
{
  //on vérifie le type
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+position);
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && nk_h->type == 0x6B6E)
  {
    //lecture de la première clée :p
    //on passe la racine avec vérification que se n'est pas "ControlSet001"
    char tmp[MAX_LINE_SIZE];
    strncpy(tmp,(char *)(buffer+position+HBIN_CELL_NK_SIZE),MAX_LINE_SIZE);
    if (nk_h->key_name_size>MAX_LINE_SIZE)tmp[MAX_LINE_SIZE-1]=0;
    else tmp[nk_h->key_name_size]=0;

    //si valide
    if (nk_h->nb_subkeys>0&& nk_h->nb_subkeys<taille_fic && ((pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE)<taille_fic) && !strcmp(tmp,NKname))
    {
      *nb_nk = nk_h->nb_subkeys;
      return nk_h->lf_offset;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL SearchNKtoClassname(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *NKname, char *Classname)
{
  //on vérifie le type
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+position);
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && nk_h->type == 0x6B6E)
  {
    //lecture de la première clée :p
    //on passe la racine avec vérification que se n'est pas "ControlSet001"
    char tmp[MAX_LINE_SIZE];
    strncpy(tmp,(char *)(buffer+position+HBIN_CELL_NK_SIZE),MAX_LINE_SIZE);
    if (nk_h->key_name_size>MAX_LINE_SIZE)tmp[MAX_LINE_SIZE-1]=0;
    else tmp[nk_h->key_name_size]=0;

    //si valide
    if (nk_h->class_name_offset>0&& nk_h->class_name_offset<taille_fic &&
        nk_h->class_name_size>0&& nk_h->class_name_size<taille_fic &&
        ((pos_fhbin+nk_h->class_name_offset-HBIN_HEADER_SIZE)<taille_fic) && !strcmp(tmp,NKname))
    {
      char *c = (char*)(buffer + nk_h->class_name_offset + 0x1004);
      snprintf(Classname,MAX_LINE_SIZE,"%S",c);
      Classname[8] = 0;
      return TRUE;
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
void AddLvSyskey(char *path, char *cJD, char *cSkew1, char *cGBG, char *cData, char *sk)
{
  //traitement et commutation des données pour générer la syskey
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];

  if (path[0]==0)lv_line[0].c[0]=0;
  else strcpy(lv_line[0].c,path);

  strcpy(lv_line[1].c,"HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Control\\Lsa\\%%\\");
  strcpy(lv_line[2].c,"JD, Skew1, GBG, Data (path, part of class name)");
  strcpy(lv_line[4].c,"(syskey) Syskey/Bootkey");
  lv_line[5].c[0]=0;

  lv_line[3].c[0x00] = cGBG[0x00];
  lv_line[3].c[0x01] = cGBG[0x01];
  lv_line[3].c[0x02] = cSkew1[0x02];
  lv_line[3].c[0x03] = cSkew1[0x03];
  lv_line[3].c[0x04] = cSkew1[0x00];
  lv_line[3].c[0x05] = cSkew1[0x01];
  lv_line[3].c[0x06] = cJD[0x04];
  lv_line[3].c[0x07] = cJD[0x05];
  lv_line[3].c[0x08] = cGBG[0x06];
  lv_line[3].c[0x09] = cGBG[0x07];
  lv_line[3].c[0x0A] = cGBG[0x02];
  lv_line[3].c[0x0B] = cGBG[0x03];
  lv_line[3].c[0x0C] = cData[0x02];
  lv_line[3].c[0x0D] = cData[0x03];
  lv_line[3].c[0x0E] = cJD[0x06];
  lv_line[3].c[0x0F] = cJD[0x07];
  lv_line[3].c[0x10] = cJD[0x00];
  lv_line[3].c[0x11] = cJD[0x01];
  lv_line[3].c[0x12] = cSkew1[0x04];
  lv_line[3].c[0x13] = cSkew1[0x05];
  lv_line[3].c[0x14] = cJD[0x02];
  lv_line[3].c[0x15] = cJD[0x03];
  lv_line[3].c[0x16] = cData[0x00];
  lv_line[3].c[0x17] = cData[0x01];
  lv_line[3].c[0x18] = cData[0x04];
  lv_line[3].c[0x19] = cData[0x05];
  lv_line[3].c[0x1A] = cGBG[0x04];
  lv_line[3].c[0x1B] = cGBG[0x05];
  lv_line[3].c[0x1C] = cData[0x06];
  lv_line[3].c[0x1D] = cData[0x07];
  lv_line[3].c[0x1E] = cSkew1[0x06];
  lv_line[3].c[0x1F] = cSkew1[0x07];
  lv_line[3].c[0x20] = 0;

  if (syskey_ == FALSE)//add global syskey
  {
    syskey_ = TRUE;
    strcpy(sk_c,lv_line[3].c);
  }

  if (sk!=NULL) strcpy(sk,lv_line[3].c);
  AddToLVRegBin(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), lv_line, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
}
//------------------------------------------------------------------------------
void SyskeyExtract(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin, char *path)
{
  //on vérifie le type
  HBIN_CELL_NK_HEADER *nk_h = (HBIN_CELL_NK_HEADER *)(buffer+position);
  if (nk_h->key_name_size >0 && nk_h->key_name_size<taille_fic && nk_h->size>0 && nk_h->type == 0x6B6E)
  {
    //lecture de la première clée :p
    //on passe la racine avec vérification que se n'est pas "ControlSet001"
    char tmp[MAX_LINE_SIZE];
    strncpy(tmp,(char *)(buffer+position+HBIN_CELL_NK_SIZE),MAX_LINE_SIZE);
    if (nk_h->key_name_size>MAX_LINE_SIZE)tmp[MAX_LINE_SIZE-1]=0;
    else tmp[nk_h->key_name_size]=0;

    //si le format ne comprend pas directement cette clé on recherche dans les clées filles
    DWORD pos_nk = 0,i;
    DWORD nb_nk = 0;
    if (strcmp(tmp,"ControlSet001")!=0)
    {
      //traitement des sous clées nk ^^
      if (nk_h->nb_subkeys>0&& nk_h->nb_subkeys<taille_fic && ((pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE)<taille_fic))
      {
        HBIN_CELL_LF_HEADER *lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+nk_h->lf_offset-HBIN_HEADER_SIZE];
        for (i=0;i<nk_h->nb_subkeys;i++)
        {
          if ((lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic && (lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE) > pos_fhbin)
          {
            pos_nk = SearchNK(buffer, taille_fic, lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, pos_fhbin, "ControlSet001", &nb_nk);
            if (pos_nk!=0) break;
          }
        }
      }
    }else
    {
      //la bonne clée ^^
      nb_nk = nk_h->nb_subkeys;
    }

    if (pos_nk == 0 || nb_nk==0)return;
    //recherche de la clé : Control
    HBIN_CELL_LF_HEADER *lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+pos_nk-HBIN_HEADER_SIZE];
    pos_nk = 0;
    DWORD nb_nk2 = 0;
    for (i=0;i<nb_nk;i++)
    {
      if ((lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic && (lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE) > pos_fhbin)
      {
        pos_nk = SearchNK(buffer, taille_fic, lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, pos_fhbin, "Control",&nb_nk2);
        if (pos_nk!=0) break;
      }
    }

    if (pos_nk == 0 || nb_nk2==0)return;
    //recherche de la clé : Lsa
    lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+pos_nk-HBIN_HEADER_SIZE];
    pos_nk = 0;
    nb_nk = 0;
    for (i=0;i<nb_nk2;i++)
    {
      if ((lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic && (lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE) > pos_fhbin)
      {
        pos_nk = SearchNK(buffer, taille_fic, lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, pos_fhbin, "Lsa",&nb_nk);
        if (pos_nk!=0) break;
      }
    }

    //lecture des répertoires spécifiques "JD", "Skew1", "GBG", "Data"
    if (pos_nk == 0 || nb_nk==0)return;
    //recherche de la clé : Lsa

    BOOL bJD=FALSE, bSkew1=FALSE, bGBG=FALSE, bData=FALSE;
    //#define SZ_PART_SYSKEY  0x20
    char cJD[SZ_PART_SYSKEY]="", cSkew1[SZ_PART_SYSKEY]="", cGBG[SZ_PART_SYSKEY]="", cData[SZ_PART_SYSKEY]="";

    lf_h = (HBIN_CELL_LF_HEADER *)&buffer[pos_fhbin+pos_nk-HBIN_HEADER_SIZE];
    for (i=0;i<nb_nk2;i++)
    {
      if ((lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE)<taille_fic && (lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE) > pos_fhbin)
      {
        if (!bJD) bJD       = SearchNKtoClassname(buffer, taille_fic, lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, pos_fhbin, "JD",cJD);
        if (!bSkew1) bSkew1 = SearchNKtoClassname(buffer, taille_fic, lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, pos_fhbin, "Skew1",cSkew1);
        if (!bGBG) bGBG     = SearchNKtoClassname(buffer, taille_fic, lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, pos_fhbin, "GBG",cGBG);
        if (!bData) bData   = SearchNKtoClassname(buffer, taille_fic, lf_h->hb_c[i].nk_of+pos_fhbin-HBIN_HEADER_SIZE, pos_fhbin, "Data",cData);
      }
    }

    //génération de la syskey
    if (bJD && bSkew1 && bGBG && bData)
    {
      //traitement
      AddLvSyskey(path, cJD, cSkew1, cGBG, cData,NULL);
    }
  }
}
