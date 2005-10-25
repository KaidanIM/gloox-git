/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef GLOOX_EXPORT_H__
#define GLOOX_EXPORT_H__

#ifdef WIN32
#define GLOOX_EXPORT __declspec( dllexport )
#else
#define GLOOX_EXPORT
#endif

#endif // GLOOX_EXPORT_H__
