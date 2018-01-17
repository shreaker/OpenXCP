/**
*@file sourceparserconstants.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Tokens for the SourceParser.
*/

#ifndef SOURCEPARSERCONSTANTS_H
#define SOURCEPARSERCONSTANTS_H

#define TOKEN_PREFIX "@XCP_"

#define TOKEN_MEASURMENT            TOKEN_PREFIX "Measurement"
#define TOKEN_CHARACTERISTIC        TOKEN_PREFIX "Characteristic"

#define TOKEN_COMMENT               TOKEN_PREFIX "Comment"
#define TOKEN_LOWER_LIMIT           TOKEN_PREFIX "LowerLimit"
#define TOKEN_UPPER_LIMIT           TOKEN_PREFIX "UpperLimit"
#define TOKEN_DISCRETE              TOKEN_PREFIX "Discrete"
#define TOKEN_MAX_REFRESH_RATE      TOKEN_PREFIX "MaxRefreshRate"
#define TOKEN_PHYS_UNIT             TOKEN_PREFIX "PhysUnit"
#define TOKEN_CONVERSION_FUNCTION   TOKEN_PREFIX "ConversionFunction"
#define TOKEN_WRITEABLE             TOKEN_PREFIX "Writeable"
#define TOKEN_STEP_SIZE             TOKEN_PREFIX "StepSize"

enum class TokenId {Measurment, Characteristic, Comment, LowerLimit, UpperLimit, Discrete, MaxRefreshRate, PhysUnit, ConversionFunction, Writeable, StepSize};


#endif // SRCPARSERCONSTANTS_H
