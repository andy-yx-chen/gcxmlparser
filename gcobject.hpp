#pragma once

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

class gcobject
{
public:
  gcobject(uint64_t addr, uint32_t size, wstring* type)
    : address_(addr), size_(size), type_(type), total_size_(size_), children_()
  {
  }

public:
  inline void add_size(uint32_t incremental){
    size_ += incremental;
  }

  inline void set_type(wstring* type){
    type_ = type;
  }

  inline wstring* type() const {
    return type_;
  }

  inline uint64_t address() const {
    return address_ & ((uint64_t) -4 );
  }

  inline bool processed() const {
    return (address_ & 3) !=0;
  }

  inline void set_processed() {
    address_ |= 1;
  }

  inline uint32_t size() const {
    return size_;
  }

  inline void add_child(gcobject* child){
    children_.push_back(child);
  }

  vector<gcobject*>::const_iterator begin_child() const {
    return children_.begin();
  }

  vector<gcobject*>::const_iterator end_child() const {
    return children_.end();
  }

private:
  uint64_t address_;
  uint32_t size_;
  wstring* type_;
  uint32_t total_size_;
  vector<gcobject*> children_;
};