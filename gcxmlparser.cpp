// gcxmlparser.cpp : Defines the entry point for the console application.
//

#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp>
#include "objects_manager.hpp"
#include <iostream>
#include <Windows.h>

#define XERCESC_LIB_ "xerces-c_static_3"

#ifdef _DEBUG
#define XERCESC_LIB XERCESC_LIB_##"D"
#else
#define XERCESC_LIB XERCESC_LIB_
#endif

#pragma comment(lib, XERCESC_LIB)

using namespace std;
using namespace xercesc;

extern void process(object_manager* obj_mgr);

extern void analyze(object_manager* obj_mgr);

DWORD WINAPI monitor_thread(PVOID parameter);

int main(int argc, char* argv[])
{
  if(argc != 2){
    cerr << "usage: gcxmlparser <file>" << endl;
    return 1;
  }
  HANDLE event_handle = CreateEventA(NULL, FALSE, FALSE, NULL);

  if(!event_handle){
    cerr << "failed to initialize application as " << hex << GetLastError() << endl;
    return 1;
  }

  CreateThread(NULL, 0, &monitor_thread, (LPVOID) event_handle, 0, NULL);
  cout << "parsing the xml file" << endl;
  SetEvent(event_handle);
  //step 1, parse the data
  try {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Error during initialization! :\n";
    cout << "Exception message is: \n"
      << message << "\n";
    XMLString::release(&message);
    return 1;
  }

  char* xmlFile = argv[1];
  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLUni::fgSAX2CoreValidation, true);

  object_manager* object_mgr = new object_manager();
  parser->setContentHandler(object_mgr);
  parser->setErrorHandler(object_mgr);

  try {
    parser->parse(xmlFile);
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Exception message is: \n"
      << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (const SAXParseException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Exception message is: \n"
      << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (...) {
    cout << "Unexpected Exception \n" ;
    return -1;
  }

  delete parser;
  cout << endl << "xml file parsed." << endl;
  SetEvent(event_handle);
  cout << "processing the object trees" << endl;
  SetEvent(event_handle);
  //step 2, process object trees
  process(object_mgr);
  cout << endl << "data ready." << endl;
  SetEvent(event_handle);
  //step 3, user analyze
  analyze(object_mgr);
  CloseHandle(event_handle);
  delete object_mgr;
  return 0;
}

DWORD WINAPI monitor_thread(LPVOID parameter) {
  HANDLE evt = (HANDLE)parameter;
  int state = 0;
  while(state < 4){
    while(WaitForSingleObject(evt, 2000) != WAIT_OBJECT_0){
      cout << ".";
    }
    state += 1;
  }
  return 0;
}