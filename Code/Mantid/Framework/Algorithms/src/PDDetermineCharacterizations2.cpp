#include "MantidAlgorithms/PDDetermineCharacterizations2.h"
#include "MantidAPI/PropertyManagerDataService.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/TimeSeriesProperty.h"

namespace Mantid {
namespace Algorithms {

using Mantid::API::ITableWorkspace_const_sptr;
using Mantid::API::PropertyManagerDataService;
using Mantid::API::WorkspaceProperty;
using Mantid::Kernel::ArrayProperty;
using Mantid::Kernel::Direction;
using Mantid::Kernel::PropertyWithValue;
using Mantid::Kernel::TimeSeriesProperty;

namespace { // anonymous namespace
/// this should match those in LoadPDCharacterizations
const std::vector<std::string> COL_NAMES = {
    "frequency",  // double
    "wavelength", // double
    "bank",       // integer
    "container",  // string
    "vanadium",   // string
    "empty",      // string
    "d_min",      // string
    "d_max",      // string
    "tof_min",    // double
    "tof_max"     // double
};
const std::string CHAR_PROP_NAME("Characterizations");
const std::string FREQ_PROP_NAME("FrequencyLogNames");
const std::string WL_PROP_NAME("WaveLengthLogNames");
}

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(PDDetermineCharacterizations2)

//----------------------------------------------------------------------------------------------
/// Constructor
PDDetermineCharacterizations2::PDDetermineCharacterizations2() {}

//----------------------------------------------------------------------------------------------
/// Destructor
PDDetermineCharacterizations2::~PDDetermineCharacterizations2() {}

//----------------------------------------------------------------------------------------------

/// Algorithms name for identification. @see Algorithm::name
const std::string PDDetermineCharacterizations2::name() const {
  return "PDDetermineCharacterizations";
}

/// Algorithm's version for identification. @see Algorithm::version
int PDDetermineCharacterizations2::version() const { return 2; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string PDDetermineCharacterizations2::category() const {
  return "Workflow/Diffraction/UsesPropertyManager";
}

/// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
const std::string PDDetermineCharacterizations2::summary() const {
  return "Determines the characterizations of a workspace.";
}

//----------------------------------------------------------------------------------------------

/// More intesive input checking. @see Algorithm::validateInputs
std::map<std::string, std::string>
PDDetermineCharacterizations2::validateInputs() {
  std::map<std::string, std::string> result;

  ITableWorkspace_const_sptr characterizations = getProperty(CHAR_PROP_NAME);

  if (!bool(characterizations))
    return result;

  std::vector<std::string> names = characterizations->getColumnNames();
  if (names.size() < COL_NAMES.size()) { // allow for extra columns
    std::stringstream msg;
    msg << "Encountered invalid number of columns in "
        << "TableWorkspace. Found " << names.size() << " expected "
        << COL_NAMES.size();
    result[CHAR_PROP_NAME] = msg.str();
  } else {
    for (auto it = COL_NAMES.begin(); it != COL_NAMES.end(); ++it) {
      if (std::find(names.begin(), names.end(), *it) == names.end()) {
        std::stringstream msg;
        msg << "Failed to find column named " << (*it);
        result[CHAR_PROP_NAME] = msg.str();
      }
    }
  }
  return result;
}

/// Initialize the algorithm's properties.
void PDDetermineCharacterizations2::init() {
  declareProperty(
      new WorkspaceProperty<>("InputWorkspace", "", Direction::Input,
                              API::PropertyMode::Optional),
      "Workspace with logs to help identify frequency and wavelength");

  declareProperty(
      new WorkspaceProperty<API::ITableWorkspace>(
          CHAR_PROP_NAME, "", Direction::Input, API::PropertyMode::Optional),
      "Table of characterization information");

  declareProperty("ReductionProperties", "__pd_reduction_properties",
                  "Property manager name for the reduction");

  const std::string defaultMsg =
      " run to use. 0 to use value in table, -1 to not use.";

  declareProperty("BackRun", 0, "Empty container" + defaultMsg);
  declareProperty("NormRun", 0, "Normalization" + defaultMsg);
  declareProperty("NormBackRun", 0, "Normalization background" + defaultMsg);

  const std::vector<std::string> DEFAULT_FREQUENCY_NAMES = {
      "SpeedRequest1", "Speed1", "frequency"};
  declareProperty(new Kernel::ArrayProperty<std::string>(
                      FREQ_PROP_NAME, DEFAULT_FREQUENCY_NAMES),
                  "Candidate log names for frequency");

  const std::vector<std::string> DEFAULT_WAVELENGTH_NAMES = {"LambdaRequest",
                                                             "lambda"};
  declareProperty(new Kernel::ArrayProperty<std::string>(
                      WL_PROP_NAME, DEFAULT_WAVELENGTH_NAMES),
                  "Candidate log names for wave length");
}

/**
 * Compare two numbers to be in agreement within 5%
 * @param left
 * @param right
 * @return
 */
bool closeEnough(const double left, const double right) {
  // the same value
  const double diff = fabs(left - right);
  if (diff == 0.)
    return true;

  // same within 5%
  const double relativeDiff = diff * 2 / (left + right);
  if (relativeDiff < .05)
    return true;

  return false;
}

/// Fill in the property manager from the correct line in the table
void PDDetermineCharacterizations2::getInformationFromTable(
    const double frequency, const double wavelength) {
  size_t numRows = m_characterizations->rowCount();

  for (size_t i = 0; i < numRows; ++i) {
    const double rowFrequency =
        m_characterizations->getRef<double>("frequency", i);
    const double rowWavelength =
        m_characterizations->getRef<double>("wavelength", i);

    if (closeEnough(frequency, rowFrequency) &&
        closeEnough(wavelength, rowWavelength)) {
      g_log.information() << "Using information from row " << i
                          << " with frequency = " << rowFrequency
                          << " and wavelength = " << rowWavelength << "\n";

      m_propertyManager->setProperty("frequency", frequency);
      m_propertyManager->setProperty("wavelength", wavelength);

      m_propertyManager->setProperty(
          "bank", m_characterizations->getRef<int>("bank", i));

      m_propertyManager->setProperty(
          "vanadium", m_characterizations->getRef<int>("vanadium", i));
      m_propertyManager->setProperty(
          "container", m_characterizations->getRef<int>("container", i));
      m_propertyManager->setProperty(
          "empty", m_characterizations->getRef<int>("empty", i));

      m_propertyManager->setPropertyValue(
          "d_min", m_characterizations->getRef<std::string>("d_min", i));
      m_propertyManager->setPropertyValue(
          "d_max", m_characterizations->getRef<std::string>("d_max", i));

      m_propertyManager->setProperty(
          "tof_min", m_characterizations->getRef<double>("tof_min", i));
      m_propertyManager->setProperty(
          "tof_max", m_characterizations->getRef<double>("tof_max", i));
      return;
    }
  }
  g_log.warning("Failed to find compatible row in characterizations table");
}

/**
 * Get a value from one of a set of logs.
 * @param run
 * @param propName
 * @return
 */
double PDDetermineCharacterizations2::getLogValue(API::Run &run,
                                                  const std::string &propName) {
  std::vector<std::string> names = getProperty(propName);

  std::string label = "frequency";
  if (propName == WL_PROP_NAME)
    label = "wavelength";

  std::set<std::string> validUnits;
  if (propName == WL_PROP_NAME) {
    validUnits.insert("Angstrom");
    validUnits.insert("A");
  } else {
    validUnits.insert("Hz");
  }

  for (auto name : names) {
    if (run.hasProperty(name)) {
      const std::string units = run.getProperty(name)->units();

      if (validUnits.find(units) != validUnits.end()) {
        double value = run.getLogAsSingleValue(name);
        if (value == 0.) {
          std::stringstream msg;
          msg << "'" << name << "' has a mean value of zero " << units;
          g_log.information(msg.str());
        } else {
          std::stringstream msg;
          msg << "Found " << label << " in log '" << name
              << "' with mean value " << value << " " << units;
          g_log.information(msg.str());
          return value;
        }
      } else {
        std::stringstream msg;
        msg << "When looking at " << name
            << " log encountered unknown units for " << label << ":" << units;
        g_log.warning(msg.str());
      }
    }
  }
  g_log.warning("Failed to determine " + label);
  return 0.;
}

/// Set the default values in the property manager
void PDDetermineCharacterizations2::setDefaultsInPropManager() {
  if (!m_propertyManager->existsProperty("frequency")) {
    m_propertyManager->declareProperty(
        new PropertyWithValue<double>("frequency", 0.));
  }
  if (!m_propertyManager->existsProperty("wavelength")) {
    m_propertyManager->declareProperty(
        new PropertyWithValue<double>("wavelength", 0.));
  }
  if (!m_propertyManager->existsProperty("bank")) {
    m_propertyManager->declareProperty(new PropertyWithValue<int>("bank", 1));
  }
  if (!m_propertyManager->existsProperty("vanadium")) {
    m_propertyManager->declareProperty(
        new PropertyWithValue<int32_t>("vanadium", 0));
  }
  if (!m_propertyManager->existsProperty("container")) {
    m_propertyManager->declareProperty(
        new PropertyWithValue<int32_t>("container", 0));
  }
  if (!m_propertyManager->existsProperty("empty")) {
    m_propertyManager->declareProperty(
        new PropertyWithValue<int32_t>("empty", 0));
  }
  if (!m_propertyManager->existsProperty("d_min")) {
    m_propertyManager->declareProperty(
        new ArrayProperty<std::vector<double>>("d_min"));
  }
  if (!m_propertyManager->existsProperty("d_max")) {
    m_propertyManager->declareProperty(
        new ArrayProperty<std::vector<double>>("d_max"));
  }
  if (!m_propertyManager->existsProperty("tof_min")) {
    m_propertyManager->declareProperty(
        new PropertyWithValue<double>("tof_min", 0.));
  }
  if (!m_propertyManager->existsProperty("tof_max")) {
    m_propertyManager->declareProperty(
        new PropertyWithValue<double>("tof_max", 0.));
  }
}

/**
 * Set the run number in the property manager from algoritm inputs.
 * @param inputName
 * @param propName
 */
void PDDetermineCharacterizations2::overrideRunNumProperty(
    const std::string &inputName, const std::string &propName) {
  int32_t runnumber = this->getProperty(inputName);
  if (runnumber != 0) {
    if (runnumber < 0)
      runnumber = 0;
    m_propertyManager->setProperty(propName, runnumber);
  }
}

/// Execute the algorithm.
void PDDetermineCharacterizations2::exec() {
  // setup property manager to return
  const std::string managerName = getPropertyValue("ReductionProperties");
  if (PropertyManagerDataService::Instance().doesExist(managerName)) {
    m_propertyManager =
        PropertyManagerDataService::Instance().retrieve(managerName);
  } else {
    m_propertyManager = boost::make_shared<Kernel::PropertyManager>();
    PropertyManagerDataService::Instance().addOrReplace(managerName,
                                                        m_propertyManager);
  }

  setDefaultsInPropManager();

  m_characterizations = getProperty(CHAR_PROP_NAME);
  if (bool(m_characterizations) && (m_characterizations->rowCount() > 0)) {
    API::MatrixWorkspace_sptr inputWS = getProperty("InputWorkspace");
    auto run = inputWS->mutableRun();

    double frequency = getLogValue(run, FREQ_PROP_NAME);

    double wavelength = getLogValue(run, WL_PROP_NAME);

    getInformationFromTable(frequency, wavelength);
  }

  overrideRunNumProperty("BackRun", "container");
  overrideRunNumProperty("NormRun", "vanadium");
  overrideRunNumProperty("NormBackRun", "empty");

  for (auto it = COL_NAMES.begin(); it != COL_NAMES.end(); ++it) {
    if (m_propertyManager->existsProperty(*it)) {
      g_log.debug() << (*it) << ":" << m_propertyManager->getPropertyValue(*it)
                    << "\n";
    } else {
      std::cout << (*it) << " DOES NOT EXIST\n";
    }
  }
}

} // namespace Algorithms
} // namespace Mantid
