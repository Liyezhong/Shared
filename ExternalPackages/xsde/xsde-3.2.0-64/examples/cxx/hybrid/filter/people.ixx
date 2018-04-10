// Copyright (c) 2005-2011 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD/e, an XML Schema
// to C++ data binding compiler for embedded systems.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
//

#ifndef PEOPLE_IXX
#define PEOPLE_IXX

// Begin prologue.
//
//
// End prologue.

// gender
//

inline
gender::
gender ()
{
}

inline
gender::
gender (value_type v)
{
  value_ = v;
}

inline
void gender::
value (value_type v)
{
  value_ = v;
}

// person
//

inline
const ::std::string& person::
first_name () const
{
  return this->first_name_;
}

inline
::std::string& person::
first_name ()
{
  return this->first_name_;
}

inline
void person::
first_name (const ::std::string& x)
{
  this->first_name_ = x;
}

inline
const ::std::string& person::
last_name () const
{
  return this->last_name_;
}

inline
::std::string& person::
last_name ()
{
  return this->last_name_;
}

inline
void person::
last_name (const ::std::string& x)
{
  this->last_name_ = x;
}

inline
const ::gender& person::
gender () const
{
  return this->gender_;
}

inline
::gender& person::
gender ()
{
  return this->gender_;
}

inline
void person::
gender (const ::gender& x)
{
  this->gender_ = x;
}

inline
unsigned short person::
age () const
{
  return this->age_;
}

inline
unsigned short& person::
age ()
{
  return this->age_;
}

inline
void person::
age (unsigned short x)
{
  this->age_ = x;
}

// people
//

inline
const people::person_sequence& people::
person () const
{
  return this->person_;
}

inline
people::person_sequence& people::
person ()
{
  return this->person_;
}

// Begin epilogue.
//
//
// End epilogue.

#endif // PEOPLE_IXX
