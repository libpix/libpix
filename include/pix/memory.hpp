/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix/memory.hpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *        License:  LGPL
 *
 *    Description:  memory management
 *
 * =============================================================
 */

#ifndef __PIX_MEMORY_HPP__
#define __PIX_MEMORY_HPP__
/**
 * @file
 * @brief %memory management
 */

#include <pix/config.h>

PIXAPI_CPP_BEG

/**
 * @defgroup memory Memory
 * @brief %memory management, buffer and smart pointer
 * @{
 */

/**
 * @brief %deal with cross-module new and delete.
 *
 * Cross-module memory allocation cross-module may cause crash.
 * This class ensure all heap object memory allocation within this module.
 *
 */
class Base {
public:
  static void* operator new (size_t size);
  static void operator delete (void *p);
private:

  /** 
   * Bug: VC have problem with delete[] cross dlls.
   * Sometimes it calls delete, not delete[], then crash.
   * So for Now , we just prevent the use of 
   * vector construction and destruction
   *
   */
  static void* operator new[] (size_t size);
  static void operator delete[] (void *p);
};


/**
 * @brief %buffer class
 *
 * buffer        [bp,ep) \n
 * read buffer   [rp,wp) \n
 * write buffer  [wp,ep) \n
 * prefix buffer [bp,rp) \n
 * stream buffer [bp,wp) \n
 *
 */

class Buffer : public Base {
public:
  Buffer(pix_len size = 0);                     /**< Constructor */
  ~Buffer();                                    /**< Destructor */
public:
  char *bp() const { return m_bp; }             /**< @brief Base pointer. */
  char *rp() const { return m_rp; }             /**< @brief Read pointer. */
  char *wp() const { return m_wp; }             /**< @brief Writer pointer. */
  char *ep() const { return m_ep; }             /**< @brief End pointer. */
  char *bp(char *p) { return m_bp = p; }        /**< @brief Set base pointer. */
  char *rp(char *p) { return m_rp = p; }        /**< @brief Set read pointer. */
  char *wp(char *p) { return m_wp = p; }        /**< @brief Set write pointer. */
  char *ep(char *p) { return m_ep = p; }        /**< @brief Set end pointer. */
  /*
   * @brief %move read pointer
   * @param off %the offset of new read pointer
   */
  char *rp(pix_pos off) { return m_rp += off; }
  /*
   * @brief %move write pointer
   * @param off the offset of new write pointer
   */
  char *wp(pix_pos off) { return m_wp += off; }
  /**
   * @brief %move read and write pointer to base pointer.
   */
  void reset() { m_rp = m_wp = m_bp; }
  /**
   * @brief %move read pointer to base pointer.
   */
  void r_reset() { m_rp = m_bp; }
  /**
   * @brief %move write pointer to read pointer.
   */
  void w_reset() { m_wp = m_rp; }
  /**
   * @brief %position of read pointer.
   */
  pix_pos r_pos() const { return static_cast<pix_pos>(m_rp - m_bp); }
  /**
   * @brief %position of write pointer. 
   */
  pix_pos w_pos() const { return static_cast<pix_pos>(m_wp - m_bp); }
  /**
   * @brief %size of read buffer.
   */
  pix_len r_avail() const { return static_cast<pix_len>(m_wp - m_rp); }
  /**
   * @brief %size of write buffer.
   */
  pix_len w_avail() const { return static_cast<pix_len>(m_ep - m_wp); }
  /**
   * @brief %size of prefix buffer.
   */
  pix_len p_avail() const { return static_cast<pix_len>(m_rp - m_bp); }
  /**
   * @brief %size of prefix and write buffer.
   */
  pix_len avail() const { return w_avail() + p_avail(); }
  /**
   * @brief %size of stream buffer.
   */
  pix_len length() const { return static_cast<pix_len>(m_wp - m_bp); }
  /**
   * @brief %size of the whole buffer.
   */
  pix_len size() const { return static_cast<pix_len>(m_ep - m_bp); }

public:
  /**
   * @brief %swap two buffers
   */
  void swap(Buffer &buf);
  /**
   * @brief %resize the buffer, abandon data, rewind the pointer
   * @param tsize %the size of the whole buffer
   * 
   */
  void resize(pix_len size);
  /**
   * @brief %expand the buffer, keep all data [bp, wp)
   * @param wsize %the minimum size of write buffer
   */
  void expand(pix_len wsize);
  /**
   * @brief %move the read buffer, exapnd if the buffer size is not enough
   * @param pos %the position of read buffer
   * @param wsize %the minimum size of write buffer
   */
  void move(pix_pos pos , pix_len wsize = 0);
  /**
   * @brief %release the buffer.
   */
  void release();
  
private:
  /**
   * @brief %get rid of unwanted copy construction.
   */
  Buffer(const Buffer &buf);
private:
  char *m_bp, *m_rp, *m_wp , *m_ep;
  char *m_buf;
};


/**
 * @brief %smart pointer
 * 
 * This smart pointer automatically release, safe to return, safe for except.
 * 
 */

template<class T>
class Block {
public:
  typedef T * ptr_t;
public:
  /**
   * @brief %alloc [count] objects
   */
  Block(pix_len count = 1) : m_ptr(0), m_buf(0), m_len(0),m_ref(0) {
    if (count <= 0) return;
    if (count == 1) m_buf = new T;
    else m_buf = new T[count];
    m_len = count;
    m_ptr = m_buf;
    m_ref = new int(1);
  }
  /**
   * @brief %manage an array of objects
   */
  Block(T *buf, pix_len count = 1) : m_ptr(0), m_buf(0), m_len(0), m_ref(0) {
    if (!buf) return;
    if (count <= 0) return;
    m_buf = buf;
    m_len = count;
    m_ptr = m_buf;
    m_ref = new int(1);
  }
  /**
   * @brief %new reference to an managed objects
   */
  Block(const Block &ptr)
  {
    m_ref = ptr.m_ref;
    if (!m_ref) {
      m_buf = 0;
      m_len = 0;
      m_ptr = 0;
      return;
    }
    (*m_ref)++;
    m_buf = ptr.m_buf;
    m_len = ptr.m_len;
    m_ptr = ptr.m_ptr;
  }
  /**
   * @brief %delete a reference, delete objects if no references
   */
  ~Block()
  {
    if (!m_ref) return;
    (*m_ref)--;
    if (*m_ref == 0) {
      delete m_ref;
      if (m_buf) {
        if (m_len == 1) delete m_buf;
        else delete []m_buf;
      }
    }
  }
public:
  /**
   * @brief %count of objects
   */
  int length() {
    return m_len;
  }
  /**
   * @brief %point to the first object
   */
  ptr_t reset() {
    return m_ptr = m_buf;
  }
  /**
   * @brief %transfer the managed objects. fail if have reference.
   */
  ptr_t detach() {
    if (!m_ref) return 0;
    if (*m_ref > 1) return 0;
    delete m_ref;
    T *buf = m_buf;
    m_buf = 0;
    m_ptr = 0;
    m_len = 0;
    return buf;
  }

public:
  /**
   * @brief %current pointer
   */
  operator ptr_t ()
  {
    return m_ptr;
  }
  /**
   * @brief %return Next pointer, move to next.
   */
  ptr_t operator ++()
  {
    m_ptr++;
    return m_ptr;
  }
  /**
   * @brief %return current pointer, move to next.
   */

  ptr_t operator ++(int l)
  {
    ptr_t before = m_ptr;
    m_ptr++;
    return before;
  }
  /**
   * @brief %return previous pointer, move to previous.
   */

  ptr_t operator --()
  {
    m_ptr--;
    return m_ptr;
  }
  /**
   * @brief %return current pointer, move to previous.
   * 
   */
  ptr_t operator --(int l)
  {
    ptr_t before = m_ptr;
    m_ptr--;
    return before;
  }
  /**
   * @brief %return next [off] pointer.
   * 
   */
  ptr_t operator +(int off)
  {
    return m_ptr + off;
  }
  /**
   * @brief %return previous [off] pointer.
   */
  ptr_t operator -(int off)
  {
    return m_ptr - off;
  }
  /**
   * @brief %content of the current object.
   * 
   */
  T& operator * ()
  {
    return *m_ptr;
  }
  /**
   * @brief %call a method of current object.
   */
  T* operator -> ()
  {
    return m_ptr;
  }
  /** 
   * @brief %return the next [off] object content.
   */
  T& operator [](int off)
  {
    if (off < 0) off += m_len;
    return m_ptr[off];
  }

private:
  ptr_t m_buf;
  ptr_t m_ptr;
  pix_len m_len;
  int *m_ref;
};

/**
 * @}
 */

PIXAPI_CPP_END

#endif
