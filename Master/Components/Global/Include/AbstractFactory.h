/****************************************************************************/
/*! \file Global/Include/AbstractFactory.h
 *
 *  \brief Definition file for class AbstractFactory.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
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

#ifndef GLOBAL_ABSTRACTFACTORY_H
#define GLOBAL_ABSTRACTFACTORY_H

#include <Global/Include/Exception.h>

#include <QMap>

namespace Global {

/****************************************************************************/
/**
 * \brief Template function to create instances of derived classes
 *
 * \return      New created instance.
 */
/****************************************************************************/
template<class TBaseClass, class TDerivedClass> TBaseClass * TemplateCreateInstance() {
    return new TDerivedClass;
}

#define TMap QMap<QString, TBaseClass*(*)()>    ///< Typedef for the function pointer map.

/****************************************************************************/
/**
 * \brief This is the abstract factory class.
 *
 * It handles the map of function pointers and registers class names with the
 * according function pointers for the creation.\n
 * Example of usage:\n
 * <b>Header file</b>
 * \code
 * //----------------------------------------------------------------------------
 * class ExampleBaseClass {
 * private:
 *     //----------------------------------------------------------------------------
 *     ExampleBaseClass(const ExampleBaseClass &);
 *     const ExampleBaseClass & operator = (const ExampleBaseClass &);
 * public:
 *     //----------------------------------------------------------------------------
 *     static void LinkerTamer();
 *     //----------------------------------------------------------------------------
 *     inline ExampleBaseClass() {
 *     }
 *     //----------------------------------------------------------------------------
 *     virtual ~ExampleBaseClass() {
 *     }
 *     //----------------------------------------------------------------------------
 *     virtual void DoSomething() = 0;
 * };
 *
 * typedef AbstractFactory<ExampleBaseClass> ExampleAF;    ///< Typedef for easier usage.
 *
 * //----------------------------------------------------------------------------
 * class ExampleDerivedA : public ExampleBaseClass {
 * private:
 *     DECLARE_REG_CLASS(ExampleBaseClass, ExampleDerivedA);   ///< Declaration of static registration instance.
 *     //----------------------------------------------------------------------------
 *     ExampleDerivedA(const ExampleDerivedA &);
 *     const ExampleDerivedA & operator = (const ExampleDerivedA &);
 * public:
 *     //----------------------------------------------------------------------------
 *     ExampleDerivedA();
 *     //----------------------------------------------------------------------------
 *     virtual ~ExampleDerivedA();
 *     //----------------------------------------------------------------------------
 *     virtual void DoSomething();
 * };
 *
 * //----------------------------------------------------------------------------
 * class ExampleDerivedB : public ExampleBaseClass {
 * private:
 *     DECLARE_REG_CLASS(ExampleBaseClass, ExampleDerivedB);   ///< Declaration of static registration instance.
 *     //----------------------------------------------------------------------------
 *     ExampleDerivedB(const ExampleDerivedB &);                           ///< Not implemented.
 *     const ExampleDerivedB & operator = (const ExampleDerivedB &);       ///< Not implemented.
 * public:
 *     //----------------------------------------------------------------------------
 *     ExampleDerivedB();
 *     //----------------------------------------------------------------------------
 *     virtual ~ExampleDerivedB();
 *     //----------------------------------------------------------------------------
 *     virtual void DoSomething();
 * };
 *
 * \endcode
 *
 * <b>Source file</b>
 * \code
 * DEFINE_REG_CLASS(ExampleBaseClass, ExampleDerivedA);    ///< Static registration member.
 * DEFINE_REG_CLASS(ExampleBaseClass, ExampleDerivedB);    ///< Static registration member.
 *
 * //----------------------------------------------------------------------------
 * void ExampleBaseClass::LinkerTamer() {
 *     // do nothing. just be here in the cpp file.
 * }
 *
 * //----------------------------------------------------------------------------
 * ExampleDerivedA::ExampleDerivedA() {
 * }
 *
 * //----------------------------------------------------------------------------
 * ExampleDerivedA::~ExampleDerivedA() {
 * }
 *
 * //----------------------------------------------------------------------------
 * void ExampleDerivedA::DoSomething() {
 *     qDebug() << "DerivedA";
 * }
 *
 * //----------------------------------------------------------------------------
 * ExampleDerivedB::ExampleDerivedB() {
 * }
 *
 * //----------------------------------------------------------------------------
 * ExampleDerivedB::~ExampleDerivedB() {
 * }
 *
 * //----------------------------------------------------------------------------
 * void ExampleDerivedB::DoSomething() {
 *     qDebug() << "DerivedB";
 * }
 * \endcode
 *
 * How to use <b>in your code</b>:
 * \code
 *
 * ExampleAF::Initialize();
 * ExampleBaseClass *pA = ExampleAF::CreateInstance("ExampleDerivedA");
 * ExampleBaseClass *pB = ExampleAF::CreateInstance("ExampleDerivedB");
 *
 * pA->DoSomething();
 * pB->DoSomething();
 *
 * delete pA;
 * delete pB;
 * \endcode
 */
/****************************************************************************/
template<class TBaseClass> class AbstractFactory {
friend class TestAbstractFactory;
private:
    static TMap     *m_pMap;    /// The map containing the registered classes and their creation function.
    /****************************************************************************/
    AbstractFactory(const AbstractFactory &);                       ///< Not implemented.
    const AbstractFactory & operator = (const AbstractFactory &);   ///< Not implemented.
protected:
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    inline AbstractFactory() {
    }
    /****************************************************************************/
    /**
     * \brief Get the map pointer.
     *
     * If no map exists, one is created.
     */
    /****************************************************************************/
    static TMap *GetMap() {
        if(m_pMap == NULL)
            m_pMap = new TMap;
        return m_pMap;
    }
public:
    /****************************************************************************/
    /**
     * \brief Destructor.
     *
     * Cleanup map.
     */
    /****************************************************************************/
    virtual ~AbstractFactory() {
        delete m_pMap;
        m_pMap = NULL;
    }
    /****************************************************************************/
    /**
     * \brief Create a class with a specific class name.
     *
     * If the class was not registered or initialized an exception is thrown.
     *
     * \param[in]   ClassName   Name of class to create.
     * \return                  The created class.
     */
    /****************************************************************************/
    static TBaseClass *CreateInstance(const QString &ClassName) {
        if (m_pMap == NULL) {
            // the m_pMap object does not exist. DO NOT Throw an exception,
            // because it is up to this class's User to deside what to do with it.
            return NULL;
        }
        if(!m_pMap->contains(ClassName)) {
            // no registered class with this name found. DO NOT Throw an exception,
            // because it is up to this class's User to deside what to do with it.
            return NULL;
        }
        // return new instance
        return m_pMap->value(ClassName)();
    }

    /****************************************************************************/
    /**
     * \brief Check if class with specific name is registered.
     *
     * \param[in]   ClassName   Name of class to check for.
     * \return      true if class is registered, false otherwise.
     */
    /****************************************************************************/
    static bool CheckClassInstance(const QString &ClassName) {
        if (m_pMap == NULL) {
            // the m_pMap object does not exist. DO NOT Throw an exception,
            // because it is up to this class's User to decide what to do with it.
            return false;
        }
        if(!m_pMap->contains(ClassName)) {
            // no registered class with this name found. DO NOT Throw an exception,
            // because it is up to this class's User to decide what to do with it.
            return false;
        }
        // an instance of class ClassName is registered -> return true
        return true;
    }

    /****************************************************************************/
    /**
     * \brief This method must be called prior to first use.
     *
     * by calling TBaseClass::LinkerTamer(), it ensures that the linker
     * creates all needed static members, although we are in a static library!
     */
    /****************************************************************************/
    static void Initialize() {
        // call the linker tamer to ensure correct instantiation of static
        // members in static library.
        TBaseClass::LinkerTamer();
    }
}; // end class AbstractFactory

template<class TBaseClass> QMap<QString, TBaseClass*(*)()> *AbstractFactory<TBaseClass>::m_pMap = NULL; ///< The map pointer.

/****************************************************************************/
/**
 * \brief Registration class for specific base class / derived class.
 *
 * Its constructor registers the class name and its creation function in
 * the base abstract factory.
 */
/****************************************************************************/
template<class TBaseClass, class TDerivedClass> class RegistrationClass : public AbstractFactory<TBaseClass> {
private:
    /****************************************************************************/
    RegistrationClass();                                                ///< Not implemented.
    RegistrationClass(const RegistrationClass &);                       ///< Not implemented.
    const RegistrationClass & operator = (const RegistrationClass&);    ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~RegistrationClass() {
    }
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * It registers the class name and its creation function.
     *
     * \param[in]   ClassName   Class name for TDerivedClass.
     */
    /****************************************************************************/
    RegistrationClass(const QString &ClassName) :
        AbstractFactory<TBaseClass>()
    {
        // we DO NOT NEED the return value of insert
        static_cast<void>(
            AbstractFactory<TBaseClass>::GetMap()->insert(ClassName, &TemplateCreateInstance<TBaseClass, TDerivedClass>)
        );
    }
}; // end class RegistrationClass

#define DECLARE_REG_CLASS(BASE, NAME) static Global::RegistrationClass<BASE, NAME> reg          ///< Macro to declare static registration member.
#define DEFINE_REG_CLASS(BASE, NAME) Global::RegistrationClass<BASE, NAME> NAME::reg(#NAME)     ///< Macro to define static registration member.

} // end namespace Global

#endif // GLOBAL_ABSTRACTFACTORY_H
