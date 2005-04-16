/*
  iksemelmm -- c++ wrapper for iksemel xml/xmpp library

  Copyright (C) 2004 Igor Goryachieff <igor@jahber.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#include "parser.h"


using namespace Iksemel;

Parser::Parser() {
  inited = false;
}

Parser::~Parser() {
  if (inited)
    iks_parser_delete(P);
}

void Parser::init(iksparser *prs) {
  P = prs;
    inited = true;
}

int Parser::parse(char *data, size_t len, int finish) {
  int rc = iks_parse(P, data, len, finish);

  if (rc != IKS_OK)
    throw rc;

  return IKS_OK;
}

unsigned long Parser::bytes() const {
  return iks_nr_bytes(P);
}

unsigned long Parser::lines() const {
  return iks_nr_lines(P);
}

void Parser::reset() {
  iks_parser_reset(P);
}
