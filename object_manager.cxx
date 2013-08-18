#include "objects_manager.hpp"
#include <tchar.h>
#include <algorithm>
#include <iostream>
#include <xercesc/sax2/Attributes.hpp>


object_manager::~object_manager() {
  for_each(objects_.begin(), objects_.end(), [](const std::pair<uint64_t, gcobject*>& item) -> void {
    if(item.second != NULL){
      delete item.second;
    }
  });

  for_each(types_.begin(), types_.end(), [](const std::pair<uint32_t, wstring*>& item) -> void {
    if(item.second != NULL){
      delete item.second;
    }
  });
}

vector<gcobject*> object_manager::roots() const {
  vector<gcobject*> roots;
  for_each(roots_.begin(), roots_.end(), [this, &roots](uint64_t addr) -> void {
    gcobject* obj = this->find_object(addr);
    if(obj != NULL){
      roots.push_back(obj);
    }
  });
  return std::move(roots);
}

gcobject* object_manager::find_object(uint64_t addr) const {
  gcobject* result = NULL;
  hash_map<uint64_t, gcobject*>::const_iterator pos = objects_.find(addr);
  if(pos != objects_.end()){
    result = pos->second;
  }
  return result;
}

void object_manager::fatalError(const xercesc::SAXParseException& err) {
  std::wcout << L"failed to parse gc heap file as " << err.getMessage() << std::endl;
  std::wcout << L"at line: " << err.getLineNumber() << L", column: " << err.getColumnNumber() << std::endl;
}

void object_manager::startElement(const XMLCh* const uri,  const XMLCh* const lname, const XMLCh* const qname, const xercesc::Attributes& attrs) {
  if(_wcsicmp(L"type", lname) == 0){
    std::wstring* name = new std::wstring(attrs.getValue(L"name"));
    uint32_t id =(uint32_t) _wtoi(attrs.getValue(L"id"));
    types_.insert(std::make_pair(id, name));
  }else if(_wcsicmp(L"root", lname) == 0){
    roots_.push_back(lexical_cast<uint64_t>(attrs.getValue(L"address")));
  }else if(_wcsicmp(L"object", lname) == 0){
    gcobject* obj = NULL;
    uint64_t address = lexical_cast<uint64_t>(attrs.getValue(L"address"));
    uint32_t size = (uint32_t)_wtoi(attrs.getValue(L"size"));
    uint32_t type_id = (uint32_t)_wtoi(attrs.getValue(L"typeid"));
    obj = find_object(address);
    if(obj == NULL){
      obj = new gcobject(lexical_cast<uint64_t>(attrs.getValue(L"address")), size, NULL);
      insert_object(obj);
    }else{
      obj->add_size(size);
    }
    hash_map<uint32_t, wstring*>::const_iterator loc = types_.find(type_id);
    if(loc != types_.end()){
      obj->set_type(loc->second);
    }
    current_ = obj;
  }else if(_wcsicmp(L"member", lname) == 0){
    if(current_ != NULL){
      uint64_t address = lexical_cast<uint64_t>(attrs.getValue(L"address"));
      gcobject* obj = find_object(address);
      if(obj == NULL){
        obj = new gcobject(address, 0, NULL);
        insert_object(obj);
      }
      current_->add_child(obj);
    }
  }
}

void object_manager::endElement(const XMLCh* const uri, const XMLCh* const lname, const XMLCh* const qname) {
  if(_wcsicmp(L"object", lname) == 0){
    current_ = NULL;
  }
}

void object_manager::insert_object(gcobject* obj) {
  objects_.insert(std::make_pair(obj->address(), obj));
}