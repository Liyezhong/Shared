R.Wobst, @(#) Feb 10 2011, 10:37:08

APPLICATION INTERFACE OF XmlChecker
===================================

1. Purpose
----------

The XmlChecker class can parse certain XML documents and check their
structure against restrictions given in the parameter of the constructor
call. The result are trees with simple element structure. The XML
documents must fulfil certain restrictions:

- There are no optional elements.
- The order of elements and their tagnames are fixed.
- There are no tail components, e.g. each text element is put between an
  opening and its closing tag.
- There is exactly one attribute, namely the format version given in the
  root tag (attribute "version").
- Only child liss may be repeated; their number may be limited by
  constraints (especially, exactly one child tree can be required).
- The name of the found root tag and the format version determine the
  expected XML format completely.

The definition of descriptions for several XML file types can be found in
XMLdescr.txt.


2. Constructor
--------------

A class instance is created by the constructor call as follows:

XmlChecker xchecker = XmlChecker(const RawEntryLst& descr);

Here "descr" is the description of one or more required XML structures:

typedef struct
{
    QString tagname;
    CheckType kind;
    QString childDesc;
} ParseDescElement;

typedef QList<ParseDescElement>         RawEntryLst;

The meaning of the structure components is explained in 4. below.


3. Using the parser
-------------------

The default usage is of the form

ResultTree parseFile(const QString& xmlFilename,
		     int *version = 0,
		     QString *pRootTag = 0);

Here "version" is the numeric format version of the document and
pRootTag the tagname of the found root tag. The XML file is parsed with
the QDom method and checked against the description dependent on root
tag name and format version. It is also possible to provide an already
parsed document:

ResultTree parseQdocument(const QDomDocument& qdoc,
			 int *version = 0,
			 QString *pRootTag = 0);

In case of some error, an exception is thrown (cf. 5.). The message
tries to describe the location of the error as precise as possible
(comparable with an XPath localization), e.g.:

illegal time format: 1d 12x 30m 15s

*** tag hierarchy:
top: reagentlist
  reagent: tag no.1
    maxtime: tag no.4



4. Using the result
-------------------

The ResultTree object returned by the parser is a QList of QPairs

QPair<QString, QVariant>

where the first component is the tag name and the second is either
integer, string, boolean value or a ResultTree Object itself.

Access is provided by iterators (QListIterator and QMutableListIterator)
or directly to elements by member function Entries().

Usage pattern for ResultIterator:

printResultTree(ResultIterator iter)
{
    ResultElement elem;

    while(iter.hasNext())
    {
        elem = iter.next();
        ... print elem.first ... (the tagname)

        if(elem.second.userType() == ResultTree::m_userType)
        {
            ResultTree child = elem.second.value<ResultTree>();
            printResultTree(child.iterator());
        }
        else
        {
            ... print elem.second ... (the value)
        }
    }
}


5. Exceptions
-------------

In case of error during the translation of the description to internal
format or during XML parsing, an DefaultError() instance is thrown. Its
public attribute msg contains the error message. Further extensions
(like adding an error ID) are to be done.

A list of detectable error can be found in 8.


6. Structure of RawEntryLst
---------------------------

An RawEntryLst object is a QList of structure types ParseDescElement:

typedef struct
{
    QString tagname;
    CheckType kind;
    QString childDesc;
} ParseDescElement;

This QList is a concatenation of sublists, each starting with an element
where the "kind" component is either TOP_LIST or CHILD_LIST, and ending with
an element where the "kind" component is LAST_ENTRY. All other elements
describe a tag and its text element, or a sublist.

Meaning of first sublist elements:

- tagname: name of root tag for TOP_LIST, or symbolic name of sublist
  for CHILD_LIST (this symbolic can be chosen freely but must be
  unique).

- kind: Is either TOP_LIST or CHILD_LIST.

- childDesc: the format version (must be an integer).

Meaning of all other elements but the last:

- tagname: the tagname

- kind: Describes of which kind must be the text element.

  - STRING: The value is some string;

  - INT: an integer,

  - TIME: an expression of the form "1d 13h 47m 12s" where at least one
    of the "d,h,m,s"-parts must occur, and this order must be kept. Times
    are converted to seconds (integers);

  - MAX_TIME: an expression of the form "10%" where the numeric value
    can be 0, 10, 20, 25, 100 or INF (defined in
    XmlChecker::checkMaxTime, definition of "values") - this means the
    percentage added to the TIME value before as an upper bound (INF
    stands for infinity);

    Internally, the computed absolute time is stored after parsing.
    It is used that MAX_TIME occurs only in the tag maxDuration which
    always follow minDuration of type TIME.

  - BOOL: must be "true", "false" (case insensitive) or "0" or "1";

  - CHILD: a child list is expected here.

- childDesc:

  - For CHILD elements, this is the symbolic name of the child list
    (used as tagname component in the child list's first element).
    After this name, separated by one blank, a constraint for the
    number of childs may follow (cf. also the next item):

        {"steps", CHILD, "one_program_step 2 ... 4"}

    (i.e., the child list with symbolic name "one_program_step" must
    occur 2, 3 or 4 times).

    In Particular, the lower bound can be 0, thus a subtree can be
    optional.

  - For all other elements, describing the constraints for the text
    part:

    - either empty if no constraints are defined,

    - or a list of admissible values (which must NOT contain spaces!)
      for STRING elements, like: 
        {"color", STRING, "pink yellow white"},

    - or a range of admissible string lengths for STRING elements in the
      form:
        {"shortname", STRING, "1 ... 4"}
      (this form also applies to the number of children for CHILD
      elements above) where blanks before and after "..." are essential,

    - or a range for INT/TIME values in the same form:
        {"maxcycles", INT, "1 ... 400"}

    - or a list of admissible values for INT values:
        {"class", INT, "0 1 2"}

An example for test purposes can be found in Test/CtrlLsts.cpp.


7. Unit tests
-------------

Unittests are bases on QtTest and are split up into two classes:

- positive tests (test function testXMLok()), where three sample files
  in xmlOkFiles/ are parsed and the output is compared with the expected; 

- negative tests (test function testXMLerrs()), where a bunch of
  erroneus files in xmlErrFiles/ is parsed and the error message is
  compared against the expected output.

Moreover, there is still one simple test slot testXMLokFmt() which
checks the correct format version.

Test generation:

To generate the test program data, in each of both mentioned directories
an executable "gentest" must be build first. Then start the Python
script create_data.py (in Test directory) with arguments xmlok.cfg resp.
xmlerr.cfg which output C++ files on stdout. The outputs for both
arguments have to be redirected to TestXMLParserOkData.cpp resp.
TestXMLParserErrData.cpp. Both generated C++ files could then be formatted
(e.g., by vim in a suitable mode). Both files should be checked manually
(this is necessary when the output format has changed).

All building, linking, test data generation and vim-formatting can be
done by the shell script mkalltest called with an arbitrary argument
(without argument: no test data generation happens, only a global
build). This script uses the command "vicc" which starts a suitably
configured vim - other users have to replace this command by their own.
Moreover, the shell scripts "," and ",," are used which start the
qmake/make process resp. cleaning procedure.

Creating new tests:

To create a new positive test, put the XML file to directory xmlOkFiles
and add a new entry in xmlok.cfg in the Test directory (format is
described in this file). For negative tests, you can use the shell
script xmlErrFiles/new_tests which copies xmlOkFiles/Reagents.xml to
xmlErrFiles under a chosen name and allows editing it. After this, a new
entry in xmlerr.cfg must be made.

8. Detected errors
------------------

The following errors are detected:

- Parse the XML description:

  - empty sublist/toplist detected (only start and end tag found)
  - illegal childDesc component for top of child list
  - child list with this name not found
  - illegal value for "kind" type in description
  - value is not an integer

- Parse a file resp. a QDomDocument:

  - can't open XML file
  - XML error during QDom parse
  - no version attribute found
  - empty value for version attribute
  - value of version attribute is not an integer
  - no description found for this root tag
  - no description found for this root tag and this format version
  - missing child list
  - unexpected child found
  - other tagname expected
  - too many child trees
  - too few child trees
  - string length not within constraints
  - string not within set of given values
  - integer out of range
  - integer not within set of given values
  - text element is not an integer
  - illegal TIME/MAX_TIME format
  - illegal value for bool

Error codes are defined in Include/XMLParserEventCodes.h; the string for
output is found after "///<" with %n markers (n=1,2,...) as placeholders
for variable parts. A marker "#@+" denotes the beginning of the long
string.
