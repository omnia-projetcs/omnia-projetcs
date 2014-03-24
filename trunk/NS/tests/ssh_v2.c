//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
BOOL LinuxStart_msgOK(char *msg, char*cmd)
{
  if (msg == NULL)return FALSE;
  else if (msg[0] == 0)return FALSE;
  else if (msg[0] == cmd[0] && msg[1] == cmd[1] && msg[2] == cmd[2])return FALSE;
  else if (msg[0] == 'Y' && msg[1] == 'o' && msg[2] == 'u')return FALSE;
  else if (msg[0] == 'T' && msg[1] == 'h' && msg[2] == 'i' && msg[3] == 's' && msg[4] == ' ' && msg[5] == 'a' && msg[6] == 'c')return FALSE;
  else if (msg[0] == 'D' && msg[1] == 'E' && msg[2] == 'L' && msg[3] == 'I' && msg[4] == 'V' && msg[5] == 'E' && msg[6] == 'R')return FALSE;
  return TRUE;
}
//----------------------------------------------------------------
//use libssh2 libraries : http://www.libssh2.org/ and lib http://josefsson.org/gnutls4win/libssh2-1.2.6.zip
int ssh_exec(DWORD iitem, char *ip, unsigned int port, char*username, char*password)
{
  if (!scan_start) return SSH_ERROR_NOT_TESTED;

  //for each command
  char buffer[MAX_MSG_SIZE+1],cmd[MAX_MSG_SIZE+1], msg[MAX_MSG_SIZE+1];
  //unsigned int nb = 0;
  unsigned int rc;
  BOOL ret = 0;
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_SSH,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  //init libssh2
  if (libssh2_init(0)==0)
  {
    #ifdef DEBUG_MODE_SSH
    char debug_msg[MAX_PATH];
    AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_init");
    #endif // DEBUG_MODE_SSH

    //connexion
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET)
    {
      #ifdef DEBUG_MODE_SSH
      AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"socket");
      #endif // DEBUG_MODE_SSH

      //connect
      struct sockaddr_in sin;
      sin.sin_family = AF_INET;
      sin.sin_port = htons(port);
      sin.sin_addr.s_addr = inet_addr(ip);
      if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
      {
        #ifdef DEBUG_MODE_SSH
        AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"connect:TCP 22");
        #endif // DEBUG_MODE_SSH

        //init a session sshv2
        session = libssh2_session_init();
        if (session != 0)
        {
          //enable compression if possible
          //libssh2_session_flag(session, LIBSSH2_FLAG_COMPRESS, 1);

          //set session timeout, default : is blocking state
          libssh2_session_set_timeout(session, SSH2_SESSION_TIMEOUT);

          #ifdef DEBUG_MODE_SSH
          AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_session_init");
          #endif // DEBUG_MODE_SSH

          //start real session, pass welcome banners...
          //if (!libssh2_session_startup(session, sock))
          if (!libssh2_session_handshake(session, sock))
          {
            #ifdef DEBUG_MODE_SSH
            AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_session_startup/libssh2_session_handshake");
            #endif // DEBUG_MODE_SSH

            //authentication
            if (!libssh2_userauth_password(session, username, password))
            {
              #ifdef DEBUG_MODE_SSH
              snprintf(debug_msg,MAX_PATH,"libssh2_userauth_password %s:%s",username,password);
              AddMsg(h_main,(char*)"DEBUG (SSH)",ip,debug_msg);
              #endif // DEBUG_MODE_SSH

              for (i=0;i<_nb_i && scan_start;i++)
              {
                cmd[0] = 0;
                if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) > MAX_MSG_SIZE)continue;
                if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXT,(WPARAM)i,(LPARAM)cmd))
                {
                  if (cmd[0] == 0 || cmd[0] == '\r' || cmd[0] == '\n')continue;

                  //create a channel for execute
                  if ((channel = libssh2_channel_open_session(session)) != 0)
                  {
                    #ifdef DEBUG_MODE_SSH
                    AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_channel_open_session");
                    #endif // DEBUG_MODE_SSH

                    //send command and read results
                    if (!libssh2_channel_exec(channel, cmd))
                    {
                      #ifdef DEBUG_MODE_SSH
                      snprintf(debug_msg,MAX_PATH,"libssh2_channel_exec %s",cmd);
                      AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,debug_msg);
                      #endif // DEBUG_MODE_SSH

                      buffer[0] = 0;
                      ret       = SSH_ERROR_OK;
                      rc        = libssh2_channel_read(channel, buffer, MAX_MSG_SIZE)-1;
                      if (rc > 0 && buffer[0] != 0)
                      {
                        snprintf(msg,MAX_MSG_SIZE,"\r\n[%s:%d\\%s]",ip,port,cmd);
                        AddLSTVUpdateItem(msg, COL_SSH, iitem);
                        int z =0;
                        do
                        {
                          if (buffer[0] == 0)break;
                          if (rc < MAX_MSG_SIZE)buffer[rc] = 0;
                          else buffer[MAX_MSG_SIZE-1] = 0;

                          ConvertLinuxToWindows(buffer, MAX_MSG_SIZE);

                          AddMsg(h_main,(char*)"FOUND (SSH)",msg,buffer);
                          AddLSTVUpdateItem(buffer, COL_SSH, iitem);
                          buffer[0] = 0;
                        }while((rc = libssh2_channel_read(channel, buffer, MAX_MSG_SIZE)) && (z++ < MAX_COUNT_MSG));
                      }
                    }else ret = SSH_ERROR_CHANNEL_EXEC;
                    //close the channel
                    libssh2_channel_close(channel);
                    libssh2_channel_free(channel);
                    channel = NULL;
                  }else
                  {
                    libssh2_session_disconnect(session,NULL);
                    libssh2_session_free(session);
                    closesocket(sock);
                    libssh2_exit();
                    return SSH_ERROR_CHANNEL;
                  }
                }
              }
            }else
            {
              libssh2_session_disconnect(session,NULL);
              libssh2_session_free(session);
              closesocket(sock);
              libssh2_exit();
              return SSH_ERROR_AUTHENT;
            }
          }else
          {
            libssh2_session_disconnect(session,NULL);
            libssh2_session_free(session);
            closesocket(sock);
            libssh2_exit();
            return SSH_ERROR_SESSIONSTART;
          }
          libssh2_session_disconnect(session,NULL);
          libssh2_session_free(session);
        }else
        {
          closesocket(sock);
          libssh2_exit();
          return SSH_ERROR_SESSIONINIT;
        }
      }else
      {
        closesocket(sock);
        libssh2_exit();
        return SSH_ERROR_CONNECT;
      }
      closesocket(sock);
    }else
    {
      libssh2_exit();
      return SSH_ERROR_SOCKET;
    }
    //clean
    libssh2_exit();
  }else return SSH_ERROR_LIBINIT;

  return ret;
}
//----------------------------------------------------------------
//use libssh2 libraries : http://www.libssh2.org/ and lib http://josefsson.org/gnutls4win/libssh2-1.2.6.zip
int ssh_exec_to_file(DWORD iitem, char *ip, unsigned int port, char*username, char*password, HANDLE hfile)
{
  if (!scan_start) return SSH_ERROR_NOT_TESTED;

  //for each command
  char buffer[MAX_MSG_SIZE+1],cmd[MAX_MSG_SIZE+1], msg[MAX_MSG_SIZE+1];
  //unsigned int nb = 0;
  unsigned int rc;
  BOOL ret        = 0;
  DWORD copiee;
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;
  DWORD i, _nb_i  = SendDlgItemMessage(h_main,CB_T_SSH,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  //init libssh2
  if (libssh2_init(0)==0)
  {
    #ifdef DEBUG_MODE_SSH
    char debug_msg[MAX_PATH];
    AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_init");
    #endif // DEBUG_MODE_SSH

    //connexion
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET)
    {
      #ifdef DEBUG_MODE_SSH
      AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"socket");
      #endif // DEBUG_MODE_SSH

      //connect
      struct sockaddr_in sin;
      sin.sin_family = AF_INET;
      sin.sin_port = htons(port);
      sin.sin_addr.s_addr = inet_addr(ip);
      if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
      {
        #ifdef DEBUG_MODE_SSH
        AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"connect:TCP 22");
        #endif // DEBUG_MODE_SSH

        //init a session sshv2
        session = libssh2_session_init();
        if (session != 0)
        {
          //enable compression if possible
          //libssh2_session_flag(session, LIBSSH2_FLAG_COMPRESS, 1);

          //set session timeout, default : is blocking state
          libssh2_session_set_timeout(session, SSH2_SESSION_TIMEOUT);

          #ifdef DEBUG_MODE_SSH
          AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_session_init");
          #endif // DEBUG_MODE_SSH

          //start real session, pass welcome banners...
          //if (!libssh2_session_startup(session, sock))
          if (!libssh2_session_handshake(session, sock))
          {
            #ifdef DEBUG_MODE_SSH
            AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_session_startup/libssh2_session_handshake");
            #endif // DEBUG_MODE_SSH

            //authentication
            if (!libssh2_userauth_password(session, username, password))
            {
              #ifdef DEBUG_MODE_SSH
              snprintf(debug_msg,MAX_PATH,"libssh2_userauth_password %s:%s",username,password);
              AddMsg(h_main,(char*)"DEBUG (SSH)",ip,debug_msg);
              #endif // DEBUG_MODE_SSH

              for (i=0;i<_nb_i && scan_start;i++)
              {
                cmd[0] = 0;
                if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) > MAX_MSG_SIZE)continue;
                if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXT,(WPARAM)i,(LPARAM)cmd))
                {
                  if (cmd[0] == 0 || cmd[0] == '\r' || cmd[0] == '\n')continue;

                  //create a channel for execute
                  if ((channel = libssh2_channel_open_session(session)) != 0)
                  {
                    #ifdef DEBUG_MODE_SSH
                    AddMsg(h_main,(char*)"DEBUG (SSH)",ip,"libssh2_channel_open_session");
                    #endif // DEBUG_MODE_SSH

                    //send command and read results
                    if (!libssh2_channel_exec(channel, cmd))
                    {
                      #ifdef DEBUG_MODE_SSH
                      snprintf(debug_msg,MAX_PATH,"libssh2_channel_exec %s",cmd);
                      AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,debug_msg);
                      #endif // DEBUG_MODE_SSH

                      buffer[0] = 0;
                      ret       = SSH_ERROR_OK;
                      rc        = libssh2_channel_read(channel, buffer, MAX_MSG_SIZE)-1;
                      if (rc > 0 && buffer[0] != 0)
                      {
                        snprintf(msg,MAX_MSG_SIZE,"\r\n[%s:%d\\%s]\r\n",ip,port,cmd);
                        AddLSTVUpdateItem(msg, COL_SSH, iitem);
                        WriteFile(hfile,msg,strlen(msg),&copiee,0);
                        int z =0;
                        do
                        {
                          if (buffer[0] == 0)break;
                          if (rc < MAX_MSG_SIZE)buffer[rc] = 0;
                          else buffer[MAX_MSG_SIZE-1] = 0;

                          ConvertLinuxToWindows(buffer, MAX_MSG_SIZE);

                          AddMsg(h_main,(char*)"FOUND (SSH)",msg,buffer);
                          AddLSTVUpdateItem(buffer, COL_SSH, iitem);
                          WriteFile(hfile,buffer,strlen(buffer),&copiee,0);
                          buffer[0] = 0;
                        }while((rc = libssh2_channel_read(channel, buffer, MAX_MSG_SIZE)) && (z++ < MAX_COUNT_MSG));
                      }
                    }else ret = SSH_ERROR_CHANNEL_EXEC;
                    //close the channel
                    libssh2_channel_close(channel);
                    libssh2_channel_free(channel);
                    channel = NULL;
                  }else
                  {
                    libssh2_session_disconnect(session,NULL);
                    libssh2_session_free(session);
                    closesocket(sock);
                    libssh2_exit();
                    return SSH_ERROR_CHANNEL;
                  }
                }
              }
            }else
            {
              libssh2_session_disconnect(session,NULL);
              libssh2_session_free(session);
              closesocket(sock);
              libssh2_exit();
              return SSH_ERROR_AUTHENT;
            }
          }else
          {
            libssh2_session_disconnect(session,NULL);
            libssh2_session_free(session);
            closesocket(sock);
            libssh2_exit();
            return SSH_ERROR_SESSIONSTART;
          }
          libssh2_session_disconnect(session,NULL);
          libssh2_session_free(session);
        }else
        {
          closesocket(sock);
          libssh2_exit();
          return SSH_ERROR_SESSIONINIT;
        }
      }else
      {
        closesocket(sock);
        libssh2_exit();
        return SSH_ERROR_CONNECT;
      }
      closesocket(sock);
    }else
    {
      libssh2_exit();
      return SSH_ERROR_SOCKET;
    }
    //clean
    libssh2_exit();
  }else return SSH_ERROR_LIBINIT;

  return ret;
}
//----------------------------------------------------------------
int ssh_exec_cmd(DWORD iitem, char *ip, unsigned int port, char*username, char*password, long int id_account, char *cmd, char *buffer, DWORD buffer_size, BOOL msg_OK, BOOL msg_auth)
{
  buffer[0] = 0;

  if (!scan_start) return SSH_ERROR_NOT_TESTED;

  //for each command
  char msg[MAX_MSG_SIZE+1]="";
  //unsigned int nb = 0;
  int rc;
  BOOL ret = 0;
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;
  if (cmd[0] == 0 || cmd[0] == '\r' || cmd[0] == '\n')return 0;

  //init libssh2
  if (libssh2_init(0)==0)
  {
    #ifdef DEBUG_MODE_SSH
    char debug_msg[MAX_PATH];
    AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,"libssh2_init");
    #endif // DEBUG_MODE_SSH
    //connexion
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET)
    {
      #ifdef DEBUG_MODE_SSH
      AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,"socket");
      #endif // DEBUG_MODE_SSH
      //connect
      struct sockaddr_in sin;
      sin.sin_family = AF_INET;
      sin.sin_port = htons(port);
      sin.sin_addr.s_addr = inet_addr(ip);
      if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
      {
        #ifdef DEBUG_MODE_SSH
        AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,"connect:TCP 22");
        #endif // DEBUG_MODE_SSH

        //init a session sshv2
        session = libssh2_session_init();
        if (session != 0)
        {
          #ifdef DEBUG_MODE_SSH
          AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,"libssh2_session_init");
          #endif // DEBUG_MODE_SSH

          //set session timeout for bad ssh server (hold or not real server)
          libssh2_session_set_timeout(session, SSH2_SESSION_TIMEOUT);

          //start real session, pass welcome banners...
          //if (!libssh2_session_startup(session, sock))
          if (!libssh2_session_handshake(session, sock))
          {
            #ifdef DEBUG_MODE_SSH
            AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,"libssh2_session_startup/libssh2_session_handshake");
            #endif // DEBUG_MODE_SSH
            if (msg_OK)
            {
              snprintf(msg,MAX_MSG_SIZE,"(SSH) Enable on %s:%d",ip,port);
              AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
              AddLSTVUpdateItem(msg, COL_SSH, iitem);
            }

            //authentication
            if (!libssh2_userauth_password(session, username, password))
            {
              #ifdef DEBUG_MODE_SSH
              snprintf(debug_msg,MAX_PATH,"libssh2_userauth_password %s:%s",username,password);
              AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,debug_msg);
              #endif // DEBUG_MODE_SSH

              if (msg_auth)
              {
                if (id_account == -1)
                {
                  snprintf(msg,sizeof(msg),"%s:%d with %s account.",ip,port,username);
                  AddMsg(h_main,(char*)"LOGIN (SSH)",msg,(char*)"");

                  snprintf(msg,sizeof(msg),"Login SSH %s:%d with %s account.",ip,port,username);
                  AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
                }else
                {
                  snprintf(msg,sizeof(msg),"%s:%d with %s (%02d) account.",ip,port,username,(unsigned int)id_account);
                  AddMsg(h_main,(char*)"LOGIN (SSH)",msg,(char*)"");

                  snprintf(msg,sizeof(msg),"Login SSH %s:%d with %s (%02d) account.",ip,port,username,(unsigned int)id_account);
                  AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
                }
              }

              //create a channel for execute
              if ((channel = libssh2_channel_open_session(session)) != 0)
              {
                #ifdef DEBUG_MODE_SSH

                AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,"libssh2_channel_open_session");
                #endif // DEBUG_MODE_SSH

                //send command and read results
                if (!libssh2_channel_exec(channel, cmd))
                {
                  #ifdef DEBUG_MODE_SSH
                  snprintf(debug_msg,MAX_PATH,"libssh2_channel_exec %s",cmd);
                  AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,debug_msg);
                  #endif // DEBUG_MODE_SSH

                  rc = libssh2_channel_read(channel, buffer, buffer_size);
                  if (buffer[0] != 0)
                  {
                    if (((unsigned int)(rc-1)) < buffer_size)buffer[(unsigned int)(rc-1)] = 0;
                    else buffer[buffer_size-1] = 0;

                    ConvertLinuxToWindows(buffer, buffer_size);

                    #ifdef DEBUG_MODE_SSH
                    AddMsg(h_main,(char*)"DEBUG (SSH CMD)",ip,buffer);
                    #endif // DEBUG_MODE_SSH
                  }

                  ret = SSH_ERROR_OK;
                }else ret = SSH_ERROR_CHANNEL_EXEC;
                //close the channel
                libssh2_channel_close(channel);
                libssh2_channel_free(channel);
                channel = NULL;
              }else
              {
                libssh2_session_disconnect(session,NULL);
                libssh2_session_free(session);
                closesocket(sock);
                libssh2_exit();
                return SSH_ERROR_CHANNEL;
              }
            }else
            {
              libssh2_session_disconnect(session,NULL);
              libssh2_session_free(session);
              closesocket(sock);
              libssh2_exit();
              return SSH_ERROR_AUTHENT;
            }
          }else
          {
            libssh2_session_disconnect(session,NULL);
            libssh2_session_free(session);
            closesocket(sock);
            libssh2_exit();
            return SSH_ERROR_SESSIONSTART;
          }
          libssh2_session_disconnect(session,NULL);
          libssh2_session_free(session);
        }else
        {
          closesocket(sock);
          libssh2_exit();
          return SSH_ERROR_SESSIONINIT;
        }
      }else
      {
        closesocket(sock);
        libssh2_exit();
        return SSH_ERROR_CONNECT;
      }
      closesocket(sock);
    }else
    {
      libssh2_exit();
      return SSH_ERROR_SOCKET;
    }
    //clean
    libssh2_exit();
  }else return SSH_ERROR_LIBINIT;
  return ret;
}
//----------------------------------------------------------------
BOOL TCP_port_open(DWORD iitem, char *ip, unsigned int port, BOOL msg_OK)
{
  if (!scan_start) return FALSE;

  WaitForSingleObject(hs_tcp,INFINITE);

  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock != INVALID_SOCKET)
  {
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip);

    //if possible non bloquant socket
    unsigned long iMode = 1; //non bloquant
    if (ioctlsocket(sock, FIONBIO, &iMode)==0)
    {
      connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in));

      //set a time out
      fd_set read;
      FD_ZERO(&read);
      FD_SET(sock, &read);

      struct timeval timeout;
      timeout.tv_sec  = 5;   //5 seconds
      timeout.tv_usec = 0;

      //check
      int nb = select(-1, NULL, &(read), NULL, &(timeout));
      if (nb != 0 && nb!=SOCKET_ERROR)
      {
        if (msg_OK)
        {
          char msg[MAX_PATH];
          snprintf(msg,sizeof(msg),"(SSH) Enable on %s:%d",ip,port);
          AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
          AddLSTVUpdateItem(msg, COL_SSH, iitem);
        }

        FD_CLR(sock, &read);
        closesocket(sock);
        ReleaseSemaphore(hs_tcp,1,NULL);
        return TRUE;
      }
      //clean
      FD_CLR(sock, &read);

    }else //mode bloquant :(
    {
      if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
      {
        if (msg_OK)
        {
          char msg[MAX_PATH];
          snprintf(msg,sizeof(msg),"(SSH) Enable on %s:%d",ip,port);
          AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
          AddLSTVUpdateItem(msg, COL_SSH, iitem);
        }

        closesocket(sock);
        ReleaseSemaphore(hs_tcp,1,NULL);
        return TRUE;
      }
    }
    closesocket(sock);
  }
  ReleaseSemaphore(hs_tcp,1,NULL);
  return FALSE;
}
