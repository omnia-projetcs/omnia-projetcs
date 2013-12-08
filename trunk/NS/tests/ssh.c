//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
//use libssh2 libraries : http://www.libssh2.org/ and lib http://josefsson.org/gnutls4win/libssh2-1.2.6.zip
int ssh_exec(DWORD iitem,char *ip, char*username, char*password)
{
  //for each command
  char buffer[MAX_MSG_SIZE],cmd[MAX_MSG_SIZE], msg[MAX_MSG_SIZE];
  unsigned int nb = 0;
  int rc;
  BOOL ret = 0;
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_SSH,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  //init libssh2
  if (libssh2_init(0)==0)
  {
    //connexion
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET)
    {
      //connect
      struct sockaddr_in sin;
      sin.sin_family = AF_INET;
      sin.sin_port = htons(22);
      sin.sin_addr.s_addr = inet_addr(ip);
      if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
      {
        //init a session sshv2
        session = libssh2_session_init();
        if (session != 0)
        {
          //start real session, pass welcome banners...
          if (!libssh2_session_startup(session, sock))
          {
            //authentication
            if (!libssh2_userauth_password(session, username, password))
            {
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
                    //send command and read results
                    if (!libssh2_channel_exec(channel, cmd))
                    {
                      buffer[0] = 0;
                      ret       = SSH_ERROR_OK;
                      rc        = libssh2_channel_read(channel, buffer, sizeof(buffer));
                      if (rc > 0)
                      {

                        snprintf(msg,sizeof(msg),"[%s\\%s]\r\n",ip,cmd);
                        AddLSTVUpdateItem(msg, COL_SSH, iitem);
                        snprintf(msg,sizeof(msg),"[%s\\%s]",ip,cmd);
                        do
                        {
                          buffer[rc] = 0;
                          printf("*%s\n",buffer);
                          ConvertLinuxToWindows(buffer, sizeof(buffer));
                          AddMsg(h_main,(char*)"FOUND (SSH)",msg,buffer);
                          AddLSTVUpdateItem(buffer, COL_SSH, iitem);
                          buffer[0] = 0;
                        }while((rc = libssh2_channel_read(channel, buffer, sizeof(buffer))));
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
}/*
int ssh_exec(DWORD iitem,char *ip, char*username, char*password)
{
  //for each command
  char buffer[MAX_MSG_SIZE],cmd[MAX_MSG_SIZE], msg[MAX_MSG_SIZE];
  unsigned int nb = 0;
  int rc;
  BOOL ret = 0;
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_SSH,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  for (i=0;i<_nb_i && scan_start;i++)
  {
    cmd[0] = 0;
    if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) > MAX_MSG_SIZE)continue;
    if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXT,(WPARAM)i,(LPARAM)cmd))
    {
      if (cmd[0] == 0 || cmd[0] == '\r' || cmd[0] == '\n')continue;

      //init libssh2
      if (libssh2_init(0)==0)
      {
        //connexion
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock != INVALID_SOCKET)
        {
          //connect
          struct sockaddr_in sin;
          sin.sin_family = AF_INET;
          sin.sin_port = htons(22);
          sin.sin_addr.s_addr = inet_addr(ip);
          if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
          {
            //init a session sshv2
            session = libssh2_session_init();
            if (session != 0)
            {
              //start real session, pass welcome banners...
              if (!libssh2_session_startup(session, sock))
              {
                //authentication
                if (!libssh2_userauth_password(session, username, password))
                {
                  //create a channel for execute
                  if ((channel = libssh2_channel_open_session(session)) != 0)
                  {
                    //send command and read results
                    if (!libssh2_channel_exec(channel, cmd))
                    {
                      buffer[0] = 0;
                      ret       = SSH_ERROR_OK;
                      rc        = libssh2_channel_read(channel, buffer, sizeof(buffer));
                      if (rc > 0)
                      {

                        snprintf(msg,sizeof(msg),"[%s\\%s]\r\n",ip,cmd);
                        AddLSTVUpdateItem(msg, COL_SSH, iitem);
                        snprintf(msg,sizeof(msg),"[%s\\%s]",ip,cmd);
                        do
                        {
                          buffer[rc] = 0;
                          printf("*%s\n",buffer);
                          ConvertLinuxToWindows(buffer, sizeof(buffer));
                          AddMsg(h_main,(char*)"FOUND (SSH)",msg,buffer);
                          AddLSTVUpdateItem(buffer, COL_SSH, iitem);
                          buffer[0] = 0;
                        }while((rc = libssh2_channel_read(channel, buffer, sizeof(buffer))));
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
    }
  }
  return ret;
}*/
//----------------------------------------------------------------
int ssh_exec_cmd(DWORD iitem,char *ip, char*username, char*password, long int id_account, char *cmd, char *buffer, DWORD buffer_size, BOOL msg_OK)
{
  //for each command
  char msg[MAX_MSG_SIZE];
  unsigned int nb = 0;
  int rc;
  BOOL ret = 0;
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;
  if (cmd[0] == 0 || cmd[0] == '\r' || cmd[0] == '\n')return 0;

  //init libssh2
  if (libssh2_init(0)==0)
  {
    //connexion
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET)
    {
      //connect
      struct sockaddr_in sin;
      sin.sin_family = AF_INET;
      sin.sin_port = htons(22);
      sin.sin_addr.s_addr = inet_addr(ip);
      if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
      {
        //init a session sshv2
        session = libssh2_session_init();
        if (session != 0)
        {
          //start real session, pass welcome banners...
          if (!libssh2_session_startup(session, sock))
          {
            //authentication
            if (!libssh2_userauth_password(session, username, password))
            {
              if (msg_OK)
              {
                if (id_account == -1) snprintf(msg,sizeof(msg),"Login (SSH) in %s IP with %s account.",ip,username,id_account);
                else snprintf(msg,sizeof(msg),"Login (SSH) in %s IP with %s (%02d) account.",ip,username,id_account);
                AddMsg(h_main,(char*)"INFORMATION",msg,(char*)"");
                AddLSTVUpdateItem(msg, COL_CONFIG, iitem);
              }

              //create a channel for execute
              if ((channel = libssh2_channel_open_session(session)) != 0)
              {
                //send command and read results
                if (!libssh2_channel_exec(channel, cmd))
                {
                  rc = libssh2_channel_read(channel, buffer, buffer_size);
                  buffer[(DWORD)(rc-1)] = 0;
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
//use libssh libraries : https://red.libssh.org/projects/libssh/files
/*void ssh_exec(DWORD iitem,char *ip, char*login, char*mdp)
{
  ssh_session session = NULL;
  ssh_channel channel = NULL;

  int verbosity = SSH_LOG_PROTOCOL;
  int port = 22;

  char buffer[MAX_LINE_SIZE],cmd[MAX_LINE_SIZE];
  unsigned int nb = 0;
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_SSH,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  for (i=0;i<_nb_i && scan_start;i++)
  {
    if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXTLEN,(WPARAM)i,(LPARAM)NULL) > MAX_LINE_SIZE)continue;
    if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXT,(WPARAM)i,(LPARAM)cmd))
    {
      session = ssh_new();
      if (session != NULL)
      {
        ssh_options_set(session, SSH_OPTIONS_HOST, ip);
        ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
        ssh_options_set(session, SSH_OPTIONS_PORT, &port);
        ssh_options_set(session, SSH_OPTIONS_USER, login);

        if (ssh_connect(session) == SSH_OK)
        {
          if (ssh_userauth_password(session, NULL, mdp) == SSH_AUTH_SUCCESS)
          {
            channel = ssh_channel_new(session);
            if (channel != NULL)
            {
              if (ssh_channel_open_session(channel) == SSH_OK)
              {
                //execute command
                if (ssh_channel_request_exec(channel, cmd) == SSH_OK)
                {
                  nb = ssh_channel_read(channel, buffer, MAX_LINE_SIZE, 0);
                  while (nb)
                  {
                    AddMsg(h_main,(char*)"FOUND (SSH)",cmd,buffer);
                    nb = ssh_channel_read(channel, buffer, MAX_LINE_SIZE, 0);
                  }
                }
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
              }
              ssh_channel_free(channel);
            }
          }
          ssh_disconnect(session);
        }
        ssh_free(session);
      }
    }
  }
}*/
