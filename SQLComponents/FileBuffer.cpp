#include "stdafx.h"
#include "FileBuffer.h"

#ifndef __FILEBUFFER__

// Static globals for the server as a whole
// Can be set through the web.config reading of the HTTPServer
unsigned long g_streaming_limit = STREAMING_LIMIT;

#endif