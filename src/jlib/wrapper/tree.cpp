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

#include "tree.h"


using namespace Iksemel;

Tree::Tree() {
  this->X = iks_new("");
}

Tree::Tree(iks *node) {
  this->X = node;
}

Tree::Tree(ikstack *s) {
  this->X = iks_new_within("", s);
}

Tree::Tree(const std::string& name) {
  this->X = iks_new(name.c_str());
}

Tree::~Tree() {
  iks_delete(this->X);
}

iks *Tree::tag(iks *node, const std::string& name) {
  return iks_insert(node, name.c_str());
}

iks *Tree::tag(const std::string& name) {
  return this->tag(this->X, name);
}

iks *Tree::cdata(iks *node, const std::string& data) {
  return iks_insert_cdata(node, data.c_str(), data.length());
}

iks *Tree::cdata(const std::string& data) {
  return this->cdata(this->X, data);
}

iks *Tree::cdata(const std::string& tag, const std::string& data) {
  return this->cdata(this->tag(tag), data);
}

iks *Tree::attrib(iks *node, const std::string& name, const std::string& value) {
  return iks_insert_attrib(node, name.c_str(), value.c_str());
}

iks *Tree::attrib(const std::string& name, const std::string& value) {
  return this->attrib(this->X, name, value);
}

iks *Tree::attrib(const std::string& tag, const std::string& name, const std::string& value) {
  return this->attrib(this->tag(tag), name, value);
}

iks *Tree::next() const {
  return iks_next(this->X);
}

iks *Tree::move_next() {
  this->X = this->next();
  return this->X;
}

iks *Tree::prev() const {
  return iks_prev(this->X);
}

iks *Tree::move_prev() {
  this->X = this->prev();
  return this->X;
}

iks *Tree::parent() const {
  return iks_parent(this->X);
}

iks *Tree::move_parent() {
  this->X = this->parent();
  return this->X;
}

iks *Tree::child() const {
  return iks_child(this->X);
}

iks *Tree::move_child() {
  this->X = this->child();
  return this->X;
}

iks *Tree::attr() const {
  return iks_attrib(this->X);
}

iks *Tree::move_attr() {
  this->X = this->attr();
  return this->X;
}

iks *Tree::root() const {
  return iks_root(this->X);
}

iks *Tree::move_root() {
  this->X = this->root();
  return this->X;
}

iks *Tree::next_tag() const {
  return iks_next_tag(this->X);
}

iks *Tree::move_next_tag() {
  this->X = this->next_tag();
  return this->X;
}

iks *Tree::prev_tag() const {
  return iks_prev_tag(this->X);
}

iks *Tree::move_prev_tag() {
  this->X = this->prev_tag();
  return this->X;
}

iks *Tree::first_tag() const {
  return iks_first_tag(this->X);
}

iks *Tree::move_first_tag() {
  this->X = this->first_tag();
  return this->X;
}

const enum ikstype Tree::type() const {
  return iks_type(this->X);
}

const std::string Tree::name() const {
  return iks_name(this->X);
}

const std::string Tree::cdata_name() const {
  return iks_cdata(this->X);
}

const size_t Tree::size_cdata() const {
  return iks_cdata_size(this->X);
}

const bool Tree::has_children() const {
  return iks_has_children(this->X);
}

const bool Tree::has_attribs() const {
  return iks_has_attribs(this->X);
}

iks *Tree::find_node(const std::string& name) const {
  return iks_find(this->X, name.c_str());
}

const std::string Tree::find_cdata(const std::string& name) const {
  return iks_find_cdata(this->X, name.c_str());
}

const std::string Tree::find_attrib(const std::string& name) const {
  return iks_find_attrib(this->X, name.c_str());
}

iks *Tree::find_with_attrib(iks *node, const std::string& tag, const std::string& attr, const std::string& value) const {
  return iks_find_with_attrib(node, tag.c_str(), attr.c_str(), value.c_str());
}

iks *Tree::find_with_attrib(const std::string& tag, const std::string& attr, const std::string& value) const {
  return this->find_with_attrib(this->X, tag, attr, value);
}

const bool Tree::boolean() const {
  return this->X;
}

iks *Tree::get_iks() const {
  return this->X;
}

ikstack *Tree::get_stack() const {
  return iks_stack(this->X);
}

std::string Tree::string(ikstack *stack, iks *node) const {
  return iks_string(stack, node);
}

std::string Tree::string(iks *node) const {
  return this->string(iks_stack(node), node);
}

std::string Tree::string() const {
  return this->string(this->X);
}
