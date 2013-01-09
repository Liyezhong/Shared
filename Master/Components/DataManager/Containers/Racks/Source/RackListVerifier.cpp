/****************************************************************************/
/*! \file RackListVerifier.cpp
 *
 *  \brief CRackListVerifier implementation.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 2012-01-02, 2012-04-23
 *   $Author:  $ M.Scherer, Raju
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

#include <QDebug>

#include "DataManager/Containers/Racks/Include/RackListVerifier.h"
#include "DataManager/Containers/Racks/Include/RackList.h"

namespace DataManager {

const int RACKS_MAX        = 100; ///< Maximum racks
const int RFID_LENGTH      = 10;  ///< RF ID length
const int STATIONID_LENGTH = 10;  ///< Station ID length
const int STEPINDEX_LENGTH = 5;   ///< Step index length

/****************************************************************************/
/*!
 *  \brief  Constructor
 */
/****************************************************************************/
CRackListVerifier::CRackListVerifier() : mp_DRL(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies a list of racks
 *
 *  \iparam p_RackList = List of racks
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackListVerifier::VerifyData(CDataContainerBase* p_RackList)
{
    // to store the error description
    QString ErrorDescription;
    // by default make the verification flag to true
    bool VerifiedData = true;
    // assign pointer to member variable
    mp_DRL = static_cast<CRackList*>(p_RackList);

    // check constraints
    // check max number of racks (100)
    if (mp_DRL->GetNumberOfRacks() > RACKS_MAX) {
        ErrorDescription += "Too many racks (max 100 allowed): "
                + QString::number(mp_DRL->GetNumberOfRacks()) + "\n";
        VerifiedData = false;
    }

    //int NoOfRacks = static_cast<int>(mp_DRL->GetNumberOfRacks()); ///< To store the number of racks

    for (int RackCounter = 0; RackCounter < mp_DRL->GetNumberOfRacks(); RackCounter++) {
        const CRack *p_Rack = mp_DRL->GetRack(RackCounter); // Get the rack from the rack list
        // get the rack from the rack list
        if (p_Rack) {
            // check for the RF ID length
            if (QString::number(p_Rack->GetRFIDUniqueID()).length() > RFID_LENGTH) {
                ErrorDescription += "RF ID is too big (max 9999999999 allowed): "
                        + QString::number(p_Rack->GetRFIDUniqueID()) + "\n";
                VerifiedData = false;
            }

            // check for the program ID length
            if (p_Rack->GetProgramID().length() < 0) {
                ErrorDescription += "Program ID is empty: "
                        + p_Rack->GetProgramID() + "\n";
                VerifiedData = false;
            }
            else {
                //Verify program ID
                if (p_Rack->GetProgramID().length() == 0)
                {
                    ErrorDescription +=  "Missing Program Id\n";
                    VerifiedData = false;
                }
                else
                {
                    if ((p_Rack->GetProgramID().at(0) != 'U') && (p_Rack->GetProgramID().at(0) != 'L')) {
                        ErrorDescription +=  "Invalid Program Id: " + p_Rack->GetProgramID() + "\n";
                        VerifiedData = false;
                    }
                    bool Ok = false;
                    int ProgramId = p_Rack->GetProgramID().mid(1).toInt(&Ok);
                    if ((!Ok) && (!(ProgramId >= 1))) {
                        ErrorDescription += "### Invalid program Id: " + p_Rack->GetProgramID() + "\n";
                        VerifiedData = false;
                    }
                }
            }

            // check for the program step index length
            if (QString::number(p_Rack->GetProgramStepIndex()).length() > STEPINDEX_LENGTH) {
                ErrorDescription += "Step index is too big (max 99999 allowed): "
                        + QString::number(p_Rack->GetProgramStepIndex()) + "\n";
                VerifiedData = false;
            }

            // check for the station ID length
            if (p_Rack->GetStationID().length() > STATIONID_LENGTH) {
                ErrorDescription += "Station ID is too big (max 10 characters are allowed): "
                        + p_Rack->GetStationID() + "\n";
                VerifiedData = false;
            }

            // verify the rack date time whether it is null or valid or not
            // i.e. like colorado date time, sepia date time and station date time
            // and also update the error description and verification flag
            ValidateRackDateTime(p_Rack, VerifiedData, ErrorDescription);

            // store all the colors in a list, so it will be easy to compare the values
            // otherwise we need to write 9 checks for the condition. Or switch also can be used.
            QStringList ColorList;
            ColorList << "white" << "yellow" << "lightblue" << "darkblue" << "pink" << "red"
                      << "green" << "black" << "grey";
            // check the color
            if (!(ColorList.contains(p_Rack->GetColor(), Qt::CaseSensitive))) {
                ErrorDescription += "Wrong color (only white, yellow, lightblue, darkblue, pink, red, green, black, grey are allowed): "
                        + p_Rack->GetColor() + "\n";
                VerifiedData = false;
            }
        }
    }

    //qDebug() << "Error Description" << ErrorDescription;

    if (!VerifiedData) {
        // store error string with verification code
        //m_ErrorsList.append(QString("0,") + ErrorDescription);
    }

    // return the boolean flag
    return VerifiedData;

}


/****************************************************************************/
/*!
 *  \brief  Compares the date time of the racks
 *
 *  \iparam p_Rack = Pointer to CRack
 *  \iparam VerifiedData = verifier flag value
 *  \iparam ErrorDescription = Description of the error
 *
 */
/****************************************************************************/
void CRackListVerifier::ValidateRackDateTime(const CRack* p_Rack, bool& VerifiedData, QString& ErrorDescription)
{
    bool CheckColoradoDateTime = true; ///< check whether colorado exists or not to validate date time

    // check for the start time value null or not
    if (p_Rack->GetStartTime().isNull()) {
        ErrorDescription += "DateTime format is null for the Rack start time: "
                + p_Rack->GetStartTime().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
        VerifiedData = false;
    }

    // check for the start time format valid or not
    if (!p_Rack->GetStartTime().isValid()) {
        ErrorDescription += "DateTime format is invalid for the Rack start time: "
                + p_Rack->GetStartTime().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
        VerifiedData = false;
    }

    // check the transported flag
    if (p_Rack->GetTransported()) {
        // if the transporting flag set then rack should consist the Sepia station flag
//        if (!p_Rack->GetSepiaStation()) {
//            ErrorDescription += "Sepia station flag is not set but the transport station flag is set /r/n";
//            VerifiedData = false;
//        }
    }
    else {
        if (p_Rack->GetSepiaStation()) {
            CheckColoradoDateTime = false;
        }
    }

    // check whether colorado time validation requires or not
    if (CheckColoradoDateTime) {
        // check for the end time of colorado value null or not
        if (p_Rack->GetEndTimeColorado().isNull()) {
            ErrorDescription += "DateTime format is null for the Rack end time of colorado: "
                    + p_Rack->GetEndTimeColorado().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }

        // check for the end time of colorado format valid or not
        if (!p_Rack->GetEndTimeColorado().isValid()) {
            ErrorDescription += "DateTime format is invalid for the Rack end time of colorado: "
                    + p_Rack->GetEndTimeColorado().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }
    }

    if (p_Rack->GetSepiaStation()) {
        // check for the end time of sepia value null or not
        if (p_Rack->GetEndTimeSepia().isNull()) {
            ErrorDescription += "DateTime format is null for the Rack end time of sepia: "
                    + p_Rack->GetEndTimeSepia().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }

        // check for the end time of sepia format valid or not
        if (!p_Rack->GetEndTimeSepia().isValid()) {
            ErrorDescription += "DateTime format is invalid for the Rack end time of sepia: "
                    + p_Rack->GetEndTimeSepia().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }
    }
    // compare the rack date time like colorado end time, Sepia end time and Station end time
    //CompareRackDataTime(p_Rack, CheckColoradoDateTime, VerifiedData, ErrorDescription);

}


/****************************************************************************/
/*!
 *  \brief  Compares the date time of the rack like Colorado endtime, Sepia
 *  end time, Station end time
 *
 *  \iparam p_Rack = Pointer to CRack
 *  \iparam CheckColoradoDateTime = Check colorado date time flag value
 *  \iparam VerifiedData = verifier flag value
 *  \iparam ErrorDescription = Description of the error
 *
 */
/****************************************************************************/
void CRackListVerifier::CompareRackDataTime(CRack* p_Rack, bool CheckColoradoDateTime, bool& VerifiedData, QString& ErrorDescription)
{
#if 0
    // first check all the end times with start time of the rack.
    // Because end time cannot be less than start time
    //if (!CompareDateTime(p_Rack->GetStartTime(), p_Rack->GetEndTimeStation())) {
    if (p_Rack->GetStartTime().toTime_t() > p_Rack->GetEndTimeStation().toTime_t()) {
        ErrorDescription += "End time cannot be less than the start time: "
                + p_Rack->GetStartTime().toString("'M'M'd'd'y'yyyyhh:mm:ss") + " End time : "
                + p_Rack->GetEndTimeStation().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
        VerifiedData = false;
    }

    if (CheckColoradoDateTime) {
        // check the end time of colorado with start time of the rack
        //if (!CompareDateTime(p_Rack->GetStartTime(), p_Rack->GetEndTimeColorado())) {
        if (p_Rack->GetStartTime().toTime_t() > p_Rack->GetEndTimeColorado().toTime_t()) {
            ErrorDescription += "Colorado end time cannot be less than the start time: "
                    + p_Rack->GetStartTime().toString("'M'M'd'd'y'yyyyhh:mm:ss") + " End time : "
                    + p_Rack->GetEndTimeColorado().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }
    }

    // check the end time of sepia with start time of the rack
    if (p_Rack->GetSepiaStation()) {
        //if (!CompareDateTime(p_Rack->GetStartTime(), p_Rack->GetEndTimeSepia())) {
        if (p_Rack->GetStartTime().toTime_t() > p_Rack->GetEndTimeSepia().toTime_t()) {
            ErrorDescription += "Sepia end time cannot be less than the start time: "
                    + p_Rack->GetStartTime().toString("'M'M'd'd'y'yyyyhh:mm:ss") + " End time : "
                    + p_Rack->GetEndTimeSepia().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }
    }

    // first check all the end times of the Colorado and Sepia with end time of the rack.
    // Because end time cannot be less than end of the Colorado and Sepia
    // check the end time of colorado with start time of the rack
    if (CheckColoradoDateTime) {
        //if (!CompareDateTime(p_Rack->GetEndTimeStation(), p_Rack->GetEndTimeColorado())) {
        if (p_Rack->GetEndTimeStation().toTime_t() > p_Rack->GetEndTimeColorado().toTime_t()) {
            ErrorDescription += "Colorado end time cannot be less than the end time of the station: "
                    + p_Rack->GetEndTimeStation().toString("'M'M'd'd'y'yyyyhh:mm:ss")
                    + " End time of Colorado : " + p_Rack->GetEndTimeColorado().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }
    }

    // check the end time of sepia with start time of the rack
    if (p_Rack->GetSepiaStation()) {
        //if (!CompareDateTime(p_Rack->GetEndTimeStation(), p_Rack->GetEndTimeSepia())) {
        if (p_Rack->GetEndTimeStation().toTime_t() > p_Rack->GetEndTimeSepia().toTime_t()) {
            ErrorDescription += "Sepia end time cannot be less than the end time of the station: "
                    + p_Rack->GetEndTimeStation().toString("'M'M'd'd'y'yyyyhh:mm:ss")
                    + " End time of Sepia : " + p_Rack->GetEndTimeSepia().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }
    }

    // check the end time of Sepia with end time of colorado
    if (p_Rack->GetSepiaStation() && CheckColoradoDateTime) {
        //if (!CompareDateTime(p_Rack->GetEndTimeColorado(), p_Rack->GetEndTimeSepia())) {
        if (p_Rack->GetEndTimeColorado().toTime_t() > p_Rack->GetEndTimeSepia().toTime_t()) {
            ErrorDescription += "Sepia end time cannot be less than the end time of the Colorado: "
                    + p_Rack->GetEndTimeColorado().toString("'M'M'd'd'y'yyyyhh:mm:ss")
                    + " End time of Sepia : " + p_Rack->GetEndTimeSepia().toString("'M'M'd'd'y'yyyyhh:mm:ss") + "\n";
            VerifiedData = false;
        }
    }
#endif
}

/****************************************************************************/
/*!
 *  \brief  Resets the read/write lock of a list
 *
 *  \iparam Source = Source date time configuration
 *  \iparam Destination = Destination date time configuration
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CRackListVerifier::CompareDateTime(QDateTime Source, QDateTime Destination)
{    
    // check date first
    if (Destination.date().day() <= Source.date().day()) {
        if (Destination.date().dayOfWeek() <= Source.date().dayOfWeek()) {
            if (Destination.date().dayOfYear() <= Source.date().dayOfYear()) {
                if (Destination.time().hour() <= Source.time().hour()) {
                    if (Destination.time().minute() <= Source.time().minute()) {
                        if (Destination.time().second() < Source.time().second()) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CRackListVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CRackListVerifier::ResetLastErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
bool CRackListVerifier::IsLocalVerifier()
{
    return true;
}

}  // namespace DataManager

