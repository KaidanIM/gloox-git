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

#include "stream.h"


using namespace Iksemel;

static int __on_stream(void *udata, int type, iks *node) {
        if (udata) {
                Stream *S = (Stream *) udata;
                S->on_stream(type, node);
        }

        return IKS_OK;
}

static int __on_log(void *udata, const char *data, size_t size, int is_incoming) {
        if (udata) {
                Stream *S = (Stream *) udata;
                S->on_log(data, size, is_incoming);
        }

  return IKS_OK;
}

Stream::Stream() {
  this->init(iks_stream_new(IKS_NS_CLIENT, this, (iksStreamHook *) __on_stream));
}

Stream::Stream(char *name_space) {
  this->init(iks_stream_new(name_space, this, (iksStreamHook *) __on_stream));
}

Stream::~Stream() {
  this->disconnect();
}

void Stream::set_log_hook() {
  iks_set_log_hook(this->P, (iksLogHook *) __on_log);
}

int Stream::connect( iksparser *prs, const std::string& server, int port, const std::string& server_name ) {
  return iks_connect_via( prs, server.c_str(), port, server_name.c_str() );
}

int Stream::connect( const std::string& server, int port, const std::string& server_name ) {
  return this->connect( this->P, server, port, server_name );
}

int Stream::connect( const std::string& server, const std::string& server_name ) {
  return this->connect( server, IKS_JABBER_PORT, server_name );
}

int Stream::connect(iksparser *prs, int fd) {
  return iks_connect_fd(prs, fd);
}

int Stream::connect(int fd) {
  return this->connect(this->P, fd);
}

void Stream::disconnect(iksparser *prs) {
  iks_disconnect(prs);
}

void Stream::disconnect() {
  this->disconnect(this->P);
}

int Stream::recv(iksparser *prs, int timeout) {
  return iks_recv(prs, timeout);
}

int Stream::recv(int timeout) {
  return this->recv(this->P, timeout);
}

int Stream::recv() {
  return this->recv(-1);
}

int Stream::fd(iksparser *prs) {
  return iks_fd(prs);
}

int Stream::fd() {
  return this->fd(this->P);
}

int Stream::send(iksparser *prs, iks *node) {
  return iks_send(prs, node);
}

int Stream::send() {
  return this->send(this->P, this->X);
}

int Stream::send( iks* x ) {
  return this->send(this->P, x);
}

int Stream::send(iksparser *prs, const std::string& xmlstr) {
  return iks_send_raw(prs, xmlstr.c_str());
}

int Stream::send(const std::string& xmlstr) {
  return this->send(this->P, xmlstr);
}

int Stream::header(iksparser *prs, const std::string& to) {
  return iks_send_header(prs, to.c_str());
}

int Stream::header(const std::string& to)
{
  return this->header(this->P, to);
}

bool Stream::has_tls()
{
  return iks_has_tls();
}

int Stream::start_tls(iksparser *prs)
{
  return iks_start_tls(prs);
}

int Stream::start_tls() {
  return this->start_tls(this->P);
}

bool Stream::is_secure(iksparser *prs) {
  return iks_is_secure(prs);
}

bool Stream::is_secure() {
  return this->is_secure(this->P);
}

int Stream::start_sasl(iksparser* prs, enum ikssasltype type, char *username, char *pass) {
  return iks_start_sasl(prs, type, username, pass);
}

int Stream::start_sasl(enum ikssasltype type, char *username, char *pass) {
  return this->start_sasl(this->P, type, username, pass);
}

iks* Stream::make_resource_bind(iksid *id)
{
  iks *x, *y, *z;

  x = iks_new("iq");
  iks_insert_attrib(x, "type", "set");
  y = iks_insert(x, "bind");
  iks_insert_attrib(y, "xmlns", IKS_NS_XMPP_BIND);
  if (id->resource && iks_strcmp(id->resource, "")) {
    z = iks_insert(y, "resource");
    iks_insert_cdata(z, id->resource, 0);
  }
  return x;
}
