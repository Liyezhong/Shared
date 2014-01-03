/* $Id: AeXML.c,v 1.8 2008/05/21 18:24:35 dkhodos Exp $ */

#include "Ae.h"

static void AeXMLCharDataWrite(AeXMLContent *pContent, AeArray *pArray);
static AeInt32 AeXMLCharDataGetFormattedSize(AeXMLContent *pContent);
static void AeXMLAttributeWrite(AeXMLAttribute *pAttribute, AeArray *pArray);
static AeInt32 AeXMLAttributeGetFormattedSize(AeXMLAttribute *pAttribute);
static void AeXMLElementWrite(AeXMLContent *pContent, AeArray *pArray);
static AeInt32 AeXMLElementGetFormattedSize(AeXMLContent *pContent);

static void AeXMLStartElementHandler(void *pUserData, const XML_Char *pName, const XML_Char **ppAttrs);
static void AeXMLEndElementHandler(void *pUserData, const XML_Char *pName);
static void AeXMLCharacterDataHandler(void *pUserData, const XML_Char *pData, int iLength);

typedef struct _AeXMLParseContext AeXMLParseContext;

struct _AeXMLParseContext
{
    AeXMLDocument   *pDocument;
    AeXMLElement    *pCurElement;
};

AeXMLContentVTable g_abXMLCharDataVTable =
{
    AeXMLCharDataDestroy,
    AeXMLCharDataWrite,
    AeXMLCharDataGetFormattedSize
};

AeXMLContentVTable g_abXMLElementVTable =
{
    AeXMLElementDestroy,
    AeXMLElementWrite,
    AeXMLElementGetFormattedSize
};

/******************************************************************************/
AeXMLCharData *AeXMLCharDataNew(void)
{
    AeXMLCharData *pCharData;

    pCharData = AeAlloc(sizeof(AeXMLCharData));
    if (!pCharData)
        return NULL;
    memset(pCharData, 0, sizeof(AeXMLCharData));

    pCharData->content.pVTable = &g_abXMLCharDataVTable;
    pCharData->content.bElement = AeFalse;
    pCharData->content.pParent = NULL;
    pCharData->content.pNext = NULL;
    pCharData->pData = NULL;

    return pCharData;
}

/******************************************************************************/
void AeXMLCharDataDestroy(AeXMLContent *pContent)
{
    AeXMLCharData *pCharData;

    pCharData = (AeXMLCharData *) pContent;

    if (pCharData->pData)
        AeFree(pCharData->pData);

    AeFree(pCharData);
}

/******************************************************************************/
static void AeXMLCharDataWrite(AeXMLContent *pContent, AeArray *pArray)
{
    AeXMLCharData *pCharData;

    pCharData = (AeXMLCharData *) pContent;
    if (pCharData->pData)
        AeArrayAppend(pArray, pCharData->pData, strlen(pCharData->pData));
}

/******************************************************************************/
static AeInt32 AeXMLCharDataGetFormattedSize(AeXMLContent *pContent)
{
    AeXMLCharData *pCharData;

    pCharData = (AeXMLCharData *) pContent;
    if (pCharData->pData)
        return strlen(pCharData->pData);
    
    return 0;
}

/******************************************************************************/
AeXMLAttribute *AeXMLAttributeNew(void)
{
    AeXMLAttribute *pAttribute;

    pAttribute = AeAlloc(sizeof(AeXMLAttribute));
    if (!pAttribute)
        return NULL;
    memset(pAttribute, 0, sizeof(AeXMLAttribute));

    pAttribute->pName = NULL;
    pAttribute->pValue = NULL;
    pAttribute->pNext = NULL;

    return pAttribute;
}

/******************************************************************************/
void AeXMLAttributeDestroy(AeXMLAttribute *pAttribute)
{
    if (pAttribute->pName)
        AeFree(pAttribute->pName);
    if (pAttribute->pValue)
        AeFree(pAttribute->pValue);

    AeFree(pAttribute);
}

/******************************************************************************/
void AeXMLAttributeSetValue(AeXMLAttribute *pAttribute, AeChar *pValue)
{
    AeArray *pTmp;

    pTmp = AeArrayNew(1);
    if (!pTmp)
    {
        AeSetString(&pAttribute->pValue, NULL, -1);
        return;
    }

    /* entitize attribute value */
    AeXMLEntitize(pValue, pTmp);

    /* truncate long attribute value */
    if (AeArrayCount(pTmp) > AE_XML_MAX_ATTR_SIZE)
        AeArraySet(pTmp, AE_XML_MAX_ATTR_SIZE, "\0", 1);

    AeSetString(&pAttribute->pValue, AeArrayGet(pTmp, 0), -1);

    AeArrayDestroy(pTmp);
}

/******************************************************************************/
static void AeXMLAttributeWrite(AeXMLAttribute *pAttribute, AeArray *pArray)
{
    if (!pAttribute->pName || !pAttribute->pValue)
        return;

    AeArrayAppend(pArray, " ", 1);
    AeArrayAppend(pArray, pAttribute->pName, strlen(pAttribute->pName));
    AeArrayAppend(pArray, "=\"", 2);
    AeArrayAppend(pArray, pAttribute->pValue, strlen(pAttribute->pValue));
    AeArrayAppend(pArray, "\"", 1);
}

/******************************************************************************/
static AeInt32 AeXMLAttributeGetFormattedSize(AeXMLAttribute *pAttribute)
{
    if (!pAttribute->pName || !pAttribute->pValue)
        return 0;

    /* add space for name, value, whitespace, equals sign and two quotes */
    return (strlen(pAttribute->pName) + strlen(pAttribute->pValue) + 4);
}

/******************************************************************************/
AeXMLElement *AeXMLElementNew(void)
{
    AeXMLElement *pElement;

    pElement = AeAlloc(sizeof(AeXMLElement));
    if (!pElement)
        return NULL;
    memset(pElement, 0, sizeof(AeXMLElement));

    pElement->content.pVTable = &g_abXMLElementVTable;
    pElement->content.bElement = AeTrue;
    pElement->content.pParent = NULL;
    pElement->content.pNext = NULL;
    pElement->pName = NULL;
    pElement->pFirstAttribute = NULL;
    pElement->pLastAttribute = NULL;
    pElement->pFirstChild = NULL;
    pElement->pLastChild = NULL;
    pElement->bEmpty = AeFalse;

    return pElement;
}

/******************************************************************************/
void AeXMLElementDestroy(AeXMLContent *pContent)
{
    AeXMLElement *pElement;
    AeXMLAttribute *pAttribute, *pAttrTmp;
    AeXMLContent *pChild, *pChildTmp;

    pElement = (AeXMLElement *) pContent;

    pAttribute = pElement->pFirstAttribute;
    while (pAttribute)
    {
        pAttrTmp = pAttribute;
        pAttribute = pAttribute->pNext;
        AeXMLAttributeDestroy(pAttrTmp);
    }

    pChild = pElement->pFirstChild;
    while (pChild)
    {
        pChildTmp = pChild;
        pChild = pChild->pNext;
        AeXMLContentDestroy(pChildTmp);
    }

    if (pElement->pName)
        AeFree(pElement->pName);

    AeFree(pElement);
}

/******************************************************************************/
AeXMLAttribute *AeXMLElementAddAttribute(AeXMLElement *pElement, AeChar *pName, AeChar *pValue)
{
    AeXMLAttribute *pAttribute;

    if (!pName || !pValue)
        return NULL;

    pAttribute = AeXMLAttributeNew();
    if (!pAttribute)
        return NULL;

    AeXMLAttributeSetName(pAttribute, pName);
    AeXMLAttributeSetValue(pAttribute, pValue);

    if (!pElement->pLastAttribute)
        pElement->pFirstAttribute = pAttribute;
    else
        pElement->pLastAttribute->pNext = pAttribute;
    pElement->pLastAttribute = pAttribute;

    return pAttribute;
}

/******************************************************************************/
AeXMLElement *AeXMLElementAddElement(AeXMLElement *pElement, AeChar *pName, AeBool bEmpty)
{
    AeXMLElement *pNewElement;

    pNewElement = AeXMLElementNew();
    if (!pNewElement)
        return NULL;

    AeXMLElementSetName(pNewElement, pName);
    AeXMLElementSetEmpty(pNewElement, bEmpty);

    AeXMLElementAddContent(pElement, (AeXMLContent *) pNewElement);

    return pNewElement;
}

/******************************************************************************/
AeXMLCharData *AeXMLElementAddCharData(AeXMLElement *pElement, AeChar *pData, AeInt32 iLength)
{
    AeXMLCharData *pCharData;

    if (iLength == 0 || !pData)
        return NULL;

    pCharData = AeXMLCharDataNew();
    if (!pCharData)
        return NULL;

    AeXMLCharDataSetData(pCharData, pData, iLength);

    AeXMLElementAddContent(pElement, (AeXMLContent *) pCharData);

    return pCharData;
}

/******************************************************************************/
void AeXMLElementAddContent(AeXMLElement *pElement, AeXMLContent *pContent)
{
    pContent->pParent = pElement;

    if (!pElement->pLastChild)
        pElement->pFirstChild = pContent;
    else
        pElement->pLastChild->pNext = pContent;
    pElement->pLastChild = pContent;
}

/******************************************************************************/
static void AeXMLElementWrite(AeXMLContent *pContent, AeArray *pArray)
{
    AeXMLElement *pElement;
    AeXMLAttribute *pAttribute;
    AeXMLContent *pChild;

    pElement = (AeXMLElement *) pContent;

    if (!pElement->pName)
        return;

    AeArrayAppend(pArray, "<", 1);
    AeArrayAppend(pArray, pElement->pName, strlen(pElement->pName));

    /* write all attributes */
    pAttribute = pElement->pFirstAttribute;
    while (pAttribute)
    {
        AeXMLAttributeWrite(pAttribute, pArray);
        pAttribute = pAttribute->pNext;
    }

    if (!pElement->bEmpty)
    {
        AeArrayAppend(pArray, ">", 1);

        /* write all content entries (recursively) */
        pChild = pElement->pFirstChild;
        while (pChild)
        {
            AeXMLContentWrite(pChild, pArray);
            pChild = pChild->pNext;
        }

        AeArrayAppend(pArray, "</", 2);
        AeArrayAppend(pArray, pElement->pName, strlen(pElement->pName));
        AeArrayAppend(pArray, ">", 1);
    }
    else
        AeArrayAppend(pArray, "/>", 2);
}

/******************************************************************************/
static AeInt32 AeXMLElementGetFormattedSize(AeXMLContent *pContent)
{
    AeXMLElement *pElement;
    AeXMLAttribute *pAttribute;
    AeXMLContent *pChild;
    AeInt32 iSize;

    pElement = (AeXMLElement *) pContent;

    if (!pElement->pName)
        return 0;

    /* add space for name and opening angle bracket */
    iSize = strlen(pElement->pName) + 1;

    /* add space for all attributes */
    pAttribute = pElement->pFirstAttribute;
    while (pAttribute)
    {
        iSize += AeXMLAttributeGetFormattedSize(pAttribute);
        pAttribute = pAttribute->pNext;
    }

    if (!pElement->bEmpty)
    {
        /* add space for closing angle bracket */
        iSize += 1;

        /* add space for all content entries (recursively) */
        pChild = pElement->pFirstChild;
        while (pChild)
        {
            iSize += AeXMLContentGetFormattedSize(pChild);
            pChild = pChild->pNext;
        }

        /* add space for name, opening and closing angle brackets and slash */
        iSize += strlen(pElement->pName) + 3;
    }
    else
        /* add space for slash and closing angle bracket */
        iSize += 2;

    return iSize;
}

/******************************************************************************/
AeXMLDocument *AeXMLDocumentNew(void)
{
    AeXMLDocument *pDocument;

    pDocument = AeAlloc(sizeof(AeXMLDocument));
    if (!pDocument)
        return NULL;
    memset(pDocument, 0, sizeof(AeXMLDocument));

    pDocument->pRoot = NULL;

    return pDocument;
}

/******************************************************************************/
AeBool AeXMLDocumentParse(AeXMLDocument *pDocument, AeChar *pSource, AeInt iLength)
{
    XML_Parser parser;
    AeXMLParseContext context;
    AeInt iOK;

    parser = XML_ParserCreate(NULL);
    if (!parser)
        return AeFalse;

    context.pDocument = pDocument;
    context.pCurElement = NULL;
    XML_SetUserData(parser, &context);

    XML_SetElementHandler(parser, AeXMLStartElementHandler, AeXMLEndElementHandler);
    XML_SetCharacterDataHandler(parser, AeXMLCharacterDataHandler);

    iOK = XML_Parse(parser, pSource, iLength, 1);

    XML_ParserFree(parser);

    return (iOK ? AeTrue : AeFalse);
}

/******************************************************************************/
void AeXMLDocumentDestroy(AeXMLDocument *pDocument)
{
    if (pDocument->pRoot)
        AeXMLElementDestroy((AeXMLContent *) pDocument->pRoot);

    AeFree(pDocument);
}

/******************************************************************************/
void AeXMLDocumentWrite(AeXMLDocument *pDocument, AeArray *pArray)
{
    AeArrayAppend(pArray, AE_XML_PROLOG, strlen(AE_XML_PROLOG));
    AeXMLElementWrite((AeXMLContent *) pDocument->pRoot, pArray);
}

/******************************************************************************/
AeInt32 AeXMLDocumentGetFormattedSize(AeXMLDocument *pDocument)
{
    return (strlen(AE_XML_PROLOG) +
        AeXMLElementGetFormattedSize((AeXMLContent *) pDocument->pRoot));
}

/******************************************************************************/
static void AeXMLStartElementHandler(void *pUserData, const XML_Char *pName, const XML_Char **ppAttrs)
{
    AeXMLParseContext *pContext;
    AeXMLElement *pElement;
    XML_Char **ppCurAttr;

    pContext = (AeXMLParseContext *) pUserData;

    pElement = AeXMLElementNew();
    if (!pElement)
        return;

    pElement->content.pParent = pContext->pCurElement;

    AeXMLElementSetName(pElement, (AeChar *) pName);

    ppCurAttr = (XML_Char **) ppAttrs;
    while (*ppCurAttr)
    {
        AeXMLElementAddAttribute(pElement, ppCurAttr[0], ppCurAttr[1]);
        ppCurAttr += 2;
    }

    if (!pContext->pCurElement)
        pContext->pDocument->pRoot = pElement;
    else
        AeXMLElementAddContent(pContext->pCurElement, (AeXMLContent *) pElement);

    pContext->pCurElement = pElement;
}

/******************************************************************************/
static void AeXMLEndElementHandler(void *pUserData, const XML_Char *pName)
{
    AeXMLParseContext *pContext;

    pContext = (AeXMLParseContext *) pUserData;

    if (!pContext->pCurElement || strcasecmp(pContext->pCurElement->pName, pName))
        return;

    if (!pContext->pCurElement->pFirstChild)
        AeXMLElementSetEmpty(pContext->pCurElement, AeTrue);

    pContext->pCurElement = pContext->pCurElement->content.pParent;
}

/******************************************************************************/
static void AeXMLCharacterDataHandler(void *pUserData, const XML_Char *pData, int iLength)
{
    AeXMLParseContext *pContext;
    AeXMLCharData *pCharData;

    pContext = (AeXMLParseContext *) pUserData;

    if (!pContext->pCurElement)
        return;

    /* remove leading whitespace */
    if (!pContext->pCurElement->pLastChild ||
        pContext->pCurElement->pLastChild->bElement)
    {
        while (iLength > 0)
        {
            if (*pData == ' ' || *pData == '\t' || *pData == '\n')
            {
                pData += 1;
                iLength -= 1;
            }
            else
                break;
        }

        if (iLength <= 0)
            return;
    }

    pCharData = AeXMLCharDataNew();
    if (!pCharData)
        return;

    pCharData->content.pParent = pContext->pCurElement;

    AeXMLCharDataSetData(pCharData, (AeChar *) pData, iLength);

    AeXMLElementAddContent(pContext->pCurElement, (AeXMLContent *) pCharData);
}

/******************************************************************************/
void AeXMLEntitize(AeChar *pInput, AeArray *pOutput)
{
    AeChar *pCh;

    pCh = pInput;
    while (*pCh)
    {
        switch (*pCh)
        {
            case '&':
                AeArrayAppend(pOutput, AE_XML_ENTITY_AMP, strlen(AE_XML_ENTITY_AMP));
                break;
            case '<':
                AeArrayAppend(pOutput, AE_XML_ENTITY_LT, strlen(AE_XML_ENTITY_LT));
                break;
            case '>':
                AeArrayAppend(pOutput, AE_XML_ENTITY_GT, strlen(AE_XML_ENTITY_GT));
                break;
            case '\'':
                AeArrayAppend(pOutput, AE_XML_ENTITY_APOS, strlen(AE_XML_ENTITY_APOS));
                break;
            case '\"':
                AeArrayAppend(pOutput, AE_XML_ENTITY_QUOT, strlen(AE_XML_ENTITY_QUOT));
                break;
            default:
                AeArrayAppend(pOutput, pCh, 1);
                break;
        }

        pCh += 1;
    }
}
