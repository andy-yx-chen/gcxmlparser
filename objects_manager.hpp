#pragma once

#include <xercesc/sax2/DefaultHandler.hpp>
#include <hash_map>
#include <sstream>
#include "gcobject.hpp"

class object_manager : public xercesc::DefaultHandler
{
public:
  object_manager() 
    : current_(NULL), objects_(), types_(), roots_()
  {
  }

  ~object_manager();

public:
  virtual void startElement
    (
    const   XMLCh* const    uri,
    const   XMLCh* const    localname,
    const   XMLCh* const    qname
    , const xercesc::Attributes&	attrs
    );

  virtual void endElement
    (
    const XMLCh* const uri,
    const XMLCh* const localname,
    const XMLCh* const qname
    );

  virtual void fatalError
    (
    const xercesc::SAXParseException& error
    );

  gcobject* find_object(uint64_t address) const;

  vector<gcobject*> roots() const;

private:
  void insert_object(gcobject* obj);
private:
  gcobject* current_;
  hash_map<uint64_t, gcobject*> objects_;
  hash_map<uint32_t, wstring*> types_;
  vector<uint64_t> roots_;
};

template<typename T2, typename T1>
inline T2 lexical_cast(const T1& in)
{
  T2 out;
  wstringstream ss;
  ss << in;
  ss >> std::hex >> out;
  return out;
}