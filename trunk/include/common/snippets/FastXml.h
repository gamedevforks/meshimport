#ifndef FAST_XML_H

#define FAST_XML_H

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
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


// This code snippet provides an extremely lightweight and fast XML parser.
// This parser only handles data elements as if they were streamed data.
// It is important to note that all pointers returned by this parser are
// persistent for the lifetime of the FastXml class.  This means you can cache
// copies of the pointers (rather than copying any data) if this matches your
// needs.


// Simpy call createFastXml to get a copy of the FastXml parsing interface
// To parse an XML file, have your application inherit the pure virtual
// interface called 'FastXmlInterface' and implement the single method 'processElement'
//
// For each element in the XML file you will get a callback with the following
// data.
//
// 'elementName' the name of the element (this pointer is persistent)
// 'argc'  The total number of attributes and values for this element.
//         The number of attribute/value pairs is equal to argc/2
// 'argv'  The attribute/value pairs in the form of attribute/value, attribute/value..
//         These pointers are persistent and can be cached if needed (until FastXml is released)
// 'elementData' optional data (i.e. text) associated with the element.  If this is a null pointer
//         then the element had no data.  This pointer is persistent.
// 'lineno'  The line number in the source XML file.
//
// After calling your routine 'processElement' you must return 'true' to continue parsing
// If you want to stop parsing early, return false.
//
// If the call to process an XML file fails, it will return false.
// You can then call the method 'getError' to get a description of why it failed
// and on what line number of the source XML file it occurred.

class FastXmlInterface
{
public:

  // return true to continue processing the XML document, false to skip.
  virtual bool processElement(const char *elementName,         // name of the element
                              int         argc,                // number of attributes
                              const char **argv,               // list of attributes.
                              const char  *elementData,        // element data, null if none
                              int         lineno) = 0;         // line number in the source XML file

};

class FastXml
{
public:
  virtual bool processXml(const char *inputData,unsigned int dataLen,FastXmlInterface *iface) = 0;
  virtual const char * getError(int &lineno) = 0; // report the reason for a parsing error, and the line number where it occurred.
};

FastXml * createFastXml(void);
void      releaseFastXml(FastXml *f);

#endif
