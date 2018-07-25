#ifndef MANTID_DATAHANDLING_SAVEMFTTEST_H_
#define MANTID_DATAHANDLING_SAVEMFTTEST_H_

#include <cxxtest/TestSuite.h>
#include "MantidDataHandling/SaveMFT.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/Workspace.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidDataObjects/WorkspaceCreation.h"
#include <iterator>
#include <Poco/File.h>
#include <Poco/TemporaryFile.h>

using namespace Mantid::API;
using namespace Mantid::DataHandling;
using namespace Mantid::DataObjects;

class SaveMFTTest : public CxxTest::TestSuite {

public:
  static SaveMFTTest *createSuite() { return new SaveMFTTest(); }
  static void destroySuite(SaveMFTTest *suite) { delete suite; }

  SaveMFTTest() {}
  ~SaveMFTTest() override {}

  void testInit() {
    SaveMFT alg;
    alg.initialize();
    TS_ASSERT(alg.isInitialized())
  }

  void test_invalid_InputWorkspace() {
    SaveMFT alg;
    alg.initialize();
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Filename", "ws"))
    TS_ASSERT_THROWS(alg.setProperty("InputWorkspace", "abc"),
                     std::invalid_argument)
    TS_ASSERT_THROWS(alg.execute(), std::runtime_error)
    TS_ASSERT(!alg.isExecuted())
  }

  void test_point_data() {
    const auto &x1 = Mantid::HistogramData::Points({0.33, 0.34});
    const auto &y1 = Mantid::HistogramData::Counts({3., 6.6});
    Mantid::HistogramData::Histogram histogram(x1, y1);
    const Workspace_sptr ws = create<Workspace2D>(1, histogram);
    const std::string wsname = "ws1";
    SaveMFT alg;
    alg.initialize();
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", ws))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Filename", wsname))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted());
    std::string filename = alg.getPropertyValue("Filename");
    TS_ASSERT(Poco::File(filename).exists())
    std::vector<std::string> data;
    data.reserve(5);
    data.emplace_back("MFT");
    data.emplace_back("");
    data.emplace_back("                            q                    refl   "
                      "             refl_err");
    data.emplace_back("\t3.300000000000000e-01\t3.000000000000000e+00\t1."
                      "732050807568877e+00");
    data.emplace_back("\t3.400000000000000e-01\t6.600000000000000e+00\t2."
                      "569046515733026e+00");
    std::ifstream in(filename);
    TS_ASSERT(not_empty(in))
    std::string fullline;
    auto it = data.begin();
    while (std::getline(in, fullline)) {
      if (fullline.find(" : ") == std::string::npos)
        TS_ASSERT_EQUALS(fullline, *(it++));
    }
    TS_ASSERT(in.eof())
    try {
      Poco::File(filename).remove();
    } catch (...) {
      TS_FAIL("Error deleting file " + filename);
    }
  }

  void test_histogram_data() {
    const auto &x1 = Mantid::HistogramData::BinEdges({2.4, 3.7, 10.8});
    const auto &y1 = Mantid::HistogramData::Counts({3., 6.6});
    Mantid::HistogramData::Histogram histogram(x1, y1);
    Workspace2D_sptr ws = create<Workspace2D>(1, histogram);
    const std::string wsname = "ws1";
    SaveMFT alg;
    alg.initialize();
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", ws))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Filename", wsname))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted());
    std::string filename = alg.getPropertyValue("Filename");
    TS_ASSERT(Poco::File(filename).exists());
    std::vector<std::string> data;
    data.reserve(5);
    data.emplace_back("MFT");
    data.emplace_back("");
    data.emplace_back("                            q                    refl   "
                      "             refl_err");
    data.emplace_back(
        "\t3.050000000000000e+00\t3.000000000000000e+00\t1.732050807568877e+"
        "00");
    data.emplace_back(
        "\t7.250000000000000e+00\t6.600000000000000e+00\t2.569046515733026e+"
        "00");
    std::ifstream in(filename);
    std::string fullline;
    auto it = data.begin();
    TS_ASSERT(not_empty(in))
    while (std::getline(in, fullline)) {
      if (fullline.find(" : ") == std::string::npos)
        TS_ASSERT_EQUALS(fullline, *(it++))
    }
    TS_ASSERT(in.eof())
    try {
      Poco::File(filename).remove();
    } catch (...) {
      TS_FAIL("Error deleting file " + filename);
    }
  }

  void test_empty_workspace() {
    auto ws = boost::make_shared<Mantid::DataObjects::Workspace2D>();
    std::string wsname = "ws1";
    SaveMFT alg;
    alg.initialize();
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", ws))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Filename", wsname))
    TS_ASSERT_THROWS_ANYTHING(alg.execute())
    TS_ASSERT(!alg.isExecuted())
    std::string filename = alg.getPropertyValue("Filename");
    TS_ASSERT(!Poco::File(filename).exists())
  }

  void test_number_lines_for_two_data_values() {
    const auto &x1 = Mantid::HistogramData::Points({0.33, 0.34});
    const auto &y1 = Mantid::HistogramData::Counts({3., 6.6});
    Mantid::HistogramData::Histogram histogram(x1, y1);
    auto ws = boost::make_shared<Mantid::DataObjects::Workspace2D>();
    ws->initialize(1, histogram);
    std::string wsname = "ws1";
    SaveMFT alg;
    alg.initialize();
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", ws))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Filename", wsname))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted())
    std::string filename = alg.getPropertyValue("Filename");
    TS_ASSERT(Poco::File(filename).exists())
    std::ifstream in(filename);
    // Total number of lines
    TS_ASSERT(not_empty(in))
    TS_ASSERT_EQUALS(std::count(std::istreambuf_iterator<char>(in),
                                std::istreambuf_iterator<char>(),
                                in.widen('\n')),
                     25)
    try {
      Poco::File(filename).remove();
    } catch (...) {
      TS_FAIL("Error deleting file " + filename);
    }
  }

  void test_dx_values() {
    const auto &x1 = Mantid::HistogramData::Points({0.33, 0.34});
    const auto &y1 = Mantid::HistogramData::Counts({3., 6.6});
    Mantid::HistogramData::Histogram histogram(x1, y1);
    histogram.setPointStandardDeviations(std::vector<double>{1.1, 1.3});
    const Workspace_sptr ws = create<Workspace2D>(1, histogram);
    const std::string wsname = "ws1";
    SaveMFT alg;
    alg.initialize();
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", ws))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Filename", wsname))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted());
    std::string filename = alg.getPropertyValue("Filename");
    TS_ASSERT(Poco::File(filename).exists())
    std::vector<std::string> data;
    data.reserve(5);
    data.emplace_back("MFT");
    data.emplace_back("");
    data.emplace_back("                            q                    refl   "
                      "             refl_err            q_res (FWHM)");
    data.emplace_back("\t3.300000000000000e-01\t3.000000000000000e+00\t1."
                      "732050807568877e+00\t1.100000000000000e+00");
    data.emplace_back("\t3.400000000000000e-01\t6.600000000000000e+00\t2."
                      "569046515733026e+00\t1.300000000000000e+00");
    std::ifstream in(filename);
    TS_ASSERT(not_empty(in))
    std::string fullline;
    auto it = data.begin();
    while (std::getline(in, fullline)) {
      if (fullline.find(" : ") == std::string::npos)
        TS_ASSERT_EQUALS(fullline, *(it++));
    }
    TS_ASSERT(in.eof())
    try {
      Poco::File(filename).remove();
    } catch (...) {
      TS_FAIL("Error deleting file " + filename);
    }
  }

  void test_no_header() {
    const auto &x1 = Mantid::HistogramData::Points({0.33, 0.34});
    const auto &y1 = Mantid::HistogramData::Counts({3., 6.6});
    Mantid::HistogramData::Histogram histogram(x1, y1);
    const Workspace_sptr ws = create<Workspace2D>(1, histogram);
    const std::string wsname = "ws1";
    SaveMFT alg;
    alg.initialize();
    alg.setRethrows(true);
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("InputWorkspace", ws))
    TS_ASSERT_THROWS_NOTHING(alg.setProperty("Filename", wsname))
    TS_ASSERT_THROWS_NOTHING(
        alg.setProperty("Header", "Do not write header lines"))
    TS_ASSERT_THROWS_NOTHING(alg.execute())
    TS_ASSERT(alg.isExecuted());
    std::string filename = alg.getPropertyValue("Filename");
    TS_ASSERT(Poco::File(filename).exists())
    std::vector<std::string> data;
    data.reserve(2);
    data.emplace_back("\t3.300000000000000e-01\t3.000000000000000e+00\t1."
                      "732050807568877e+00");
    data.emplace_back("\t3.400000000000000e-01\t6.600000000000000e+00\t2."
                      "569046515733026e+00");
    std::ifstream in(filename);
    TS_ASSERT(not_empty(in))
    std::string fullline;
    auto it = data.begin();
    while (std::getline(in, fullline)) {
      TS_ASSERT_EQUALS(fullline, *(it++));
    }
    TS_ASSERT(in.eof())
    try {
      Poco::File(filename).remove();
    } catch (...) {
      TS_FAIL("Error deleting file " + filename);
    }
  }

  void test_override_existing_file() {}

  void test_undefined_log() {}

  void test_more_than_nine_logs() {}

  void test_defined_log() {}

private:
  bool not_empty(std::ifstream &in) {
    return in.peek() != std::ifstream::traits_type::eof();
  }
};
#endif /*MANTID_DATAHANDLING_SAVEMFTTEST_H_*/
