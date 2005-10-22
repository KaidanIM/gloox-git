/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef DATAFORMRESULT_H__
#define DATAFORMRESULT_H__

#include "dataformfield.h"
#include "dataformbase.h"

namespace gloox
{

  /**
   * @brief An abstraction of an &lt;result&gt; element in a JEP-0004 Data Form of type result.
   *
   * There are some constraints regarding usage of this element you should be aware of. Check JEP-0004
   * section 3.4. This class does not enforce correct usage at this point.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.7
   */
  class DataFormResult : public DataFormBase, DataFormField
  {
    public:
      /**
       *
       */
      DataFormItem() {};

      /**
       * Virtual destructor.
       */
      virtual ~DataFormItem() {};

  };

}

#endif // DATAFORMRESULT_H__
