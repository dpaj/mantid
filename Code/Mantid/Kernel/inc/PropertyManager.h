#ifndef MANTID_KERNEL_PROPERTYMANAGER_H_
#define MANTID_KERNEL_PROPERTYMANAGER_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "IProperty.h"
#include <vector>

namespace Mantid
{
namespace Kernel
{
//----------------------------------------------------------------------
// Forward declaration
//----------------------------------------------------------------------
class Property;

/** @class PropertyManager PropertyManager.h Kernel/PropertyManager.h

    Property manager helper class.
    This class is used by algorithms and services for helping to manage their own set of properties.
    It implements the IProperty interface.

    @author Russell Taylor, Tessella Support Services plc
    @author Based on the Gaudi class PropertyMgr (see http://proj-gaudi.web.cern.ch/proj-gaudi/)
    @date 20/11/2007
    
    Copyright &copy; 2007 STFC Rutherford Appleton Laboratories

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport PropertyManager //: public IProperty
{
public:
	PropertyManager();
	virtual ~PropertyManager();
	
	// Overloaded functions to declare properties (i.e. store them)
	void declareProperty( Property *p );
	void declareProperty( const std::string &name, int value, const std::string &doc="" );
  void declareProperty( const std::string &name, double value, const std::string &doc="" );
  void declareProperty( const std::string &name, std::string value, const std::string &doc="" );
  
  // IProperty methods
  void setProperty( const std::string &name, const std::string &value );
  bool checkProperty( Property *p ) const;
  std::string getPropertyValue( const std::string &name ) const;
  Property* getProperty( std::string name ) const;
  const std::vector< Property* >& getProperties() const;
	
private:
  bool checkProperty( const std::string& name) const;
  
  /// The properties under management
  std::vector<Property*> m_properties;
};

} // namespace Kernel
} // namespace Mantid

#endif /*MANTID_KERNEL_PROPERTYMANAGER_H_*/
