//------------------------
// Includes
//------------------------
#include <vector>

#include "boost/python.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"
#include "boost/cstdint.hpp"

#include "MantidPythonAPI/FrameworkManager.h"
#include "MantidPythonAPI/PyAlgorithm.h"
#include "MantidAPI/IAlgorithm.h"
#include "MantidAPI/Algorithm.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/WorkspaceHistory.h"
#include "MantidAPI/AlgorithmHistory.h"
#include "MantidKernel/EnvironmentHistory.h"
#include "MantidKernel/Property.h"
#include "MantidKernel/PropertyManager.h"
#include "MantidGeometry/IDetector.h"
#include "MantidAPI/SpectraDetectorMap.h"

/**
  * For the moment we cannot use TableWorkspace as there is no API and 
  * if we link it directly to DataObjects then if the MantidDataObjects library is not in the same directory the loading will 
  * fail. We should only use things through their API classes
  */
//#include "MantidDataObjects/TableWorkspace.h"

//Given the frequent use, this makes things more readable
using namespace boost::python;

namespace Mantid 
{

namespace PythonAPI
{

/**
 * Wrappers for the C++ code
 * @name Python wrappers
 */
  //@cond
//@{
  /// An wrapper for IAlgorithm
  struct Mantid_API_IAlgorithm_Wrapper: Mantid::API::IAlgorithm
  {
    Mantid_API_IAlgorithm_Wrapper(PyObject* py_self_, const Mantid::API::IAlgorithm& p0):
      Mantid::API::IAlgorithm(p0), py_self(py_self_) {}

    Mantid_API_IAlgorithm_Wrapper(PyObject* py_self_):
      Mantid::API::IAlgorithm(), py_self(py_self_) {}

    void initialize() {
      call_method< void >(py_self, "initialize");
    }

    bool execute() {
      return call_method< bool >(py_self, "execute");
    }

    bool isInitialized() const {
      return call_method< bool >(py_self, "isInitialized");
    }

    bool isExecuted() const {
      return call_method< bool >(py_self, "isExecuted");
    }

    void setPropertyValue(int p0, int p1) {
      call_method< void >(py_self, "setPropertyValue", p0, p1);
    }

    bool getPropertyValue(int p0) const {
      return call_method< bool >(py_self, "getPropertyValue", p0);
    }
        
    const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& getProperties() const {
      return call_method< const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& >(py_self, "getProperties");
    }

    PyObject* py_self;
  };

  /// An wrapper for MatrixWorkspace
  struct Mantid_API_MatrixWorkspace_Wrapper: Mantid::API::MatrixWorkspace
  {
    const std::string id() const {
      return call_method< const std::string >(py_self, "id");
    }

    void init(const int& p0, const int& p1, const int& p2) {
      call_method< void >(py_self, "init", p0, p1, p2);
    }

    long int getMemorySize() const {
      return call_method< long int >(py_self, "getMemorySize");
    }

    long int default_getMemorySize() const {
      return Mantid::API::MatrixWorkspace::getMemorySize();
    }

    int size() const {
      return call_method< int >(py_self, "size");
    }

    const int getNumberHistograms() const {
      return call_method< const int >(py_self, "getNumberHistograms");
    }

    int blocksize() const {
      return call_method< int >(py_self, "blocksize");
    }

    int spectraNo(const int p0) const {
      return call_method< int >(py_self, "spectraNo", p0);
    }

    int& spectraNo(const int p0) {
      return call_method< int& >(py_self, "spectraNo", p0);
    }

    const std::vector<double,std::allocator<double> >& getX(const int p0) const {
      return call_method< const std::vector<double,std::allocator<double> >& >(py_self, "getX", p0);
    }

    const std::vector<double,std::allocator<double> >& getY(const int p0) const {
      return call_method< const std::vector<double,std::allocator<double> >& >(py_self, "getY", p0);
    }

    const std::vector<double,std::allocator<double> >& getE(const int p0) const {
      return call_method< const std::vector<double,std::allocator<double> >& >(py_self, "getE", p0);
    }

    PyObject* py_self;
  };

  // /// An wrapper for TableWorkspace
  // struct Mantid_DataObjects_TableWorkspace_Wrapper: Mantid::DataObjects::TableWorkspace
  // {
   
    // int columnCount() const {
      // return call_method<int>(py_self, "columnCount");
    // }
    
    // std::vector<std::string> getColumnNames() {
      // return call_method<std::vector<std::string> >(py_self, "getColumnNames");
    // }
    
    // int rowCount() const {
      // return call_method<int>(py_self, "rowCount");
    // }

    // PyObject* py_self;
  // };

  /// Some function pointers since MSVC can't figure out the function to call when placing this directlyin the .def functions below
  // std::vector<int>& (Mantid::DataObjects::TableWorkspace::*TableWorkspace_GetIntegerColumn)(const std::string&) = &Mantid::DataObjects::TableWorkspace::getStdVector<int>;
  // std::vector<double>& (Mantid::DataObjects::TableWorkspace::*TableWorkspace_GetDoubleColumn)(const std::string&) = &Mantid::DataObjects::TableWorkspace::getStdVector<double>;
  // std::vector<std::string>& (Mantid::DataObjects::TableWorkspace::*TableWorkspace_GetStringColumn)(const std::string&) = &Mantid::DataObjects::TableWorkspace::getStdVector<std::string>;
  
  
  /// An wrapper for PropertyManager
  struct Mantid_Kernel_PropertyManager_Wrapper: Mantid::Kernel::PropertyManager
  {

    void setProperties(const std::string& p0) {
      call_method< void >(py_self, "setProperties", p0);
    }

    void default_setProperties(const std::string& p0) {
      Mantid::Kernel::PropertyManager::setProperties(p0);
    }

    void setPropertyValue(const std::string& p0, const std::string& p1) {
      call_method< void >(py_self, "setPropertyValue", p0, p1);
    }

    void default_setPropertyValue(const std::string& p0, const std::string& p1) {
      Mantid::Kernel::PropertyManager::setPropertyValue(p0, p1);
    }

    void setPropertyOrdinal(const int& p0, const std::string& p1) {
      call_method< void >(py_self, "setPropertyOrdinal", p0, p1);
    }

    void default_setPropertyOrdinal(const int& p0, const std::string& p1) {
      Mantid::Kernel::PropertyManager::setPropertyOrdinal(p0, p1);
    }

    bool existsProperty(const std::string& p0) const {
      return call_method< bool >(py_self, "existsProperty", p0);
    }

    bool default_existsProperty(const std::string& p0) const {
      return Mantid::Kernel::PropertyManager::existsProperty(p0);
    }

    bool validateProperties() const {
      return call_method< bool >(py_self, "validateProperties");
    }

    bool default_validateProperties() const {
      return Mantid::Kernel::PropertyManager::validateProperties();
    }

    std::string getPropertyValue(const std::string& p0) const {
      return call_method< std::string >(py_self, "getPropertyValue", p0);
    }

    std::string default_getPropertyValue(const std::string& p0) const {
      return Mantid::Kernel::PropertyManager::getPropertyValue(p0);
    }

    const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& getProperties() const {
      return call_method< const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& >(py_self, "getProperties");
    }

    const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& default_getProperties() const {
      return Mantid::Kernel::PropertyManager::getProperties();
    }

    PyObject* py_self;
  };

  /// A wrapper for Property
  struct Mantid_Kernel_Property_Wrapper: Mantid::Kernel::Property
  {
    const bool isValid() const {
      return call_method< const bool >(py_self, "isValid");
    }

    const bool default_isValid() const {
      return Mantid::Kernel::Property::isValid();
    }

    const std::string getValidatorType() const {
      return call_method< const std::string >(py_self, "getValidatorType");
    }

    const std::string default_getValidatorType() const {
      return Mantid::Kernel::Property::getValidatorType();
    }

    std::string value() const {
      return call_method< std::string >(py_self, "value");
    }

    bool setValue(const std::string& p0) {
      return call_method< bool >(py_self, "setValue", p0);
    }

    const std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > allowedValues() const {
      return call_method< const std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >(py_self, "allowedValues");
    }

    const std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >,std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > default_allowedValues() const {
      return Mantid::Kernel::Property::allowedValues();
    }

    PyObject* py_self;
  };

  ///A wrapper for Algorithm
  struct Mantid_API_Algorithm_Wrapper: Mantid::API::Algorithm
  {
    const std::string name() const {
      return call_method< const std::string >(py_self, "name");
    }

    const std::string default_name() const {
      return Mantid::API::Algorithm::name();
    }

    const int version() const {
      return call_method< const int >(py_self, "version");
    }

    const int default_version() const {
      return Mantid::API::Algorithm::version();
    }

    const std::string category() const {
      return call_method< const std::string >(py_self, "category");
    }

    const std::string default_category() const {
      return Mantid::API::Algorithm::category();
    }

    bool isInitialized() const {
      return call_method< bool >(py_self, "isInitialized");
    }

    bool default_isInitialized() const {
      return Mantid::API::Algorithm::isInitialized();
    }

    bool isExecuted() const {
      return call_method< bool >(py_self, "isExecuted");
    }

    bool default_isExecuted() const {
      return Mantid::API::Algorithm::isExecuted();
    }

    void setPropertyOrdinal(const int& p0, const std::string& p1) {
      call_method< void >(py_self, "setPropertyOrdinal", p0, p1);
    }

    void default_setPropertyOrdinal(const int& p0, const std::string& p1) {
      Mantid::API::Algorithm::setPropertyOrdinal(p0, p1);
    }

    void setPropertyValue(const std::string& p0, const std::string& p1) {
      call_method< void >(py_self, "setPropertyValue", p0, p1);
    }

    void default_setPropertyValue(const std::string& p0, const std::string& p1) {
      Mantid::API::Algorithm::setPropertyValue(p0, p1);
    }

    std::string getPropertyValue(const std::string& p0) const {
      return call_method< std::string >(py_self, "getPropertyValue", p0);
    }

    std::string default_getPropertyValue(const std::string& p0) const {
      return Mantid::API::Algorithm::getPropertyValue(p0);
    }

    bool checkPropertiesValid() const {
      return call_method< bool >(py_self, "checkPropertiesValid");
    }

    bool default_validateProperties() const {
      return Mantid::API::Algorithm::validateProperties();
    }

    const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& getProperties() const {
      return call_method< const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& >(py_self, "getProperties");
    }

    const std::vector<Mantid::Kernel::Property*,std::allocator<Mantid::Kernel::Property*> >& default_getProperties() const {
      return Mantid::API::Algorithm::getProperties();
    }

    void init() {
      call_method< void >(py_self, "init");
    }

    void exec() {
      call_method< void >(py_self, "exec");
    }

    PyObject* py_self;
  };
  
  ///A wrapper for IDetector
  struct Mantid_Geometry_IDetector_Wrapper: Mantid::Geometry::IDetector
  {
    Mantid_Geometry_IDetector_Wrapper(PyObject* py_self_, const Mantid::Geometry::IDetector& p0):
      Mantid::Geometry::IDetector(p0), py_self(py_self_) {}

    Mantid_Geometry_IDetector_Wrapper(PyObject* py_self_):
      Mantid::Geometry::IDetector(), py_self(py_self_) {}

    int getID() const {
      return call_method< int >(py_self, "getID");
    }

    bool isDead() const {
      return call_method< bool >(py_self, "isDead");
    }

    void markDead() {
      call_method< void >(py_self, "markDead");
    }

    PyObject* py_self;
  };

    ///A wrapper for PyAlgorithm
  struct Mantid_PythonAPI_PyAlgorithm_Wrapper: Mantid::PythonAPI::PyAlgorithm
  {
    Mantid_PythonAPI_PyAlgorithm_Wrapper(PyObject* py_self_, std::string p0):
      Mantid::PythonAPI::PyAlgorithm(p0), py_self(py_self_) {}

    const std::string name() const {
      return call_method< const std::string >(py_self, "name");
    }

    const std::string default_name() const {
      return Mantid::PythonAPI::PyAlgorithm::name();
    }

    void PyInit() {
      call_method< void >(py_self, "PyInit");
    }

    void default_PyInit() {
      Mantid::PythonAPI::PyAlgorithm::PyInit();
    }

    void PyExec() {
      call_method< void >(py_self, "PyExec");
    }

    void default_PyExec() {
      Mantid::PythonAPI::PyAlgorithm::PyExec();
    }

    PyObject* py_self;
  };
//@endcond
//@}


/// Declare functions with overloaded names
/**@name Function overloads. */
//@{
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_1, createAlgorithm, 1, 1)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_2, createAlgorithm, 2, 2)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_3, createAlgorithm, 2, 2)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_4, createAlgorithm, 3, 3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_PythonAPI_FrameworkManager_execute_overloads_1, execute, 2, 2)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_PythonAPI_FrameworkManager_execute_overloads_2, execute, 3, 3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_API_WorkspaceHistory_printSelf_overloads_1_2, printSelf, 1, 2)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Mantid_API_PropertyHistory_printSelf_overloads_1_2, printSelf, 1, 2)
//@}

} //PythonAPI namespace

} //Mantid namespace


/**
 * A macro to quickly register container classes
 */
#define REGISTER_VECTOR_WITH_PYTHON(classname, pythonname) \
  class_< std::vector<classname> >( pythonname ) \
  .def( vector_indexing_suite<std::vector<classname> >() ) \
  .def("size", &std::vector<classname>::size) \
  .def("push_back", &std::vector<classname>::push_back) \
  ;

/**
 * The actual module definition begins here. The names are different for
 * Windows and Linux due to the difference in library names
 */
#ifdef _WIN32
BOOST_PYTHON_MODULE(MantidPythonAPI)
#else
BOOST_PYTHON_MODULE(libMantidPythonAPI)
#endif
{
  /**
   * Python needs to know how to handle the some standard containers
   * @name Standard containers
   */
  //@{
  /// A vector of strings
  REGISTER_VECTOR_WITH_PYTHON(std::string, "StringVector")
  /// A vector of doubles
  REGISTER_VECTOR_WITH_PYTHON(double, "DoubleVector")
  ///  A vector of integers
  REGISTER_VECTOR_WITH_PYTHON(int, "IntVector")
  /// A vector of AlgorithmHistory objects
  REGISTER_VECTOR_WITH_PYTHON(Mantid::API::AlgorithmHistory, "AlgHistVector")
  /// A vector of PropertyHistory objects
  REGISTER_VECTOR_WITH_PYTHON(Mantid::Kernel::PropertyHistory, "PropHistVector")	  
  /// A vector of Property pointers
  REGISTER_VECTOR_WITH_PYTHON(Mantid::Kernel::Property*, "PropPtrVector")	  
  //@}

  /**
   * Expose some pointers in Python
   * @name Mantid pointers
   */
  //@{
  /// A pointer to a MatrixWorkspace object
  register_ptr_to_python< boost::shared_ptr<Mantid::API::MatrixWorkspace> >();

  /// A pointer to a TableWorkspace object
  // register_ptr_to_python< boost::shared_ptr<Mantid::DataObjects::TableWorkspace> >();

  /// A pointer to a Unit object
  register_ptr_to_python< boost::shared_ptr<Mantid::Kernel::Unit> >();

  /// A pointer to a SpectraDetectorMap object
  register_ptr_to_python< boost::shared_ptr<Mantid::API::SpectraDetectorMap> >();
  //@}
  
  //Make this namespace available
  using namespace Mantid::PythonAPI;

   //IAlgorithm Class
   class_< Mantid::API::IAlgorithm, boost::noncopyable, Mantid_API_IAlgorithm_Wrapper >("IAlgorithm", no_init)
     .def("initialize", pure_virtual(&Mantid::API::IAlgorithm::initialize))
     .def("execute", pure_virtual(&Mantid::API::IAlgorithm::execute))
     .def("isInitialized", pure_virtual(&Mantid::API::IAlgorithm::isInitialized))
     .def("isExecuted", pure_virtual(&Mantid::API::IAlgorithm::isExecuted))
     .def("setPropertyValue", pure_virtual(&Mantid::API::IAlgorithm::setPropertyValue))
     .def("getPropertyValue", pure_virtual(&Mantid::API::IAlgorithm::getPropertyValue))
     .def("getProperties", pure_virtual(&Mantid::API::IAlgorithm::getProperties), return_value_policy< copy_const_reference >())
     ;

   //PropertyManager Class
   class_< Mantid::Kernel::PropertyManager, boost::noncopyable, Mantid_Kernel_PropertyManager_Wrapper >("PropertyManager", no_init)
     .def("setProperties", &Mantid::Kernel::PropertyManager::setProperties, &Mantid_Kernel_PropertyManager_Wrapper::default_setProperties)
     .def("setPropertyValue", &Mantid::Kernel::PropertyManager::setPropertyValue, &Mantid_Kernel_PropertyManager_Wrapper::default_setPropertyValue)
     .def("setPropertyOrdinal", &Mantid::Kernel::PropertyManager::setPropertyOrdinal, &Mantid_Kernel_PropertyManager_Wrapper::default_setPropertyOrdinal)
     .def("existsProperty", &Mantid::Kernel::PropertyManager::existsProperty, &Mantid_Kernel_PropertyManager_Wrapper::default_existsProperty)
     .def("validateProperties", &Mantid::Kernel::PropertyManager::validateProperties, &Mantid_Kernel_PropertyManager_Wrapper::default_validateProperties)
     .def("getPropertyValue", &Mantid::Kernel::PropertyManager::getPropertyValue, &Mantid_Kernel_PropertyManager_Wrapper::default_getPropertyValue)
     .def("getProperties", &Mantid::Kernel::PropertyManager::getProperties, &Mantid_Kernel_PropertyManager_Wrapper::default_getProperties, return_value_policy< copy_const_reference >())
     ;

   //Algorithm Class
   class_< Mantid::API::Algorithm, bases<Mantid::Kernel::PropertyManager, Mantid::API::IAlgorithm>, boost::noncopyable, Mantid_API_Algorithm_Wrapper >("Algorithm", no_init)
     .def("name", &Mantid::API::Algorithm::name, &Mantid_API_Algorithm_Wrapper::default_name)
     .def("version", &Mantid::API::Algorithm::version, &Mantid_API_Algorithm_Wrapper::default_version)
     .def("category", &Mantid::API::Algorithm::category, &Mantid_API_Algorithm_Wrapper::default_category)
     .def("isInitialized", &Mantid::API::Algorithm::isInitialized, &Mantid_API_Algorithm_Wrapper::default_isInitialized)
     .def("isExecuted", &Mantid::API::Algorithm::isExecuted, &Mantid_API_Algorithm_Wrapper::default_isExecuted)
     .def("setPropertyOrdinal", &Mantid::API::Algorithm::setPropertyOrdinal, &Mantid_API_Algorithm_Wrapper::default_setPropertyOrdinal)
     .def("setPropertyValue", &Mantid::API::Algorithm::setPropertyValue, &Mantid_API_Algorithm_Wrapper::default_setPropertyValue)
     .def("getPropertyValue", &Mantid::API::Algorithm::getPropertyValue, &Mantid_API_Algorithm_Wrapper::default_getPropertyValue)
     .def("validateProperties", &Mantid::API::Algorithm::validateProperties, &Mantid_API_Algorithm_Wrapper::default_validateProperties)
     .def("getProperties", &Mantid::API::Algorithm::getProperties, &Mantid_API_Algorithm_Wrapper::default_getProperties, return_value_policy< copy_const_reference >())
     .def("initialize", &Mantid::API::Algorithm::initialize)
     .def("execute", &Mantid::API::Algorithm::execute)
     .def("isChild", &Mantid::API::Algorithm::isChild)
     .def("setChild", &Mantid::API::Algorithm::setChild)
     ;

   //MatrixWorkspace class
   class_< Mantid::API::MatrixWorkspace, boost::noncopyable, Mantid_API_MatrixWorkspace_Wrapper >("MatrixWorkspace", no_init)
     .def("id", pure_virtual(&Mantid::API::MatrixWorkspace::id))
     .def("initialize", &Mantid::API::MatrixWorkspace::initialize)
     .def("getMemorySize", &Mantid::API::MatrixWorkspace::getMemorySize, &Mantid_API_MatrixWorkspace_Wrapper::default_getMemorySize)
     .def("size", pure_virtual(&Mantid::API::MatrixWorkspace::size))
     .def("blocksize", pure_virtual(&Mantid::API::MatrixWorkspace::blocksize))
     .def("setTitle", &Mantid::API::MatrixWorkspace::setTitle)
     .def("setComment", &Mantid::API::MatrixWorkspace::setComment)
     .def("getNumberHistograms", pure_virtual(&Mantid::API::MatrixWorkspace::getNumberHistograms))
     .def("getComment", &Mantid::API::MatrixWorkspace::getComment, return_value_policy< copy_const_reference >())
     .def("getTitle", &Mantid::API::MatrixWorkspace::getTitle, return_value_policy< copy_const_reference >())
     //.def("isDistribution", (const bool& (Mantid::API::MatrixWorkspace::*)() const)&Mantid::API::MatrixWorkspace::isDistribution, return_value_policy< copy_const_reference >())
     .def("isDistribution", (bool& (Mantid::API::MatrixWorkspace::*)(bool))&Mantid::API::MatrixWorkspace::isDistribution, return_self<>())
     .def("readX", pure_virtual((const std::vector<double,std::allocator<double> >* (Mantid::API::MatrixWorkspace::*)(const int) const)&Mantid::API::MatrixWorkspace::readX), return_value_policy< reference_existing_object >())
     .def("readY", pure_virtual((const std::vector<double,std::allocator<double> >* (Mantid::API::MatrixWorkspace::*)(const int) const)&Mantid::API::MatrixWorkspace::readY), return_value_policy< reference_existing_object >())
     .def("readE", pure_virtual((const std::vector<double,std::allocator<double> >* (Mantid::API::MatrixWorkspace::*)(const int) const)&Mantid::API::MatrixWorkspace::readE), return_value_policy< reference_existing_object >())
     .def("getHistory", &Mantid::API::MatrixWorkspace::getHistory, return_value_policy< copy_const_reference >())
     ;

   // //TableWorkspace class
   // class_< Mantid::DataObjects::TableWorkspace, boost::noncopyable, Mantid_DataObjects_TableWorkspace_Wrapper >("TableWorkspace", no_init)
     // .def("columnCount", &Mantid::DataObjects::TableWorkspace::columnCount)
     // .def("getColumnNames",&Mantid::DataObjects::TableWorkspace::getColumnNames)
     // .def("rowCount", &Mantid::DataObjects::TableWorkspace::rowCount)
     // .def("getIntColumn", Mantid::PythonAPI::TableWorkspace_GetIntegerColumn, return_value_policy<reference_existing_object>() )
     // .def("getDoubleColumn", Mantid::PythonAPI::TableWorkspace_GetDoubleColumn, return_value_policy<reference_existing_object>() )
     // .def("getStringColumn", Mantid::PythonAPI::TableWorkspace_GetStringColumn, return_value_policy<reference_existing_object>() )
     // ;

   //Framework Class
   class_< Mantid::PythonAPI::FrameworkManager, boost::noncopyable >("FrameworkManager", init<  >())
     .def("clear", &Mantid::PythonAPI::FrameworkManager::clear)
     .def("clearAlgorithms", &Mantid::PythonAPI::FrameworkManager::clearAlgorithms)
     .def("clearData", &Mantid::PythonAPI::FrameworkManager::clearData)
     .def("clearInstruments", &Mantid::PythonAPI::FrameworkManager::clearInstruments)
     .def("createAlgorithm", (Mantid::API::IAlgorithm* (Mantid::PythonAPI::FrameworkManager::*)(const std::string&) )&Mantid::PythonAPI::FrameworkManager::createAlgorithm, return_value_policy< reference_existing_object >(), Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_1())
     .def("createAlgorithm", (Mantid::API::IAlgorithm* (Mantid::PythonAPI::FrameworkManager::*)(const std::string&, const int&) )&Mantid::PythonAPI::FrameworkManager::createAlgorithm, return_value_policy< reference_existing_object >(), Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_2())
     .def("createAlgorithm", (Mantid::API::IAlgorithm* (Mantid::PythonAPI::FrameworkManager::*)(const std::string&, const std::string&) )&Mantid::PythonAPI::FrameworkManager::createAlgorithm, return_value_policy< reference_existing_object >(), Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_3())
     .def("createAlgorithm", (Mantid::API::IAlgorithm* (Mantid::PythonAPI::FrameworkManager::*)(const std::string&, const std::string&, const int&) )&Mantid::PythonAPI::FrameworkManager::createAlgorithm, return_value_policy< reference_existing_object >(), Mantid_PythonAPI_FrameworkManager_createAlgorithm_overloads_4())
     .def("execute", (Mantid::API::IAlgorithm* (Mantid::PythonAPI::FrameworkManager::*)(const std::string&, const std::string&) )&Mantid::PythonAPI::FrameworkManager::execute, return_value_policy< reference_existing_object >(), Mantid_PythonAPI_FrameworkManager_execute_overloads_1())
     .def("execute", (Mantid::API::IAlgorithm* (Mantid::PythonAPI::FrameworkManager::*)(const std::string&, const std::string&, const int&) ) &Mantid::PythonAPI::FrameworkManager::execute, return_value_policy< reference_existing_object >(), Mantid_PythonAPI_FrameworkManager_execute_overloads_2())
     .def("getMatrixWorkspace", &Mantid::PythonAPI::FrameworkManager::getMatrixWorkspace, return_value_policy< reference_existing_object >())
//     .def("getTableWorkspace", &Mantid::PythonAPI::FrameworkManager::getTableWorkspace, return_value_policy< reference_existing_object >())
     .def("deleteWorkspace", &Mantid::PythonAPI::FrameworkManager::deleteWorkspace)
     .def("getAlgorithmNames", &Mantid::PythonAPI::FrameworkManager::getAlgorithmNames)
     .def("getWorkspaceNames", &Mantid::PythonAPI::FrameworkManager::getWorkspaceNames)
     .def("createPythonSimpleAPI", &Mantid::PythonAPI::FrameworkManager::createPythonSimpleAPI)
     .def("addPythonAlgorithm", &Mantid::PythonAPI::FrameworkManager::addPythonAlgorithm)
     .def("executePythonAlgorithm", &Mantid::PythonAPI::FrameworkManager::executePythonAlgorithm)
     ;

   //Property Class
   class_< Mantid::Kernel::Property, boost::noncopyable, Mantid_Kernel_Property_Wrapper >("Property", no_init)
     .def("isValid", &Mantid::Kernel::Property::isValid, &Mantid_Kernel_Property_Wrapper::default_isValid)
     .def("getValidatorType", &Mantid::Kernel::Property::getValidatorType, &Mantid_Kernel_Property_Wrapper::default_getValidatorType)
     .def("value", pure_virtual(&Mantid::Kernel::Property::value))
     .def("setValue", pure_virtual(&Mantid::Kernel::Property::setValue))
     .def("allowedValues", &Mantid::Kernel::Property::allowedValues, &Mantid_Kernel_Property_Wrapper::default_allowedValues)
     .def("name", &Mantid::Kernel::Property::name, return_value_policy< copy_const_reference >())
     .def("documentation", &Mantid::Kernel::Property::documentation, return_value_policy< copy_const_reference >())
     .def("type_info", &Mantid::Kernel::Property::type_info, return_value_policy< reference_existing_object >())
     .def("type", &Mantid::Kernel::Property::type)
     .def("isDefault", &Mantid::Kernel::Property::isDefault)
     .def("setDocumentation", &Mantid::Kernel::Property::setDocumentation)
     ;

   //PropertyHistory Class
   class_< Mantid::Kernel::PropertyHistory >("PropertyHistory", no_init)
     .def("name", &Mantid::Kernel::PropertyHistory::name, return_value_policy< copy_const_reference >())
     .def("value", &Mantid::Kernel::PropertyHistory::value, return_value_policy< copy_const_reference >())
     .def("type", &Mantid::Kernel::PropertyHistory::type, return_value_policy< copy_const_reference >())
     .def("isDefault", &Mantid::Kernel::PropertyHistory::isDefault, return_value_policy< copy_const_reference >())
     .def("direction", &Mantid::Kernel::PropertyHistory::direction, return_value_policy< copy_const_reference >())
     ;

   //AlgorithmHistory Class
   class_< Mantid::API::AlgorithmHistory >("AlgorithmHistory", no_init)
     .def("name", &Mantid::API::AlgorithmHistory::name, return_value_policy< copy_const_reference >())
     .def("version", &Mantid::API::AlgorithmHistory::version, return_value_policy< copy_const_reference >())
     .def("getProperties", &Mantid::API::AlgorithmHistory::getProperties, return_value_policy< copy_const_reference >())
     ;

   //WorkspaceHistory Class
   class_< Mantid::API::WorkspaceHistory >("WorkspaceHistory", no_init)
     .def("getAlgorithmHistories", (const std::vector<Mantid::API::AlgorithmHistory,std::allocator<Mantid::API::AlgorithmHistory> >& (Mantid::API::WorkspaceHistory::*)() const)&Mantid::API::WorkspaceHistory::getAlgorithmHistories, return_value_policy< copy_const_reference >())
    
     ;

   //IDetector Class
   class_< Mantid::Geometry::IDetector, boost::noncopyable, Mantid_Geometry_IDetector_Wrapper >("IDetector", no_init)
     .def("getID", pure_virtual(&Mantid::Geometry::IDetector::getID))
     .def("isMasked", pure_virtual(&Mantid::Geometry::IDetector::isMasked))
     ;

   //SpectraDetectorMap Class
   class_< Mantid::API::SpectraDetectorMap, boost::noncopyable >("SpectraDetectorMap", no_init)
     .def("remap", &Mantid::API::SpectraDetectorMap::remap)
     .def("ndet", &Mantid::API::SpectraDetectorMap::ndet)
     .def("getDetectors", &Mantid::API::SpectraDetectorMap::getDetectors)
     .def("getSpectra", &Mantid::API::SpectraDetectorMap::getSpectra)
     .def("nElements", &Mantid::API::SpectraDetectorMap::nElements)
     ;

   //PyAlgorithm Class
   class_< Mantid::PythonAPI::PyAlgorithm, boost::noncopyable, Mantid_PythonAPI_PyAlgorithm_Wrapper >("PyAlgorithm", init< std::string >())
     .def("name", &Mantid::PythonAPI::PyAlgorithm::name, &Mantid_PythonAPI_PyAlgorithm_Wrapper::default_name)
     .def("PyInit", &Mantid::PythonAPI::PyAlgorithm::PyInit, &Mantid_PythonAPI_PyAlgorithm_Wrapper::default_PyInit)
     .def("PyExec", &Mantid::PythonAPI::PyAlgorithm::PyExec, &Mantid_PythonAPI_PyAlgorithm_Wrapper::default_PyExec)
     ;

}

