#ifndef WIN_MSG_H

#define WIN_MSG_H


/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
**
** If you wish to contact me you can use the following methods:
**
** email: jratcliffscarab@gmail.com
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, freeof charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

// handles inter-application communication by sending windows messages to hidden windows.

class WinMsg
{
public:
	virtual const char * receiveWindowsMessage(void) = 0;
	virtual const char * receiveWindowsMessageBlob(const void *&msg,NxU32 &len) = 0;
    virtual const char ** getArgs(const char *input,NxU32 &argc) = 0; // parse string into a series of arguments.
    virtual bool sendWinMsg(const char *app,const char *fmt,...) = 0;
    virtual bool sendWinMsgBinary(const char *app,const char *blobType,const void *mem,NxU32 len) = 0;
    virtual bool hasWindow(const char *app) = 0; // returns true if this application window exists on the system.
};

WinMsg * createWinMsg(const char *windowName); // can be null if this is being used only for send messages.
void     releaseWinMsg(WinMsg *msg);


#endif
