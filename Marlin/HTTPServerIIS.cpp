/////////////////////////////////////////////////////////////////////////////////
//
// SourceFile: HTTPServerIIS.cpp
//
// Marlin Server: Internet server/client
// 
// Copyright (c) 2014-2024 ir. W.E. Huisman
// All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
// HTTP Server using IIS Modules
//
//////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "HTTPServerIIS.h"
#include "HTTPSiteIIS.h"
#include "AutoCritical.h"
#include "WebServiceServer.h"
#include "HTTPURLGroup.h"
#include "HTTPError.h"
#include "HTTPTime.h"
#include "GetLastErrorAsString.h"
#include "WebSocketServerIIS.h"
#include "ConvertWideString.h"
#pragma warning (disable:4091)
#include <httpserv.h>
#pragma warning (error:4091)
#pragma warning (disable:6387)

#ifdef _AFX
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// Logging macro's
#define DETAILLOG1(text)          if(MUSTLOG(HLL_LOGGING) && m_log) { DetailLog (_T(__FUNCTION__),LogType::LOG_INFO,text); }
#define DETAILLOGS(text,extra)    if(MUSTLOG(HLL_LOGGING) && m_log) { DetailLogS(_T(__FUNCTION__),LogType::LOG_INFO,text,extra); }
#define DETAILLOGV(text,...)      if(MUSTLOG(HLL_LOGGING) && m_log) { DetailLogV(_T(__FUNCTION__),LogType::LOG_INFO,text,__VA_ARGS__); }
#define WARNINGLOG(text,...)      if(MUSTLOG(HLL_LOGGING) && m_log) { DetailLogV(_T(__FUNCTION__),LogType::LOG_WARN,text,__VA_ARGS__); }
#define ERRORLOG(code,text)       ErrorLog (_T(__FUNCTION__),code,text)
#define HTTPERROR(code,text)      HTTPError(_T(__FUNCTION__),code,text)

HTTPServerIIS::HTTPServerIIS(XString p_name)
              :HTTPServer(p_name)
{
  m_counter.Start();

  m_marlinConfig = new MarlinConfig(_T("Marlin.config"));
}

HTTPServerIIS::~HTTPServerIIS()
{
  // Cleanup the server objects
  HTTPServerIIS::Cleanup();
}

XString
HTTPServerIIS::GetVersion()
{
  return XString(_T(MARLIN_SERVER_VERSION) _T(" on Microsoft IIS"));
}

// Initialise a HTTP server
bool
HTTPServerIIS::Initialise()
{
  AutoCritSec lock(&m_sitesLock);

  // See if there is something to do!
  if(m_initialized)
  {
    return true;
  }

  // STEP 1: LOGGING
  // Init logging, so we can complain about errors
  InitLogging();

  // STEP 2: CHECKING OF SETTINGS
  if(GeneralChecks() == false)
  {
    return false;
  }

  // STEP 3: Set the hard limits
  InitHardLimits();

  // STEP 4: Set the event stream parameters
  InitEventstreamKeepalive();

  // STEP 5: Init the response headers to send
  InitHeaders();

  // STEP 6: Init the threadpool
  InitThreadPool();

  // We are airborne!
  return (m_initialized = true);
}


// Cleanup the server
void
HTTPServerIIS::Cleanup()
{
  AutoCritSec lock1(&m_sitesLock);
  AutoCritSec lock2(&m_eventLock);

  // Remove all remaining sockets
  for(const auto& it : m_sockets)
  {
    delete it.second;
  }
  m_sockets.clear();

  // Remove all event streams within the scope of the eventLock
  for(const auto& it : m_eventStreams)
  {
    delete it.second;
  }
  m_eventStreams.clear();

  // Remove all services
  while(!m_allServices.empty())
  {
    ServiceMap::iterator it = m_allServices.begin();
    it->second->Stop();
  }

  // Remove all URL's in the sites map
  while(!m_allsites.empty())
  {
    SiteMap::iterator it = m_allsites.begin();
    if (it->second->StopSite(true) == false)
    {
      m_allsites.erase(it);
    };
  }

  // Closing the logging file
  if(m_log && m_logOwner)
  {
    LogAnalysis::DeleteLogfile(m_log);
    m_log = NULL;
  }
}

// Initialise general server header settings
// Can only be overridden from within your ServerApp.
void
HTTPServerIIS::InitHeaders()
{
  XString headertype;
  switch(m_sendHeader)
  {
    case SendHeader::HTTP_SH_MICROSOFT:   headertype = _T("Microsoft defined server header"); break;
    case SendHeader::HTTP_SH_MARLIN:      headertype = _T("Standard Marlin server header");   break;
    case SendHeader::HTTP_SH_APPLICATION: headertype = _T("Application's server name");       break;
    case SendHeader::HTTP_SH_WEBCONFIG:   headertype = _T("Configured header type name");     break;
    case SendHeader::HTTP_SH_HIDESERVER:  headertype = _T("Hide server response header");     break;
  }
  DETAILLOGV(_T("Header type: %s"),headertype.GetString());
}

// Running the server 
void
HTTPServerIIS::Run()
{
  Initialise();

  // See if we are in a state to receive requests
  if(!m_initialized)
  {
    ERRORLOG(ERROR_INVALID_PARAMETER,_T("RunHTTPServer called too early"));
    return;
  }
  DETAILLOG1(_T("HTTPServer initialised and ready to go!"));

  // Check if all sites were properly started
  // Catches programmers who forget to call HTTPSite::StartSite()
  CheckSitesStarted();

  // START OUR MAIN LOOP
  m_running = true;
  DETAILLOG1(_T("HTTPServerIIS started"));
}

// Stop the server
void
HTTPServerIIS::StopServer()
{
  AutoCritSec lock(&m_eventLock);
  DETAILLOG1(_T("Received a StopServer request"));

  // See if we are running at all
  if(m_running == false)
  {
    return;
  }

  // Try to remove all WebSockets
  while(!m_sockets.empty())
  {
    WebSocket* socket = m_sockets.begin()->second;
    if(socket)
    {
      socket->CloseSocket();
      UnRegisterWebSocket(socket);
    }
  }
  // Try to remove all event streams
  for(auto& it : m_eventStreams)
  {
    // SEND OnClose event
    ServerEvent* event = new ServerEvent(_T("close"));
    SendEvent(it.second->m_port,it.second->m_baseURL,event);
  }
  // Try to remove all event streams
  while(!m_eventStreams.empty())
  {
    const EventStream* stream = m_eventStreams.begin()->second;
    CloseEventStream(stream);
  }

  // See if we have a running server-push-event heartbeat monitor
  if(m_eventEvent)
  {
    // Explicitly pulse the event heartbeat monitor
    // this abandons the monitor in one go!
    DETAILLOG1(_T("Abandon the server-push-events heartbeat monitor"));
    SetEvent(m_eventEvent);
  }

  // main loop will stop on next iteration
  m_running = false;

  // Wait for a maximum of 30 seconds for the queue to stop
  // All server action should be ended after that.
  for(int ind = 0; ind < 300; ++ind)
  {
    Sleep(100);
    // Wait till the  event Monitor has stopped
    if(m_eventMonitor == nullptr)
    {
      break;
    }
  }
  // Cleanup the sites and groups
  Cleanup();
}

// Create a site to bind the traffic to
HTTPSite*
HTTPServerIIS::CreateSite(PrefixType    p_type
                         ,bool          p_secure
                         ,int           p_port
                         ,XString       p_baseURL
                         ,bool          p_subsite  /* = false */
                         ,LPFN_CALLBACK p_callback /* = NULL  */)
{
  // USE OVERRIDES FROM WEBCONFIG 
  // BUT USE PROGRAM'S SETTINGS AS DEFAULT VALUES
  
  if(p_type != PrefixType::URLPRE_Strong || p_secure ||
     p_port != INTERNET_DEFAULT_HTTP_PORT)
  {
    DETAILLOG1(_T("In IIS the prefix type, https and port number are controlled by IIS Admin, and cannot be set!"));
  }
  // Create our URL prefix
  XString prefix = CreateURLPrefix(p_type,p_secure,p_port,p_baseURL);
  if(!prefix.IsEmpty())
  {
    HTTPSite* mainSite = nullptr;
    if(p_subsite)
    {
      // Do sub-site lookup on receiving messages
      m_hasSubsites = true;
      // Finding the main site of this sub-site
      mainSite = FindHTTPSite(p_port,p_baseURL);
      if(mainSite == nullptr)
      {
        // No luck: No main site to register against
        XString message;
        message.Format(_T("Tried to register a sub-site, without a main-site: %s"),p_baseURL.GetString());
        ERRORLOG(ERROR_NOT_FOUND,message);
        return nullptr;
      }
    }
    // Create and register a URL
    // Remember URL Prefix strings, and create the site
    HTTPSiteIIS* registeredSite = new HTTPSiteIIS(this,p_port,p_baseURL,prefix,mainSite,p_callback);
    if(RegisterSite(registeredSite,prefix))
    {
      // Site created and registered
      return registeredSite;
    }
    delete registeredSite;
  }
  // No luck
  return nullptr;
}

// Delete a channel (from prefix OR base URL forms)
bool
HTTPServerIIS::DeleteSite(int p_port,XString p_baseURL,bool /*p_force /*=false*/)
{
  AutoCritSec lock(&m_sitesLock);
  // Default result
  bool result = false;

  XString search(MakeSiteRegistrationName(p_port,p_baseURL));
  SiteMap::iterator it = m_allsites.find(search);
  if(it != m_allsites.end())
  {
    // Finding our site
    HTTPSite* site = it->second;

    // See if other sites are dependent on this one
    if(site->GetHasSubSites())
    {
      // Walk all sites, to see if sub-sites still dependent on this main site
      for(SiteMap::iterator fit = m_allsites.begin(); fit != m_allsites.end(); ++fit)
      {
        if(fit->second->GetMainSite() == site)
        {
          fit->second->StopSite(true);
          fit = m_allsites.begin();
        }
      }
    }
    // And remove from the site map
    DETAILLOGS(_T("Removed site: "),site->GetPrefixURL());
    delete site;
    m_allsites.erase(it);
    result = true;
  }
  return result;
}

//////////////////////////////////////////////////////////////////////////
//
// FUNCTIONS FOR IIS
//
//////////////////////////////////////////////////////////////////////////

void
HTTPServerIIS::SetWebConfigIIS(WebConfigIIS* p_config)
{
  m_webConfigIIS = p_config;
}

WebConfigIIS* 
HTTPServerIIS::GetWebConfigIIS()
{
  return m_webConfigIIS;
}

// Get a stream: Valid return status are
// 0:  -> Not a stream request
// 1:  -> Valid new stream request
// -1: -> Stream request, but invalid (e.q. authentication failed)
int
HTTPServerIIS::GetHTTPStreamFromRequest(IHttpContext* p_context
                                       ,HTTPSite*     p_site
                                       ,PHTTP_REQUEST p_request)
{
  // Grab the senders accepted types
  XString acceptTypes = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderAccept].pRawValue);
  acceptTypes.Trim();

  // Receiving the initiation of an event stream for the server ?
  if((p_request->Verb == HttpVerbGET) &&
     (p_site->GetIsEventStream() || acceptTypes.Left(17).CompareNoCase(_T("text/event-stream")) == 0))
  {
    // Grab the rest of the needed content out of the request
    PSOCKADDR_IN6 sender   = (PSOCKADDR_IN6)p_request->Address.pRemoteAddress;
    XString   absolutePath = WStringToString(p_request->CookedUrl.pAbsPath);

    if(CheckUnderDDOSAttack(sender,absolutePath))
    {
      return -1;
    }

    // Grab the raw URL and the desktop for the stream
    XString   rawUrl       = WStringToString(p_request->CookedUrl.pFullUrl);
    int       remDesktop   = FindRemoteDesktop(p_request->Headers.UnknownHeaderCount
                                              ,p_request->Headers.pUnknownHeaders);
    // Open an event stream
    EventStream* stream = SubscribeEventStream(sender
                                              ,remDesktop
                                              ,p_site
                                              ,p_site->GetSite()
                                              ,absolutePath
                                              ,(HTTP_OPAQUE_ID)p_context
                                              ,NULL);
    if(stream)
    {
      // Getting the impersonated user
      IHttpUser* user = p_context->GetUser();
      if(user)
      {
        stream->m_user = WStringToString(user->GetRemoteUserName());
      }
      stream->m_baseURL = rawUrl;

      HTTPMessage* message = GetHTTPMessageFromRequest(p_context,p_site,p_request);

      // To do for this stream, not for a message
      DETAILLOGV(_T("Accepted an event-stream for SSE (Server-Sent-Events) from %s/%s"), stream->m_user.GetString(), rawUrl.GetString());
      if(p_site->HandleEventStream(message,stream))
      {
        // Return the fact that the request turned into a stream
        delete message;
        return 1;
      }
      RemoveEventStream(stream);
      delete message;
      delete stream;
    }
    return -1;
  }
  // Not a stream request or stream not initiated
  return 0;
}

// Building the essential HTTPMessage from the request area
HTTPMessage* 
HTTPServerIIS::GetHTTPMessageFromRequest(IHttpContext* p_context
                                        ,HTTPSite*     p_site
                                        ,PHTTP_REQUEST p_request)
{
  // Grab the senders content
  XString   contentType    = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderContentType    ].pRawValue);
  XString   contentLength  = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderContentLength  ].pRawValue);
  XString   acceptEncoding = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderAcceptEncoding ].pRawValue);
  XString   cookie         = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderCookie         ].pRawValue);
  XString   authorize      = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderAuthorization  ].pRawValue);
  XString   modified       = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderIfModifiedSince].pRawValue);
  XString   referrer       = LPCSTRToString(p_request->Headers.KnownHeaders[HttpHeaderReferer        ].pRawValue);
  XString   rawUrl         = WStringToString(p_request->CookedUrl.pFullUrl);
  PSOCKADDR sender         = p_request->Address.pRemoteAddress;
  int       remDesktop     = FindRemoteDesktop(p_request->Headers.UnknownHeaderCount
                                              ,p_request->Headers.pUnknownHeaders);

  // Log earliest as possible
  DETAILLOGV(_T("Received HTTP %s call from [%s] with length: %s for: %s")
             ,GetHTTPVerb(p_request->Verb,p_request->pUnknownVerb).GetString()
             ,SocketToServer((PSOCKADDR_IN6)sender).GetString()
             ,contentLength.GetString()
             ,rawUrl.GetString());

  // Find our charset
  Encoding encoding = Encoding::EN_ACP;
  XString charset = FindCharsetInContentType(contentType);
  if(!charset.IsEmpty())
  {
    encoding = (Encoding)CharsetToCodepage(charset);
  }

  // Trace the request in full
  LogTraceRequest(p_request,nullptr,encoding);

  // See if we must substitute for a sub-site
  if(m_hasSubsites)
  {
    XString absPath = WStringToString(p_request->CookedUrl.pAbsPath);
    p_site = FindHTTPSite(p_site,absPath);
  }

  // Check our authentication
  HANDLE token = NULL;
  if(p_site->GetAuthentication())
  {
    if(!CheckAuthentication(p_request,(HTTP_OPAQUE_ID)p_context,p_site,rawUrl,authorize,token))
    {
      // No authentication, answer already sent
      return nullptr;
    }
  }

  // Translate the command. Now reduced to just this switch
  HTTPCommand type = HTTPCommand::http_no_command;
  switch(p_request->Verb)
  {
    case HttpVerbOPTIONS:   type = HTTPCommand::http_options;    break;
    case HttpVerbGET:       type = HTTPCommand::http_get;        break;
    case HttpVerbHEAD:      type = HTTPCommand::http_head;       break;
    case HttpVerbPOST:      type = HTTPCommand::http_post;       break;
    case HttpVerbPUT:       type = HTTPCommand::http_put;        break;
    case HttpVerbDELETE:    type = HTTPCommand::http_delete;     break;
    case HttpVerbTRACE:     type = HTTPCommand::http_trace;      break;
    case HttpVerbCONNECT:   type = HTTPCommand::http_connect;    break;
    case HttpVerbMOVE:      type = HTTPCommand::http_move;       break;
    case HttpVerbCOPY:      type = HTTPCommand::http_copy;       break;
    case HttpVerbPROPFIND:  type = HTTPCommand::http_proppfind;  break;
    case HttpVerbPROPPATCH: type = HTTPCommand::http_proppatch;  break;
    case HttpVerbMKCOL:     type = HTTPCommand::http_mkcol;      break;
    case HttpVerbLOCK:      type = HTTPCommand::http_lock;       break;
    case HttpVerbUNLOCK:    type = HTTPCommand::http_unlock;     break;
    case HttpVerbSEARCH:    type = HTTPCommand::http_search;     break;
    default:                // Try to get a less known verb as 'last resort'
                            type = GetUnknownVerb(p_request->pUnknownVerb);
                            if(type == HTTPCommand::http_no_command)
                            {
                              ERRORLOG(ERROR_INVALID_PARAMETER,_T("Unknown HTTP Verb"));
                              HTTPMessage msg(HTTPCommand::http_response,HTTP_STATUS_NOT_SUPPORTED);
                              msg.SetRequestHandle((HTTP_OPAQUE_ID) p_context);
                              msg.SetHTTPSite(p_site);
                              RespondWithServerError(&msg,HTTP_STATUS_NOT_SUPPORTED,_T("Not implemented"));
                              return nullptr;
                            }
                            break;
  }

  // For all types of requests: Create the HTTPMessage
  HTTPMessage* message = new HTTPMessage(type,p_site);
  message->SetURL(rawUrl);
  message->SetReferrer(referrer);
  message->SetAuthorization(authorize);
  message->SetConnectionID(p_request->ConnectionId);
  message->SetRemoteDesktop(remDesktop);
  message->SetSender((PSOCKADDR_IN6)sender);
  message->SetCookiePairs(cookie);
  message->SetAcceptEncoding(acceptEncoding);
  message->SetRequestHandle((HTTP_OPAQUE_ID)p_context);
  message->SetContentType(contentType);
  message->SetContentLength((size_t)_ttoll(contentLength));
  message->SetAllHeaders(&p_request->Headers);
  message->SetUnknownHeaders(&p_request->Headers);
  message->SetEncoding(encoding);

  // Finding the impersonation access token (if any)
  FindingAccessToken(p_context,message);

  // Handle modified-since 
  // Rest of the request is then not needed any more
  if(type == HTTPCommand::http_get && !modified.IsEmpty())
  {
    message->SetHTTPTime(modified);
    if(DoIsModifiedSince(message))
    {
      delete message;
      return nullptr;
    }
  }

  // Find routing information within the site
  CalculateRouting(p_site,message);

  // Find X-HTTP-Method VERB Tunneling
  if(type == HTTPCommand::http_post && p_site->GetVerbTunneling())
  {
    if(message->FindVerbTunneling())
    {
      DETAILLOGV(_T("Request VERB changed to: %s"),message->GetVerb().GetString());
    }
  }

  // See if there are more entity chunks in the queue of IIS
  if(p_request->Flags & HTTP_REQUEST_FLAG_MORE_ENTITY_BODY_EXISTS)
  {
    // Remember the fact that we should read the rest of the message, and how much
    // We offload this to the HTTPSite handler for local servers and IIS alike
    message->SetReadBuffer(true,_ttoi(contentLength));
  }
  else
  {
    // Chunks already read by IIS, so just copy them in the message
    // No more are coming, we already have everything for a HTTPMessage
    if(p_request->EntityChunkCount)
    {
      ReadEntityChunks(message,p_request,encoding);
    }
  }
  // This is the result
  return message;
}

// Reading the first chunks directly from the request handle from IIS
void
HTTPServerIIS::ReadEntityChunks(HTTPMessage* p_message,PHTTP_REQUEST p_request,Encoding p_encoding)
{
  unsigned count = p_request->EntityChunkCount;
  FileBuffer* buffer = p_message->GetFileBuffer();

  // Read all chunks
  for(unsigned ind = 0; ind < count; ++ind)
  {
    PHTTP_DATA_CHUNK chunk = &p_request->pEntityChunks[ind];
    switch(chunk->DataChunkType)
    {
      case HttpDataChunkFromMemory:            buffer->AddBuffer(reinterpret_cast<uchar*>
                                                                (chunk->FromMemory.pBuffer)
                                                                ,chunk->FromMemory.BufferLength);
                                               break;
      case HttpDataChunkFromFileHandle:        // Should not happen upon receive
                                               [[fallthrough]];
      case HttpDataChunkFromFragmentCache:     [[fallthrough]];
      case HttpDataChunkFromFragmentCacheEx:   ERRORLOG(ERROR_INVALID_PARAMETER,_T("Unhandled HTTP chunk type from IIS"));
                                               break;
    }
  }
  // Log & Trace the chunks that we just read 
  LogTraceRequestBody(p_message,p_encoding);
}

// Receive incoming HTTP request (p_request->Flags > 0)
// But only reading extra buffer parts into an already partially filled buffer
bool
HTTPServerIIS::ReceiveIncomingRequest(HTTPMessage* p_message,Encoding p_encoding)
{
  UNREFERENCED_PARAMETER(p_message);
  IHttpContext* context     = reinterpret_cast<IHttpContext*>(p_message->GetRequestHandle());
  IHttpRequest* httpRequest = context->GetRequest();
  size_t      contentLength = p_message->GetContentLength();

  // Reading the buffer
  FileBuffer* fbuffer    = p_message->GetFileBuffer();
  BYTE*       bytebuffer = new BYTE[INIT_HTTP_BUFFERSIZE + 1];

  // Main loop: as long as we must read extra bytes
  while(httpRequest->GetRemainingEntityBytes() > 0)
  {
    DWORD  received  = 0;
    HRESULT hr = httpRequest->ReadEntityBody(bytebuffer,(DWORD)INIT_HTTP_BUFFERSIZE,FALSE,&received);
    if(HRESULT_CODE(hr) == ERROR_HANDLE_EOF)
    {
      // Ready reading the message
      break;
    }
    if(!SUCCEEDED(hr) && HRESULT_CODE(hr) != ERROR_MORE_DATA)
    {
      ERRORLOG(HRESULT_FROM_WIN32(hr),_T("Cannot read incoming HTTP buffer"));
      return false;
    }
    // Add to file buffer
    if(received > 0)
    {
      fbuffer->AddBuffer(bytebuffer,received);
    }
  }
  delete[] bytebuffer;

    // Check if we received the total predicted message
  // This includes all blocks from initial chunk and extra reads
  if(fbuffer->GetLength() < contentLength)
  {
      ERRORLOG(ERROR_INVALID_DATA,_T("Total received message shorter dan 'ContentLength' header."));
  }

  // Now also trace the request body of the message
  LogTraceRequestBody(p_message,p_encoding);

  // Now everything has been read for this message
  p_message->SetReadBuffer(false,0);

  return true;
}

// Create a new WebSocket in the subclass of our server
WebSocket* 
HTTPServerIIS::CreateWebSocket(XString p_uri)
{
  WebSocketServerIIS* socket = new WebSocketServerIIS(p_uri);

  // Connect the server logfile, and logging level
  socket->SetLogfile(m_log);
  socket->SetLogLevel(m_logLevel);

  return socket;
}

// Receive the WebSocket stream and pass on the the WebSocket
void
HTTPServerIIS::ReceiveWebSocket(WebSocket* p_socket,HTTP_OPAQUE_ID /*p_request*/)
{
  WebSocketServerIIS* socket = reinterpret_cast<WebSocketServerIIS*>(p_socket);
  if(socket)
  {
    // Enter the ASYNC listener loop of the WebSocket
    socket->SocketListener();
  }
}

bool
HTTPServerIIS::FlushSocket(HTTP_OPAQUE_ID p_request,XString /*p_prefix*/)
{
  IHttpContext*     context  = reinterpret_cast<IHttpContext*>(p_request);
  IHttpResponse*    response = context->GetResponse();
  IHttpCachePolicy* policy   = response->GetCachePolicy();
  DWORD bytesSent = 0;

  // Do not buffer. Also turn dynamic compression OFF in IIS-Admin!
  response->DisableBuffering();    // Disable buffering
  response->DisableKernelCache(9); // 9 = HANDLER_HTTPSYS_UNFRIENDLY
  policy->DisableUserCache();      // Disable user caching

  BOOL completion = FALSE;
  HRESULT hr = response->Flush(FALSE,TRUE,&bytesSent,&completion);
  if(hr != S_OK)
  {
    ERRORLOG(HRESULT_FROM_WIN32(hr),_T("Flushing WebSocket failed!"));
    CancelRequestStream(p_request);
    return false;
  }
  return true;
}

// Finding the impersonation access token
void
HTTPServerIIS::FindingAccessToken(IHttpContext* p_context,HTTPMessage* p_message)
{
  // In case of authentication done: get the authentication token
  HANDLE token = NULL;
  IHttpUser* user = p_context->GetUser();
  if(user)
  {
    // Make duplicate of the token, otherwise IIS will crash!
    if(DuplicateTokenEx(user->GetImpersonationToken()
                        ,TOKEN_DUPLICATE | TOKEN_IMPERSONATE | TOKEN_ALL_ACCESS | TOKEN_READ | TOKEN_WRITE
                        ,NULL
                        ,SecurityImpersonation
                        ,TokenImpersonation
                        ,&token) == FALSE)
    {
      token = NULL;
    }
    else
    {
      // Store the context with the message, so we can handle all derived messages
      p_message->SetAccessToken(token);
    }
  }
}

// Init the stream response
bool
HTTPServerIIS::InitEventStream(EventStream& p_stream)
{
  IHttpContext*    context = reinterpret_cast<IHttpContext*>(p_stream.m_requestID);
  IHttpResponse*  response = context->GetResponse();
  IHttpCachePolicy* policy = response->GetCachePolicy();

  // First comment to push to the stream (not an event!)
  // Always UTF-8 compatible, so simple ANSI string
  char* init = ":init event-stream\r\n\r\n";
  int length = (int) strlen(init);

  response->SetStatus(HTTP_STATUS_OK,"OK");

  // Add event/stream header
  SetResponseHeader(response,HttpHeaderContentType,_T("text/event-stream"),true);

  HTTP_DATA_CHUNK dataChunk;
  DWORD bytesSent = 0;

  // Only if a buffer present
  dataChunk.DataChunkType           = HttpDataChunkFromMemory;
  dataChunk.FromMemory.pBuffer      = reinterpret_cast<void*>(init);
  dataChunk.FromMemory.BufferLength = static_cast<ULONG>(length);

  // Buffering should be turned off, so chunks get written right away
  response->DisableBuffering();    // Disable buffering
  response->DisableKernelCache(9); // 9 = HANDLER_HTTPSYS_UNFRIENDLY
  policy->DisableUserCache();      // Disable user caching

  HRESULT hr = response->WriteEntityChunks(&dataChunk,1,FALSE,true,&bytesSent);
  if(hr != S_OK)
  {
    ERRORLOG(HRESULT_FROM_WIN32(hr),_T("HttpSendResponseEntityBody failed initialisation of an event stream"));
  }
  else
  {
    // Increment chunk count
    ++p_stream.m_chunks;

    DETAILLOGV(_T("WriteEntityChunks for event stream sent [%d] bytes"),bytesSent);
    // Possibly log and trace what we just sent
    LogTraceResponse(response->GetRawHttpResponse(),reinterpret_cast<uchar*>(init),length);

    hr = response->Flush(false,true,&bytesSent);
  }
  return (hr == S_OK);
}

void
HTTPServerIIS::SetResponseHeader(IHttpResponse* p_response,XString p_name,XString p_value,bool p_replace)
{
#ifdef _UNICODE
  AutoCSTR name(p_name);
  AutoCSTR value(p_value);
  HRESULT hr = p_response->SetHeader(name.cstr(),value.cstr(),(USHORT)value.size(),p_replace);
#else
  HRESULT hr = p_response->SetHeader(p_name,p_value,(USHORT)p_value.GetLength(),p_replace);
#endif
  if(hr != S_OK)
  {
    XString bark;
    bark.Format(_T("Cannot set HTTP response header [%s] to value [%s]"),p_name.GetString(),p_value.GetString());
    ERRORLOG(HRESULT_FROM_WIN32(hr),bark);
  }
}

void 
HTTPServerIIS::SetResponseHeader(IHttpResponse* p_response,HTTP_HEADER_ID p_id,XString p_value,bool p_replace)
{
#ifdef _UNICODE
  AutoCSTR value(p_value);
  HRESULT hr = p_response->SetHeader(p_id,value.cstr(),(USHORT)value.size(),p_replace);
#else
  HRESULT hr = p_response->SetHeader(p_id,p_value,(USHORT)p_value.GetLength(),p_replace);
#endif
  if(hr != S_OK)
  {
    XString bark;
    bark.Format(_T("Cannot set HTTP response header [%d] to value [%s]"),p_id,p_value.GetString());
    ERRORLOG(HRESULT_FROM_WIN32(hr),bark);
  }
}

void
HTTPServerIIS::AddUnknownHeaders(IHttpResponse* p_response,UKHeaders& p_headers)
{
  // Something to do?
  if(p_headers.empty())
  {
    return;
  }
  for(auto& header : p_headers)
  {
    SetResponseHeader(p_response,header.m_name,header.m_value,false);
  }
}

// Setting the overall status of the response message
void
HTTPServerIIS::SetResponseStatus(IHttpResponse* p_response,USHORT p_status,XString p_statusMessage)
{
  DETAILLOGV(_T("HTTP Response: %u %s"),p_status,p_statusMessage.GetString());
#ifdef _UNICODE
  AutoCSTR message(p_statusMessage);
  p_response->SetStatus(p_status,message.cstr());
#else
  p_response->SetStatus(p_status,p_statusMessage);
#endif
}

// Sending a response as a chunk
void
HTTPServerIIS::SendAsChunk(HTTPMessage* p_message,bool p_final /*= false*/)
{
  // Check if multi-part buffer or file
  FileBuffer* buffer = p_message->GetFileBuffer();
  if(!buffer->GetFileName().IsEmpty() )
  {
    ERRORLOG(ERROR_INVALID_PARAMETER,_T("Send as chunk cannot send a file!"));
    return;
  }
  // Chunk encode the file buffer
  if(!buffer->ChunkedEncoding(p_final))
  {
    ERRORLOG(ERROR_NOT_ENOUGH_MEMORY,_T("Cannot chunk-encode the message for transfer-encoding!"));
  }

  // If we want to send a (g)zipped buffer, that should have been done already by now
  p_message->SetAcceptEncoding(_T(""));

  // Get the chunk number (first->next)
  unsigned chunk = p_message->GetChunkNumber();
  p_message->SetChunkNumber(++chunk);
  if(chunk == 1)
  {
    // Send the first chunk
    SendResponse(p_message);
  }
  else
  {
    // Send all next chunks
    IHttpContext*  context  = reinterpret_cast<IHttpContext*>(p_message->GetRequestHandle());
    IHttpResponse* response = context ? context->GetResponse() : nullptr;
    if(response)
    {
      // Send the response
      SendResponseBuffer(response,buffer,buffer->GetLength(),!p_final);
      // Possibly log and trace what we just sent
      LogTraceResponse(response->GetRawHttpResponse(),p_message,p_message->GetEncoding());
    }
  }
  if(p_final)
  {
    // Do **NOT** send an answer twice
    p_message->SetHasBeenAnswered();
  }
}

// Sending response for an incoming message
void       
HTTPServerIIS::SendResponse(HTTPMessage* p_message)
{
  IHttpContext*   context     = reinterpret_cast<IHttpContext*>(p_message->GetRequestHandle());
  IHttpResponse*  response    = context ? context->GetResponse() : nullptr;
  FileBuffer*     buffer      = p_message->GetFileBuffer();
  XString         contentType(_T("application/octet-stream")); 
  bool            moredata    = false;

  // See if there is something to send
  if(context == nullptr || response == nullptr)
  {
    ERRORLOG(ERROR_INVALID_PARAMETER,_T("Nothing found to send"));
    return;
  }

  // Respond to general HTTP status
  int status = p_message->GetStatus();
  XString date = HTTPGetSystemTime();

  // Protocol switch must keep the channel open (here for: WebSocket!)
  if(status == HTTP_STATUS_SWITCH_PROTOCOLS)
  {
    moredata = true;
  }

  // Setting the response status
  SetResponseStatus(response,(USHORT) p_message->GetStatus(),GetHTTPStatusText(p_message->GetStatus()));

  // In case of a 401, we challenge to the client to identify itself
  if (status == HTTP_STATUS_DENIED)
  {
    if(!p_message->GetXMLHttpRequest())
    {
      // See if the message already has an authentication scheme header
      XString challenge = p_message->GetHeader(_T("AuthenticationScheme"));
      if(challenge.IsEmpty())
      {
        // Add authentication scheme
        HTTPSite* site = p_message->GetHTTPSite();
        challenge = BuildAuthenticationChallenge(site->GetAuthenticationScheme()
                                                ,site->GetAuthenticationRealm());
        SetResponseHeader(response,HttpHeaderWwwAuthenticate,challenge,true);
      }
    }
    SetResponseHeader(response,HttpHeaderDate,date,true);
  }

  // In case we want IIS to handle the response, and we do nothing!
  // This status get's caught in the MarlinModule::GetCompletionStatus
  if(status == HTTP_STATUS_CONTINUE)
  {
    // Log that we do not do this message, but we pass it on to IIS
    DETAILLOGV(_T("We do **NOT** handle this url: "),p_message->GetCrackedURL().SafeURL().GetString());

    // Do **NOT** send an answer twice
    p_message->SetHasBeenAnswered();

    return;
  }

  // Add a known header. (octet-stream or the message content type)
  if(!p_message->GetContentType().IsEmpty())
  {
    contentType = p_message->GetContentType();
  }
  else
  {
    XString cttype = p_message->GetHeader(_T("Content-type"));
    if(!cttype.IsEmpty())
    {
      contentType = cttype;
    }
  }
  p_message->DelHeader(_T("Content-Type"));

  // AddKnownHeader(response,HttpHeaderContentType,contentType);
  SetResponseHeader(response,HttpHeaderContentType,contentType,true);

  // Add the server header or suppress it
  switch(m_sendHeader)
  {
    case SendHeader::HTTP_SH_MICROSOFT:   // Do nothing, Microsoft will add the server header
                                          break;
    case SendHeader::HTTP_SH_MARLIN:      SetResponseHeader(response,HttpHeaderServer,_T(MARLIN_SERVER_VERSION),true);
                                          break;
    case SendHeader::HTTP_SH_APPLICATION: SetResponseHeader(response,HttpHeaderServer,m_name,true);
                                          break;
    case SendHeader::HTTP_SH_WEBCONFIG:   SetResponseHeader(response,HttpHeaderServer,m_configServerName,true);
                                          break;
    case SendHeader::HTTP_SH_HIDESERVER:  // Fill header with empty string will suppress it
                                          SetResponseHeader(response,HttpHeaderServer,_T(""),true);
                                          break;
  }
  p_message->DelHeader(_T("Server"));

  // Cookie settings
  bool cookiesHasSecure(false);
  bool cookiesHasHttp(false);
  bool cookiesHasSame(false);
  bool cookiesHasPath(false);
  bool cookiesHasDomain(false);
  bool cookiesHasExpires(false);
  bool cookiesHasMaxAge(false);

  bool cookieSecure(false);
  bool cookieHttpOnly(false);
  CookieSameSite cookieSameSite(CookieSameSite::NoSameSite);
  XString    cookiePath;
  XString    cookieDomain;
  int        cookieExpires = 0;
  int        cookieMaxAge  = 0;

  // Getting the site settings
  HTTPSite* site = p_message->GetHTTPSite();
  if(site)
  {
    cookiesHasSecure  = site->GetCookieHasSecure();
    cookiesHasHttp    = site->GetCookieHasHttpOnly();
    cookiesHasSame    = site->GetCookieHasSameSite();
    cookiesHasPath    = site->GetCookieHasPath();
    cookiesHasDomain  = site->GetCookieHasDomain();
    cookiesHasExpires = site->GetCookieHasExpires();
    cookiesHasMaxAge  = site->GetCookieHasMaxAge();

    cookieSecure      = site->GetCookiesSecure();
    cookieHttpOnly    = site->GetCookiesHttpOnly();
    cookieSameSite    = site->GetCookiesSameSite();
    cookiePath        = site->GetCookiesPath();
    cookieDomain      = site->GetCookiesDomain();
    cookieExpires     = site->GetCookiesExpires();
    cookieMaxAge      = site->GetCookiesMaxAge();
  }

  // Add cookies to the unknown response headers
  // Because we can have more than one Set-Cookie: header
  // and HTTP API just supports one set-cookie.
  XString setcookie = p_message->GetHeader(_T("Set-Cookie"));
  if(!setcookie.IsEmpty())
  {
    SetResponseHeader(response,HttpHeaderSetCookie,setcookie,false);
    p_message->DelHeader(_T("Set-Cookie"));
  }
  UKHeaders ukheaders;
  Cookies& cookies = p_message->GetCookies();
  if(!cookies.GetCookies().empty())
  {
    for(auto& cookie : cookies.GetCookies())
    {
      if(cookiesHasSecure)  cookie.SetSecure(cookieSecure);
      if(cookiesHasHttp)    cookie.SetHttpOnly(cookieHttpOnly);
      if(cookiesHasSame)    cookie.SetSameSite(cookieSameSite);
      if(cookiesHasPath)    cookie.SetPath(cookiePath);
      if(cookiesHasDomain)  cookie.SetDomain(cookieDomain);
      if((cookie.GetMaxAge() != 0) && cookiesHasMaxAge)
      {
        cookie.SetMaxAge(cookieMaxAge);
      }
      if(cookiesHasExpires && cookieExpires > 0 && cookie.GetMaxAge() >= 0)
      {
        SYSTEMTIME current;
        GetSystemTime(&current);
        AddSecondsToSystemTime(&current,&current,60 * (double)cookieExpires);
        cookie.SetExpires(&current);
      }
      ukheaders.push_back(UKHeader(_T("Set-Cookie"),cookie.GetSetCookieText()));
    }
  }


  // Add extra headers from the message, except for content-length and set-cookie
  p_message->DelHeader(_T("Content-Length"));

  HeaderMap* map = p_message->GetHeaderMap();
  for(HeaderMap::iterator it = map->begin(); it != map->end(); ++it)
  {
    ukheaders.push_back(UKHeader(it->first,it->second));
  }

  // Add other optional security headers like CORS etc.
  if(site)
  {
    site->AddSiteOptionalHeaders(ukheaders);
  }

  // Possible zip the contents, and add content-encoding header
  if(p_message->GetHTTPSite()->GetHTTPCompression() && buffer)
  {
    // But only if the client side requested it
    if(p_message->GetAcceptEncoding().Find(_T("gzip")) >= 0)
    {
      if(buffer->ZipBuffer())
      {
        DETAILLOGV(_T("GZIP the buffer to size: %lu"),buffer->GetLength());
        ukheaders.push_back(UKHeader(_T("Content-Encoding"),_T("gzip")));
      }
    }
  }

  // Now add all unknown headers to the response
  AddUnknownHeaders(response,ukheaders);

  // Now after the compression, add the total content length
  size_t totalLength = buffer ? buffer->GetLength() : 0;
  if(p_message->GetChunkNumber())
  {
    moredata = true;
    SetResponseHeader(response,HttpHeaderTransferEncoding,_T("chunked"),true);
  }
  else
  {
    XString contentLength;
#ifdef _WIN64
    contentLength.Format(_T("%I64u"), totalLength);
#else
    contentLength.Format(_T("%lu"), totalLength);
#endif
    SetResponseHeader(response,HttpHeaderContentLength,contentLength,true);
  }
  // Dependent on the filling of FileBuffer
  // Send 1 or more buffers or the file
  if(buffer && buffer->GetHasBufferParts())
  {
    SendResponseBufferParts(response,buffer,totalLength,moredata);
  }
  else if(buffer && buffer->GetFileName().IsEmpty())
  {
    SendResponseBuffer(response,buffer,totalLength,moredata);
  }
  else
  {
    SendResponseFileHandle(response,buffer,moredata);
  }

  if(status >= HTTP_STATUS_AMBIGUOUS)
  {
    DWORD sent = 0;
    response->Flush(FALSE,moredata,&sent);

    if(::GetLastError())
    {
      // Error handler
      XString message = GetLastErrorAsString();
      m_log->AnalysisLog(_T(__FUNCTION__),LogType::LOG_ERROR,true,_T("HTTP OS error [%d:%s]"),::GetLastError(),message.GetString());
      SetLastError(NO_ERROR);
    }
  }
  // Possibly log and trace what we just sent
  LogTraceResponse(response->GetRawHttpResponse(),p_message,p_message->GetEncoding());

  if(!p_message->GetChunkNumber())
  {
	  // Do **NOT** send an answer twice
	  p_message->SetHasBeenAnswered();
  }
}

// Sub-functions for SendResponse
bool
HTTPServerIIS::SendResponseBuffer(IHttpResponse*  p_response
                                 ,FileBuffer*     p_buffer
                                 ,size_t          p_totalLength
                                 ,bool            p_more /*= false*/)
{
  uchar* entity       = NULL;
  DWORD  result       = 0;
  size_t entityLength = 0;
  HTTP_DATA_CHUNK dataChunk;

  // Get buffer to send
  if(p_totalLength)
  {
    p_buffer->GetBuffer(entity,entityLength);
  }

  // Only if a buffer present
  if(entity && entityLength)
  {
    // Add an entity chunk.
    dataChunk.DataChunkType           = HttpDataChunkFromMemory;
    dataChunk.FromMemory.pBuffer      = entity;
    dataChunk.FromMemory.BufferLength = (ULONG)entityLength;

    DWORD sent = 0L;
    BOOL  completion = false;
    HRESULT hr = p_response->WriteEntityChunks(&dataChunk,1,false,p_more,&sent,&completion);
    if(SUCCEEDED(hr))
    {
      DETAILLOGV(_T("ResponseBuffer [%u] bytes sent"),(ULONG)entityLength);
    }
    else
    {
      result = HRESULT_FROM_WIN32(hr);
      ERRORLOG(result,_T("ResponseBuffer"));
    }
  }
  return (result == NO_ERROR);
}

void
HTTPServerIIS::SendResponseBufferParts(IHttpResponse* p_response
                                      ,FileBuffer*    p_buffer
                                      ,size_t         p_totalLength
                                      ,bool           p_moredata /*= false*/)
{
  int    transmitPart = 0;
  uchar* entityBuffer = NULL;
  size_t entityLength = 0;
  size_t totalSent    = 0;
  DWORD  bytesSent    = 0;
  HTTP_DATA_CHUNK dataChunk;
  memset(&dataChunk,0,sizeof(HTTP_DATA_CHUNK));

  while(p_buffer->GetBufferPart(transmitPart,entityBuffer,entityLength))
  {
    if(entityBuffer)
    {
      // Add an entity chunk.
      dataChunk.DataChunkType           = HttpDataChunkFromMemory;
      dataChunk.FromMemory.pBuffer      = entityBuffer;
      dataChunk.FromMemory.BufferLength = (ULONG)entityLength;
    }
    // Flag to calculate the last sending part
    bool moreData = p_moredata;
    if(p_moredata == false)
    {
      moreData = (totalSent + entityLength) < p_totalLength;
    }
    BOOL completion = false;

    HRESULT hr = p_response->WriteEntityChunks(&dataChunk,1,false,moreData,&bytesSent,&completion);

    if(SUCCEEDED(hr))
    {
      DETAILLOGV(_T("HTTP ResponseBufferPart [%d] bytes sent"),bytesSent);
    }
    else
    {
      DWORD result = HRESULT_FROM_WIN32(hr);
      ERRORLOG(result,_T("HTTP ResponseBufferPart error"));
      break;
    }
    // Next buffer part
    totalSent += entityLength;
    ++transmitPart;
  }
}

void
HTTPServerIIS::SendResponseFileHandle(IHttpResponse* p_response,FileBuffer* p_buffer,bool p_more /*= false*/)
{
  HANDLE file = NULL;
  HTTP_DATA_CHUNK dataChunk;
  memset(&dataChunk,0,sizeof(HTTP_DATA_CHUNK));

  // Check input
  if(!p_response || !p_buffer)
  {
    ERRORLOG(ERROR_INVALID_PARAMETER,_T("FATAL Error sending FILE handle!"));
    return;
  }

  // File to transmit
  if(p_buffer->OpenFile() == false)
  {
    ERRORLOG(::GetLastError(),_T("OpenFile for SendHttpResponse"));
    return;
  }
  // Get the file handle from buffer
  file = p_buffer->GetFileHandle();

  // See if file is under 4G large
  DWORD high = 0L;
  ULONG fileSize = GetFileSize(file,&high);
  if(high)
  {
    // 413: Request entity too large
    SendResponseError(p_response,m_clientErrorPage,413,_T("Request entity too large"));
    // Error state
    ERRORLOG(ERROR_INVALID_PARAMETER,_T("File to send is too big (>4G)"));
    // Close the file handle
    p_buffer->CloseFile();
    return;
  }

  // Send entity body from a file handle.
  dataChunk.DataChunkType = HttpDataChunkFromFileHandle;
  dataChunk.FromFileHandle.ByteRange.StartingOffset.QuadPart = 0;
  dataChunk.FromFileHandle.ByteRange.Length.QuadPart = fileSize; // HTTP_BYTE_RANGE_TO_EOF;
  dataChunk.FromFileHandle.FileHandle = file;

  DWORD sent = 0L;
  BOOL  completion = false;

  // Writing the chunk
  HRESULT hr = p_response->WriteEntityChunks(&dataChunk,1,false,p_more,&sent,&completion);
  if(SUCCEEDED(hr))
  {
    DETAILLOGV(_T("SendResponseEntityBody for file. Bytes: %ul"),fileSize);
  }
  else
  {
    DWORD result = HRESULT_FROM_WIN32(hr);
    ERRORLOG(result,_T("SendResponseEntityBody for file"));
  }
  // Now close our file handle
  p_buffer->CloseFile();
}

void
HTTPServerIIS::SendResponseError(IHttpResponse* p_response
                                ,XString&       p_page
                                ,int            p_error
                                ,LPCTSTR        p_reason)
{
  XString sending;
  HTTP_DATA_CHUNK dataChunk;
  memset(&dataChunk,0,sizeof(HTTP_DATA_CHUNK));

  // Format our error page
  sending.Format(p_page,p_error,p_reason);

  // Add an entity chunk.
  dataChunk.DataChunkType           = HttpDataChunkFromMemory;
  dataChunk.FromMemory.pBuffer      = reinterpret_cast<void*>(const_cast<TCHAR*>(sending.GetString()));
  dataChunk.FromMemory.BufferLength = sending.GetLength();

  DWORD sent = 0L;
  BOOL  completion = false;

  // Writing the chunk
  HRESULT hr = p_response->WriteEntityChunks(&dataChunk,1,false,false,&sent,&completion);
  if(SUCCEEDED(hr))
  {
    DETAILLOGV(_T("SendResponseError. Bytes sent: %d"),sent);
    if(completion == false)
    {
      ERRORLOG(ERROR_INVALID_FUNCTION,_T("ResponseFileHandle: But IIS does not expect to deliver the contents!!"));
    }
    // Possibly log and trace what we just sent
    LogTraceResponse(p_response->GetRawHttpResponse(),reinterpret_cast<uchar*>(const_cast<TCHAR*>(sending.GetString())),sending.GetLength());
  }
  else
  {
    DWORD result = HRESULT_FROM_WIN32(hr);
    ERRORLOG(result,_T("SendResponseEntityBody for file"));
  }
}

// Sending a chunk to an event stream
bool 
HTTPServerIIS::SendResponseEventBuffer(HTTP_OPAQUE_ID     p_response
                                      ,CRITICAL_SECTION*  p_lock
                                      ,BYTE**             p_buffer
                                      ,size_t             p_length
                                      ,bool               p_continue /*= true*/)
{
  AutoCritSec lockme(p_lock);

  // Check if we have something to send
  if(p_response == NULL || p_buffer == nullptr)
  {
    return false;
  }

  // Getting our context
  DWORD  bytesSent = 0;
  HTTP_DATA_CHUNK dataChunk;
  IHttpContext*   context  = reinterpret_cast<IHttpContext*>(p_response);
  IHttpResponse*  response = context->GetResponse();

  // Check that we have a response object
  if(response == nullptr)
  {
    return false;
  }

  // Only if a buffer present
  dataChunk.DataChunkType           = HttpDataChunkFromMemory;
  dataChunk.FromMemory.pBuffer      = (void*)*p_buffer;
  dataChunk.FromMemory.BufferLength = (ULONG)p_length;

  HRESULT hr = response->WriteEntityChunks(&dataChunk,1,FALSE,p_continue,&bytesSent);
  if(hr != S_OK)
  {
    ERRORLOG(HRESULT_FROM_WIN32(hr),_T("WriteEntityChunks failed for SendEvent"));
  }
  else
  {
    DETAILLOGV(_T("WriteEntityChunks for event stream sent [%d] bytes"),p_length);
    hr = response->Flush(false,p_continue,&bytesSent);
    if(hr != S_OK && p_continue)
    {
      ERRORLOG(HRESULT_FROM_WIN32(hr),_T("Flushing event stream failed!"));
    }
    else
    {
      // Possibly log and trace what we just sent
      LogTraceResponse(nullptr,*p_buffer,(unsigned)p_length);
    }
  }
  // Final closing of the connection
  if(p_continue == false)
  {
    CancelRequestStream(p_response);
  }
  return (hr == S_OK);
}

// Used for canceling a WebSocket or an event stream
void
HTTPServerIIS::CancelRequestStream(HTTP_OPAQUE_ID p_response,bool p_doReset /*=false*/)
{
  IHttpContext*  context  = reinterpret_cast<IHttpContext*>(p_response);
  IHttpResponse* response = context->GetResponse();

  try
  {
    // Set disconnection
    if(p_doReset)
    {
      context->PostCompletion(0);
    }
    else
    {
      response->SetNeedDisconnect();
    }
    // Now ready with the IIS context. Original request is finished
    context->IndicateCompletion(RQ_NOTIFICATION_FINISH_REQUEST);

    DETAILLOG1(_T("Event/Socket connection closed"));
  }
  catch(StdException& er)
  {
    ReThrowSafeException(er);
    ERRORLOG(ERROR_INVALID_PARAMETER,_T("Cannot close Event/WebSocket stream! ") + er.GetErrorMessage());
  }
}

