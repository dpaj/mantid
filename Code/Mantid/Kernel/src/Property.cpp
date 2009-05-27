#include "MantidKernel/Property.h"
#include "MantidKernel/Exception.h"

namespace Mantid
{
namespace Kernel
{

/** Constructor
 *  @param name The name of the property
 *  @param type The type of the property
 *  @param direction Whether this is a Direction::Input, Direction::Output or Direction::InOut (Input & Output) property
 */
Property::Property( const std::string &name, const std::type_info &type, const unsigned int direction ) :
  m_name( name ),
  m_documentation( "" ),
  m_typeinfo( &type ),
  m_direction( direction )
{
    // Make sure a random int hasn't been passed in for the direction
    // Property & PropertyWithValue destructors will be called in this case
    if (m_direction > 2) throw std::out_of_range("direction should be a member of the Direction enum");
}

/// Copy constructor
Property::Property( const Property& right ) :
  m_name( right.m_name ),
  m_documentation( right.m_documentation ),
  m_typeinfo( right.m_typeinfo ),
  m_direction( right.m_direction )
{
}

/// Virtual destructor
Property::~Property()
{
}

/** Copy assignment operator. Does nothing.
* @param right The right hand side value
*/
Property& Property::operator=( const Property& right )
{
  return *this;
}

/** Get the property's name
 *  @return The name of the property
 */
const std::string& Property::name() const
{
  return m_name;
}

/** Get the property's documentation string
 *  @return The documentation string
 */
const std::string& Property::documentation() const
{
  return m_documentation;
}

/** Get the property type_info
 *  @return The type of the property
 */
const std::type_info* Property::type_info() const
{
  return m_typeinfo;
}

/** Returns the type of the property as a string.
 *  Note that this is implementation dependent.
 *  @return The property type
 */
const std::string Property::type() const
{
  return m_typeinfo->name();
}

/** Overridden functions checks whether the property has a valid value.
 *  
 *  @return empty string ""
 */
std::string Property::isValid() const
{
  // the no error condition
  return "";
}

  /** Returns the type of the validator as a string
   *  \returns Returns ""
   */
 const std::string Property::getValidatorType() const
  {
    return "";
  }

/** Sets the property's (optional) documentation string
 *  @param documentation The string containing the descriptive comment
 */
void Property::setDocumentation( const std::string& documentation )
{
  m_documentation = documentation;
}

/** Returns the set of valid values for this property, if such a set exists.
 *  If not, it returns an empty set.
 */
const std::vector<std::string> Property::allowedValues() const
{
  return std::vector<std::string>();
}

/// Create a PropertyHistory object representing the current state of the Property.
const PropertyHistory Property::createHistory() const
{
  return PropertyHistory(this->name(),this->value(),this->type(),this->isDefault(),this->direction());
}


} // namespace Kernel
} // namespace Mantid
