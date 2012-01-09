//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
void TraiterDataUserassist(LINE_ITEM *item)
{
  //size of data
  unsigned int size = strlen(item[3].c);

  if (size == 16) //8 octets réels
  {
    char tmp[MAX_PATH];
    sprintf(tmp,"0x%c%c%c%c%c%c%c%c",item[3].c[14],item[3].c[15],item[3].c[12],item[3].c[13],item[3].c[10],item[3].c[11],item[3].c[8],item[3].c[9]);
    strcpy(item[3].c,tmp);
    item[4].c[0]=0;
    item[5].c[0]=0;
  }else if (size == 32) //16 octets
  {
    char tmp[MAX_PATH];
    sprintf(tmp,"0x%c%c%c%c%c%c%c%c",item[3].c[14],item[3].c[15],item[3].c[12],item[3].c[13],item[3].c[10],item[3].c[11],item[3].c[8],item[3].c[9]);

    char Date1[8];//récupération en hexa
    char Date2[8];//récupération en hexa
    sprintf(Date1,"%c%c%c%c%c%c%c%c",item[3].c[22],item[3].c[23],item[3].c[20],item[3].c[21],item[3].c[18],item[3].c[19],item[3].c[16],item[3].c[17]);
    sprintf(Date2,"%c%c%c%c%c%c%c%c",item[3].c[30],item[3].c[31],item[3].c[28],item[3].c[29],item[3].c[26],item[3].c[27],item[3].c[24],item[3].c[25]);

    FILETIME FileTime, LocalFileTime;
    SYSTEMTIME SysTime;
    FileTime.dwLowDateTime = (DWORD)HexaToDec(Date1,8);
    FileTime.dwHighDateTime = (DWORD)HexaToDec(Date2,8);

    //dans le cas ou ==0 non utilisable et pas de modification sur  le nombre d'utilisation
    if (( FileTime.dwLowDateTime!=0)||(FileTime.dwHighDateTime!=0))
    {
       if (FileTimeToLocalFileTime(&FileTime, &LocalFileTime))
       {
         if(FileTimeToSystemTime(&LocalFileTime, &SysTime))
          sprintf(item[4].c,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
         else item[4].c[0]=0;
       }else item[4].c[0]=0;
    }else item[4].c[0]=0;

    item[5].c[0]=0;
    strcpy(item[3].c,tmp);
  }else if (size >= 136) //68
  {
    char tmp[MAX_PATH];
    sprintf(tmp,"0x%c%c%c%c%c%c%c%c",item[3].c[2],item[3].c[3],item[3].c[4],item[3].c[5],item[3].c[6],item[3].c[7],item[3].c[8],item[3].c[9]);

    char Date1[8];//récupération en hexa
    char Date2[8];//récupération en hexa
    sprintf(Date1,"%c%c%c%c%c%c%c%c",item[3].c[126],item[3].c[127],item[3].c[124],item[3].c[125],item[3].c[122],item[3].c[123],item[3].c[120],item[3].c[121]);
    sprintf(Date2,"%c%c%c%c%c%c%c%c",item[3].c[134],item[3].c[135],item[3].c[132],item[3].c[133],item[3].c[130],item[3].c[131],item[3].c[128],item[3].c[129]);

    FILETIME FileTime, LocalFileTime;
    SYSTEMTIME SysTime;
    FileTime.dwLowDateTime = (DWORD)HexaToDec(Date1,8);
    FileTime.dwHighDateTime = (DWORD)HexaToDec(Date2,8);

    //dans le cas ou ==0 non utilisable et pas de modification sur  le nombre d'utilisation
    if (( FileTime.dwLowDateTime!=0)||(FileTime.dwHighDateTime!=0))
    {
       if (FileTimeToLocalFileTime(&FileTime, &LocalFileTime))
       {
         if(FileTimeToSystemTime(&LocalFileTime, &SysTime))
          sprintf(item[4].c,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
         else item[4].c[0]=0;
       }else item[4].c[0]=0;
    }else item[4].c[0]=0;

    item[5].c[0]=0;
    strcpy(item[3].c,tmp);
  }else
  {
    //init les valeures
    item[3].c[0]=0;
    item[4].c[0]=0;
    item[5].c[0]=0;
  }
}
//---------------------------------------------------------------------------------------------------------------
// on donne une valeur hexa de 8 caracteres et on obtient le resultat
// gestion des chiffres + caractères minuscules et majuscules
//---------------------------------------------------------------------------------------------------------------
char HexaToDecS(char *hexa)
{
  char k=0;
  char j=16;
  switch(hexa[0])
  {
   case '1':k=k+j;break;
   case '2':k=k+(2*j);break;
   case '3':k=k+(3*j);break;
   case '4':k=k+(4*j);break;
   case '5':k=k+(5*j);break;
   case '6':k=k+(6*j);break;
   case '7':k=k+(7*j);break;
   case '8':k=k+(8*j);break;
   case '9':k=k+(9*j);break;
   case 'a':k=k+(10*j);break;
   case 'A':k=k+(10*j);break;
   case 'b':k=k+(11*j);break;
   case 'B':k=k+(11*j);break;
   case 'c':k=k+(12*j);break;
   case 'C':k=k+(12*j);break;
   case 'd':k=k+(13*j);break;
   case 'D':k=k+(13*j);break;
   case 'e':k=k+(14*j);break;
   case 'E':k=k+(14*j);break;
   case 'f':k=k+(15*j);break;
   case 'F':k=k+(15*j);break;
  };
  j=1;
  switch(hexa[1])
  {
   case '1':k=k+j;break;
   case '2':k=k+(2*j);break;
   case '3':k=k+(3*j);break;
   case '4':k=k+(4*j);break;
   case '5':k=k+(5*j);break;
   case '6':k=k+(6*j);break;
   case '7':k=k+(7*j);break;
   case '8':k=k+(8*j);break;
   case '9':k=k+(9*j);break;
   case 'a':k=k+(10*j);break;
   case 'A':k=k+(10*j);break;
   case 'b':k=k+(11*j);break;
   case 'B':k=k+(11*j);break;
   case 'c':k=k+(12*j);break;
   case 'C':k=k+(12*j);break;
   case 'd':k=k+(13*j);break;
   case 'D':k=k+(13*j);break;
   case 'e':k=k+(14*j);break;
   case 'E':k=k+(14*j);break;
   case 'f':k=k+(15*j);break;
   case 'F':k=k+(15*j);break;
  };
  return k;
};
//------------------------------------------------------------------------------
//conversion de chaine Wildstring stocké sous forme hexa en char
char *SHexaToString(char *src, char *dst)
{
  unsigned int i,size = strlen(src);
  char *d = dst;
  for (i=0;i<size;i+=4)
  {
    if (src[i]=='1' && src[i+1]=='9' && src[i+2]=='2' && src[i+3]=='0')*d++ = '\'';
    else *d++ = HexaToDecS(&src[i]);
  }
  *d =0;
  return dst;
}
//------------------------------------------------------------------------------
//conversion de chaine string stocké sous forme hexa en char
char *SHexaToChar(char *src, char *dst)
{
  unsigned int i,size = strlen(src);
  char *d = dst;
  for (i=0;i<size;i+=2)
  {
    if (src[i]=='0' && src[i+1]=='0')break;
    else *d++ = HexaToDecS(&src[i]);
  }
  *d =0;
  return dst;
}
//------------------------------------------------------------------------------
unsigned long int HTD(char *src) //pour conversion en hexa
{
 unsigned long int k=0;//resultat
 unsigned long int j=1,i; // j =coef multiplicateur et i variable de boucle

 //inversion pour test de traitement
 char hexa[8];
 hexa[0] = src[7];
 hexa[1] = src[6];
 hexa[2] = src[5];
 hexa[3] = src[4];
 hexa[4] = src[3];
 hexa[5] = src[2];
 hexa[6] = src[1];
 hexa[7] = src[0];

    for (i=0;i<8;i++)//calcul de la valeur hexa en decimal
    {
        switch(hexa[i])
        {
         case '1':k=k+j;break;
         case '2':k=k+(2*j);break;
         case '3':k=k+(3*j);break;
         case '4':k=k+(4*j);break;
         case '5':k=k+(5*j);break;
         case '6':k=k+(6*j);break;
         case '7':k=k+(7*j);break;
         case '8':k=k+(8*j);break;
         case '9':k=k+(9*j);break;
         case 'a':k=k+(10*j);break;
         case 'A':k=k+(10*j);break;
         case 'b':k=k+(11*j);break;
         case 'B':k=k+(11*j);break;
         case 'c':k=k+(12*j);break;
         case 'C':k=k+(12*j);break;
         case 'd':k=k+(13*j);break;
         case 'D':k=k+(13*j);break;
         case 'e':k=k+(14*j);break;
         case 'E':k=k+(14*j);break;
         case 'f':k=k+(15*j);break;
         case 'F':k=k+(15*j);break;
        };
        j=j*16;
    };
  return k;
};
//------------------------------------------------------------------------------
DWORD HTDF(char *src, unsigned int nb) //pour conversion en hexa
{
 DWORD k=0;//resultat
 DWORD j=1,i; // j =coef multiplicateur et i variable de boucle

    for (i=nb;i>0;i--)//calcul de la valeur hexa en decimal
    {
        switch(src[i-1])
        {
         case '1':k=k+j;break;
         case '2':k=k+(2*j);break;
         case '3':k=k+(3*j);break;
         case '4':k=k+(4*j);break;
         case '5':k=k+(5*j);break;
         case '6':k=k+(6*j);break;
         case '7':k=k+(7*j);break;
         case '8':k=k+(8*j);break;
         case '9':k=k+(9*j);break;
         case 'a':k=k+(10*j);break;
         case 'A':k=k+(10*j);break;
         case 'b':k=k+(11*j);break;
         case 'B':k=k+(11*j);break;
         case 'c':k=k+(12*j);break;
         case 'C':k=k+(12*j);break;
         case 'd':k=k+(13*j);break;
         case 'D':k=k+(13*j);break;
         case 'e':k=k+(14*j);break;
         case 'E':k=k+(14*j);break;
         case 'f':k=k+(15*j);break;
         case 'F':k=k+(15*j);break;
        };
        j=j*16;
    };
  return k;
};
//------------------------------------------------------------------------------
//extraction des données de la valeur F du profil
void TraiterUserDataFromSAM_F(LINE_ITEM *item)
{
  item[2].c[0]=0;
  item[4].c[0]=0;
  item[5].c[0]=0;
  item[6].c[0]=0;
  item[7].c[0]=0;
  item[8].c[0]=0;
  item[9].c[0]=0;
  item[10].c[0]=0;
  item[11].c[0]=0;

  if (strlen(item[3].c)>0x8F)
  {
    char tmp[MAX_PATH];

    //Offset 0008 : date de dernière connexion
    FILETIME LocalFileTime;
    SYSTEMTIME SysTimeLAstConnexion;

    tmp[0] = item[3].c[0x16];
    tmp[1] = item[3].c[0x17];
    tmp[2] = item[3].c[0x14];
    tmp[3] = item[3].c[0x15];
    tmp[4] = item[3].c[0x12];
    tmp[5] = item[3].c[0x13];
    tmp[6] = item[3].c[0x10];
    tmp[7] = item[3].c[0x11];
    LocalFileTime.dwLowDateTime = HTDF(tmp,8);

    tmp[0] = item[3].c[0x1E];
    tmp[1] = item[3].c[0x1F];
    tmp[2] = item[3].c[0x1C];
    tmp[3] = item[3].c[0x1D];
    tmp[4] = item[3].c[0x1A];
    tmp[5] = item[3].c[0x1B];
    tmp[6] = item[3].c[0x18];
    tmp[7] = item[3].c[0x19];
    LocalFileTime.dwHighDateTime = HTDF(tmp,8);

    if (FileTimeToSystemTime(&LocalFileTime, &SysTimeLAstConnexion) && LocalFileTime.dwHighDateTime>0 && LocalFileTime.dwLowDateTime>0)
    {
      snprintf(item[6].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",
               SysTimeLAstConnexion.wYear,SysTimeLAstConnexion.wMonth,SysTimeLAstConnexion.wDay,
               SysTimeLAstConnexion.wHour,SysTimeLAstConnexion.wMinute,SysTimeLAstConnexion.wSecond);
    }else strcpy(item[6].c,"Never");

    //Last password change
    tmp[0] = item[3].c[0x36];
    tmp[1] = item[3].c[0x37];
    tmp[2] = item[3].c[0x34];
    tmp[3] = item[3].c[0x35];
    tmp[4] = item[3].c[0x32];
    tmp[5] = item[3].c[0x33];
    tmp[6] = item[3].c[0x30];
    tmp[7] = item[3].c[0x31];
    LocalFileTime.dwLowDateTime = HTDF(tmp,8);

    tmp[0] = item[3].c[0x3E];
    tmp[1] = item[3].c[0x3F];
    tmp[2] = item[3].c[0x3C];
    tmp[3] = item[3].c[0x3D];
    tmp[4] = item[3].c[0x3A];
    tmp[5] = item[3].c[0x3B];
    tmp[6] = item[3].c[0x38];
    tmp[7] = item[3].c[0x39];
    LocalFileTime.dwHighDateTime = HTDF(tmp,8);

    if (FileTimeToSystemTime(&LocalFileTime, &SysTimeLAstConnexion) && LocalFileTime.dwHighDateTime>0 && LocalFileTime.dwLowDateTime>0)
    {
      snprintf(item[7].c,MAX_PATH,"%02d/%02d/%02d-%02d:%02d:%02d",
               SysTimeLAstConnexion.wYear,SysTimeLAstConnexion.wMonth,SysTimeLAstConnexion.wDay,
               SysTimeLAstConnexion.wHour,SysTimeLAstConnexion.wMinute,SysTimeLAstConnexion.wSecond);

    }else strcpy(item[7].c,"Never\0");


    //Offste 0038, second digit = State
    if (item[3].c[0x71]=='0')strcpy(item[9].c,"State : Enable");
    else strcpy(item[9].c,"State : Disable");

    //Password Expire
    if (item[3].c[0x73]=='2'||item[3].c[0x73]=='3'||item[3].c[0x73]=='6'||item[3].c[0x73]=='7'||item[3].c[0x73]=='A'||item[3].c[0x73]=='B'||item[3].c[0x73]=='E'||item[3].c[0x73]=='F')
      strcat(item[9].c," (Password never expire)\0");

    //Offset 0030 : le RID utilisateur
    tmp[0] = item[3].c[0x66];
    tmp[1] = item[3].c[0x67];
    tmp[2] = item[3].c[0x64];
    tmp[3] = item[3].c[0x65];
    tmp[4] = item[3].c[0x62];
    tmp[5] = item[3].c[0x63];
    tmp[6] = item[3].c[0x60];
    tmp[7] = item[3].c[0x61];
    snprintf(item[3].c,MAX_PATH,"S-1-5-?-?-?-?-%lu",HTDF(tmp,8));

    //nombre de connexion 0x42
    tmp[0] = item[3].c[0x86];
    tmp[1] = item[3].c[0x87];
    tmp[2] = item[3].c[0x84];
    tmp[3] = item[3].c[0x85];
    snprintf(item[8].c,MAX_PATH,"%lu",HTDF(tmp,4));

  }else item[3].c[0]=0;
}
//------------------------------------------------------------------------------
//extraction des données de la valeur C de groupe
//------------------------------------------------------------------------------
//extraction des SID des user par groupe
//#define DATA_USER_HEADER_SIZE  8
/*typedef struct data_user
{
  char valid;       //0x01
  char nb_sid;      //0x05 ou 0x01
  char padding[5];  //0x0000 0000 00
  char type;        //0x05
  char sid[1];      //par defaut découpé en section de 2octet *nb_part
}DATA_USER;*/

//SAM\SAM\Domains\Builtin\Aliases\00000220 le C

//0x10 = emplacement nom de groupe en unicode (taille totale) + 52
//0x14 = taille nom de groupe

//0x1C = emplacmen description + 52
//0x20 = taille description en unicode (taille totale)

//0x28 = début du 1er SID structure + 52
//0x30 = nombre d'instances (user + paddings)
void TraiterGroupDataFromSAM_C(LINE_ITEM *item, HANDLE hlv)
{
  char tmp[MAX_LINE_SIZE];

  //init
  item[2].c[0]=0;
  item[4].c[0]=0;
  item[5].c[0]=0;
  item[6].c[0]=0;
  item[7].c[0]=0;
  item[8].c[0]=0;
  item[9].c[0]=0;
  item[10].c[0]=0;
  item[11].c[0]=0;
  strcpy(item[12].c,item[3].c);
  unsigned long int size_total = strlen(item[3].c);

  //lecture de la taille du nom sur 1 int  = 4octets
  //0x14 = taille nom de groupe
  tmp[0] = item[3].c[46];
  tmp[1] = item[3].c[47];
  tmp[2] = item[3].c[44];
  tmp[3] = item[3].c[45];
  tmp[4] = item[3].c[42];
  tmp[5] = item[3].c[43];
  tmp[6] = item[3].c[40];
  tmp[7] = item[3].c[41];
  unsigned int taille_nom = HTDF(tmp,8)/2;

  //lecture de la taille de la description sur 1 int  = 4octets
  //0x20 = taille de description
  tmp[0] = item[3].c[70];
  tmp[1] = item[3].c[71];
  tmp[2] = item[3].c[68];
  tmp[3] = item[3].c[69];
  tmp[4] = item[3].c[66];
  tmp[5] = item[3].c[67];
  tmp[6] = item[3].c[64];
  tmp[7] = item[3].c[65];
  unsigned int taille_description = HTDF(tmp,8)/2;

  //0x10 = emplacement nom de groupe en unicode (taille totale) + 52
  tmp[0] = item[3].c[38];
  tmp[1] = item[3].c[39];
  tmp[2] = item[3].c[36];
  tmp[3] = item[3].c[37];
  tmp[4] = item[3].c[34];
  tmp[5] = item[3].c[35];
  tmp[6] = item[3].c[32];
  tmp[7] = item[3].c[33];
  unsigned int of_name = (52+ HTDF(tmp,8))*2;

  //0x1C = emplacmen description + 52
  tmp[0] = item[3].c[62];
  tmp[1] = item[3].c[63];
  tmp[2] = item[3].c[60];
  tmp[3] = item[3].c[61];
  tmp[4] = item[3].c[58];
  tmp[5] = item[3].c[59];
  tmp[6] = item[3].c[56];
  tmp[7] = item[3].c[57];
  unsigned int of_description = (52+ HTDF(tmp,8))*2;

  //0x28 = début du 1er SID structure + 52
  tmp[0] = item[3].c[86];
  tmp[1] = item[3].c[87];
  tmp[2] = item[3].c[84];
  tmp[3] = item[3].c[85];
  tmp[4] = item[3].c[82];
  tmp[5] = item[3].c[83];
  tmp[6] = item[3].c[80];
  tmp[7] = item[3].c[81];
  unsigned int of_sid = (52+ HTDF(tmp,8))*2;

  //0x30 = nombre d'instances (user)
  tmp[0] = item[3].c[102];
  tmp[1] = item[3].c[103];
  tmp[2] = item[3].c[100];
  tmp[3] = item[3].c[101];
  tmp[4] = item[3].c[98];
  tmp[5] = item[3].c[99];
  tmp[6] = item[3].c[96];
  tmp[7] = item[3].c[97];
  DWORD nb_sid = HTDF(tmp,8);
  item[3].c[0]=0;

  //2 = name
  if (taille_nom>0 && taille_nom<size_total && of_name>0 && of_name<size_total)
  {
    tmp[0]=0;
    strncpy(tmp,(char*)(item[12].c+of_name),MAX_LINE_SIZE);
    SHexaToString(tmp,item[2].c);
    item[2].c[taille_nom]=0;
  }

  //4 = description
  if (taille_description>0 && taille_description<size_total && of_description>0 && of_description<size_total)
  {
    tmp[0]=0;
    strncpy(tmp,(char*)(item[12].c+of_description),MAX_LINE_SIZE);
    SHexaToString(tmp,item[4].c);
    item[4].c[taille_description]=0;
  }

  //3 = sid
 /* #define DATA_USER_HEADER_SIZE  8
  typedef struct data_user
  {
    char valid;       //0x01
    char nb_sid;      //0x05 ou 0x01
    char padding[5];  //0x0000 0000 00
    char type;        //0x05
    char sid[1];      //par defaut découpé en section de 2octet *nb_part
  }DATA_USER;
  */


  if (of_sid+(DATA_USER_HEADER_SIZE+nb_sid*4)*2>size_total)   //cas de groupes vides
  {
    strcpy(item[5].c, "Group");
    AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
    return;
  }


  DWORD i,j, nb_c_sid;
  char *d,*c = &item[12].c[of_sid];
  for (i=0;i<nb_sid;i++)
  {
    //lecture du nombre de sid
    nb_c_sid = HexaToDecS(c+2);
    if (nb_c_sid>6)nb_c_sid = 6;

    strcpy(item[3].c,"S-1-5");
    //transformation du SID en chaine ^^

    if (nb_c_sid == 1)
    {
      d = c+(DATA_USER_HEADER_SIZE*2);
      tmp[0] = d[6];
      tmp[1] = d[7];
      tmp[2] = d[4];
      tmp[3] = d[5];
      tmp[4] = d[2];
      tmp[5] = d[3];
      tmp[6] = d[0];
      tmp[7] = d[1];

      j = HTDF(tmp,8);
      //suivant le type on ajoute un commentaire ^^
      snprintf(item[3].c,MAX_PATH,"S-1-5-%lu",HTDF(tmp,8));
      switch (j)
      {
        case 1: strncat(item[3].c," (Dialup)\0",MAX_PATH);break;
        case 2: strncat(item[3].c," (Network)\0",MAX_PATH);break;
        case 3: strncat(item[3].c," (Batch)\0",MAX_PATH);break;
        case 4: strncat(item[3].c," (Interative)\0",MAX_PATH);break;
        case 5: strncat(item[3].c," (Driver)\0",MAX_PATH);break;
        case 6: strncat(item[3].c," (Service)\0",MAX_PATH);break;
        case 7: strncat(item[3].c," (Anonymous logon)\0",MAX_PATH);break;
        case 8: strncat(item[3].c," (Proxy)\0",MAX_PATH);break;
        case 9: strncat(item[3].c," (Entreprise domain controllers)\0",MAX_PATH);break;
        case 10: strncat(item[3].c," (Self)\0",MAX_PATH);break;
        case 11: strncat(item[3].c," (Authenticated Users)\0",MAX_PATH);break;
        case 12: strncat(item[3].c," (Restricted)\0",MAX_PATH);break;
        case 13: strncat(item[3].c," (Terminal server user)\0",MAX_PATH);break;
        case 14: strncat(item[3].c," (Remote interactive logon)\0",MAX_PATH);break;
        case 15: strncat(item[3].c," (This Organization)\0",MAX_PATH);break;
        case 18: strncat(item[3].c," (System)\0",MAX_PATH);break;
        case 19: strncat(item[3].c," (Local service)\0",MAX_PATH);break;
        case 20: strncat(item[3].c," (Network service)\0",MAX_PATH);break;
      }
    }else
    {
      d = c+(DATA_USER_HEADER_SIZE*2);
      for (j=0;j<nb_c_sid;j++)
      {
        tmp[0] = d[6];
        tmp[1] = d[7];
        tmp[2] = d[4];
        tmp[3] = d[5];
        tmp[4] = d[2];
        tmp[5] = d[3];
        tmp[6] = d[0];
        tmp[7] = d[1];
        snprintf(item[5].c,MAX_PATH,"-%lu",HTDF(tmp, 8));
        strncat(item[3].c,item[5].c,MAX_LINE_SIZE);
        d = d+8;
      }
      strncat(item[3].c,"\0",MAX_LINE_SIZE);
    }

    //ajout de l'item ^^
    strcpy(item[5].c, "Group");
    AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);

    //passage au suivant ^^
    c = c+((DATA_USER_HEADER_SIZE+nb_c_sid*4)*2);
  }
}
//------------------------------------------------------------------------------
//extraction des données de la valeur V du profil
int TraiterUserDataFromSAM_V(LINE_ITEM *item)
{
  unsigned int rid = 0;

  char tmp[MAX_PATH],tmp2[MAX_PATH],tmp3[MAX_PATH];
  //Data = item[3].c
  //Name = col2 + Description = col4
  item[2].c[0]=0;
  item[4].c[0]=0;
  item[9].c[0]=0;
  item[10].c[0]=0;
  item[11].c[0]=0;
  strcpy(item[12].c,item[3].c);

  unsigned long int size_total = strlen(item[3].c);

  //possibilité aussi de chercher :
  //chercher dans la chaine la chaine suivante : 000001020000000520000000200200000102000000052000000020020000
  //+Nom(Wildstring) + 0000 + Description + 0100/0102/FFFF

  //lecture de la taille du nom sur 1 int  = 4octets
  //0x10 = taille nom user
  tmp[0] = item[3].c[38];
  tmp[1] = item[3].c[39];
  tmp[2] = item[3].c[36];
  tmp[3] = item[3].c[37];
  tmp[4] = item[3].c[34];
  tmp[5] = item[3].c[35];
  tmp[6] = item[3].c[32];
  tmp[7] = item[3].c[33];
  unsigned int taille_nom = HTDF(tmp,8)/2;

  //lecture de la taille du nom complet sur 1 int  = 4octets
  //0x1C = taille du nom complet
  tmp[0] = item[3].c[62];
  tmp[1] = item[3].c[63];
  tmp[2] = item[3].c[60];
  tmp[3] = item[3].c[61];
  tmp[4] = item[3].c[58];
  tmp[5] = item[3].c[59];
  tmp[6] = item[3].c[56];
  tmp[7] = item[3].c[57];
  unsigned int taille_full_name = HTDF(tmp,8)/2;

  //lecture de la taille de la description sur 1 int  = 4octets
  //0x28 = taille de description
  tmp[0] = item[3].c[86];
  tmp[1] = item[3].c[87];
  tmp[2] = item[3].c[84];
  tmp[3] = item[3].c[85];
  tmp[4] = item[3].c[82];
  tmp[5] = item[3].c[83];
  tmp[6] = item[3].c[80];
  tmp[7] = item[3].c[81];
  unsigned int taille_description = HTDF(tmp,8)/2;

  //emplacement du nom (taille de la strcuturitem[10].c[0]=0;e d'entête + emplacement)
  tmp[0] = item[3].c[30];
  tmp[1] = item[3].c[31];
  tmp[2] = item[3].c[28];
  tmp[3] = item[3].c[29];
  tmp[4] = item[3].c[26];
  tmp[5] = item[3].c[27];
  tmp[6] = item[3].c[24];
  tmp[7] = item[3].c[25];
  unsigned int of_name = (204+ HTDF(tmp,8))*2;

  //emplacement de la description (taille de la strcuture d'entête + emplacement)
  tmp[0] = item[3].c[54];
  tmp[1] = item[3].c[55];
  tmp[2] = item[3].c[52];
  tmp[3] = item[3].c[53];
  tmp[4] = item[3].c[50];
  tmp[5] = item[3].c[51];
  tmp[6] = item[3].c[48];
  tmp[7] = item[3].c[49];
  unsigned int of_full_name = (204+ HTDF(tmp,8))*2;

  //emplacement de la description (taille de la strcuture d'entête + emplacement)
  tmp[0] = item[3].c[78];
  tmp[1] = item[3].c[79];
  tmp[2] = item[3].c[76];
  tmp[3] = item[3].c[77];
  tmp[4] = item[3].c[74];
  tmp[5] = item[3].c[75];
  tmp[6] = item[3].c[72];
  tmp[7] = item[3].c[73];
  unsigned int of_description = (204+ HTDF(tmp,8))*2;

  //password hash
  tmp[0] = item[3].c[318];
  tmp[1] = item[3].c[319];
  tmp[2] = item[3].c[316];
  tmp[3] = item[3].c[317];
  tmp[4] = item[3].c[314];
  tmp[5] = item[3].c[315];
  tmp[6] = item[3].c[312];
  tmp[7] = item[3].c[313];
  unsigned int of_lmpw = (204+ HTDF(tmp,8))*2;

  tmp[0] = item[3].c[342];
  tmp[1] = item[3].c[343];
  tmp[2] = item[3].c[340];
  tmp[3] = item[3].c[341];
  tmp[4] = item[3].c[338];
  tmp[5] = item[3].c[339];
  tmp[6] = item[3].c[336];
  tmp[7] = item[3].c[337];
  unsigned int of_ntpw = (204+ HTDF(tmp,8))*2;

  tmp[0] = item[3].c[326];
  tmp[1] = item[3].c[327];
  tmp[2] = item[3].c[324];
  tmp[3] = item[3].c[325];
  tmp[4] = item[3].c[322];
  tmp[5] = item[3].c[323];
  tmp[6] = item[3].c[320];
  tmp[7] = item[3].c[321];
  unsigned int taille_lmpw = HTDF(tmp,8)*2;

  tmp[0] = item[3].c[350];
  tmp[1] = item[3].c[351];
  tmp[2] = item[3].c[348];
  tmp[3] = item[3].c[349];
  tmp[4] = item[3].c[346];
  tmp[5] = item[3].c[347];
  tmp[6] = item[3].c[344];
  tmp[7] = item[3].c[345];
  unsigned int taille_ntpw = HTDF(tmp,8)*2;

  //lecture du nom
  if (taille_nom>0 && taille_nom<size_total && of_name>0 && of_name<size_total)
  {
    tmp[0]=0;
    strncpy(tmp,(char*)(item[3].c+of_name),MAX_PATH);
    SHexaToString(tmp,item[2].c);
    item[2].c[taille_nom]=0;
  }
  //lecture de la description (fullname)
  if (taille_full_name>0 && taille_full_name<size_total && of_full_name>0 && of_full_name<size_total)
  {
    tmp[0]=0;
    strncpy(tmp,(char*)(item[3].c+of_full_name),MAX_PATH);
    SHexaToString(tmp,item[4].c);
    item[4].c[taille_full_name]=0;
  }

  //lecture de la description (comment)
  if (taille_description>0 && taille_description<size_total && of_description>0 && of_description<size_total)
  {
    tmp[0]=0;
    strncpy(tmp,(char*)(item[3].c+of_description),MAX_PATH);
    SHexaToString(tmp,tmp2);
    tmp2[taille_description]=0;
    strncat(item[4].c,", ",MAX_PATH);
    strncat(item[4].c,tmp2,MAX_PATH);
    strncat(item[4].c,"\0",MAX_PATH);
  }

  //Ajout du type de compte ^^
  if ((item[3].c[8]=='B' || item[3].c[8]=='b') && (item[3].c[9]=='C' || item[3].c[9]=='c')) //Administrateur
    strncat(item[4].c," (Administrator)\0",MAX_PATH);
  else if ((item[3].c[8]=='B' || item[3].c[8]=='b') && item[3].c[9]=='0') //Guest
    strncat(item[4].c," (Guest)\0",MAX_PATH);
  else if ((item[3].c[8]=='D' || item[3].c[8]=='d') && item[3].c[9]=='4') //Guest
    strncat(item[4].c," (User)\0",MAX_PATH);

  //Group = col5
  item[5].c[0]=0;

  //Last logon = col6
  item[6].c[0]=0;
  //Last password change = col7
  item[7].c[0]=0;
  //Bytes 9–16 store the last log-on time

  //Nb Connection = col8
  item[8].c[0]=0;

  //State = col9
  item[9].c[0]=0;

  //SID : après 12 octets donc 24 caractères + dernière clée : 2o donc 4caractères : 2400 4400 0200 0105 0000 0000 0005 1500 0000
  //fin : 0000
  //SID = col3
  unsigned long int type_id = 0, type_id2=0, last_id=0;
  unsigned long int i = Contient(item[3].c,"2400440002000105000000000005"); // 1500 0000 = 21 le SID de début
  if (i>0 && i<(strlen(item[3].c)-40))
  {
    //création du SID : 4o-4o-4o-4o-4o
    sprintf(tmp,"%c%c%c%c%c%c%c%c",item[3].c[i+6],item[3].c[i+7],item[3].c[i+4],item[3].c[i+5],item[3].c[i+2],item[3].c[i+3],item[3].c[i],item[3].c[i+1]);
    type_id = HTD(tmp);
    snprintf(tmp2,MAX_PATH,"S-1-5-%lu",type_id);
    strcpy(tmp3,tmp2);

    sprintf(tmp,"%c%c%c%c%c%c%c%c",item[3].c[i+14],item[3].c[i+15],item[3].c[i+12],item[3].c[i+13],item[3].c[i+10],item[3].c[i+11],item[3].c[i+8],item[3].c[i+9]);
    type_id2 = HTD(tmp);
    snprintf(tmp2,MAX_PATH,"-%lu",type_id2);
    strncat(tmp3,tmp2,MAX_PATH);

    sprintf(tmp,"%c%c%c%c%c%c%c%c",item[3].c[i+22],item[3].c[i+23],item[3].c[i+20],item[3].c[i+21],item[3].c[i+18],item[3].c[i+19],item[3].c[i+16],item[3].c[i+17]);
    snprintf(tmp2,MAX_PATH,"-%lu",HTD(tmp));
    strncat(tmp3,tmp2,MAX_PATH);

    sprintf(tmp,"%c%c%c%c%c%c%c%c",item[3].c[i+30],item[3].c[i+31],item[3].c[i+28],item[3].c[i+29],item[3].c[i+26],item[3].c[i+27],item[3].c[i+24],item[3].c[i+25]);
    snprintf(tmp2,MAX_PATH,"-%lu",HTD(tmp));
    strncat(tmp3,tmp2,MAX_PATH);

    sprintf(tmp,"%c%c%c%c%c%c%c%c",item[3].c[i+38],item[3].c[i+39],item[3].c[i+36],item[3].c[i+37],item[3].c[i+34],item[3].c[i+35],item[3].c[i+32],item[3].c[i+33]);
    last_id = HTD(tmp);
    snprintf(tmp2,MAX_PATH,"-%lu",last_id);
    strncat(tmp3,tmp2,MAX_PATH);
    strncat(tmp3,"\0",MAX_PATH);

    strcpy(item[3].c,tmp3);
  }else
  {
    if (item[1].c[0]!=0)
    {
      //extract Compte ID
      last_id = 0;
      char *c = item[1].c;
      while (*c)c++;
      c--;
      while (*c != '\\')c--;
      c++;
      last_id = HTDF(c,8);

      snprintf(item[3].c,MAX_PATH,"S-1-5-?-?-?-?-%lu",last_id);

    }else item[3].c[0]=0;
  }

  rid = last_id;

  //suivant le type d'id on ajoute des infos de compte :
  switch (type_id)
  {
    case 1: strncat(item[4].c," Rights : Dialup\0",MAX_PATH);
    case 2: strncat(item[4].c," Rights : Network\0",MAX_PATH);
    case 3: strncat(item[4].c," Rights : Batch\0",MAX_PATH);
    case 4: strncat(item[4].c," Rights : Interative\0",MAX_PATH);
    case 5: strncat(item[4].c," Rights : Driver\0",MAX_PATH);
    case 6: strncat(item[4].c," Rights : Service\0",MAX_PATH);
    case 7: strncat(item[4].c," Rights : Anonymous logon\0",MAX_PATH);
    case 8: strncat(item[4].c," Rights : Proxy\0",MAX_PATH);
    case 9: strncat(item[4].c," Rights : Entreprise domain controllers\0",MAX_PATH);
    case 10: strncat(item[4].c," Rights : Self\0",MAX_PATH);
    case 11: strncat(item[4].c," Rights : Authenticated Users\0",MAX_PATH);
    case 12: strncat(item[4].c," Rights : Restricted\0",MAX_PATH);
    case 13: strncat(item[4].c," Rights : Terminal server user\0",MAX_PATH);
    case 14: strncat(item[4].c," Rights : Remote interactive logon\0",MAX_PATH);
    case 15: strncat(item[4].c," Rights : This Organization\0",MAX_PATH);
    case 18: strncat(item[4].c," Rights : System\0",MAX_PATH);
    case 19: strncat(item[4].c," Rights : Local service\0",MAX_PATH);
    case 20: strncat(item[4].c," Rights : Network service\0",MAX_PATH);
    case 21:
      switch(last_id)
      {
        case 500 : strncat(item[4].c," Rights : Local Administrator\0",MAX_PATH);break;
        case 501 : strncat(item[4].c," Rights : Local Guest\0",MAX_PATH);break;
        case 502 : strncat(item[4].c," Rights : Krbtgt\0",MAX_PATH);break;
        case 512 : strncat(item[4].c," Rights : Domain Admins\0",MAX_PATH);break;
        case 513 : strncat(item[4].c," Rights : Domain Users\0",MAX_PATH);break;
        case 514 : strncat(item[4].c," Rights : Domain Guests\0",MAX_PATH);break;
        case 515 : strncat(item[4].c," Rights : Domain Computers\0",MAX_PATH);break;
        case 516 : strncat(item[4].c," Rights : Domain Controllers\0",MAX_PATH);break;
        case 517 : strncat(item[4].c," Rights : Cert Publishers\0",MAX_PATH);break;
        case 518 : strncat(item[4].c," Rights : Schema Admins\0",MAX_PATH);break;
        case 519 : strncat(item[4].c," Rights : Enterprise Admins\0",MAX_PATH);break;
        case 520 : strncat(item[4].c," Rights : Group Policy Creator Owners\0",MAX_PATH);break;
        case 553 : strncat(item[4].c," Rights : RAS and IAS Servers\0",MAX_PATH);break;
      }
    break;
    case 32:
      switch(last_id)
      {
        case 544 : strncat(item[4].c," Rights : Administrators\0",MAX_PATH);break;
        case 545 : strncat(item[4].c," Rights : Users\0",MAX_PATH);break;
        case 546 : strncat(item[4].c," Rights : Guests\0",MAX_PATH);break;
        case 547 : strncat(item[4].c," Rights : Power Users\0",MAX_PATH);break;
        case 548 : strncat(item[4].c," Rights : Account Operators\0",MAX_PATH);break;
        case 549 : strncat(item[4].c," Rights : Server Operators\0",MAX_PATH);break;
        case 550 : strncat(item[4].c," Rights : Print Operators\0",MAX_PATH);break;
        case 551 : strncat(item[4].c," Rights : Backup Operators\0",MAX_PATH);break;
        case 552 : strncat(item[4].c," Rights : Replicator \0",MAX_PATH);break;
        case 554 : strncat(item[4].c," Rights : Pre-Windows 2000 Compatible Access\0",MAX_PATH);break;
        case 555 : strncat(item[4].c," Rights : Remote Desktop Users\0",MAX_PATH);break;
        case 556 : strncat(item[4].c," Rights : Network Configuration Operators\0",MAX_PATH);break;
        case 557 : strncat(item[4].c," Rights : Incoming Forest Trust Builders\0",MAX_PATH);break;
        case 558 : strncat(item[4].c," Rights : Performance Monitor Users\0",MAX_PATH);break;
        case 559 : strncat(item[4].c," Rights : Performance Log Users\0",MAX_PATH);break;
        case 560 : strncat(item[4].c," Rights : Windows Authorization Access Group\0",MAX_PATH);break;
        case 561 : strncat(item[4].c," Rights : Terminal Server License Servers\0",MAX_PATH);break;
      }
    break;
    case 64:
      switch(last_id)
      {
        case 10 : strncat(item[4].c," Rights : NTLM Authentication\0",MAX_PATH);break;
        case 14 : strncat(item[4].c," Rights : SChannel Authentication\0",MAX_PATH);break;
        case 21 : strncat(item[4].c," Rights : Digest Authentication\0",MAX_PATH);break;
      }
    break;
  }

  //hash NT::LM
  tmp2[0]=0;
  tmp3[0]=0;
  if (taille_lmpw>8 && taille_lmpw<size_total && of_lmpw>0 && (of_lmpw + taille_lmpw+8)<size_total)
  {
    if (taille_lmpw > 32)taille_lmpw = 32;
    strncpy(tmp2,item[12].c+of_lmpw+8,MAX_PATH);
    tmp2[taille_lmpw]=0;
  }else strcpy(tmp2,"<NO LM PASSWORD>");

  if (taille_ntpw>8 && taille_ntpw<size_total && of_ntpw>0 && (of_ntpw + taille_ntpw+8)<size_total)
  {
    if (taille_ntpw > 32)taille_ntpw = 32;
    strncpy(tmp3,item[12].c+of_ntpw+8,MAX_PATH);
    tmp3[taille_ntpw]=0;
  }else strcpy(tmp3,"<NO NT PASSWORD>");

  if (tmp2[0]!=0 && tmp3[0]!=0)
  {
    //pwdump format
    //<user>:<id>:<lanman pw>:<NT pw>:comment:homedir:
    //snprintf(item[10].c,MAX_PATH,"%s:%lu:%s:%s:::",item[2].c,last_id,tmp2,tmp3);
    snprintf(item[10].c,MAX_PATH,":%s:%s",tmp2,tmp3);
  }

  return rid;
}
//------------------------------------------------------------------------------
void GetWindowsCDKeySHex(char * lpszBuffer)
{
	int nCur			= 0;

	BYTE lpRawData[MAX_PATH];
	BYTE lpEncSerial[MAX_PATH];

	char lpszSerial[MAX_PATH];
	char lpszTemp[MAX_PATH];

	char lpKeyChars[25] = "BCDFGHJKMPQRTVWXY2346789";
	int i,j,c;

	//On transforme le sHexa en Binaire
	for (i=0,j=0;i<strlen(lpszBuffer) && i<MAX_PATH;i+=2,j++)
  {
    lpRawData[j]=HexaToDecS(&lpszBuffer[i]);
  }

	//decrypyte
  for(i=52;i<=66;i++)
    lpEncSerial[i-52] = lpRawData[i];

  for(i=24;i>=0;i--)
  {
    nCur = 0;
    for(c=14;c>-1;c--)
    {
      nCur = nCur * 256;
      nCur ^= lpEncSerial[c];
      lpEncSerial[c] = nCur / 24;
      nCur %= 24;
    }
    lpszSerial[i] = lpKeyChars[nCur];
  }

  lpszBuffer[0]=0;
  for(i=0;lpszSerial[i] && (i+i/5) < 30;i++){
    if(i % 5 == 0 && i>0) lstrcat(lpszBuffer,"-");
    wsprintf(lpszTemp,"%c",lpszSerial[i]);
    lstrcat(lpszBuffer,lpszTemp);
    ZeroMemory(lpszTemp,MAX_PATH);
  }
}
//------------------------------------------------------------------------------
//au moment d'ajouter un item on vérifie s'il n'appartient pas à un onglet ^^
void FiltreRegData(LINE_ITEM *item)
{
  //recherche par path puis traitement suivant le type de donnée
  unsigned long int i,j,k,l;
  HANDLE hlv;
  BOOL test_ok;
  char tmp[MAX_LINE_SIZE];
  //MessageBox(0,item[1].c,"TEST!",MB_OK|MB_TOPMOST);

  AddToLV_Registry(item);

  //Path sans quitter (spécial)
  if (!strcmp(item[2].c,"Path") || !strcmp(item[2].c,"InstallerLocation") || !strcmp(item[2].c,"Installation Sources") || Contient(item[1].c,"Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"))
  {
    AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PATH), item, NB_COLONNE_LV[LV_REGISTRY_PATH_NB_COL]);
  }
  if (item[2].c == 0 && (Contient(item[1].c,"\\shell\\open\\command") || Contient(item[1].c,"\\shell\\Open\\command")))
  {
    AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PATH), item, NB_COLONNE_LV[LV_REGISTRY_PATH_NB_COL]);
  }

  //Autoruns
  for (i=0;i<NB_MAX_REF_SEARCH_AUTORUN;i++)
  {
    if (Contient(item[1].c,ref_autorun_search[i].v))
    {
      AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_START), item, NB_COLONNE_LV[LV_REGISTRY_START_NB_COL]);
      return;
    }
  }

  //UserAssist
  for (i=0;i<NB_MAX_REF_SEARCH_HIDDENLOG;i++)
  {
    if (Contient(item[1].c,ref_hiddenlog_search[i].v))
    {
      //traitement des données avant ajout ^^
      //conversion du nom de la clée
      ROTTOASCII(item[2].c,tmp,strlen(item[2].c));
      strcpy(item[2].c,tmp);
      //lecture des données
      TraiterDataUserassist(item);
      AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_HISTORIQUE), item, NB_COLONNE_LV[LV_REGISTRY_HISTORIQUE_NB_COL] );
      return;
    }
  }

  //Configuration
  if (item[3].c[0]!=0)
  {
    for (i=0;i<NB_MAX_REF_SEARCH_CONF;i++)
    {
      if (Contient(item[1].c,ref_conf_search[i].v))
      {
        //on test si la bonne valeure
        for (j=0;j<NB_MAX_REF_SEARCH_CONF_VAR;j+=2)
        {
          if (!strcmp(item[2].c,ref_conf_var_search[j].v))
          {
            strcpy(item[4].c,ref_conf_var_search[j+1].v);

            AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), item, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
            return;
          }
        }
      }
    }

    //cas des serials ms
    if (Contient(item[1].c,"Windows NT\\CurrentVersion"))
    {
      if (!strcmp(item[2].c,"DigitalProductId"))
      {
        //on transcode l'hexastring en binaire et on décrypte
        GetWindowsCDKeySHex(item[3].c);

        strcpy(item[4].c,"(Serial) MS product serial");
        AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), item, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
        return;
      }
    }

    if (Contient(item[1].c,"Microsoft\\Command Processor"))
    {
      if (!strcmp(item[2].c,"Autorun"))
      {
        strcpy(item[4].c,"(malware) Command to execute with all cmd command");
        AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), item, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
        return;
      }
    }
    if (Contient(item[1].c,"CurrentControlSet\\Control\\Session Manager\\Memory Management"))
    {
      if (!strcmp(item[2].c,"ClearPageFileAtShutdown"))
      {
        strcpy(item[4].c,"(attack vector) 0x01:Enable, clear virtual memory pagefile when system shuts down");
        AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), item, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
        return;
      }
    }
    if (Contient(item[1].c,"CurrentVersion\\Setup\\RecoveryConsole"))
    {
      if (!strcmp(item[2].c,"SecurityLevel"))
      {
        strcpy(item[4].c,"(attack vector) 0x00:Disable recovery Console, allow automatic administrative logon with no password");
        AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), item, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
        return;
      }
    }
    if (Contient(item[1].c,"CurrentControlSet\\Services\\Netlogon\\Parameters"))
    {
      if (!strcmp(item[2].c,"requirestrongkey"))
      {
        strcpy(item[4].c,"(authentication) 0x01:Enable secure channel, require strong (Windows 2000 or later) session key");
        AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), item, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
        return;
      }
    }

    //liste des path des différentes hives de la base de registre
    if (Contient(item[1].c,"Control\\hivelist"))
    {
      if (strlen(item[2].c)>0)
      {
        strcpy(item[4].c,"(configuration) Registry hive path");
        AddToLV(GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_CONF), item, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
        return;
      }
    }
  }

  //Software
  for (i=0;i<NB_MAX_REF_SEARCH_SOFTWARE;i++)
  {
    if (Contient(item[1].c,ref_software_search[i].v))
    {
      //recherche de data
      for (j=0;j<NB_MAX_REF_SEARCH_SOFTWARE_VAR;j++)
      {
        if (!strcmp(ref_software_var_search[j].v,item[2].c))
        {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LOGICIEL);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }

          if (test_ok) //ajout à l'item
          {
            if (j==0 || j==1)// nom du package
            {
              //application reconnues ?
              if ((item[3].c[0] == 'M' &&(item[3].c[5] == 'w' || item[3].c[7] == 'j' || item[3].c[8] == 't' || (item[3].c[1] == 'S' && item[3].c[2] == 'D' && item[3].c[3] == 'N')))
               || (item[3].c[0] == 'C' && item[3].c[8] == 'f')||(item[3].c[0] == 'H' && item[3].c[5] == 'x')||(item[3].c[0] == 'S' && item[3].c[9] == 'U')
               || (item[3].c[0] == 'W' && item[3].c[6] == 's' && item[3].c[8] != 'L')
               || (item[3].c[0] == 'L' && item[3].c[8] != 'W' && item[3].c[16] != 'M')) ListView_SetItemText(hlv,l,8,"OK");

              ListView_SetItemText(hlv,l,2,item[3].c);
            }else if (j==3)//date formatage
            {
              tmp[0]=0;
              strcpy(tmp,item[3].c);
              tmp[4]='/';
              tmp[5]=item[3].c[4];
              tmp[6]=item[3].c[5];
              tmp[7]='/';
              tmp[8]=item[3].c[6];
              tmp[9]=item[3].c[7];
              tmp[10]=0;
              ListView_SetItemText(hlv,l,4,tmp);
            }else if (j==2) {ListView_SetItemText(hlv,l,3,item[3].c);}//publisher
            else if (j>=4 && j<8){ListView_SetItemText(hlv,l,5,item[3].c);}//UninstallString
            else if (j==8) {ListView_SetItemText(hlv,l,6,item[3].c);}
            else if (j>=9) {ListView_SetItemText(hlv,l,7,item[3].c);}
          }else//nouvel item
          {
            //copie des données normales
            tmp[0]=0;
            strcpy(tmp,item[3].c);
            item[2].c[0]=0;

            if (j==0 || j==1)// nom du package
            {
              //application reconnues ?
              if ((item[3].c[0] == 'M' &&(item[3].c[5] == 'w' || item[3].c[7] == 'j' || item[3].c[8] == 't' || (item[3].c[1] == 'S' && item[3].c[2] == 'D' && item[3].c[3] == 'N')))
               || (item[3].c[0] == 'C' && item[3].c[8] == 'f')||(item[3].c[0] == 'H' && item[3].c[5] == 'x')||(item[3].c[0] == 'S' && item[3].c[9] == 'U')
               || (item[3].c[0] == 'W' && item[3].c[6] == 's' && item[3].c[8] != 'L')
               || (item[3].c[0] == 'L' && item[3].c[8] != 'W' && item[3].c[16] != 'M')) strcpy(item[8].c,"OK");

              strcpy(item[2].c,tmp);
              item[3].c[0]=0;
            }else if (j==3)//date formatage
            {
              tmp[4]='/';
              tmp[5]=item[3].c[4];
              tmp[6]=item[3].c[5];
              tmp[7]='/';
              tmp[8]=item[3].c[6];
              tmp[9]=item[3].c[7];
              tmp[10]=0;
              strcpy(item[5].c,tmp);
              item[3].c[0]=0;
            }else if (j==2)//date formatage
            {
              strcpy(item[3].c,tmp);
            }else if (j>=4 && j<8)
            {
              item[3].c[0]=0;
              strcpy(item[4].c,tmp);
            }else if (j==8)
            {
              item[3].c[0]=0;
              strcpy(item[6].c,tmp);
            }else if (j>=9)
            {
              item[3].c[0]=0;
              strcpy(item[7].c,tmp);
            }
            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_LOGICIEL_NB_COL]);
          }
          return;
        }
      }
    }
  }

  //Update
  for (i=0;i<NB_MAX_REF_SEARCH_UPDATE;i++)
  {
    if (Contient(item[1].c,ref_update_search[i].v))
    {
      //recherche de data Manuel
      if (!strcmp(item[2].c,"PackageName")||!strcmp(item[2].c,"DisplayName")||!strcmp(item[2].c,"InstallName")) //Nom : col3
      {
        //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
        hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MAJ);
        k = ListView_GetItemCount(hlv);
        test_ok = FALSE;

        for (l=0;l<k;l++)
        {
          tmp[0]=0;
          ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
          if (!strcmp(tmp,item[1].c))
          {
            test_ok = TRUE;
            break;
          }
        }

        //trouvé on ajoute seulement l'item
        if (test_ok) ListView_SetItemText(hlv,l,3,item[3].c)
        else //ajout d'une nouvelle ligne
        {
          item[2].c[0]=0;
          item[4].c[0]=0;
          item[6].c[0]=0;
          AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
        }
      }else if (!strcmp(item[2].c,"Publisher")) //Component : col 2
      {
        //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
        hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MAJ);
        k = ListView_GetItemCount(hlv);
        test_ok = FALSE;

        for (l=0;l<k;l++)
        {
          tmp[0]=0;
          ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
          if (!strcmp(tmp,item[1].c))
          {
            test_ok = TRUE;
            break;
          }
        }

        //trouvé on ajoute seulement l'item
        if (test_ok) ListView_SetItemText(hlv,l,2,item[3].c)
        else //ajout d'une nouvelle ligne
        {
          strcpy(item[2].c,item[3].c);
          item[3].c[0]=0;
          item[4].c[0]=0;
          item[6].c[0]=0;
          AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
        }
      }else if (!strcmp(item[2].c,"Description")||!strcmp(item[2].c,"InstallLocation")) //Description : col4 ou InstallLocation
      {
        //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
        hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MAJ);
        k = ListView_GetItemCount(hlv);
        test_ok = FALSE;

        for (l=0;l<k;l++)
        {
          tmp[0]=0;
          ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
          if (!strcmp(tmp,item[1].c))
          {
            test_ok = TRUE;
            break;
          }
        }

        //trouvé on ajoute seulement l'item
        if (test_ok) ListView_SetItemText(hlv,l,4,item[3].c)
        else //ajout d'une nouvelle ligne
        {
          strcpy(item[4].c,item[3].c);
          item[3].c[0]=0;
          item[2].c[0]=0;
          item[6].c[0]=0;
          AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
        }
      }else if (!strcmp(item[2].c,"InstalledDate")) //InstalledDate : col5
      {
        //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
        hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MAJ);
        k = ListView_GetItemCount(hlv);
        test_ok = FALSE;

        for (l=0;l<k;l++)
        {
          tmp[0]=0;
          ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
          if (!strcmp(tmp,item[1].c))
          {
            test_ok = TRUE;
            break;
          }
        }

        //trouvé on ajoute seulement l'item
        if (test_ok) ListView_SetItemText(hlv,l,5,item[3].c)
        else //ajout d'une nouvelle ligne
        {
          strcpy(item[5].c,item[3].c);
          item[3].c[0]=0;
          item[4].c[0]=0;
          item[2].c[0]=0;
          item[6].c[0]=0;
          AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
        }
      }else if (!strcmp(item[2].c,"InstalledBy")||!strcmp(item[2].c,"InstallUser")) //InstallUser : col : 6
      {
        //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
        hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MAJ);
        k = ListView_GetItemCount(hlv);
        test_ok = FALSE;

        for (l=0;l<k;l++)
        {
          tmp[0]=0;
          ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
          if (!strcmp(tmp,item[1].c))
          {
            test_ok = TRUE;
            break;
          }
        }

        //trouvé on ajoute seulement l'item
        if (test_ok) ListView_SetItemText(hlv,l,6,item[3].c)
        else //ajout d'une nouvelle ligne
        {
          strcpy(item[6].c,item[3].c);
          item[3].c[0]=0;
          item[4].c[0]=0;
          item[2].c[0]=0;
          AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
        }
      }
      return;
    }
  }

  //USB
  for (i=0;i<NB_MAX_REF_SEARCH_USB;i++)
  {
    if (Contient(item[1].c,ref_usb_search[i].v))
    {
      if (!strcmp(item[2].c,"FriendlyName"))
      {
        //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
        hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USB);
        k = ListView_GetItemCount(hlv);
        test_ok = FALSE;

        for (l=0;l<k;l++)
        {
          tmp[0]=0;
          ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
          if (!strcmp(tmp,item[1].c))
          {
            test_ok = TRUE;
            break;
          }
        }

        //trouvé on ajoute seulement l'item
        if (test_ok) ListView_SetItemText(hlv,l,2,item[3].c)
        else //ajout d'une nouvelle ligne
        {
          strcpy(item[2].c,item[3].c);
          strcpy(item[4].c,item[5].c);
          item[3].c[0]=0;
          AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_USB_NB_COL]);
        }
      }else if (!strcmp(item[2].c,"Class")||!strcmp(item[2].c,"Mfg"))
      {
        //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
        hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USB);
        k = ListView_GetItemCount(hlv);
        test_ok = FALSE;

        for (l=0;l<k;l++)
        {
          tmp[0]=0;
          ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
          if (!strcmp(tmp,item[1].c))
          {
            test_ok = TRUE;
            break;
          }
        }

        //trouvé on ajoute seulement l'item
        if (test_ok)
        {
          //on récupère le contenu de la colonne et on y ajoute les données
          tmp[0]=0;
          ListView_GetItemText(hlv,l,3,tmp,MAX_LINE_SIZE);
          strncat(item[3].c,tmp,MAX_LINE_SIZE);
          strncat(item[3].c,"\0",MAX_LINE_SIZE);
          ListView_SetItemText(hlv,l,3,item[3].c)
        }else //ajout d'une nouvelle ligne
        {
          item[2].c[0]=0;
          strcpy(item[4].c,item[5].c);
          AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_USB_NB_COL]);
        }
      }
      return;
    }
  }

  //Réseau
  for (i=0;i<NB_MAX_REF_SEARCH_NETWORK;i++)
  {
    if (Contient(item[1].c,ref_network_search[i].v))
    {
      if (strcmp(item[3].c,"0.0.0.0") && strcmp(item[3].c,"30002e0030002e0030002e00300000000000") && strlen(item[3].c)>6) //si pas une fausse adresse on utilise
      {
        if (!strcmp(item[2].c,"IPAddress")||!strcmp(item[2].c,"DhcpIPAddress")) // col 3
        {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }

          if (item[3].c[2]=='0' && item[3].c[3]=='0')//si au format Hexa Wildstring
          {
            tmp[0]=0;
            SHexaToString(item[3].c,tmp);
            strcpy(item[3].c,tmp);
          }

          if (test_ok)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,3,tmp,MAX_LINE_SIZE);
            if(strlen(tmp)>6)
            {
              snprintf(item[0].c,MAX_LINE_SIZE,"%s/%s",item[3].c,tmp);
              ListView_SetItemText(hlv,l,3,item[0].c);
            }else ListView_SetItemText(hlv,l,3,item[3].c);
          }else
          {
            item[2].c[0]=0;
            item[4].c[0]=0;
            item[5].c[0]=0;
            item[6].c[0]=0;
            item[7].c[0]=0;
            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
          }
        }else if (!strcmp(item[2].c,"SubnetMask")||!strcmp(item[2].c,"DhcpSubnetMask")) //ip/netmask col 3
        {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }

          if (item[3].c[2]=='0' && item[3].c[3]=='0')//si au format Hexa Wildstring
          {
            tmp[0]=0;
            SHexaToString(item[3].c,tmp);
            strcpy(item[3].c,tmp);
          }

          if (test_ok)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,3,tmp,MAX_LINE_SIZE);
            if(strlen(tmp)>6)
            {
              snprintf(item[0].c,MAX_LINE_SIZE,"%s/%s",tmp,item[3].c);
              ListView_SetItemText(hlv,l,3,item[0].c);
            }else ListView_SetItemText(hlv,l,3,item[3].c);

          }else
          {
            item[2].c[0]=0;
            item[4].c[0]=0;
            item[5].c[0]=0;
            item[6].c[0]=0;
            item[7].c[0]=0;
            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
          }
        }else if (!strcmp(item[2].c,"NameServer")||!strcmp(item[2].c,"DhcpNameServer")) //DNS col 4
        {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }

          if (item[3].c[2]=='0' && item[3].c[3]=='0')//si au format Hexa Wildstring
          {
            tmp[0]=0;
            SHexaToString(item[3].c,tmp);
            strcpy(item[3].c,tmp);
          }

          if (test_ok)ListView_SetItemText(hlv,l,4,item[3].c)
          else
          {
            item[2].c[0]=0;
            strcpy(item[4].c,item[3].c);
            item[3].c[0]=0;
            item[5].c[0]=0;
            item[6].c[0]=0;
            item[7].c[0]=0;
            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
          }
        }else if (!strcmp(item[2].c,"DefaultGateway")||!strcmp(item[2].c,"DhcpDefaultGateway")) //Passerelle col 5
        {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }

          if (item[3].c[2]=='0' && item[3].c[3]=='0')//si au format Hexa Wildstring
          {
            tmp[0]=0;
            SHexaToString(item[3].c,tmp);
            strcpy(item[3].c,tmp);
          }

          if (test_ok)ListView_SetItemText(hlv,l,5,item[3].c)
          else
          {
            item[2].c[0]=0;
            strcpy(item[5].c,item[3].c);
            item[3].c[0]=0;
            item[4].c[0]=0;
            item[6].c[0]=0;
            item[7].c[0]=0;
            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
          }
        }else if (!strcmp(item[2].c,"EnableDHCP")||!strcmp(item[2].c,"DhcpServer")) //Passerelle col 6
        {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }

          if (test_ok)
          {
            if (item[3].c[0]=='0' && item[3].c[1]=='0' && item[3].c[2]=='0' && item[3].c[3]=='0' &&
                item[3].c[4]=='0' && item[3].c[5]=='0' && item[3].c[6]=='0')//EnableDHCP
            {
              tmp[0]=0;
              ListView_GetItemText(hlv,l,6,tmp,MAX_LINE_SIZE);
              if(strlen(tmp)>0)
              {
                if (item[3].c[7]=='1')snprintf(item[3].c,MAX_LINE_SIZE,"YES (%s)",tmp);
                else snprintf(item[3].c,MAX_LINE_SIZE,"NO (%s)",tmp);

                ListView_SetItemText(hlv,l,6,item[3].c);
              }else
              {
                if (item[3].c[7]=='1')ListView_SetItemText(hlv,l,6,"YES")
                else ListView_SetItemText(hlv,l,6,"NO");
              }
            }else //DhcpServer
            {
              //on vérifie si Wildstring
              if (item[3].c[2]=='0' && item[3].c[3]=='0')//si au format Hexa Wildstring
              {
                tmp[0]=0;
                SHexaToString(item[3].c,tmp);
                strcpy(item[3].c,tmp);
              }

              tmp[0]=0;
              ListView_GetItemText(hlv,l,6,tmp,MAX_LINE_SIZE);
              if(strlen(tmp)>0)
              {
                snprintf(item[0].c,MAX_LINE_SIZE,"%s (%s)",tmp,item[3].c);
                ListView_SetItemText(hlv,l,6,item[0].c);
              }else ListView_SetItemText(hlv,l,6,item[3].c);
            }
          }else
          {
            if (item[3].c[0]=='0' && item[3].c[1]=='0' && item[3].c[2]=='0' && item[3].c[3]=='0' &&
                item[3].c[4]=='0' && item[3].c[5]=='0' && item[3].c[6]=='0')//EnableDHCP
            {
              item[2].c[0]=0;
              if (item[3].c[7]=='1')strcpy(item[6].c,"YES");
              else strcpy(item[6].c,"NO");
              item[3].c[0]=0;
              item[4].c[0]=0;
              item[5].c[0]=0;
              item[7].c[0]=0;
              AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
            }else //DhcpServer
            {
              //on vérifie si Wildstring
              if (item[3].c[2]=='0' && item[3].c[3]=='0')//si au format Hexa Wildstring
              {
                tmp[0]=0;
                SHexaToString(item[3].c,tmp);
                strcpy(item[3].c,tmp);
              }
              item[2].c[0]=0;
              if (item[3].c[7]=='1')strcpy(item[6].c,item[3].c);
              else strcpy(item[6].c,item[3].c);
              item[3].c[0]=0;
              item[4].c[0]=0;
              item[5].c[0]=0;
              item[7].c[0]=0;
              AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
            }
          }
        }
      }
      return;
    }
  }
  //la même chose avec le Wifi ^^
  if (Contient(item[1].c,"WZCSVC\\Parameters\\Interfaces\\"))
  {
    //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
    hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_LAN);
    k = ListView_GetItemCount(hlv);
    test_ok = FALSE;

    char tmp_GUID[50];
    snprintf(tmp_GUID,"%s",&(item[1].c[strlen(item[1].c)-37]));

    for (l=0;l<k;l++)
    {
      tmp[0]=0;
      ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
      //if (!strcmp(tmp,item[1].c))
      if(Contient(tmp,tmp_GUID))
      {
        test_ok = TRUE;
        break;
      }
    }
    char tmp3[MAX_LINE_SIZE];
    if (test_ok)//update
    {
      //lecture des données lues
      tmp[0]=0;
      ListView_GetItemText(hlv,l,7,tmp,MAX_LINE_SIZE);
      char tmp2[MAX_LINE_SIZE];

      //pour chaque paramètre on ajoute à l'item ou création d'un item vide ^^
      if ((Contient(item[2].c,"Static#") || Contient(item[2].c,"ActiveSettings")) && strlen(item[3].c)>105)
      {
        switch (item[3].c[105])
        {
          case '0':snprintf(tmp2,MAX_LINE_SIZE,"%s %s/WEP",tmp,SHexaToChar(&(item[3].c[40]),tmp3));break;
          case '1':snprintf(tmp2,MAX_LINE_SIZE,"%s %s/Open",tmp,SHexaToChar(&(item[3].c[40]), tmp3));break;
          case '4':item[3].c[9]=='3'?snprintf(tmp2,MAX_LINE_SIZE,"%s %s/WPA-PSK:TKIP",tmp,SHexaToChar(&(item[3].c[40]), tmp3)):snprintf(tmp2,MAX_LINE_SIZE,"%s %s/WPA:TKIP",tmp,SHexaToChar(&(item[3].c[40]), tmp3));break;
          case '6':item[3].c[9]=='3'?snprintf(tmp2,MAX_LINE_SIZE,"%s %s/WPA-PSK:AES",tmp,SHexaToChar(&(item[3].c[40]), tmp3)):snprintf(tmp2,MAX_LINE_SIZE,"%s %s/WPA:AES",tmp,SHexaToChar(&(item[3].c[40]), tmp3));break;
          default:
          if (item[3].c[9] == '3')//PSK
            snprintf(tmp2,MAX_LINE_SIZE,"%s %s/WPA2-PSK ? : CODE: 0x%c%c",tmp,SHexaToChar(&(item[3].c[40]), tmp3),item[3].c[104],item[3].c[105]);
          else
            snprintf(tmp2,MAX_LINE_SIZE,"%s %s/WPA2 ? : CODE: 0x%c%c",tmp,SHexaToChar(&(item[3].c[40]), tmp3),item[3].c[104],item[3].c[105]);
          break;
        }
        ListView_SetItemText(hlv,l,7,tmp2);
      }else if (!strcmp(item[2].c,"ControlFlags") || !strcmp(item[2].c,"LayoutVersion"))
      {
        snprintf(tmp2,MAX_LINE_SIZE,"%s %s:%s",tmp,item[2].c,item[3].c);
        ListView_SetItemText(hlv,l,7,tmp2);
      }
    }else
    {
      //pour chaque paramètre on ajoute à l'item ou création d'un item vide ^^
      if ((Contient(item[2].c,"Static#") || Contient(item[2].c,"ActiveSettings")) && strlen(item[3].c)>105)
      {
        switch (item[3].c[105])
        {
          case '0':snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/WEP",tmp,SHexaToChar(&(item[3].c[40]), tmp3));break;
          case '1':snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/Open",tmp,SHexaToChar(&(item[3].c[40]), tmp3));break;
          case '4':item[3].c[9]=='3'?snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/WPA-PSK:TKIP",tmp,SHexaToChar(&(item[3].c[40]), tmp3)):snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/WPA:TKIP",tmp,SHexaToChar(&(item[3].c[40]), tmp3));break;
          case '6':item[3].c[9]=='3'?snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/WPA-PSK:AES",tmp,SHexaToChar(&(item[3].c[40]), tmp3)):snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/WPA:AES",tmp,SHexaToChar(&(item[3].c[40]), tmp3));break;
          default:
          if (item[3].c[9] == '3')//PSK
            snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/WPA2-PSK ? : CODE: 0x%c%c",tmp,SHexaToChar(&(item[3].c[40]), tmp3),item[3].c[104],item[3].c[105]);
          else
            snprintf(item[7].c,MAX_LINE_SIZE,"%s %s/WPA2 ? : CODE: 0x%c%c",tmp,SHexaToChar(&(item[3].c[40]), tmp3),item[3].c[104],item[3].c[105]);
          break;
        }
        //item[1].c[0]=0;
        item[2].c[0]=0;
        item[3].c[0]=0;
        item[4].c[0]=0;
        item[5].c[0]=0;
        item[6].c[0]=0;
        AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
      }else if (!strcmp(item[2].c,"ControlFlags") || !strcmp(item[2].c,"LayoutVersion"))
      {
        snprintf(item[7].c,MAX_LINE_SIZE,"%s:%s",item[2].c,item[3].c);
        //item[1].c[0]=0;
        item[2].c[0]=0;
        item[3].c[0]=0;
        item[4].c[0]=0;
        item[5].c[0]=0;
        item[6].c[0]=0;
        AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
      }
    }
  }

  //Services
  for (i=0;i<NB_MAX_REF_SEARCH_SERVICE;i++)
  {
    if (Contient(item[1].c,ref_service_search[i].v))
    {
      //recherche de data
      for (j=0;j<NB_MAX_REF_SEARCH_SERVICE_VAR;j++)
      {
        if (!strcmp(ref_service_var_search[j].v,item[2].c))
        {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_SERVICES);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }

          if (test_ok) //ajout à l'item
          {
            if (j == 1) //Start
            {
              switch(item[3].c[7])
              {                    //val3
                case '0':ListView_SetItemText(hlv,l,3,"Kernel module");break;
                case '1':ListView_SetItemText(hlv,l,3,"Starting by system");break;
                case '2':ListView_SetItemText(hlv,l,3,"Automatic start");break;
                case '3':ListView_SetItemText(hlv,l,3,"Manual start");break;
                case '4':ListView_SetItemText(hlv,l,3,"Disable");break;
                default:
                  sprintf(tmp,"%s (unknow)",item[3].c);
                  ListView_SetItemText(hlv,l,3,tmp);
                break;
              };
            }else if (j == 2) //Path
            {
              //on vérifie que les données ne soient pas encodées en hexa/wildstring
              if ((item[3].c[0]>47 && item[3].c[0]<58) && item[3].c[2]=='0' && item[3].c[3]=='0')
              {
                tmp[0]=0;
                SHexaToString(item[3].c,tmp);
                ListView_SetItemText(hlv,l,j+2,tmp);
              }else ListView_SetItemText(hlv,l,j+2,item[3].c);
            }else if (j == 4) //Type
            {
              if (item[3].c[7] == '1')ListView_SetItemText(hlv,l,6,"DRIVER")
              else ListView_SetItemText(hlv,l,6,"SERVICE");
            }else ListView_SetItemText(hlv,l,j+2,item[3].c);
          }else //new item
          {
            tmp[0]=0;
            strcpy(tmp,item[3].c);
            strcpy(item[7].c,item[5].c);
            item[2].c[0]=0;
            item[3].c[0]=0;
            item[4].c[0]=0;
            item[5].c[0]=0;
            item[6].c[0]=0;

            //traitement ^^
            if (j == 1) //Start
            {
              switch(tmp[7])
              {                    //val3
                case '0':strcpy(item[3].c,"Kernel module");break;
                case '1':strcpy(item[3].c,"Starting by system");break;
                case '2':strcpy(item[3].c,"Automatic start");break;
                case '3':strcpy(item[3].c,"Manual start");break;
                case '4':strcpy(item[3].c,"Disable");break;
                default: sprintf(item[3].c,"%s (unknow)",tmp);break;
              };
            }else if (j == 2) //Path
            {
              //on vérifie que les données ne soient pas encodées en hexa/wildstring
              if ((tmp[0]>47 && tmp[0]<58) && tmp[2]=='0' && tmp[3]=='0')
              {
                item[j+2].c[0]=0;
                SHexaToString(tmp,item[j+2].c);
              }else strcpy(item[j+2].c,tmp);
            }else if (j == 4) //Type
            {
              if (tmp[7] == '1')strcpy(item[6].c,"DRIVER");
              else strcpy(item[6].c,"SERVICE");
            }else strcpy(item[j+2].c,tmp);

            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
          }
        }
      }
      return;
    }
  }

  //Users
  //lecture du secret, utilisé avec la syskey pour obtenir les hash md5
  if (Contient(item[1].c,"\\Domains\\Account") && item[2].c[0]=='F')
  {
    DWORD size = strlen(item[1].c);
    if (item[1].c[size-1]=='t' && strlen(item[3].c) > 0x80*2)
    {
      //nous somme bien sur la bonne clée ^^
      //conversion en binaire
      secret_ = TRUE;
      DWORD i;
      size = strlen(item[3].c);
      if (size/2 > MAX_LINE_SIZE) size = MAX_LINE_SIZE*2;
      for (i=0;i<size/2 && i<MAX_LINE_SIZE;i++)
      {
        secret_c[i] = (BYTE)HexaToDecS(&item[3].c[i*2]);
      }
    }
  }

  for (i=0;i<NB_MAX_REF_SEARCH_USERS;i++)
  {
    if (Contient(item[1].c,ref_users_search[i].v))
    {
       if (item[2].c[0]=='V' && item[2].c[1]==0) //V
       {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USERS);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          //+test shadow copie + test SID copie
          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }
          if (test_ok)
          {
            //conversion des données en champs hexa pour exploiter le tout ^^
            int rid = TraiterUserDataFromSAM_V(item);
            ListView_SetItemText(hlv,l,2,item[2].c);
            ListView_SetItemText(hlv,l,4,item[4].c);
            ListView_SetItemText(hlv,l,9,item[9].c);

            //traitement des hash ^^
            if (syskey_ && secret_)
            {
              DecodeSAMHash(sk_c,item[9].c, rid, item[2].c, secret_c);
              ListView_SetItemText(hlv,l,9,item[9].c);
            }

            //test shadow copie
            j=l;
            for (l=0;l<k;l++)
            {
              if (l == j)continue;

              tmp[0]=0;
              ListView_GetItemText(hlv,l,5,tmp,MAX_LINE_SIZE); //vérif si pas un group
              if (tmp[0]==0)
              {
                ListView_GetItemText(hlv,l,11,tmp,MAX_LINE_SIZE);
                if (!strcmp(tmp,item[11].c) && tmp[0]!=0 && item[11].c[0]!=0)
                {
                  ListView_SetItemText(hlv,l,10,"YES");
                  ListView_SetItemText(hlv,j,10,"YES");
                }else
                {
                  tmp[0]=0;
                  ListView_GetItemText(hlv,l,3,tmp,MAX_LINE_SIZE);
                  if (!strcmp(tmp,item[3].c) && tmp[0]!=0 && item[3].c[0]!=0)
                  {
                    ListView_SetItemText(hlv,l,10,"YES");
                    ListView_SetItemText(hlv,j,10,"YES");
                  }
                }
              }
            }

            if (item[3].c[0]!=0) ListView_SetItemText(hlv,j,3,item[3].c);
            ListView_SetItemText(hlv,j,11,item[11].c);
          }else
          {
            //conversion des données en champs hexa pour exploiter le tout ^^
            strcpy(item[11].c,item[3].c);
            int rid = TraiterUserDataFromSAM_V(item);

            //traitement des hash ^^
            if (syskey_ && secret_)
            {
              DecodeSAMHash(sk_c,item[9].c, rid, item[2].c, secret_c);
            }

            //test shadow copie
            for (l=0;l<k;l++)
            {
              tmp[0]=0;
              ListView_GetItemText(hlv,l,5,tmp,MAX_LINE_SIZE); //vérif si pas un group
              if (tmp[0]==0)
              {
                ListView_GetItemText(hlv,l,11,tmp,MAX_LINE_SIZE);
                if (!strcmp(tmp,item[11].c) && tmp[0]!=0 && item[11].c[0]!=0)
                {
                  ListView_SetItemText(hlv,l,10,"YES");
                  strcpy(item[10].c,"YES");
                }else
                {
                  tmp[0]=0;
                  ListView_GetItemText(hlv,l,3,tmp,MAX_LINE_SIZE);
                  if (!strcmp(tmp,item[3].c) && tmp[0]!=0 && item[3].c[0]!=0)
                  {
                    ListView_SetItemText(hlv,l,10,"YES");
                    strcpy(item[10].c,"YES");
                  }
                }
              }
            }
            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
          }
       //valeure F = contient date de connexion count de connexion + état
       }else if (item[2].c[0]=='F' && item[2].c[1]==0) //F
       {
          //on vérifie toute les lignes du tableau en recherchant un enregistrement existant ^^
          hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USERS);
          k = ListView_GetItemCount(hlv);
          test_ok = FALSE;

          for (l=0;l<k;l++)
          {
            tmp[0]=0;
            ListView_GetItemText(hlv,l,1,tmp,MAX_LINE_SIZE);
            if (!strcmp(tmp,item[1].c))
            {
              test_ok = TRUE;
              break;
            }
          }
          if (test_ok)//existe
          {
            TraiterUserDataFromSAM_F(item);
            ListView_SetItemText(hlv,l,6,item[6].c);
            ListView_SetItemText(hlv,l,7,item[7].c);
            ListView_SetItemText(hlv,l,8,item[8].c);
            ListView_SetItemText(hlv,l,9,item[9].c);

            tmp[0]=0;
            ListView_GetItemText(hlv,l,3,tmp,MAX_LINE_SIZE);
            if (tmp[0]==0 && item[3].c[0]!=0)
              ListView_SetItemText(hlv,l,3,item[3].c)

          }else
          {
            TraiterUserDataFromSAM_F(item);
            AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
          }
       }
       return;
    }
  }
  //Groupes
  if ((Contient(item[1].c,"\\Domains\\Builtin\\Aliases\\") || Contient(item[1].c,"\\Domains\\Account\\Aliases\\"))&& item[2].c[0]=='C')
  {
    TraiterGroupDataFromSAM_C(item, GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_USERS));
    return;
  }

  //Passwords
  if (Contient(item[1].c,"WinVNC") && !strcmp(item[2].c,"Password"))
  {
    if (item[3].c[0]!=0)
    {
      hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PASSWORD);
      strcpy(item[5].c,"VNC");
      if (strlen(item[3].c)>7)
      {
        strcpy(item[4].c,item[3].c);
        item[4].c[8]=0;
        vncpwd((u_char *)(item[4].c),8);
      }else item[4].c[0]=0;
      AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
    }
    return;
  }
  if (Contient(item[1].c,"\\CONTROL PANEL\\DESKTOP") && !strcmp(item[2].c,"ScreenSave_Data"))
  {
    if (item[3].c[0]!=0)
    {
      hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PASSWORD);
      strcpy(item[5].c,"Screen saver");
      item[4].c[0]=0;
      AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
    }
    return;
  }
  if (Contient(item[1].c,"\\Control\\Terminal Server\\WinStations\\") && !strcmp(item[2].c,"password"))
  {
    if (item[3].c[0]!=0 && strcmp("0022f600",item[3].c)!=0 && strcmp("00000000",item[3].c)!=0)//valeures par défaut
    {
      hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PASSWORD);
      strcpy(item[5].c,"Windows auto-logon");
      item[4].c[0]=0;
      AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
    }
    return;
  }
  if (Contient(item[1].c,"\\Winlogon") && (!strcmp(item[2].c,"DefaultUserName") || !strcmp(item[2].c,"DefaultPassword") || !strcmp(item[2].c,"DefaultDomainName")))
  {
    if (item[3].c[0]!=0)//valeures par défaut
    {
      hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_PASSWORD);
      strcpy(item[5].c,"Terminal Server");
      item[4].c[0]=0;
      AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
    }
    return;
  }

  //MRU
  for (i=0;i<NB_MAX_REF_SEARCH_MRU;i++)
  {
    if (Contient(item[1].c,ref_mru_search[i].v) && strcmp(item[2].c,ref_mru_var_search[i*2].v)!=0)
    {
      hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MRU);
      strcpy(item[4].c,ref_mru_var_search[i*2+1].v);
      AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
      return;
    }
  }
  if (Contient(item[1].c,"CurrentVersion\\Applets\\Regedit") && !strcmp(item[2].c,"LastKey"))
  {
    hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MRU);
    strcpy(item[4].c,"Last open registry key");
    AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
    return;
  }

  //Wildstring
  for (i=0;i<NB_MAX_REF_SEARCH_MRU_WILD;i++)
  {
    if (Contient(item[1].c,ref_mru_wild_search[i].v) && strcmp(item[2].c,ref_mru_var_wils_search[i*2].v)!=0 && strcmp(item[2].c,"ViewStream")!=0)
    {
      hlv = GetDlgItem(Tabl[TABL_REGISTRY],LV_REGISTRY_MRU);
      strcpy(item[4].c,ref_mru_var_wils_search[i*2+1].v);

      //traitement du Wildstring
      SHexaToString(item[3].c, tmp);
      strcpy(item[3].c,tmp);

      AddToLV(hlv, item, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
      return;
    }
  }
}
