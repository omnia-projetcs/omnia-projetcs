//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
#include "rc4.h"
#include "md5.h"
#include "des.h"
//#include "des_old.h"
// inspiré de bkhive et samdump2 Nicola Cuomo - ncuomo@studenti.unina.it
/*#define DES_KEY_SZ 8
static const unsigned char odd_parity[256]={
  1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
 16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
 32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
 49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
 64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
 81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
 97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254};

void DES_set_odd_parity(unsigned char *key)
	{
	unsigned int i;

	for (i=0; i<DES_KEY_SZ; i++)
		key[i]=odd_parity[key[i]];
	}*/
/*
* Convert a 7 byte array into an 8 byte des key with odd parity.
*/

void str_to_key(unsigned char *str,unsigned char *key)
{
      // void des_set_odd_parity(des_cblock *);
      int i;

      key[0] = str[0]>>1;
      key[1] = ((str[0]&0x01)<<6) | (str[1]>>2);
      key[2] = ((str[1]&0x03)<<5) | (str[2]>>3);
      key[3] = ((str[2]&0x07)<<4) | (str[3]>>4);
      key[4] = ((str[3]&0x0F)<<3) | (str[4]>>5);
      key[5] = ((str[4]&0x1F)<<2) | (str[5]>>6);
      key[6] = ((str[5]&0x3F)<<1) | (str[6]>>7);
      key[7] = str[6]&0x7F;
      for (i=0;i<8;i++) {
            key[i] = (key[i]<<1);
      }
      DES_set_odd_parity((DES_cblock *)key);
      //des_set_odd_parity(key);
}

/*
* Function to convert the RID to the first decrypt key.
*/

void sid_to_key1(unsigned long sid,unsigned char deskey[8])
{
      unsigned char s[7];

      s[0] = (unsigned char)(sid & 0xFF);
      s[1] = (unsigned char)((sid>>8) & 0xFF);
      s[2] = (unsigned char)((sid>>16) & 0xFF);
      s[3] = (unsigned char)((sid>>24) & 0xFF);
      s[4] = s[0];
      s[5] = s[1];
      s[6] = s[2];

      str_to_key(s,deskey);
}

/*
* Function to convert the RID to the second decrypt key.
*/

void sid_to_key2(unsigned long sid,unsigned char deskey[8])
{
      unsigned char s[7];

      s[0] = (unsigned char)((sid>>24) & 0xFF);
      s[1] = (unsigned char)(sid & 0xFF);
      s[2] = (unsigned char)((sid>>8) & 0xFF);
      s[3] = (unsigned char)((sid>>16) & 0xFF);
      s[4] = s[0];
      s[5] = s[1];
      s[6] = s[2];

      str_to_key(s,deskey);
}
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

//
      unsigned char deskey1[MAX_PATH], deskey2[MAX_PATH], ks1[MAX_PATH], ks2[MAX_PATH];

      sid_to_key1(rid,(unsigned char *)deskey1);
      DES_set_key_checked((const_DES_cblock*)deskey1,(DES_key_schedule*)(ks1));
      sid_to_key2(rid,(unsigned char *)deskey2);
      DES_set_key_unchecked((const_DES_cblock*)deskey2,(DES_key_schedule*)(ks2));

      /* Decrypt the lanman password hash as two 8 byte blocks. */
      DES_ecb_encrypt((const_DES_cblock *)obfkey,(DES_cblock *)fb, (DES_key_schedule*)ks1, 0/*DES_DECRYPT*/);
      DES_ecb_encrypt((const_DES_cblock *)(obfkey + 8),(DES_cblock *)&fb[8],(DES_key_schedule*)ks2, 0/*DES_DECRYPT*/);

      //sf25( obfkey, (int*)&rid, fb );

      //transformation des données en hexa ^^
      result[0]=0;
      for (j=0;j<0x10;j++)
      {
        snprintf(tmp,10,"%.2X",fb[j]);
        strncat(result,tmp,MAX_LINE_SIZE);
      }
      strncat(result,"\0",MAX_LINE_SIZE);

      if (!strcmp(result,"AAD3B435B51404EEAAD3B435B51404EE"))
        strncat(hsc,"NO LM PASSWORD******************:\0",MAX_LINE_SIZE);
      else
      {
        strncat(hsc,result,MAX_LINE_SIZE);
        strncat(hsc,":\0",MAX_LINE_SIZE);
      }
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

      unsigned char deskey1[MAX_PATH], deskey2[MAX_PATH], ks1[MAX_PATH], ks2[MAX_PATH];
      sid_to_key1(rid,(unsigned char *)deskey1);
      des_set_key((const_DES_cblock*)deskey1,ks1);
      sid_to_key2(rid,(unsigned char *)deskey2);
      des_set_key((const_DES_cblock*)deskey2,ks2);

      DES_ecb_encrypt((const_DES_cblock *)obfkey,(DES_cblock *)fb, (DES_key_schedule*)ks1, 0/*DES_DECRYPT*/);
      DES_ecb_encrypt((const_DES_cblock *)(obfkey + 8),(DES_cblock *)&fb[8],(DES_key_schedule*)ks2, 0/*DES_DECRYPT*/);

      //sf27( obfkey, (int*)&rid, fb );

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
/*
//V1 XP/2003
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


      MD5_Init( &md5c );
      MD5_Update( &md5c, hbootkey, 0x10 );
      MD5_Update( &md5c, &rid, 0x4 );
      MD5_Update( &md5c, almpassword, 0xb );
      MD5_Final( md5hash, &md5c );

      RC4_set_key( &rc4k, 0x10, md5hash );
      RC4( &rc4k, 0x10, b_LM, obfkey );

      sf25( obfkey, (int*)&rid, fb );

      //transformation des données en hexa ^^
      result[0]=0;
      for (j=0;j<0x10;j++)
      {
        snprintf(tmp,10,"%.2X",fb[j]);
        strncat(result,tmp,MAX_LINE_SIZE);
      }
      strncat(result,"\0",MAX_LINE_SIZE);

      if (!strcmp(result,"AAD3B435B51404EEAAD3B435B51404EE"))
        strncat(hsc,"NO PASSWORD*********************:\0",MAX_LINE_SIZE);
      else
      {
        strncat(hsc,result,MAX_LINE_SIZE);
        strncat(hsc,":\0",MAX_LINE_SIZE);
      }
    }else strncat(hsc,"NO PASSWORD*********************:\0",MAX_LINE_SIZE);

    if (b_NT[0]!= 0)
    {
      MD5_Init( &md5c );
      MD5_Update( &md5c, hbootkey, 0x10 );
      MD5_Update( &md5c, &rid, 0x4 );
      MD5_Update( &md5c, antpassword, 0xb );
      MD5_Final( md5hash, &md5c );

      RC4_set_key( &rc4k, 0x10, md5hash );
      RC4( &rc4k, 0x10,b_NT, obfkey );

      sf27( obfkey, (int*)&rid, fb );

      result[0]=0;
      for (j=0;j<0x10;j++)
      {
        snprintf(tmp,10,"%.2X",fb[j]);
        strncat(result,tmp,MAX_PATH);
      }
      strncat(result,"\0",MAX_PATH);

      if (!strcmp(result,"31D6CFE0D16AE931B73C59D7E0C089C0"))
        strncat(hsc,"NO PASSWORD*********************\0",MAX_LINE_SIZE);
      else strncat(hsc,result,MAX_LINE_SIZE);

      strncat(hsc,":::\0",MAX_LINE_SIZE);
    }else strncat(hsc,"NO PASSWORD*********************:::\0",MAX_LINE_SIZE);

    FreeLibrary(hDLL);
  }
}
*/
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
