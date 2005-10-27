/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef GLOOX_MACROS_H__
#define GLOOX_MACROS_H__

#ifdef WIN32
#define GLOOX_EXPORT __declspec( dllexport )
#else
#define GLOOX_EXPORT
#endif


#if __GNUC__ - 0 > 3 || ( __GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 >= 2 )
#define GLOOX_DEPRECATED __attribute__ ( (deprecated) )
#elif defined( _MSC_VER ) && ( _MSC_VER >= 1300 )
#define GLOOX_DEPRECATED __declspec( deprecated )
#else
#define GLOOX_DEPRECATED
#endif


#endif // GLOOX_MACROS_H__
