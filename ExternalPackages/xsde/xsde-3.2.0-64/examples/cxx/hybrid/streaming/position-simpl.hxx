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

#ifndef POSITION_SIMPL_HXX
#define POSITION_SIMPL_HXX

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_SAGGR
#  define XSDE_OMIT_SAGGR
#  define POSITION_SIMPL_HXX_CLEAR_OMIT_SAGGR
#endif

#include "position-sskel.hxx"

#include <xsde/cxx/stack.hxx>

class position_simpl: public position_sskel
{
  public:
  virtual void
  pre (const ::position&);

  // Attributes.
  //
  virtual float
  lat ();

  virtual float
  lon ();

  public:
  struct position_simpl_state
  {
    const ::position* position_;
  };

  position_simpl_state position_simpl_state_;
};

#include "object-simpl.hxx"

#ifdef POSITION_SIMPL_HXX_CLEAR_OMIT_SAGGR
#  undef XSDE_OMIT_SAGGR
#endif

#ifndef XSDE_OMIT_SAGGR

// Serializer aggregate for the object element.
//
class object_saggr
{
  public:
  object_saggr ();

  void
  pre (const ::object& x)
  {
    this->object_s_.pre (x);
  }

  void
  post ()
  {
    this->object_s_.post ();
  }

  ::object_simpl&
  root_serializer ()
  {
    return this->object_s_;
  }

  static const char*
  root_name ();

  static const char*
  root_namespace ();

  void
  reset ()
  {
    this->object_s_._reset ();
  }

  public:
  ::xml_schema::float_simpl float_s_;
  ::object_simpl object_s_;
  ::position_simpl position_s_;
  ::xml_schema::unsigned_int_simpl unsigned_int_s_;
};

#endif // XSDE_OMIT_SAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // POSITION_SIMPL_HXX
