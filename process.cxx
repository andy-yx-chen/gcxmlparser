#include "objects_manager.hpp"
#include <stack>
#include <algorithm>

using namespace std;

void process(object_manager* obj_mgr) {
  gcobject seperator(0, 0, NULL);
  stack<gcobject*> process_stack;
  stack<gcobject*> level_stack;

  vector<gcobject*> roots(move(obj_mgr->roots()));
  for(vector<gcobject*>::const_iterator it = roots.begin(); it != roots.end(); ++it) {
    (*it)->set_processed();
    process_stack.push(*it);
  }
  gcobject* current = NULL;
  while(!process_stack.empty()) {
    current = process_stack.top();
    process_stack.pop();
    while(current == &seperator) {
      if(!level_stack.empty()){
        gcobject* child = level_stack.top();
        level_stack.pop();
        if(!level_stack.empty()){
          level_stack.top()->add_size(child->size());
        }//end if
      }//end if not empty
      if(process_stack.empty()) {
        //we done
        return;
      }//end if process_stack is empty
      current = process_stack.top();
      process_stack.pop();
    }//end while
    process_stack.push(&seperator);
    for(vector<gcobject*>::const_iterator cit = current->begin_child(); cit != current->end_child(); ++cit){
      if(! (*cit)->processed()){
        (*cit)->set_processed();
        process_stack.push(*cit);
      }
    }
    level_stack.push(current);
  }
}