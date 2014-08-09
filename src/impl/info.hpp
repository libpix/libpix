/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  _info.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 *
 */

#ifndef __IMPL_INFO_HPP__ 
#define __IMPL_INFO_HPP__

#include <impl/config.h>
#include <pix/string.hpp>
#include <stdio.h>

PIXAPI_CPP_BEG
PIXIMPL_BEG

class Info {
public:
  class Data {
  public:
    Data() : next(0), prev(0) { }
    Data(const String &k, const String &v) : 
      key(k), val(v), next(0), prev(0) { } 
    ~Data() {
    }
    String key;
    String val;
    Data *next;
    Data *prev;
  };

  class Node {
  public:
    Data *data;
    String *content;
    Node *child;
    Node *next;

    Node *prev;
    Node *parent;
  public:
    Node() : data(0), content(0), child(0), next(0), prev(0), parent(0) { } 
    template<class T>
    void release(T * ptr)
    {
      if (!ptr) return;
      ptr->prev->next = 0;
      T *p = ptr;
      T *q;

      do {
        q = p->next;
        delete p;
        p = q;
      } while(p);
    }
    ~Node() 
    {
      if (content) delete content;
      release(data);
      release(child);
    }
    bool empty() 
    {
      if (data != 0) return false;
      if (content != 0) return false;
      if (child != 0) return false;
      return true;
    }


  };


public:
  Node *info;
  Node *now;
  Data *data;
public:


  Info() {
    info = new Node;
    now = info;

    new_child();

    top();

  }
  ~Info() 
  {
    if (info) delete info;
  }
  bool top()
  {
    data = 0;
    now = info->child->next;
    return true;
  }
  bool first()
  {
    data = 0;
    now = now->parent->child->next;
    return true;
  }
  bool next()
  {
    data = 0;
    if (now->next == now->parent->child) return false;
    now = now->next;
    return true;
  }
  bool prev()
  {
    data = 0;
    if (now->prev == now->parent->child) return false;
    now = now->prev;
    return true;
  }

  bool child()
  {
    data = 0;
    if (!now->child) return false;
    now = now->child->next;
    return true;
  }

  bool parent()
  {
    data = 0;
    if (now->parent == info) return false;
    now = now->parent;
    return true;
  }

  bool data_first(String &key, String &val)
  {
    key.clear();
    val.clear();
    if (!now->data) return false;
    data = now->data->next;
    key = data->key;
    val = data->val;
    return true;

  }
  bool data_next(String &key, String &val)
  {
    key.clear();
    val.clear();
    if (data) {
      if (data->next == now->data) return false;
      data = data->next;
      key = data->key;
      val = data->val;
      return true;
    }
    return data_first(key, val);
  }

  void new_node()
  {
    data = 0;
    if (now->empty()) return;
    Node *head = now->parent->child;
    Node *last = head->prev;
    if (last->empty()) {
      now = last;
      return;
    }
    Node *node = new Node;
    node->parent = head->parent;
    node->next = head;
    node->prev = last;
    head->prev = node;
    last->next = node;
    now = node;
  }
  void new_node(const String & content)
  {
    new_node();
    now->content = new String(content);
  }

  void new_child()
  {
    data = 0;
    if (now->child) {
      child();
      new_node();
      return;
    }
    Node *head = new Node;

    now->child = head;
    head->parent = now;

    Node *node = new Node;
    node->parent = now;
    head->next = node;
    node->next = head;
    node->prev = head;
    head->prev = node;
    now = node;
  }

  void new_child(const String &content)
  {
    new_child();
    now->content = new String(content);
  }
  bool get_data(const String &key, String &val) const
  {
    Data *head = now->data;
    Data *p = head->next;
    while (p->next != head) {
      if (key == p->key) {
        val = p->val;
        return true;
      }
    }
    return false;
  }
  bool get_content(String &content)
  {
    if (now->content) {
      content = *now->content;
      return true;
    }
    return false;
  }

  void set_data(const String &key, const String &val)
  {
    if (!now->data) {
      Data *head = new Data;
      now->data = head;
      Data *node = new Data(key,val);
      head->next = node;
      node->next = head;
      node->prev = head;
      head->prev = node;
    } else {
      Data *head = now->data;
      Data *node = new Data(key,val);
      Data *last = head->prev;
      node->next = head;
      node->prev = last;
      head->prev = node;
      last->next = node;
    }
  }
  void set_content(const String &content)
  {
    if (!now->content) now->content = new String(content);
    else *now->content = content;
  }

  bool empty() const
  {
    return now->empty();
  }


};

PIXIMPL_END

PIXAPI_CPP_END

#endif

