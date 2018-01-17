 /**
* @file  xcp_comment_template.c
* @author Wolf Michael
* @brief XCP Comment Example integrated in Doxygen.
* Use custom command definition "xrefitem" to get XCP commands parsed by Doxygen.
*/

 //xcp_measurement
 /**
 * @brief Doxygen Comment
 *
 * @XCP_Measurement
 * @XCP_Comment Vehicle speed
 * @XCP_LowerLimit 0
 * @XCP_UpperLimit 1000
 * @XCP_Discrete false
 * @XCP_MaxRefreshRate 10
 * @XCP_PhysUnit km/h
 * @XCP_ConversionFunction
 * @XCP_ReadWrite ro
 */
 uint16 speed;

//xcp_characteristic 
/**
 * @brief Doxygen Comment
 *
 * @XCP_Characteristic
 * @XCP_Comment 
 * @XCP_LowerLimit 0
 * @XCP_UpperLimit 255
 * @XCP_PhysUnit 
 * @XCP_StepSize 1
 */
 uint8 modifyMe;
 
/**
* @brief XCP command definition for doxygen config.
*
* ALIASES                = "XCP_Measurement=\xrefitem XCP_Measurements \"XCP_Measurement\" \"XCP_Measurements\"" \
*                         "XCP_Comment=\xrefitem XCP_Comments \"XCP_Comment\" \"XCP_Comments\"" \
*                         "XCP_LowerLimit=\xrefitem XCP_LowerLimits \"XCP_LowerLimit\" \"XCP_LowerLimits\"" \
*                         "XCP_UpperLimit=\xrefitem XCP_UpperLimits \"XCP_UpperLimit\" \"XCP_UpperLimits\"" \
*                         "XCP_Discrete=\xrefitem XCP_Discretes \"XCP_Discrete\" \"XCP_Discretes\"" \
*                         "XCP_MaxRefreshRate=\xrefitem XCP_MaxRefreshRates \"XCP_MaxRefreshRate\" \"XCP_MaxRefreshRates\"" \
*                         "XCP_PhysUnit=\xrefitem XCP_PhysUnits \"XCP_PhysUnit\" \"XCP_PhysUnits\"" \
*                         "XCP_ConversionFunction=\xrefitem XCP_ConversionFunctions \"XCP_ConversionFunction\" \"XCP_ConversionFunctions\"" \
*                         "XCP_ReadWrite=\xrefitem XCP_ReadWrites \"XCP_ReadWrite\" \"XCP_ReadWrites\"" \
*                         "XCP_Characteristic=\xrefitem XCP_Characteristics \"XCP_LowerLimit\" \"XCP_Characteristics\"" \
*                         "XCP_StepSize=\xrefitem XCP_StepSizes \"XCP_StepSize\" \"XCP_StepSizes\""
*/