#ifndef MANTID_MANTIDWIDGETS_PARSEKEYVALUESTRINGTEST_H
#define MANTID_MANTIDWIDGETS_PARSEKEYVALUESTRINGTEST_H

#include "MantidQtWidgets/Common/ParseKeyValueString.h"
#include <cxxtest/TestSuite.h>

using MantidQt::MantidWidgets::parseKeyValueString;
using MantidQt::MantidWidgets::parseKeyValueQString;
using MantidQt::MantidWidgets::parseKeyValueMap;

class ParseKeyValueStringTest : public CxxTest::TestSuite {

public:
  void testParseKeyValueString() {
    std::map<std::string, std::string> kvp = parseKeyValueString(
        "a = 1,b=2.0, c=3, d='1,2,3',e=\"4,5,6\",f=1+1=2, g = '\\''");

    TS_ASSERT_EQUALS(kvp["a"], "1");
    TS_ASSERT_EQUALS(kvp["b"], "2.0");
    TS_ASSERT_EQUALS(kvp["c"], "3");
    TS_ASSERT_EQUALS(kvp["d"], "1,2,3");
    TS_ASSERT_EQUALS(kvp["e"], "4,5,6");
    TS_ASSERT_EQUALS(kvp["f"], "1+1=2");
    TS_ASSERT_EQUALS(kvp["g"], "'");

    TS_ASSERT_THROWS(parseKeyValueString("a = 1, b = 2, c = 3,"),
                     std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueString("a = 1, b = 2, c = 3,d"),
                     std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueString(",a = 1"), std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueString(",a = 1 = 2,="), std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueString("=,=,="), std::runtime_error);
  }

  void testParseKeyValueQString() {
    std::map<QString, QString> kvp = parseKeyValueQString(
        "a = 1,b=2.0, c=3, d='1,2,3',e=\"4,5,6\",f=1+1=2, g = '\\''");

    TS_ASSERT_EQUALS(kvp["a"], "1");
    TS_ASSERT_EQUALS(kvp["b"], "2.0");
    TS_ASSERT_EQUALS(kvp["c"], "3");
    TS_ASSERT_EQUALS(kvp["d"], "1,2,3");
    TS_ASSERT_EQUALS(kvp["e"], "4,5,6");
    TS_ASSERT_EQUALS(kvp["f"], "1+1=2");
    TS_ASSERT_EQUALS(kvp["g"], "'");

    TS_ASSERT_THROWS(parseKeyValueQString("a = 1, b = 2, c = 3,"),
                     std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString("a = 1, b = 2, c = 3,d"),
                     std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString(",a = 1"), std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString(",a = 1 = 2,="), std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString("=,=,="), std::runtime_error);
  }

  void testParseKeyValueMap() {
    std::map<QString, QString> inputMap = {{"a", "1"},
                                           {"b", "2.0"},
                                           {"c", "3"},
                                           {"d", "'1,2,3'"},
                                           {"e", "\"4,5,6\""},
                                           {"f", "1+1=2"},
                                           {"g", "'\\''"}};
    std::map<QString, QString> kvp = parseKeyValueMap(inputMap);

    TS_ASSERT_EQUALS(kvp["a"].toStdString(), "1");
    TS_ASSERT_EQUALS(kvp["b"].toStdString(), "2.0");
    TS_ASSERT_EQUALS(kvp["c"].toStdString(), "3");
    TS_ASSERT_EQUALS(kvp["d"].toStdString(), "1,2,3");
    TS_ASSERT_EQUALS(kvp["e"].toStdString(), "4,5,6");
    TS_ASSERT_EQUALS(kvp["f"].toStdString(), "1+1=2");
    TS_ASSERT_EQUALS(kvp["g"].toStdString(), "'");

    TS_ASSERT_THROWS(parseKeyValueQString("a = 1, b = 2, c = 3,"),
                     std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString("a = 1, b = 2, c = 3,d"),
                     std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString(",a = 1"), std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString(",a = 1 = 2,="), std::runtime_error);
    TS_ASSERT_THROWS(parseKeyValueQString("=,=,="), std::runtime_error);
  }
};

#endif // MANTID_MANTIDWIDGETS_PARSEKEYVALUESTRINGTEST_H
