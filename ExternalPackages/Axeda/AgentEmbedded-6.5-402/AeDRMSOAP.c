/* $Id: AeDRMSOAP.c,v 1.3 2004/03/02 17:43:51 dkhodos Exp $ */

#include "Ae.h"

struct _AeDRMSOAP
{
    AeXMLDocument   *pDocument;
};

static AeHandle  AeDRMSOAPFindNextMethod(AeXMLContent *pContent);
static AeHandle  AeDRMSOAPFindNextParameter(AeXMLContent *pContent);

/******************************************************************************/
AeDRMSOAP *AeDRMSOAPNew(void)
{
    AeDRMSOAP *pSOAP;
    
    pSOAP = (AeDRMSOAP *) AeAlloc(sizeof(AeDRMSOAP));
    if (!pSOAP)
        return NULL;
    memset(pSOAP, 0, sizeof(AeDRMSOAP));

    pSOAP->pDocument = AeXMLDocumentNew();
    if (!pSOAP->pDocument)
        return NULL;

    return pSOAP;
}

/******************************************************************************/
void AeDRMSOAPDestroy(AeDRMSOAP *pSOAP)
{
    if (pSOAP->pDocument)
        AeXMLDocumentDestroy(pSOAP->pDocument);

    AeFree(pSOAP);
}

/******************************************************************************/
AeBool AeDRMSOAPParse(AeDRMSOAP *pSOAP, AeChar *pSource, AeInt iLength)
{
    return AeXMLDocumentParse(pSOAP->pDocument, pSource, iLength);
}

/******************************************************************************/
AeHandle AeDRMSOAPGetFirstMethod(AeDRMSOAP *pSOAP)
{
    /* root element must be "SoapResponseBundle" */
    if (!pSOAP->pDocument->pRoot ||
        strcmp(pSOAP->pDocument->pRoot->pName, "SoapResponseBundle"))
        return NULL;

    return AeDRMSOAPFindNextMethod(pSOAP->pDocument->pRoot->pFirstChild);
}

/******************************************************************************/
AeHandle AeDRMSOAPGetNextMethod(AeHandle pMethod)
{
    return AeDRMSOAPFindNextMethod(((AeXMLElement *) pMethod)->content.pParent->content.pParent->content.pNext);
}

/******************************************************************************/
AeHandle AeDRMSOAPGetMethodByName(AeDRMSOAP *pSOAP, AeChar *pName)
{
    AeHandle pMethod;

    pMethod = AeDRMSOAPGetFirstMethod(pSOAP);
    while (pMethod)
    {
        if (!strcmp(((AeXMLElement *) pMethod)->pName, pName))
            return pMethod;

        pMethod = AeDRMSOAPGetNextMethod(pMethod);
    }

    return NULL;
}

/******************************************************************************/
AeChar *AeDRMSOAPGetMethodName(AeHandle pMethod)
{
    return ((AeXMLElement *) pMethod)->pName;
}

/******************************************************************************/
AeHandle AeDRMSOAPGetFirstParameter(AeHandle pMethod)
{
    return AeDRMSOAPFindNextParameter(((AeXMLElement *) pMethod)->pFirstChild);
}

/******************************************************************************/
AeHandle AeDRMSOAPGetNextParameter(AeHandle pParameter)
{
    return AeDRMSOAPFindNextParameter(((AeXMLElement *) pParameter)->content.pNext);
}

/******************************************************************************/
AeHandle AeDRMSOAPGetParameterByName(AeHandle pMethod, AeChar *pName)
{
    AeHandle pParameter;

    pParameter = AeDRMSOAPGetFirstParameter(pMethod);
    while (pParameter)
    {
        if (!strcmp(((AeXMLElement *) pParameter)->pName, pName))
            return pParameter;

        pParameter = AeDRMSOAPGetNextParameter(pParameter);
    }

    return NULL;
}

/******************************************************************************/
AeHandle AeDRMSOAPGetParameterFirstChild(AeHandle pParameter)
{
    AeXMLContent *pPtr;

    pPtr = ((AeXMLElement *) pParameter)->pFirstChild;
    while (pPtr)
    {
        if (pPtr->bElement)
            return pPtr;

        pPtr = pPtr->pNext;
    }

    return NULL;
}

/******************************************************************************/
AeChar *AeDRMSOAPGetParameterName(AeHandle pParameter)
{
    return ((AeXMLElement *) pParameter)->pName;
}

/******************************************************************************/
AeChar *AeDRMSOAPGetParameterValue(AeHandle pParameter)
{
    AeXMLContent *pPtr;

    pPtr = ((AeXMLElement *) pParameter)->pFirstChild;
    while (pPtr)
    {
        if (!pPtr->bElement)
            return ((AeXMLCharData *) pPtr)->pData;

        pPtr = pPtr->pNext;
    }

    return NULL;
}

/******************************************************************************/
AeChar *AeDRMSOAPGetParameterValueByName(AeHandle pMethod, AeChar *pName)
{
    AeHandle pParameter;

    pParameter = AeDRMSOAPGetParameterByName(pMethod, pName);
    if (pParameter)
        return AeDRMSOAPGetParameterValue(pParameter);

    return NULL;
}

/******************************************************************************/
AeHandle AeDRMSOAPGetFirstAttribute(AeHandle pNode)
{
    return (AeHandle) ((AeXMLElement *) pNode)->pFirstAttribute;
}

/******************************************************************************/
AeHandle AeDRMSOAPGetNextAttribute(AeHandle pAttribute)
{
    return (AeHandle) ((AeXMLAttribute *) pAttribute)->pNext;
}

/******************************************************************************/
AeHandle AeDRMSOAPGetAttributeByName(AeHandle pNode, AeChar *pName)
{
    AeXMLAttribute *pAttribute;

    pAttribute = ((AeXMLElement *) pNode)->pFirstAttribute;
    while (pAttribute)
    {
        if (!strcasecmp(pAttribute->pName, pName))
            return (AeHandle) pAttribute;

        pAttribute = pAttribute->pNext;
    }

    return NULL;
}

/******************************************************************************/
AeChar *AeDRMSOAPGetAttributeName(AeHandle pAttribute)
{
    return ((AeXMLAttribute *) pAttribute)->pName;
}

/******************************************************************************/
AeChar *AeDRMSOAPGetAttributeValue(AeHandle pAttribute)
{
    return ((AeXMLAttribute *) pAttribute)->pValue;
}

/******************************************************************************/
AeChar *AeDRMSOAPGetAttributeValueByName(AeHandle pNode, AeChar *pName)
{
    AeHandle *pAttribute;

    pAttribute = AeDRMSOAPGetAttributeByName(pNode, pName);
    if (pAttribute)
        return ((AeXMLAttribute *) pAttribute)->pValue;

    return NULL;
}

/******************************************************************************/
static AeHandle AeDRMSOAPFindNextMethod(AeXMLContent *pContent)
{
    AeXMLContent *pPtr1, *pPtr2, *pPtr3;

    /* search for an envelope */
    pPtr1 = pContent;
    while (pPtr1)
    {
        if (pPtr1->bElement &&
            !strcmp(((AeXMLElement *) pPtr1)->pName, "SOAP-ENV:Envelope"))
        {
            /* search for the body inside envelope */
            pPtr2 = ((AeXMLElement *) pPtr1)->pFirstChild;
            while (pPtr2)
            {
                if (pPtr2->bElement &&
                    !strcmp(((AeXMLElement *) pPtr2)->pName, "SOAP-ENV:Body"))
                {
                    pPtr3 = ((AeXMLElement *) pPtr2)->pFirstChild;
                    while (pPtr3)
                    {
                        if (pPtr3->bElement)
                            return (AeHandle) pPtr3;

                        pPtr3 = pPtr3->pNext;
                    }
                }

                pPtr2 = pPtr2->pNext;
            }
        }

        pPtr1 = pPtr1->pNext;
    }

    return NULL;
}

/******************************************************************************/
static AeHandle AeDRMSOAPFindNextParameter(AeXMLContent *pContent)
{
    AeXMLContent *pPtr;

    pPtr = pContent;
    while (pPtr)
    {
        if (pPtr->bElement)
            return (AeHandle) pPtr;

        pPtr = pPtr->pNext;
    }

    return NULL;
}
