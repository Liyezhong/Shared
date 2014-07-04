/* $Id: AeVersion.h,v 1.12.2.11 2010/08/13 16:01:39 dkhodos Exp $ */

/**************************************************************************
 *
 *  Copyright (c) 1999-2007 Axeda Corporation. All rights reserved.
 *
 **************************************************************************
 *
 *  Filename   :  AeVersion.h
 *
 *  Subsystem  :  Axeda Agent Embedded
 *
 *  Description:  Version macros
 *
 **************************************************************************/
#ifndef _AE_VERSION_H_
#define _AE_VERSION_H_

#define AE_VERSION_MAJOR 6
#define AE_VERSION_MINOR 6
#define AE_VERSION_BUILD 502

#define AE_VERSION_STRING(major , minor, build) AE_VERSION_STRING_FUNC(major , minor, build)
#define AE_VERSION_STRING_FUNC(major , minor, build) #major "." #minor "-" #build

#endif
