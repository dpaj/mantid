#ifndef MANTID_ALGORITHMS_CONVOLUTIONFITSEQUENTIALTEST_H_
#define MANTID_ALGORITHMS_CONVOLUTIONFITSEQUENTIALTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/WorkspaceFactory.h"

#include "MantidWorkflowAlgorithms/ConvolutionFitSequential.h"

#include "MantidDataObjects/Workspace2D.h"

#include "MantidKernel/TimeSeriesProperty.h"

#include "MantidTestHelpers/WorkspaceCreationHelper.h"

using Mantid::Algorithms::ConvolutionFitSequential;
using namespace Mantid::API;
using namespace Mantid::DataObjects;

class ConvolutionFitSequentialTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static ConvolutionFitSequentialTest *createSuite() {
    return new ConvolutionFitSequentialTest();
  }
  static void destroySuite(ConvolutionFitSequentialTest *suite) {
    delete suite;
  }

  ConvolutionFitSequentialTest() { FrameworkManager::Instance(); }

  void test_fit_function_is_valid_for_convolution_fitting() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    TS_ASSERT_THROWS_NOTHING(alg.setProperty(
        "Function", "function=test,name=Convolution,name=Resolution"));
  }

  //-------------------------- Failure cases ----------------------------
  void test_empty_function_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());

    TS_ASSERT_THROWS(alg.setPropertyValue("Function", ""),
                     std::invalid_argument);
  }

  void test_empty_startX_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());

    TS_ASSERT_THROWS(alg.setPropertyValue("StartX", ""), std::invalid_argument);
  }

  void test_empty_endX_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());

    TS_ASSERT_THROWS(alg.setPropertyValue("EndX", ""), std::invalid_argument);
  }

  void test_empty_specMin_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());

    TS_ASSERT_THROWS(alg.setPropertyValue("SpecMin", ""),
                     std::invalid_argument);
  }

  void test_empty_specMax_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());

    TS_ASSERT_THROWS(alg.setPropertyValue("SpecMax", ""),
                     std::invalid_argument);
  }

  void test_empty_maxIterations_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());

    TS_ASSERT_THROWS(alg.setPropertyValue("MaxIterations", ""),
                     std::invalid_argument);
  }

  void test_spectra_min_or_max_number_can_not_be_negative() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    TS_ASSERT_THROWS(alg.setPropertyValue("SpecMin", "-1"),
                     std::invalid_argument);
    TS_ASSERT_THROWS(alg.setPropertyValue("SpecMax", "-1"),
                     std::invalid_argument);
  }

  void test_max_iterations_can_not_be_a_negative_number() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    TS_ASSERT_THROWS(alg.setPropertyValue("MaxIterations", "-1"),
                     std::invalid_argument);
  }

  void test_fit_function_that_does_not_contain_resolution_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    TS_ASSERT_THROWS(
        alg.setProperty("Function", "function=test,name=Convolution"),
        std::invalid_argument);
  }

  void test_fit_function_that_does_not_contain_convolution_is_not_allowed() {
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    TS_ASSERT_THROWS(
        alg.setProperty("Function", "function=test,name=Resolution"),
        std::invalid_argument);
  }

  //------------------------- Execution cases ---------------------------
  void test_exec_with_red_file() {
    const int totalBins = 6;
    auto resWs = create2DWorkspace(5, 1);
    auto redWs = create2DWorkspace(totalBins, 5);
    createConvFitResWorkspace(5, totalBins);
    AnalysisDataService::Instance().add("ResolutionWs_", resWs);
    AnalysisDataService::Instance().add("ReductionWs_", redWs);
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    alg.setProperty("InputWorkspace", redWs);
    alg.setProperty("Function",
                    "name=LinearBackground,A0=0,A1=0,ties=(A0=0.000000,A1=0.0);"
                    "(composite=Convolution,FixResolution=true,NumDeriv=true;"
                    "name=Resolution,Workspace=__ConvFit_Resolution,"
                    "WorkspaceIndex=0;((composite=ProductFunction,NumDeriv="
                    "false;name=Lorentzian,Amplitude=1,PeakCentre=0,FWHM=0."
                    "0175)))");
    alg.setProperty("BackgroundType", "Fixed Flat");
    alg.setProperty("StartX", 0.0);
    alg.setProperty("EndX", 3.0);
    alg.setProperty("SpecMin", 0);
    alg.setProperty("SpecMax", 5);
    alg.setProperty("Convolve", true);
    alg.setProperty("Minimizer", "Levenberg-Marquardt");
    alg.setProperty("MaxIterations", 500);
    TS_ASSERT_THROWS_NOTHING(alg.execute());
    TS_ASSERT(alg.isExecuted());

    // Retrieve and analyse parameter table - Param table does not require
    // further testing as this is tested in the ProcessIndirectFitParameters
    // Algorithm
    ITableWorkspace_sptr paramTable;
    TS_ASSERT_THROWS_NOTHING(
        paramTable =
            AnalysisDataService::Instance().retrieveWS<ITableWorkspace>(
                "ReductionWs_conv_1LFixF_s0_to_5_Parameters"));

    // Retrieve and analyse results table
    MatrixWorkspace_sptr resultWs;
    TS_ASSERT_THROWS_NOTHING(
        resultWs = AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
            "ReductionWs_conv_1LFixF_s0_to_5_Result"));
    TS_ASSERT_EQUALS(resultWs->blocksize(), totalBins);

    // Retrieve and analyse group table
    WorkspaceGroup_sptr groupWs;
    TS_ASSERT_THROWS_NOTHING(
        groupWs = AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>(
            "ReductionWs_conv_1LFixF_s0_to_5_Workspaces"));

    // Check number of expected Histograms and Histogram deminsions
    int entities = groupWs->getNumberOfEntries();
    TS_ASSERT_EQUALS(entities, redWs->getNumberHistograms());
    auto groupMember =
        groupWs->getItem("ReductionWs_conv_1LFixF_s0_to_5_0_Workspace");
    auto matrixMember =
        boost::dynamic_pointer_cast<MatrixWorkspace>(groupMember);

    TS_ASSERT_EQUALS(matrixMember->blocksize(), resWs->blocksize());

    // Check oringal Log was copied correctly
    auto &memberRun = matrixMember->mutableRun();
    auto &originalRun = redWs->mutableRun();

    TS_ASSERT_EQUALS(memberRun.getLogData().at(1)->value(),
                     originalRun.getLogData().at(1)->value());

    // Check new Log data is present
    auto memberLogs = memberRun.getLogData();

    TS_ASSERT_EQUALS(memberLogs.at(2)->value(), "FixF");
    TS_ASSERT_EQUALS(memberLogs.at(3)->value(), "true");
    TS_ASSERT_EQUALS(memberLogs.at(4)->value(), "false");
    TS_ASSERT_EQUALS(memberLogs.at(5)->value(), "ConvFit");
    TS_ASSERT_EQUALS(memberLogs.at(6)->value(), "ReductionWs_");
    TS_ASSERT_EQUALS(memberLogs.at(7)->value(), "1");

    AnalysisDataService::Instance().clear();
  }

  void test_exec_with_sqw_file() {
    auto sqwWs = createGenericWorkspace("SqwWs_", true);
    auto resWs = createGenericWorkspace("ResolutionWs_", false);
    auto convFitRes = createGenericWorkspace("__ConvFit_Resolution", false);
    Mantid::Algorithms::ConvolutionFitSequential alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    alg.setProperty("InputWorkspace", sqwWs);
    alg.setProperty("Function",
                    "name=LinearBackground,A0=0,A1=0,ties=(A0=0.000000,A1=0.0);"
                    "(composite=Convolution,FixResolution=true,NumDeriv=true;"
                    "name=Resolution,Workspace=__ConvFit_Resolution,"
                    "WorkspaceIndex=0;((composite=ProductFunction,NumDeriv="
                    "false;name=Lorentzian,Amplitude=1,PeakCentre=0,FWHM=0."
                    "0175)))");
    alg.setProperty("BackgroundType", "Fixed Flat");
    alg.setProperty("StartX", 0.0);
    alg.setProperty("EndX", 5.0);
    alg.setProperty("SpecMin", 0);
    alg.setProperty("SpecMax", 0);
    alg.setProperty("Convolve", true);
    alg.setProperty("Minimizer", "Levenberg-Marquardt");
    alg.setProperty("MaxIterations", 500);
    TS_ASSERT_THROWS_NOTHING(alg.execute());
    TS_ASSERT(alg.isExecuted());

    // Assert that output is in ADS
    TS_ASSERT_THROWS_NOTHING(
        AnalysisDataService::Instance().retrieveWS<ITableWorkspace>(
            "SqwWs_conv_1LFixF_s0_to_0_Parameters"));

    TS_ASSERT_THROWS_NOTHING(
        AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
            "SqwWs_conv_1LFixF_s0_to_0_Result"));

    TS_ASSERT_THROWS_NOTHING(
        AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>(
            "SqwWs_conv_1LFixF_s0_to_0_Workspaces"));

    AnalysisDataService::Instance().clear();
  }

  //------------------------ Private Functions---------------------------

  MatrixWorkspace_sptr createGenericWorkspace(const std::string &wsName,
                                              const bool numericAxis) {
    const std::vector<double> xData{1, 2, 3, 4, 5};
    const std::vector<double> yData{0, 1, 3, 1, 0};

    auto createWorkspace =
        AlgorithmManager::Instance().create("CreateWorkspace");
    createWorkspace->initialize();
    if (numericAxis) {
      createWorkspace->setProperty("UnitX", "DeltaE");
      createWorkspace->setProperty("VerticalAxisUnit", "MomentumTransfer");
      createWorkspace->setProperty("VerticalAxisValues", "1");
    } else {
      createWorkspace->setProperty("UnitX", "DeltaE");
      createWorkspace->setProperty("VerticalAxisUnit", "SpectraNumber");
    }
    createWorkspace->setProperty("DataX", xData);
    createWorkspace->setProperty("DataY", yData);
    createWorkspace->setProperty("NSpec", 1);
    createWorkspace->setPropertyValue("OutputWorkspace", wsName);
    createWorkspace->execute();
    MatrixWorkspace_sptr sqwWS =
        AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(wsName);
    return sqwWS;
  }

  MatrixWorkspace_sptr create2DWorkspace(int xlen, int ylen) {
    auto ws = WorkspaceCreationHelper::create2DWorkspaceWithFullInstrument(
        xlen, ylen, false, false, true, "testInst");
    boost::shared_ptr<Mantid::MantidVec> x1 =
        boost::make_shared<Mantid::MantidVec>(xlen, 0.0);
    boost::shared_ptr<Mantid::MantidVec> y1(
        new Mantid::MantidVec(xlen - 1, 3.0));
    boost::shared_ptr<Mantid::MantidVec> e1(
        new Mantid::MantidVec(xlen - 1, sqrt(3.0)));

    MatrixWorkspace_sptr testWs(ws);
    testWs->initialize(ylen, xlen, xlen - 1);
    double j = 1.0;

    for (int i = 0; i < xlen; i++) {
      (*x1)[i] = j * 0.5;
      j += 1.5;
    }

    for (int i = 0; i < ylen; i++) {
      testWs->setX(i, x1);
      testWs->setData(i, y1, e1);
    }

    testWs->getAxis(0)->setUnit("DeltaE");

    for (int i = 0; i < xlen; i++) {
      testWs->setEFixed((i + 1), 0.50);
    }

    auto &run = testWs->mutableRun();
    auto timeSeries =
        new Mantid::Kernel::TimeSeriesProperty<std::string>("TestTimeSeries");
    timeSeries->addValue("2010-09-14T04:20:12", "0.02");
    run.addProperty(timeSeries);
    auto test = run.getLogData("TestTimeSeries")->value();
    return testWs;
  }

  void createConvFitResWorkspace(int totalHist, int totalBins) {
    auto convFitRes = WorkspaceFactory::Instance().create(
        "Workspace2D", totalHist + 1, totalBins + 1, totalBins);
    boost::shared_ptr<Mantid::MantidVec> x1(
        new Mantid::MantidVec(totalBins + 1, 0.0));
    boost::shared_ptr<Mantid::MantidVec> y1(
        new Mantid::MantidVec(totalBins, 3.0));
    boost::shared_ptr<Mantid::MantidVec> e1(
        new Mantid::MantidVec(totalBins, sqrt(3.0)));

    MatrixWorkspace_sptr testWs(convFitRes);
    testWs->initialize(totalHist + 1, totalBins + 1, totalBins);
    double j = 1.0;

    for (int i = 0; i < totalBins; i++) {
      (*x1)[i] = j * 0.5;
      j += 1.5;
    }

    for (int i = 0; i < totalBins; i++) {
      testWs->setX(i, x1);
      testWs->setData(i, y1, e1);
    }

    AnalysisDataService::Instance().add("__ConvFit_Resolution", convFitRes);
  }
};

#endif /* MANTID_ALGORITHMS_CONVOLUTIONFITSEQUENTIALTEST_H_ */