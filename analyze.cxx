#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <xercesc/util/XMLString.hpp>
#include "objects_manager.hpp"

typedef vector<gcobject*>::const_iterator obj_iterator;

void output(vector<gcobject*>&, ofstream* out);

void outobject(const gcobject* obj, ofstream* out);

void analyze(object_manager* objmgr) {
  shared_ptr<ofstream> outstr;
  string command;
  cout << "do you want to save your analyze log to a file? (y/n)";
  while(true){
    cin >> command;
    if(_stricmp(command.c_str(), "y") == 0 || _stricmp(command.c_str(), "n") == 0){
      break;
    }
    cout << "(y/n)?";
  }

  if(_stricmp(command.c_str(), "y") == 0){
    while(true){
      cout << "please enter a log file name: ";
      cin >> command;
      try{
        outstr.reset(new ofstream(command));
        break;
      }catch(const std::exception& err){
        cout << "bad file name as " << err.what() << endl;
      }
    }
  }

  vector<gcobject*> roots(move(objmgr->roots()));
  output(roots, outstr.get());

  while(true){
    cout << "object:>";
    cin >> command;
    uint64_t address = lexical_cast<uint64_t>(command.c_str());
    if(address == 0){
      cout << "bye" << endl;
      break;
    }
    gcobject* obj = objmgr->find_object(address);
    if(obj == NULL){
      cout << "not found." << endl;
    }else{
      outobject(obj, outstr.get());
      if(obj->begin_child() != obj->end_child()){
        cout << "<<<<<<<<<< begin children >>>>>>>>>>" << std::endl;
        if(outstr){
          *outstr << "<<<<<<<<<< begin children >>>>>>>>>>" << std::endl;
        }
        for(obj_iterator it = obj->begin_child(); it != obj->end_child(); ++it){
          outobject(*it, outstr.get());
        }
        cout << "<<<<<<<<<<< end children >>>>>>>>>>>" << std::endl;
        if(outstr){
          *outstr << "<<<<<<<<<<< end children >>>>>>>>>>>" << std::endl;
        }
      }//end if not empty
    }
  }
}

void output(vector<gcobject*>& list, ofstream* out) {
  for(obj_iterator it = list.begin(); it != list.end(); ++it) {
    outobject(*it, out);
  }
}

void outobject(const gcobject* obj, ofstream* out) {
  if(obj == NULL) {
    return;
  }

  char* type = xercesc::XMLString::transcode(obj->type()->c_str());
  cout << setw(8) << "object:" << setw(16) << hex << obj->address() << setw(8) << "size:" << dec << setw(16) << obj->size() << endl;
  cout << setw(8) << "type:" << type << endl;
  if(out != NULL){
    *out << setw(8) << "object:" << setw(16) << hex << obj->address() << setw(8) << "size:" << dec << setw(16) << obj->size() << endl;
    *out << setw(8) << "type:" << type << endl;
  }
  xercesc::XMLString::release(&type);
}