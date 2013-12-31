// ================================================================================================
/** @file Platform/Master/Components/Global/Include/IndexT.h
 *
 *  @brief
 *  Declaration and definition of template class IndexT
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2011-01-31
 *  $Author:    $ Martin Weitzel
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 * ================================================================================================
*/
    // ==============================
    // FlexeLint Message Suppressions
    // ==============================

    //lint -esym(1060, Global::IndexT<*>::NotValid) [MW]
    //lint -esym(1060, Global::IndexT<*>::ValidRange) [MW]
    //lint -esym(1060, Global::IndexT<*>::intValue*) [MW]
    //lint -esym(1060, Global::IndexT<*>::lt_cmp*) [MW]
    //lint -esym(1060, Global::IndexT<*>::eq_cmp*) [MW]
    // --------------------------------------------------------------------------------------------
    //
    // FlexeLint considers it as an C++ error (number is in lower 1000 range) to access ValidRange
    // and intValue in  non-member non-friend functions.
    // On the other hand g++ has no problem with this code.  It seems FlexeLint does not properly
    // recognize the base class typedef (Super), maybe because it is private. As the compiler
    // itself has no problem with such code, a central specific suppression by symbol was chosen
    // to avoid littering the code with individual supressions.
    //
    // TODO: Extract minimal failing case and try again on the Gimpel Web-Page.
    //
    // -- Martin Weitzel[MW] / 2011-02-09

#ifndef GLOBAL_INDEXT_H
#define GLOBAL_INDEXT_H

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                                                                 From C++ Library
#include <cstddef>
#include <stdexcept>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                                                                  From Qt-Library
#include <QtGlobal>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                                                                Platform Specific
extern uint qHash(int);     //!< hash variable

namespace Global {

    // ============================================================================================
    //                                   Definition of Classes
    // ============================================================================================

    /*********************************************************************************************/
    /**
     * Base class template to create classes for typesafe indexing.
     *
     * Classes derived from this template set the indexing limits (smallest and largest value).
     * CRTP is used to create different classes in case two derived classes have the same limits.
     * The index created contains ALL values in the defined range.
     * In addition, one more value denotes the 'invalid' case.
     * Increment and decrement operators exhibit 'wrap-around'-behavior as follows.
     *  - incrementing from the largest value results in the invalid value
     *  - incrementing from the invalid value results in the smallest value
     *  - decrementing from the smallest value results in the invalid value
     *  - decrementing from the invalid value results in the largest value
     * Provisions are made for a mapping to zero based offests (e.g. an array index).
     *
     * A typical derived class may look as follows:
     * @code
     *  class MyIndexType
     *      :public Global::IndexT<1, 50, MyIndexType> {
     *      typedef Global::IndexT<1, 50, MyIndexType> Super;
     *  public:
     *      // ------------------------------------------- Define c'tors as required
     *      MyIndexType() : Super() {}                  // <-- for default c'tor (NotValid)
     *      MyIndexType() : Super(42) {}                // <-- OR for default c'tor (val=42)
     *      explicit                                    // <-- add to AVOID AUTO-conversion
     *      MyIndexType(int v) : Super(v) {}            // <-- for construction from int
     *      // ------------------------------------------- Lift operations into visibility
     *      using Super::eq_cmp;                        // <-- support == and !=
     *      using Super::lt_cmp;                        // <-- support <, <=, >, >=
     *      using Super::intValue;                      // <-- for conversion to int
     *      using Super::operator++;                    // <-- to allow increment
     *      using Super::operator--;                    // <-- to allow decrement
     *      // ------------------------------------------- Some more things a client might need (*)
     *      using Super::ValidRange;                    // <-- number of valid values
     *      using Super::ArraySize;                     // <-- as before PLUS extraSpace
     *      using Super::zeroBasedOffset;               // <-- mapping to an unsigned
     *  };
     * @endcode
     *
     * (*: See also documention near these elements.)
     *
     * If desired, also an automatic type conversion to int could be supplied.
     * THIS IS NOT RECOMMENDED as it will make the concept of type safe indexes much less enforcable!
     * @code
     *      operator int() const { return intValue(); }
     * @endcode
     * (If the above is required in a few places only, leave it private and controll access via friend.)
    */
    /*********************************************************************************************/
    template<int minVal, int maxVal, class IndexType, std::size_t extraSpace = 1u>
    class IndexT {
        typedef IndexT<minVal, maxVal, IndexType, extraSpace> Myself; //!< shorthand for own class

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //                                                                                 Nested Types
    protected:

        /**
         * Identifier for the invalid case.
         *
         * IndexType classes may exhibit that name to their clients as following:
         * @code
         *  using Super::NotValid;
         * @endcode
         * Conceptually this turns 'not valid' from a state to a special value.
        */
        enum { NotValid = (minVal-1) };

        /**
         * Range of valid values PLUS extra space as requested by extraSpace.
         *
         * This may be used as array dimension (classical C-style).
         * By default, the array WILL include extra space for the NotValid index.
         *
         * @code
         *  SomeType someData[MyIndexType::ArraySize]
         * @endcode
         *
         * @note
         * To map an IndexT to an array index use zeroBasedOffset.
        */
        enum { ArraySize = (maxVal-minVal) + 1 + extraSpace };

        /**
         * Range of valid values (as compile time constant)
         *
         * Do NOT use this as array dimension!
         * Rather set extraSpace to zero.
        */
        enum { ValidRange = (maxVal-minVal) + 1 };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //                                                                                Class methods
    public:

        /**
         * The minimum value (supplied as first template argument).
         *
         * @note
         * This is resolved at COMPILE TIME.
         * Using \c MyIndexType::first() incures NO OVERHEAD compared to using a literal constant.
         * @return   minimum value
        */
        static
        IndexType first() {
            return IndexType(minVal);
        }

        /**
         * The maximum value (supplied as second template argument).
         *
         * @note
         * This is resolved at COMPILE TIME.
         * Using \c MyIndexType::last() incures NO OVERHEAD compared to using a literal constant.
         * @return   maximum value
        */
        static
        IndexType last() {
            return IndexType(maxVal);
        }

        /**
         * Check if integer is in alowed range.
         *
         * @iparam  iVal -- integer value to check
         * @returns false if not in range minVal maxVal
         * @throw   (never throws)
         * @post    iVal is usable as constructor argument
         *
         * @note
         * In general it is a c'tor precondition to use only int values in the allowed range.
         * (Though debug compilations will use assert to detect invalid c'tor arguments early.)
         * With this method a check can be made as follows:
         * @code
         *  int someInt = ...;  // whatever
         *  if (MyIndexType::checkValid(someInt))
         *      throw ...;      // whatever
         *  MyIndexType someIndex(someInt);
         * @endcode
        */
        static
        bool checkValid(int iVal) {
            return (minVal <= iVal && iVal <= maxVal);
        }

    protected:

        /**
         * Helper for equality comparison operators.
         *
         * @iparam  lhS -- left hand side operand.
         * @iparam  rhS -- right hand side operand.
         * @returns true if lhS and rhS are equal.
         * @throw   (never throws)
         *
         * (See class description for more information)
        */
        static
        bool eq_cmp(const Myself lhS, const Myself rhS) {
            return lhS.val == rhS.val;
        }

        /**
         * Helper for ordered comparison operators.
         *
         * @iparam  lhS -- left hand side operand.
         * @iparam  rhS -- right hand side operand.
         * @returns true if lhS is less than rhS.
         * @throw   (never throws)
         * (See class description for more information)
        */
        static
        bool lt_cmp(const Myself lhS, const Myself rhS) {
            return lhS.val < rhS.val;
        }


    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //                                                                                 Constructors
    protected:

        /**
         * Construct with default value (NotValid).
         *
         * @throw   (never throws)
         * @post    set to NotValid
        */
        IndexT()
            : val(NotValid)
        {}


        /**
         * Construct with integer value.
         *
         * @iparam   iVal -- initial index value (as int)
         * @throw   (never throws)
         * @pre     iVal is NotValid or in range minVal..maxVal
         * @post    set to NotValid or iVal
        */
        explicit
        IndexT(int iVal)
            : val(iVal)
        {
            Q_ASSERT(iVal == NotValid || isValid());
        }

        /**
         * Access value as integer.
         *
         * @returns the value as integer
         * @pre     contained value is valid
         * @post    a value in range minVal..maxVal
         *
         * @note
         * This is a protected method.
         * To enable clients of classes to use this it needs to be lifted into public.
        */
        int intValue() const {
            Q_ASSERT(isValid());
            return val;
        }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //                                                                             Instance Methods
    public:

        /**
         * Check if valid.
         *
         * @throw   (never throws)
         *@return   true if valid
        */
        bool isValid() const {
            return val != NotValid;
        }

        /**
         * Set to NotValid.
         *
         * @throw   (never throws)
         * @post    set to NotValid
        */
        void setInvalid() {
          val = NotValid;
        }

    protected:

        /**
         * Decrement and return new value.
         *
         * @throw   (never throws)
         * @post    compares less than previous value or is invalid
         *
         * Can be called 'maxVal-minVal+1' times from NotValid until again NotValid.
         * @return   decremented new value
        */
        IndexType operator--() {
            if (--val < NotValid)
                val = maxVal;
            return IndexType(val);
        }

        /**
         * Decrement and return old value.
         *
         * @throw   (never throws)
         * @post    compares less to previous value or is invalid
         *
         * Can be called 'maxVal-minVal+1' times from NotValid until again NotValid.
         * @return   decremented old value
        */
        IndexType operator--(int) {
            const int oldval(val);
            this->operator--();
            return IndexType(oldval);
        }

        /**
         * Increment and return new value.
         *
         * @throw   (never throws)
         * @post    compares greater to previous value or is invalid
         *
         * Can be called 'maxVal-minVal+1' times from NotValid until again NotValid.
         * @return  incremented new value
        */
        IndexType operator++() {
            if (++val > maxVal)
                setInvalid();
            return IndexType(val);
        }

        /**
         * Increment and return old value.
         *
         * @throw   (never throws)
         * @post    compares greater to previous value or is invalid
         *
         * Can be called 'maxVal-minVal+1' times from NotValid until again NotValid.
         * @return incremented old value
        */
        IndexType operator++(int) {
            const int oldval(val);
            this->operator++();
            return IndexType(oldval);
        }

        /**
         * Return value with given distances (if any).
         *
         * @iparam  nDist -- distance.
         * @returns previous value
         * @throw   (never throws)
         * @pre     Current value must be valid.
         *
         * The value of nDist may be positve, negative or zero.
         * It is added to the currenty value.
         * If the result is outside the range allowed for the index, an invalid index is returned.
        */
        IndexType delta(int nDist) const {
            // this could help to recognise excessive overflows,
            // but is currently disabled for performance reasons
            //
            // Q_ASSERT((nDist > 0)
            //            ? nDist <= (maxVal - minVal)
            //            : nDist >= (minVal - maxVal)
            // );
            const int deltaVal(
                val + nDist
            );
            return (minVal <= deltaVal && deltaVal <= maxVal)
                    ? IndexType(deltaVal)
                    : IndexType();
        }

        /**
         * Return previous value with given distance (if any).
         *
         * @iparam  nDist -- distance.
         * @returns Previous value with requested distance.
         * @throw   (never throws)
         * @pre     nDist must be equal to or greater than zero.
         * @pre     Current value must be valid.
         *
         * The value nDist is subtracted from the current value.
         * If the result is outside the range allowed for the index, an invalid index is returned.
         *
        */
        IndexType prev(int nDist = 1) const {
            Q_ASSERT(nDist >= 0);
            return delta(-nDist);
        }

        /**
         * Return next value with given distance (if any).
         *
         * @iparam  nDist -- distance.
         * @returns next value
         * @throw   (never throws)
         * @pre     nDist must be equal to or greater than zero.
         * @pre     Current value must be valid.
         *
         * The value nDist is added to the current value.
         * If the result is outside the range allowed for the index, an invalid index is returned.
        */
        IndexType next(int nDist = 1) const {
            Q_ASSERT(nDist >= 0);
            return delta(+nDist);
        }

        /**
         * Calculate the offset from minVal and add extraSpace.
         *
         * @tparam  ResultType -- type of result requested.
         * @returns offset of current value to minVal PLUS extraSpace.
         * @pre     Valid content.
         * @post    If extraSpace is 0 result in range 0 .. (ValidRange-1).
         * @post    Otherwise result in range extraSpace .. (extraSpace + ValidRange - 1)
         *
         * The intent is to convert an IndexT instance into an index to access physical storage.
         * The type argument has to be suppplied as follows:
         * @code
         *  ... anIdx.zeroBasedOffset<std::size_t>();
         * @endcode
         * Or:
         * @code
         *  ... anIdx.zeroBasedOffset<std::vector::size_type>();
         * @endcode
         * (This avoids to have to write call-site casts to suppress compiler warnings.)
        */
        template<class ResultType>
        ResultType zeroBasedOffset() const {
            Q_ASSERT(NotValid <= val && val <= maxVal);
            Q_ASSERT(val != NotValid || extraSpace > 0);
            return static_cast<ResultType>(val - minVal + extraSpace);
        }

        /**
         * Calculate the offset from minVal.
         *
         * @tparam  BoundType -- type of result requested.
         * @iparam  uBound -- upper bound at which result becomes invalid.
         * @returns offset of current value to minVal PLUS extraSpace.
         * @throws  std::exception::out_of_bounds -- if resulting index equals or exceeds uBound.
         * @pre     Valid content.
         * @pre     uBound must be less than or equal to ValidRange PLUS extraSpace (casted to T).
         * @post    Result in range extraSpace PLUS 0 .. (uBound - 1).
         *
         * By default this operation returns its result in the same type as the uBound argument.
         * The intend is the caller supplies the ACTUAL size of Storage as argument.
         * The index value will then be turned into
         * @code
         *  T store[MyIndexType::ArraySize];
         *  MyIndexType anIdx;
         *  ...
         *  ... anIdx.zeroBasedOffset<std::vector::size_type>(sizeof store);
         *  ...
         * @endcode
        */
        template<class BoundType>
        BoundType zeroBasedOffset(BoundType uBound) const {
            if (static_cast<BoundType>(val - minVal + extraSpace) >= uBound) {
                throw std::out_of_range(IndexType::Not_in_bounded_range);
            }
            return zeroBasedOffset<BoundType>();
        }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //                                                                                Instance Data
        /**
         * Store index value.
        */
        int val;


    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //                                                                           Friend Definitions
        /**
         * Make IndexT usable with Qt containers.
         *
         * @iparam  idxVal -- index  or which a hash value is to be created.
        */
    friend
        uint qHash(const IndexT<minVal, maxVal, IndexType, extraSpace> idxVal) {
            return qHash(idxVal.template zeroBasedOffset<int>());
        }

    };

    // ============================================================================================
    //                              Definition of Global Template Methods
    // ============================================================================================

    /**
     * Equality comparison.
     *
     * @tparam  (see class IndexT)
     * @iparam  lhS -- left hand side operand
     * @iparam  rhS -- right hand side operand
     * @returns true if lhS and rhS are equal
     *
     * Operands my be NotValid.
     * NotValid compares equal to NotValid, no valid values.
    */
    template<int minVal, int maxVal, class IndexType, std::size_t extraSpace>
    inline
    bool operator==(const IndexT<minVal, maxVal, IndexType, extraSpace> lhS,
                    const IndexT<minVal, maxVal, IndexType, extraSpace> rhS) {
        return IndexType::eq_cmp(lhS, rhS);
    }

    /**
     * In-Equality comparison.
     *
     * @tparam  (see class IndexT)
     * @iparam  lhS -- left hand side operand
     * @iparam  rhS -- right hand side operand
     * @returns true if lhS and rhS are unequal
     *
     * Operands my be NotValid.
     * NotValid compares in-equal to valid values.
    */
    template<int minVal, int maxVal, class IndexType, std::size_t extraSpace>
    inline
    bool operator!=(const IndexT<minVal, maxVal, IndexType, extraSpace> lhS,
                    const IndexT<minVal, maxVal, IndexType, extraSpace> rhS) {
        return !(lhS == rhS);
    }

    /**
     * Less-than comparision.
     *
     * @tparam  (see class IndexT)
     * @iparam  lhS -- left hand side operand
     * @iparam  rhS -- right hand side operand
     * @returns true if lhS and rhS are unequal
     * @pre     lhS and rhS must both be valid
    */
    template<int minVal, int maxVal, class IndexType, std::size_t extraSpace>
    inline
    bool operator<(const IndexT<minVal, maxVal, IndexType, extraSpace> lhS,
                   const IndexT<minVal, maxVal, IndexType, extraSpace> rhS) {
        Q_ASSERT(lhS.isValid());
        Q_ASSERT(rhS.isValid());
        return IndexType::lt_cmp(lhS, rhS);
    }

    /**
     * Less-than OR equal comparision.
     *
     * @tparam  (see class IndexT)
     * @iparam  lhS -- left hand side operand
     * @iparam  rhS -- right hand side operand
     * @returns true if lhS is less than or equal to rhS
     * @pre     lhS and rhS must both be valid
    */
    template<int minVal, int maxVal, class IndexType, std::size_t extraSpace>
    inline
    bool operator<=(const IndexT<minVal, maxVal, IndexType, extraSpace> lhS,
                    const IndexT<minVal, maxVal, IndexType, extraSpace> rhS) {
        return !(rhS < lhS);
    }

    /**
     * Less-than comparision.
     *
     * @tparam  (see class IndexT)
     * @iparam  lhS -- left hand side operand
     * @iparam  rhS -- right hand side operand
     * @returns true if lhS is greater than or equal to rhS
     * @pre     lhS and rhS must both be valid
    */
    template<int minVal, int maxVal, class IndexType, std::size_t extraSpace>
    inline
    bool operator>=(const IndexT<minVal, maxVal, IndexType, extraSpace> lhS,
                    const IndexT<minVal, maxVal, IndexType, extraSpace> rhS) {
        return !(lhS < rhS);
    }

    /**
     * Less-than comparision.
     *
     * @tparam  (see class IndexT)
     * @iparam  lhS -- left hand side operand
     * @iparam  rhS -- right hand side operand
     * @returns true if lhS and rhS are unequal
     * @returns true if lhS is greater than rhS
     * @pre     lhS and rhS must both be valid
    */
    template<int minVal, int maxVal, class IndexType, std::size_t extraSpace>
    inline
    bool operator>(const IndexT<minVal, maxVal, IndexType, extraSpace> lhS,
                   const IndexT<minVal, maxVal, IndexType, extraSpace> rhS) {
        return (rhS < lhS);
    }

} // end namespace Global

#endif // GLOBAL_INDEXT_H
