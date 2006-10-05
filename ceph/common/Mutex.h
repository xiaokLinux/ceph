// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */


/////////////////////////////////////////////////////////////////////
//  Written by Phillip Sitbon
//  Copyright 2003
//
//  Posix/Mutex.h
//    - Resource locking mechanism using Posix mutexes
//
/////////////////////////////////////////////////////////////////////

#ifndef _Mutex_Posix_
#define _Mutex_Posix_

#include <pthread.h>
#include <cassert>

class Mutex
{
  mutable pthread_mutex_t M;
  void operator=(Mutex &M) {}
  Mutex( const Mutex &M ) {}
  bool tag;
  int locked;

  public:

  Mutex() : tag(false), locked(0)
  {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&M,&attr);
    //cout << this << " mutex init = " << r << endl;
    pthread_mutexattr_destroy(&attr);
  }

  Mutex(bool t) : tag(t)
  {
    assert(0);
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&M,&attr);
    //cout << this << " mutex init = " << r << endl;
    pthread_mutexattr_destroy(&attr);
  }

  bool is_locked() { return locked > 0; }
  int get_lock_count() { return locked; }

  virtual ~Mutex()
  { 
    if (locked < 0) cerr << "Mutex(" << this << "," << pthread_self() << ").destructor locked = " << locked << " < 0" << endl;
    //pthread_mutex_unlock(&M); 
    pthread_mutex_destroy(&M); 
  }

  int Lock()  { 
    int t = tag;
    if (t) cout << this << " " << pthread_self() << endl; 
    int r = pthread_mutex_lock(&M);
    if (t) cout << "lock = " << r << endl;
    locked++;
    return r;
  }

  int Lock(char *s)  { 
    cout << "Lock: " << s << endl;
    int r = pthread_mutex_lock(&M);
    cout << this << " " << pthread_self() << " lock = " << r << endl;
    locked++;
    return r;
  }

  int Lock_Try() const
  { 
    return pthread_mutex_trylock(&M); 
  }

  int Unlock() 
  { 
    int t = tag;
    locked--;
    if (locked < 0) cerr << "Mutex(" << this << "," << pthread_self() << ").Unlock locked = " << locked << " < 0" << endl;
    if (t) cout << this << " " << pthread_self() << endl;
    int r = pthread_mutex_unlock(&M);
    if (t) cout << "lock = " << r << endl;
    return r;
  }

  int Unlock(char *s) 
  { 
    cout << "Unlock: " << s << endl;
    locked--;
    if (locked < 0) cerr << "Mutex(" << this << "," << pthread_self() << ").Unlock locked = " << locked << " < 0" << endl;
    int r = pthread_mutex_unlock(&M);
    cout << this << " " << pthread_self() << " unlock = " << r << endl;
    return r;
  }

  friend class Cond;
};

#endif // !_Mutex_Posix_
