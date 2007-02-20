/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "tlsschannel.h"

#ifdef HAVE_WINTLS

namespace gloox
{

  SChannel::SChannel( TLSHandler *th, const std::string& server )
    : TLSBase( th, server )
  {
  }

  SChannel::~SChannel()
  {
  }

  bool SChannel::encrypt( const std::string& data )
  {
    if( len <= 0 )
      return true;

    SECURITY_STATUS ret;

    m_obuffers[0].BufferType = SECBUFFER_STREAM_HEADER;
    m_obuffers[0].pvBuffer = m_oBuffer;
    m_obuffers[0].cbBuffer = m_streamSizes.cbHeader;

    m_obuffers[1].BufferType = SECBUFFER_DATA;
    m_obuffers[1].pvBuffer = m_messageOffset;

    m_obuffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
    m_obuffers[2].cbBuffer = m_streamSizes.cbTrailer;

    m_obuffers[3].BufferType = SECBUFFER_EMPTY;
    m_obuffers[3].pvBuffer = NULL;
    m_obuffers[3].cbBuffer = 0;

    m_omessage.ulVersion = SECBUFFER_VERSION;
    m_omessage.cBuffers = 4;
    m_omessage.pBuffers = m_obuffers;

    while( len > 0 )
    {
      if( m_streamSizes.cbMaximumMessage < len )
      {
        memcpy( m_messageOffset, data, m_streamSizes.cbMaximumMessage );
        len -= m_streamSizes.cbMaximumMessage;
        m_obuffers[1].cbBuffer = m_streamSizes.cbMaximumMessage;
        m_obuffers[2].pvBuffer = m_messageOffset + m_streamSizes.cbMaximumMessage;
      }
      else
      {
        memcpy( m_messageOffset, data, len );
        m_obuffers[1].cbBuffer = len;
        m_obuffers[2].pvBuffer = m_messageOffset + len;
        len = 0;
      }

      ret = m_securityFunc->EncryptMessage( &m_context, 0, &m_omessage, 0 );
      if( ret != SEC_E_OK )
      {
        printf( "encryptmessage failed %ld\n", ret );
        return false;
      }

      int t = ::send( m_socket, m_oBuffer,
                      m_obuffers[0].cbBuffer + m_obuffers[1].cbBuffer + m_obuffers[2].cbBuffer, 0 );
      if( t == SOCKET_ERROR || t == 0 )
      {
        printf( "could not send: %d\n", WSAGetLastError() );
        return false;
      }
    }

    return true;
  }

  int SChannel::recv( const std::string& data )
  {
    SECURITY_STATUS ret;
    SecBuffer *dataBuffer = 0;
    int readable = 0;

    int maxLength = m_streamSizes.cbHeader + m_streamSizes.cbMaximumMessage + m_streamSizes.cbTrailer;

    printf( "bufferOffset is %d\n", m_bufferOffset );

    int t = ::recv( m_socket, m_iBuffer + m_bufferOffset, maxLength - m_bufferOffset, 0 );
    if( t == SOCKET_ERROR )
    {
      printf( "got SocketError\n" );
      return 0;
    }
    else if( t == 0 )
    {
      printf( "got connection close\n" );
      return 0;
    }
    else
      m_bufferOffset += t;

    while( m_bufferOffset )
    {
      printf( "continuing with bufferOffset: %d\n", m_bufferOffset );

      m_ibuffers[0].pvBuffer = m_iBuffer;
      m_ibuffers[0].cbBuffer = m_bufferOffset;
      m_ibuffers[0].BufferType = SECBUFFER_DATA;

      m_ibuffers[1].BufferType = SECBUFFER_EMPTY;
      m_ibuffers[2].BufferType = SECBUFFER_EMPTY;
      m_ibuffers[3].BufferType = SECBUFFER_EMPTY;

      m_imessage.ulVersion = SECBUFFER_VERSION;
      m_imessage.cBuffers = 4;
      m_imessage.pBuffers = m_ibuffers;

      ret = m_securityFunc->DecryptMessage( &m_context, &m_imessage, 0, NULL );

      if( ret == SEC_E_INCOMPLETE_MESSAGE )
      {
        printf( "recv'ed incomplete message\n" );
        return readable;
      }


  //    if( ret == SEC_I_CONTEXT_EXPIRED )
  //      return 0;

      if( ret != SEC_E_OK && ret != SEC_I_RENEGOTIATE )
      {
        printf( "DecryptMessage returned %ld\n", ret );
        printf( "GetLastError(): %ld\n", GetLastError() );
        printf( "input buffer length: %d, read in this run: %d\n", m_bufferOffset, t );
        return false;
      }

      m_bufferOffset = 0;

      for( int i = 1; i < 4; ++i )
      {
        if( dataBuffer == 0 && m_ibuffers[i].BufferType == SECBUFFER_DATA )
        {
          dataBuffer = &m_ibuffers[i];
        }
        if( m_bufferOffset == 0 && m_ibuffers[i].BufferType == SECBUFFER_EXTRA )
        {
  //         m_extraBuffer = &m_ibuffers[i];
  printf( "git exetra buffer, size %ld\n", m_ibuffers[i].cbBuffer );
//          memcpy( m_iBuffer, m_ibuffers[i].pvBuffer, m_ibuffers[i].cbBuffer );
//          m_bufferOffset = m_ibuffers[i].cbBuffer;
        }
      }

      if( dataBuffer )
      {
        if( dataBuffer->cbBuffer > len )
        {
          memcpy( data, dataBuffer->pvBuffer, len );
          return len;
        }
        else
        {
          memcpy( data, dataBuffer->pvBuffer, dataBuffer->cbBuffer );
          readable += dataBuffer->cbBuffer;
          printf( "recvbuffer (%d): %s\n", readable, data );
        }
      }

      if( ret == SEC_I_RENEGOTIATE )
      {
        printf( "server requested reneg\n" );
        ret = handshakeLoop();
      }
    }

    return readable;
  }

  void SChannel::cleanup()
  {
    m_securityFunc->DeleteSecurityContext( &m_context );
  }

  bool SChannel::handshake()
  {
    INIT_SECURITY_INTERFACE pInitSecurityInterface;

    m_lib = LoadLibrary( "secur32.dll" );
    if( m_lib == NULL )
      return false;

    pInitSecurityInterface = (INIT_SECURITY_INTERFACE)GetProcAddress( m_lib, "InitSecurityInterfaceA" );
    if( pInitSecurityInterface == NULL )
    {
      FreeLibrary( m_lib );
      m_lib = 0;
      return false;
    }

    m_securityFunc = pInitSecurityInterface();
    if( !m_securityFunc )
    {
      FreeLibrary( m_lib );
      m_lib = 0;
      return false;
    }

    SCHANNEL_CRED schannelCred;
    memset( &schannelCred, 0, sizeof( schannelCred ) );
    memset( &m_credentials, 0, sizeof( m_credentials ) );
    memset( &m_context, 0, sizeof( m_context ) );

    schannelCred.dwVersion = SCHANNEL_CRED_VERSION;
    schannelCred.grbitEnabledProtocols = SP_PROT_TLS1_CLIENT;
    schannelCred.cSupportedAlgs = 0; // FIXME
#ifdef _MSC_VER
    schannelCred.dwMinimumCipherStrength = 0; // FIXME
    schannelCred.dwMaximumCipherStrength = 0; // FIXME
#else
    schannelCred.dwMinimumCypherStrength = 0; // FIXME
    schannelCred.dwMaximumCypherStrength = 0; // FIXME
#endif
    schannelCred.dwSessionLifespan = 0;
    schannelCred.dwFlags = SCH_CRED_NO_SERVERNAME_CHECK | SCH_CRED_NO_DEFAULT_CREDS |
                           SCH_CRED_MANUAL_CRED_VALIDATION; // FIXME check

    TimeStamp timeStamp;
    SECURITY_STATUS ret;
    ret = m_securityFunc->AcquireCredentialsHandleA( NULL, UNISP_NAME_A, SECPKG_CRED_OUTBOUND,
                                     NULL, &schannelCred, NULL,
                                     NULL, &m_credentials, &timeStamp );
    if( ret != SEC_E_OK )
    {
      printf( "AcquireCredentialsHandleA failed\n" );
      return false;
    }

    m_sspiFlags = ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_CONFIDENTIALITY | ISC_REQ_EXTENDED_ERROR
                      | ISC_REQ_MUTUAL_AUTH | ISC_REQ_REPLAY_DETECT | ISC_REQ_SEQUENCE_DETECT
                      | ISC_REQ_STREAM;

    SecBufferDesc outBufferDesc;
    SecBuffer outBuffers[1];

    outBuffers[0].BufferType = SECBUFFER_TOKEN;
    outBuffers[0].pvBuffer = NULL;
    outBuffers[0].cbBuffer = 0;

    outBufferDesc.ulVersion = SECBUFFER_VERSION;
    outBufferDesc.cBuffers = 1;
    outBufferDesc.pBuffers = outBuffers;

    long unsigned int sspiFlagsOut;
    ret = m_securityFunc->InitializeSecurityContextA( &m_credentials, NULL, NULL, m_sspiFlags, 0,
        SECURITY_NATIVE_DREP, NULL, 0, &m_context,
        &outBufferDesc, &sspiFlagsOut, &timeStamp );
    if( ret == SEC_I_CONTINUE_NEEDED && outBuffers[0].cbBuffer != 0 && outBuffers[0].pvBuffer != NULL )
    {
      printf( "OK: Continue needed: " );

      int ret = ::send( m_socket, (const char*)outBuffers[0].pvBuffer, outBuffers[0].cbBuffer, 0 );
      if( ret == SOCKET_ERROR || ret == 0 )
      {
        m_securityFunc->FreeContextBuffer( outBuffers[0].pvBuffer );
        m_securityFunc->DeleteSecurityContext( &m_context );
        return false;
      }

      m_securityFunc->FreeContextBuffer( outBuffers[0].pvBuffer );
      outBuffers[0].pvBuffer = NULL;
    }

    if( !handshakeLoop() )
    {
      printf( "handshakeLoop failed\n" );
      return false;
    }

    ret = m_securityFunc->QueryContextAttributes( &m_context, SECPKG_ATTR_STREAM_SIZES, &m_streamSizes );
    if( ret != SEC_E_OK )
    {
      printf( "could not read stream attribs (sizes)\n" );
      return false;
    }
printf( "maximumMessage: %ld\n", m_streamSizes.cbMaximumMessage );
    int maxSize = m_streamSizes.cbHeader + m_streamSizes.cbMaximumMessage + m_streamSizes.cbTrailer;
    m_iBuffer = (char*)malloc( maxSize );
    if( !m_iBuffer )
      return false;

    m_oBuffer = (char*)malloc( maxSize );
    if( !m_oBuffer )
      return false;

    m_bufferOffset = 0;
    m_messageOffset = m_oBuffer + m_streamSizes.cbHeader;

    SecPkgContext_Authority streamAuthority;
    ret = m_securityFunc->QueryContextAttributes( &m_context, SECPKG_ATTR_AUTHORITY, &streamAuthority );
    if( ret != SEC_E_OK )
    {
      printf( "could not read stream attribs (sizes)\n" );
      return false;
    }
    else
    {
      m_certInfo.issuer.assign( streamAuthority.sAuthorityName );
    }

    SecPkgContext_ConnectionInfo streamInfo;
    ret = m_securityFunc->QueryContextAttributes( &m_context, SECPKG_ATTR_CONNECTION_INFO, &streamInfo );
    if( ret != SEC_E_OK )
    {
      printf( "could not read stream attribs (sizes)\n" );
      return false;
    }
    else
    {
      if( streamInfo.dwProtocol == SP_PROT_TLS1_CLIENT )
        m_certInfo.protocol = "TLS 1.0";
      else
        m_certInfo.protocol = "unknown";

      std::ostringstream oss;
      switch( streamInfo.aiCipher )
      {
        case CALG_3DES:
          oss << "3DES";
          break;
        case CALG_AES_128:
          oss << "AES";
          break;
        case CALG_AES_256:
          oss << "AES";
          break;
        case CALG_DES:
          oss << "DES";
          break;
        case CALG_RC2:
          oss << "RC2";
          break;
        case CALG_RC4:
          oss << "RC4";
          break;
        default:
          oss << "unknown";
      }

      oss << " " << streamInfo.dwCipherStrength;
      m_certInfo.cipher = oss.str();
      oss.str( "" );

      switch( streamInfo.aiHash  )
      {
        case CALG_MD5:
          oss << "MD5";
          break;
        case CALG_SHA:
          oss << "SHA";
          break;
        default:
          oss << "unknown";
      }

      oss << " " << streamInfo.dwHashStrength;
      m_certInfo.mac = oss.str();

      m_certInfo.compression = "unknown";
    }

    m_secure = true;

    return true;
  }

  bool Connection::handshakeLoop()
  {
    const int bufsize = 65536;
    char *buf = (char*)malloc( bufsize );
    if( !buf )
      return false;

    int bufFilled = 0;
    int dataRecv = 0;
    bool doRead = true;

    SecBufferDesc outBufferDesc, inBufferDesc;
    SecBuffer outBuffers[1], inBuffers[2];

    SECURITY_STATUS ret = SEC_I_CONTINUE_NEEDED;

    while( ret == SEC_I_CONTINUE_NEEDED ||
           ret == SEC_E_INCOMPLETE_MESSAGE ||
           ret == SEC_I_INCOMPLETE_CREDENTIALS )
    {

      if( doRead )
      {
        dataRecv = ::recv( m_socket, buf + bufFilled, bufsize - bufFilled, 0 );

        if( dataRecv == SOCKET_ERROR || dataRecv == 0 )
        {
          break;
        }

        printf( "%d bytes handshake data received\n", dataRecv );

        bufFilled += dataRecv;
      }
      else
      {
        doRead = true;
      }

      outBuffers[0].BufferType = SECBUFFER_TOKEN;
      outBuffers[0].pvBuffer = NULL;
      outBuffers[0].cbBuffer = 0;

      outBufferDesc.ulVersion = SECBUFFER_VERSION;
      outBufferDesc.cBuffers = 1;
      outBufferDesc.pBuffers = outBuffers;

      inBuffers[0].BufferType = SECBUFFER_TOKEN;
      inBuffers[0].pvBuffer = buf;
      inBuffers[0].cbBuffer = bufFilled;

      inBuffers[1].BufferType = SECBUFFER_EMPTY;
      inBuffers[1].pvBuffer = NULL;
      inBuffers[1].cbBuffer = 0;

      inBufferDesc.ulVersion = SECBUFFER_VERSION;
      inBufferDesc.cBuffers = 2;
      inBufferDesc.pBuffers = inBuffers;

      printf( "buffers inited, calling InitializeSecurityContextA\n" );
      long unsigned int sspiFlagsOut;
      TimeStamp timeStamp;
      ret = m_securityFunc->InitializeSecurityContextA( &m_credentials, &m_context, NULL,
                                                        m_sspiFlags, 0,
                                                        SECURITY_NATIVE_DREP, &inBufferDesc, 0, NULL,
                                                        &outBufferDesc, &sspiFlagsOut, &timeStamp );
      if( ret == SEC_E_OK || ret == SEC_I_CONTINUE_NEEDED ||
          ( FAILED( ret ) && sspiFlagsOut & ISC_RET_EXTENDED_ERROR ) )
      {
        if( outBuffers[0].cbBuffer != 0 && outBuffers[0].pvBuffer != NULL )
        {
          printf( "ISCA returned, buffers not empty\n" );
          dataRecv = ::send( m_socket, (const char*)outBuffers[0].pvBuffer, outBuffers[0].cbBuffer, 0  );
          if( dataRecv == SOCKET_ERROR || dataRecv == 0 )
          {
            m_securityFunc->FreeContextBuffer( &outBuffers[0].pvBuffer );
            m_securityFunc->DeleteSecurityContext( &m_context );
            free( buf );
            printf( "coudl not send bufer to server, exiting\n" );
            return false;
          }

          m_securityFunc->FreeContextBuffer( outBuffers[0].pvBuffer );
          outBuffers[0].pvBuffer = NULL;
        }
      }

      if( ret == SEC_E_INCOMPLETE_MESSAGE )
        continue;

      if( ret == SEC_E_OK )
      {
        printf( "handshake successful\n" );
        break;
      }

      if( FAILED( ret ) )
      {
        printf( "ISC failed: %ld\n", ret );
        break;
      }

      if( ret == SEC_I_INCOMPLETE_CREDENTIALS )
      {
        printf( "server requested client credentials\n" );
        ret = SEC_I_CONTINUE_NEEDED;
        continue;
      }

      if( inBuffers[1].BufferType == SECBUFFER_EXTRA )
      {
        printf("some xtra mem in inbuf\n" );
        MoveMemory( buf, buf + ( bufFilled - inBuffers[1].cbBuffer ),
                   inBuffers[1].cbBuffer );

        bufFilled = inBuffers[1].cbBuffer;
      }
      else
      {
        bufFilled = 0;
      }
    }

    if( FAILED( ret ) )
      m_securityFunc->DeleteSecurityContext( &m_context );

    free( buf );

    if( ret == SEC_E_OK )
      return true;

    return false;
  }

}

#endif // HAVE_WINTLS
