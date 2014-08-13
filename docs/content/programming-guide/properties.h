/*! \page properties Properties
 *
@section property-indices Property Indices

DALi has a property system and provides several different kinds of properties. The following table
shows the index range of the different properties in place.

<table>
  <tr> <td><b>Kind</b></td>     <td><b>Description</b></td>                                                                                <td style="text-align:center;"><b>Range</b></td>         </tr>
  <tr> <td>Default</td>         <td>Properties defined within DALi Core, e.g. Dali::Actor, Dali::ShaderEffect default properties etc.</td> <td style="text-align:center;">0 to 9999999</td>         </tr>
  <tr> <td>Registered</td>      <td>Properties registered using Dali::PropertyRegistration</td>                                            <td style="text-align:center;">10000000 to 19999999</td> </tr>
  <tr> <td>Control</td>         <td>Property range reserved by Dali::Toolkit::Control</td>                                                 <td style="text-align:center;">10000000 to 10001000<br>
                                                                                      \link Dali::Toolkit::Internal::Control::CONTROL_PROPERTY_START_INDEX CONTROL_PROPERTY_START_INDEX\endlink
                                                                                      to \link Dali::Toolkit::Internal::Control::CONTROL_PROPERTY_END_INDEX CONTROL_PROPERTY_END_INDEX\endlink</td> </tr>
  <tr> <td>Derived Control</td> <td>Property range for control deriving directly from Dali::Toolkit::Control</td>                          <td style="text-align:center;">10001001 to 19999999</td> </tr>
  <tr> <td>Custom</td>          <td>Custom properties added to instance using Dali::Handle::RegisterProperty</td>                          <td style="text-align:center;">50000000 onwards</td>     </tr>
</table>

*
*/
