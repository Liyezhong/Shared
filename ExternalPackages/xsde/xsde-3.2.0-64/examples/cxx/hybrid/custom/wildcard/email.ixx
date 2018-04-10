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

#ifndef EMAIL_IXX
#define EMAIL_IXX

// Begin prologue.
//
//
// End prologue.

namespace email
{
  // binary
  //

  inline
  const ::std::string& binary::
  name () const
  {
    return this->name_;
  }

  inline
  ::std::string& binary::
  name ()
  {
    return this->name_;
  }

  inline
  void binary::
  name (const ::std::string& x)
  {
    this->name_ = x;
  }

  inline
  const ::std::string& binary::
  mime () const
  {
    return this->mime_;
  }

  inline
  ::std::string& binary::
  mime ()
  {
    return this->mime_;
  }

  inline
  void binary::
  mime (const ::std::string& x)
  {
    this->mime_ = x;
  }

  // envelope
  //

  inline
  const ::std::string& envelope_base::
  to () const
  {
    return this->to_;
  }

  inline
  ::std::string& envelope_base::
  to ()
  {
    return this->to_;
  }

  inline
  void envelope_base::
  to (const ::std::string& x)
  {
    this->to_ = x;
  }

  inline
  const ::std::string& envelope_base::
  from () const
  {
    return this->from_;
  }

  inline
  ::std::string& envelope_base::
  from ()
  {
    return this->from_;
  }

  inline
  void envelope_base::
  from (const ::std::string& x)
  {
    this->from_ = x;
  }

  inline
  const ::std::string& envelope_base::
  subject () const
  {
    return this->subject_;
  }

  inline
  ::std::string& envelope_base::
  subject ()
  {
    return this->subject_;
  }

  inline
  void envelope_base::
  subject (const ::std::string& x)
  {
    this->subject_ = x;
  }
}

// Begin epilogue.
//
//
// End epilogue.

#endif // EMAIL_IXX
