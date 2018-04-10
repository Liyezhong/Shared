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

#include "hello-pskel.hxx"

#include <assert.h>

// hello_pskel
//

void hello_pskel::
greeting (const ::std::string& x)
{
  if (this->hello_impl_)
    this->hello_impl_->greeting (x);
}

void hello_pskel::
name (const ::std::string& x)
{
  if (this->hello_impl_)
    this->hello_impl_->name (x);
}

void hello_pskel::
post_hello ()
{
  if (this->hello_impl_)
    this->hello_impl_->post_hello ();
}

void hello_pskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::parser::validating::complex_content base;
  base::_reset ();

  this->v_state_stack_.clear ();

  this->resetting_ = true;

  if (this->greeting_parser_)
    this->greeting_parser_->_reset ();

  if (this->name_parser_)
    this->name_parser_->_reset ();

  this->resetting_ = false;
}

#include <assert.h>

// Element validation and dispatch functions for hello_pskel.
//
bool hello_pskel::
_start_element_impl (const ::xsde::cxx::ro_string& ns,
                     const ::xsde::cxx::ro_string& n)
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_* vd = vs.data + (vs.size - 1);

  if (vd->func == 0 && vd->state == 0)
  {
    typedef ::xsde::cxx::parser::validating::complex_content base;
    if (base::_start_element_impl (ns, n))
      return true;
    else
      vd->state = 1;
  }

  while (vd->func != 0)
  {
    (this->*vd->func) (vd->state, vd->count, ns, n, true);

    vd = vs.data + (vs.size - 1);

    if (vd->state == ~0UL && !ctx.error_type ())
      vd = vs.data + (--vs.size - 1);
    else
      break;
  }

  if (vd->func == 0)
  {
    if (vd->state != ~0UL)
    {
      unsigned long s = ~0UL;

      if (n == "greeting" && ns.empty ())
        s = 0UL;

      if (s != ~0UL)
      {
        vd->count++;
        vd->state = ~0UL;

        vd = vs.data + vs.size++;
        vd->func = &hello_pskel::sequence_0;
        vd->state = s;
        vd->count = 0;

        this->sequence_0 (vd->state, vd->count, ns, n, true);
      }
      else
      {
        if (vd->count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          return true;
        }

        return false;
      }
    }
    else
      return false;
  }

  return true;
}

bool hello_pskel::
_end_element_impl (const ::xsde::cxx::ro_string& ns,
                   const ::xsde::cxx::ro_string& n)
{
  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_& vd = vs.data[vs.size - 1];

  if (vd.func == 0 && vd.state == 0)
  {
    typedef ::xsde::cxx::parser::validating::complex_content base;
    if (!base::_end_element_impl (ns, n))
      assert (false);
    return true;
  }

  assert (vd.func != 0);
  (this->*vd.func) (vd.state, vd.count, ns, n, false);

  if (vd.state == ~0UL)
    vs.size--;

  return true;
}

void hello_pskel::
_pre_e_validate ()
{
  this->v_state_stack_.push ();
  static_cast< v_state_* > (this->v_state_stack_.top ())->size = 0;

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_& vd = vs.data[vs.size++];

  vd.func = 0;
  vd.state = 0;
  vd.count = 0;
}

void hello_pskel::
_post_e_validate ()
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_* vd = vs.data + (vs.size - 1);

  ::xsde::cxx::ro_string empty;
  while (vd->func != 0)
  {
    (this->*vd->func) (vd->state, vd->count, empty, empty, true);

    if (ctx.error_type ())
      return;

    assert (vd->state == ~0UL);
    vd = vs.data + (--vs.size - 1);
  }

  if (vd->count < 1UL)
    this->_schema_error (::xsde::cxx::schema_error::expected_element);

  this->v_state_stack_.pop ();
}

void hello_pskel::
sequence_0 (unsigned long& state,
            unsigned long& count,
            const ::xsde::cxx::ro_string& ns,
            const ::xsde::cxx::ro_string& n,
            bool start)
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  XSDE_UNUSED (ctx);

  switch (state)
  {
    case 0UL:
    {
      if (n == "greeting" && ns.empty ())
      {
        if (start)
        {
          if (this->greeting_parser_)
          {
            this->greeting_parser_->pre ();
            ctx.nested_parser (this->greeting_parser_);
          }
        }
        else
        {
          if (this->greeting_parser_ != 0)
          {
            const ::std::string& tmp = this->greeting_parser_->post_string ();
            this->greeting (tmp);
          }

          count = 0;
          state = 1UL;
        }

        break;
      }
      else
      {
        assert (start);
        if (count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          break;
        }

        count = 0;
        state = 1UL;
        // Fall through.
      }
    }
    case 1UL:
    {
      if (n == "name" && ns.empty ())
      {
        if (start)
        {
          if (this->name_parser_)
          {
            this->name_parser_->pre ();
            ctx.nested_parser (this->name_parser_);
          }
        }
        else
        {
          if (this->name_parser_ != 0)
          {
            const ::std::string& tmp = this->name_parser_->post_string ();
            this->name (tmp);
          }

          count++;
        }

        break;
      }
      else
      {
        assert (start);
        if (count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          break;
        }

        count = 0;
        state = ~0UL;
        // Fall through.
      }
    }
    case ~0UL:
      break;
  }
}

// Begin epilogue.
//
//
// End epilogue.

