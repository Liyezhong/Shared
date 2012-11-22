/****************************************************************************/
/*! \file TestAbstractFactory.h
 *
 *  \brief Definition file for class TestAbstractFactory.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-20
 *  $Author:    $ J.Bugariu
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef GLOBAL_TESTABSTRACTFACTORY_H
#define GLOBAL_TESTABSTRACTFACTORY_H

#include <Global/Include/AbstractFactory.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Base class for testing AbstractFactory class.
 */
/****************************************************************************/
class AFTestBaseClass {
private:
    /****************************************************************************/
    AFTestBaseClass(const AFTestBaseClass &);                       ///< Not implemented.
    const AFTestBaseClass & operator = (const AFTestBaseClass &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief linker tamer.
     */
    /****************************************************************************/
    static void LinkerTamer();
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline AFTestBaseClass() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AFTestBaseClass() {
    }
    /****************************************************************************/
    /**
     * \brief Do something.
     */
    /****************************************************************************/
    virtual int GetID() = 0;
}; // end class AFTestBaseClass

typedef AbstractFactory<AFTestBaseClass> AFTestAF;      ///< Typedef for easier usage.

/****************************************************************************/
/**
 * \brief Derived class for testing AbstractFactory class.
 */
/****************************************************************************/
class AFTestDerivedClass1 : public AFTestBaseClass {
private:
    DECLARE_REG_CLASS(AFTestBaseClass, AFTestDerivedClass1);    ///< Declaration of static registration instance.
    /****************************************************************************/
    AFTestDerivedClass1(const AFTestDerivedClass1 &);                       ///< Not implemented.
    const AFTestDerivedClass1 & operator = (const AFTestDerivedClass1 &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline AFTestDerivedClass1() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AFTestDerivedClass1() {
    }
    /****************************************************************************/
    /**
     * \brief Do something.
     */
    /****************************************************************************/
    virtual int GetID() {
        return 1;
    }
}; // end class AFTestDerivedClass1

/****************************************************************************/
/**
 * \brief Derived class for testing AbstractFactory class.
 */
/****************************************************************************/
class AFTestDerivedClass2 : public AFTestBaseClass {
private:
    DECLARE_REG_CLASS(AFTestBaseClass, AFTestDerivedClass2);    ///< Declaration of static registration instance.
    /****************************************************************************/
    AFTestDerivedClass2(const AFTestDerivedClass2 &);                       ///< Not implemented.
    const AFTestDerivedClass2 & operator = (const AFTestDerivedClass2 &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline AFTestDerivedClass2() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~AFTestDerivedClass2() {
    }
    /****************************************************************************/
    /**
     * \brief Do something.
     */
    /****************************************************************************/
    virtual int GetID() {
        return 2;
    }
}; // end class AFTestDerivedClass2

} // end namespace Global

#endif // GLOBAL_TESTABSTRACTFACTORY_H
