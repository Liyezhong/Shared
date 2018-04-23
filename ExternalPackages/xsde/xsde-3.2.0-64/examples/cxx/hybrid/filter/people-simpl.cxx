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

// Begin prologue.
//
//
// End prologue.

#include "people-simpl.hxx"

#include <xsde/cxx/serializer/validating/string-common.hxx>

// gender_simpl
//

gender_simpl::
gender_simpl ()
: gender_sskel (0)
{
}

void gender_simpl::
pre (const ::gender& x)
{
  this->gender_simpl_state_ = &x;
}

void gender_simpl::
_serialize_content ()
{
  if (::xsde::cxx::serializer::validating::string_common::validate_facets (
        this->gender_simpl_state_->string (),
        this->_facets (),
        this->_context ()))
    this->_characters (this->gender_simpl_state_->string ());
}

// person_simpl
//

void person_simpl::
pre (const ::person& x)
{
  this->person_simpl_state_.person_ = &x;
}

::std::string person_simpl::
first_name ()
{
  return this->person_simpl_state_.person_->first_name ();
}

::std::string person_simpl::
last_name ()
{
  return this->person_simpl_state_.person_->last_name ();
}

const ::gender& person_simpl::
gender ()
{
  return this->person_simpl_state_.person_->gender ();
}

unsigned short person_simpl::
age ()
{
  return this->person_simpl_state_.person_->age ();
}

// people_base_simpl
//

void people_base_simpl::
pre (const ::people& x)
{
  this->people_base_simpl_state_.people_ = &x;
  this->people_base_simpl_state_.person_ = 
  this->people_base_simpl_state_.people_->person ().begin ();
  this->people_base_simpl_state_.person_end_ = 
  this->people_base_simpl_state_.people_->person ().end ();
}

bool people_base_simpl::
person_next ()
{
  return this->people_base_simpl_state_.person_ != 
  this->people_base_simpl_state_.person_end_;
}

const ::person& people_base_simpl::
person ()
{
  return *this->people_base_simpl_state_.person_++;
}

// people_saggr
//

people_saggr::
people_saggr ()
{
  this->person_s_.serializers (this->string_s_,
                               this->string_s_,
                               this->gender_s_,
                               this->unsigned_short_s_);

  this->people_s_.serializers (this->person_s_);
}

const char* people_saggr::
root_name ()
{
  return "people";
}

const char* people_saggr::
root_namespace ()
{
  return "";
}

// Begin epilogue.
//
//
// End epilogue.
