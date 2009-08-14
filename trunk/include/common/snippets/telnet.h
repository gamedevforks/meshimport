#ifndef TELNET_H

#define TELNET_H

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
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

//***********************************************************************************
//*** This code snippet allows you to embed a telnet client or server easily into any
//*** of your applications.  Currently this code only builds for Windows and the XBOX
//*** It is very desirable to me to have this code build for APPLE, Linux, and Iphone
//*** If you wish to contribute to this cause, please let me know and I will add you
//*** as a developer to the google code page.  http://code.google.com/p/telnet
//***
//*** To test the program simply run 'telnet.exe'.
//***
//*** The first time you run it, it will be set up as a telnet server.  You can
//*** connect to it by runing a telnet client and typing 'open localhost 23'
//*** If you run the program a second time, it will detect that a server is already
//*** using Port 23 and will instead start up as a client.  You can now send messages
//*** between the two instances of the application.
//*** You can keep launching as many of them as you wish.
//***********************************************************************************

namespace TELNET
{

class Telnet
{
public:
  virtual bool          isServer(void) = 0; // returns true if we are a server or a client.  First one created on a machine is a server, additional copies are clients.
  virtual bool          haveConnection(void) = 0; // returns true if we (as a client) successfully connected to the server.
  virtual bool          sendMessage(NxU32 client,const char *fmt,...) = 0; // send a message to the server, all clients (client=0) or just a specific client.
  virtual const char *  receiveMessage(NxU32 &client) = 0; // receive an incoming message (client=0) means it came from the server, otherwise it designates a specific client.
  virtual const char ** getArgs(const char *input,NxU32 &argc) = 0; // parse string into a series of arguments.

  virtual bool          sendBlob(NxU32 client,const char *blobType,const void *data,NxU32 dlen) = 0;
  virtual const char *  receiveBlob(NxU32 &client,const void *&data,NxU32 &dlen) = 0;

protected:
  virtual ~Telnet(void) { };
};

Telnet * createTelnet(const char *address="LOCALHOST",NxU32 port=23);
void     releaseTelnet(Telnet *t);

};

extern TELNET::Telnet *gTelnet; // optional global variable representing the TELNET singleton for the application.

#endif
