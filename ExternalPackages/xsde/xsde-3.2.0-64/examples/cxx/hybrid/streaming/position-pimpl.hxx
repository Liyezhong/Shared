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

#ifndef POSITION_PIMPL_HXX
#define POSITION_PIMPL_HXX

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_PAGGR
#  define XSDE_OMIT_PAGGR
#  define POSITION_PIMPL_HXX_CLEAR_OMIT_PAGGR
#endif

#include "position-pskel.hxx"

#include <xsde/cxx/stack.hxx>

class position_pimpl: public position_pskel
{
  public:
  virtual void
  pre ();

  // Attributes.
  //
  virtual void
  lat (float);

  virtual void
  lon (float);

  virtual ::position
  post_position ();

  public:
  struct position_pimpl_state
  {
    ::position position_;
  };

  position_pimpl_state position_pimpl_state_;
};

#include "object-pimpl.hxx"

#ifdef POSITION_PIMPL_HXX_CLEAR_OMIT_PAGGR
#  undef XSDE_OMIT_PAGGR
#endif

#ifndef XSDE_OMIT_PAGGR

// Parser aggregate for the object element.
//
class object_paggr
{
  public:
  object_paggr ();

  void
  pre ()
  {
    this->object_p_.pre ();
  }

  ::object*
  post ()
  {
    return this->object_p_.post_object ();
  }

  ::object_pimpl&
  root_parser ()
  {
    return this->object_p_;
  }

  static const char*
  root_name ();

  static const char*
  root_namespace ();

  void
  reset ()
  {
    this->object_p_._reset ();
  }

  public:
  ::xml_schema::float_pimpl float_p_;
  ::object_pimpl object_p_;
  ::position_pimpl position_p_;
  ::xml_schema::unsigned_int_pimpl unsigned_int_p_;
};

#endif // XSDE_OMIT_PAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // POSITION_PIMPL_HXX
