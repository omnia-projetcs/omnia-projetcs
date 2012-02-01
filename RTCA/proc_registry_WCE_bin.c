//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//traitement des fichiers de base de registre Windows CE
void AnalyseFichierRegWCEBin(char *fic)
{
  //ouverture du fichier
  HANDLE Hfic = CreateFile(fic,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);
  if (Hfic == INVALID_HANDLE_VALUE)
  {
    SB_add_T(TABL_CONF-1, "REG : File access error");
    return;
  }
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  if (taille_fic<1 || taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(Hfic);
    SB_add_T(TABL_CONF-1, "REG : File error");
    return;
  }

  //allocation
  char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
  if (!buffer)
  {
    CloseHandle(Hfic);
    SB_add_T(TABL_CONF-1, "REG : Out of memory");
    return;
  }

  //chargement du fichier en mémoire par morceau de 10mo
  //(pour éviter les pertes de synchro sur certaines machines)
  DWORD copiee, position = 0, increm = 0;
  if (taille_fic > DIXM)increm = DIXM;
  else increm = taille_fic;

  while (position<taille_fic && increm!=0)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
  {
    copiee = 0;
    ReadFile(Hfic, buffer+position, increm,&copiee,0);
    position +=copiee;
    if (taille_fic-position < increm)increm = taille_fic-position ;
  }
  CloseHandle(Hfic);

  //traitement du buffer
  if (taille_fic>0x5000)
  {
    typedef struct ekim_header
    {
      char padding[8];        //hexa : 00 04 00 00 00 00 00 00
      int id;                 //0x4D494B45 = EKIM
      char unknow[36];
      //FILETIME timestamp;     //Date de dernière modification ??
    }EKIM_HEADER;

    #define EKIM_CH_HEADER_SIZE  26
    typedef struct ekim_chaine
    {
      char id[6];             //id hexa : 00 C0 00 00 00 00
      char unknow[16];
      unsigned short size;    //taille de la chaine parfois (attention calcul en unicode)
      unsigned short type;    //type parfois
      char chaine[1];
    }EKIM_CHAINE;

    /*a coder*/


  }
  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}
