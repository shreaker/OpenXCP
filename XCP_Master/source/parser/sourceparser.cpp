/**
*@file sourceparser.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The parser for the OpenXCP comments in the C-source code of the slave (XCP attributes).
*/

#include "sourceparser.h"

#include <limits>
#include <iostream>
#include <regex>

#include "parseResultStruct.h"

using std::ifstream;

SourceParser::SourceParser() {
    tokens = {
        {TokenId::Measurment, TOKEN_MEASURMENT},
        {TokenId::Characteristic, TOKEN_CHARACTERISTIC},
        {TokenId::Comment, TOKEN_COMMENT},
        {TokenId::LowerLimit, TOKEN_LOWER_LIMIT},
        {TokenId::UpperLimit, TOKEN_UPPER_LIMIT},
        {TokenId::Discrete, TOKEN_DISCRETE},
        {TokenId::MaxRefreshRate, TOKEN_MAX_REFRESH_RATE},
        {TokenId::PhysUnit, TOKEN_PHYS_UNIT},
        {TokenId::ConversionFunction, TOKEN_CONVERSION_FUNCTION},
        {TokenId::Writeable, TOKEN_WRITEABLE},
        {TokenId::StepSize, TOKEN_STEP_SIZE}
    };
    state = SourceParserStates::ResetParser;
}

void SourceParser::resetProcessLineParser() {
    isValidCommentFound = false;
    commentFoundLines = {};
    resetTemporaryComment();
}

void SourceParser::resetTemporaryComment() {
    tmpMeasurement = SourceParseMeasurement();
    tmpCharacteristic = SourceParseCharacteristic();
    pFoundCommentType = nullptr;
}

static inline bool isSameFile(const string& lastFile, const string& currentFile) {
    return (lastFile.compare(currentFile) == 0);
}

void SourceParser::parse() {
    state = SourceParserStates::ResetParser;

    string lastFile;

    for(auto &var : variablesToLookUp) {
        string line;
        int currentlineCounter = 1;

        string currentFile = var->getSourceFilePathName();

        ifstream file(currentFile);

        if(!isSameFile(lastFile, currentFile)) {
            linesToIgnore.clear();
        }

        lastFile = currentFile;
        while(getline(file, line)) {

            if(!isLineInIgnoreList(currentlineCounter)) {
                processLine(line, currentlineCounter, *var);
            }

            if(isValidCommentFound) {
                addCommentLinesToIgnoreList();
                break;
            }
            currentlineCounter++;
        }
        file.close();
        state = SourceParserStates::ResetParser;
    }

    //cleanup
    variablesToLookUp.clear();
}

void SourceParser::processLine(const string &line, int lineNr, ParseResultBase& variableToLookUp) {
    static const  std::regex COMMENT_START(R"(\s*/\*\*)");
    static const  std::regex COMMENT_END(R"(\s*\*/)");

    switch (state) {
    case SourceParserStates::ResetParser:
        resetProcessLineParser();
        state = SourceParserStates::SearchForComment;
        break;
    case SourceParserStates::SearchForComment:
        if(std::regex_match(line, COMMENT_START)) {
            state = SourceParserStates::ReadHeadToken;
            commentFoundLines.begin = lineNr;
        } else {
            linesToIgnore.insert (lineNr);
        }
        break;
    case SourceParserStates::ReadHeadToken:
        if(isTokenInLine(TokenId::Measurment, line)) {
            pFoundCommentType = &tmpMeasurement;
            state = SourceParserStates::ReadTokens;
        } else if(isTokenInLine(TokenId::Characteristic, line)) {
            pFoundCommentType = &tmpCharacteristic;
            state = SourceParserStates::ReadTokens;
        } else if(std::regex_match(line, COMMENT_END)) {
            state = SourceParserStates::SearchForComment;
        }
        break;
    case SourceParserStates::ReadTokens:
        if(std::regex_match(line, COMMENT_END)) {
            state = SourceParserStates::CheckIfCommentBelongsToVariable;
        } else {
            TokenValue tokenValue = readTokens(line);
            if(tokenValue.isTokenFound) {
                saveTokensValueTemporarily(tokenValue);
            }
        }
        break;
    case SourceParserStates::CheckIfCommentBelongsToVariable:
        if(line.find(variableToLookUp.getName())  != std::string::npos) {
            saveTokensValueToFoundVariable(variableToLookUp);
            isValidCommentFound = true;
            commentFoundLines.end = lineNr;
            resetTemporaryComment();
            state = SourceParserStates::SearchForComment;
        } else if(line.size() > 0) {
            resetTemporaryComment();
            state = SourceParserStates::SearchForComment;
        }
        break;
    default:
        break;
    }
}

bool SourceParser::isTokenInLine(const TokenId& tokenId, const string &line) {
    return (line.find(tokens.at(tokenId)) != std::string::npos);
}

SourceParser::TokenValue SourceParser::readTokens(const string &line) {
    TokenValue ret = {};
    for(const auto &token : tokens) {
        if(isTokenInLine(token.first, line)) {
            ret.isTokenFound = true;
            ret.tokenId = token.first;
            ret.value = getValueOfCommentLine(line);
            break;
        } else {
            ret.isTokenFound = false;
        }
    }
    return ret;
}

static inline void ltrim(string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

string SourceParser::getValueOfCommentLine(const string &line) {
    std::stringstream ss(line);
    string firstString, value;
    ss >> firstString;
    ss >> firstString;
    getline(ss, value);

    ltrim(value);
    rtrim(value);
    return value;
}

bool SourceParser::isLineInIgnoreList(int lineNr) {
    return (linesToIgnore.find(lineNr) != linesToIgnore.end());
}

void SourceParser::addCommentLinesToIgnoreList() {
    for(int lineNr = commentFoundLines.begin; lineNr <= commentFoundLines.end; lineNr++) {
        linesToIgnore.insert (lineNr);
    }
}

void SourceParser::saveTokensValueTemporarily(const SourceParser::TokenValue &tokenValue) {

    switch (tokenValue.tokenId) {
    case TokenId::Comment:
        pFoundCommentType->setComment(tokenValue.value);
        break;
    case TokenId::LowerLimit:
        if(isNumberFloat(tokenValue.value)) {
            pFoundCommentType->setLowerLimit(std::stod(tokenValue.value));
        }
        break;
    case TokenId::UpperLimit:
        if(isNumberFloat(tokenValue.value)) {
            pFoundCommentType->setUpperLimit(std::stod(tokenValue.value));
        }
        break;
    case TokenId::PhysUnit:
        pFoundCommentType->setPhyUnit(tokenValue.value);
        break;
    case TokenId::Discrete:
        try {
            bool boolValue = convertValueToBool(tokenValue.value);
            tmpMeasurement.setIsDiscrete(boolValue);
        } catch(const std::invalid_argument& err) {
            std::cout << err.what() << std::endl;
        }
        break;
    case TokenId::MaxRefreshRate:
        if(isNumberInt(tokenValue.value)) {
            tmpMeasurement.setMaxRefreshRate((uint32_t) std::stoul(tokenValue.value));
        }
        break;
    case TokenId::ConversionFunction:
        tmpMeasurement.setConversionFunction(tokenValue.value);
        break;
    case TokenId::Writeable:
        try {
            bool boolValue = convertValueToBool(tokenValue.value);
            tmpMeasurement.setIsWriteable(boolValue);
        } catch (const std::invalid_argument& err) {
            std::cout << err.what() << std::endl;
        }
        break;
    case TokenId::StepSize:
        if(isNumberFloat(tokenValue.value)) {
            tmpCharacteristic.setStepSize(std::stod(tokenValue.value));
        }
        break;
    default:
        break;
    }
}

void SourceParser::saveTokensValueToFoundVariable(ParseResultBase& foundVar) {

    SourceParserObj::TypeId typeId = pFoundCommentType->getTypeId();

    if(typeId == SourceParserObj::TypeId::Measurement) {
        foundVar.setSrcMeasurement(std::make_shared<SourceParseMeasurement>((dynamic_cast<SourceParseMeasurement&>(*pFoundCommentType))));
        std::cout << *foundVar.getSrcMeasurement() << std::endl;
    } else if(typeId == SourceParserObj::TypeId::Characteristic) {
        foundVar.setSrcCharateristic(std::make_shared<SourceParseCharacteristic>((dynamic_cast<SourceParseCharacteristic&>(*pFoundCommentType))));
        std::cout << *foundVar.getSrcCharateristic() << std::endl;
    }
}

bool SourceParser::convertValueToBool(const string& value) throw (std::invalid_argument) {
    if (value.compare("true") == 0) {
        return true;
    } else if(value.compare("false") == 0) {
        return false;
    } else {
        throw std::invalid_argument("not a boolean: " + value);
    }
}

/**
 * @brief isNumberFloat
 * Valid input: "123", "123.", "123.1", "123.123" and negative numbers
 * @param line
 * @return isFloat
 */
bool SourceParser::isNumberFloat(const std::string &line) {
    static const  std::regex FLOAT(R"(-?[0-9]+\.?[0-9]*)");
    return (std::regex_match(line, FLOAT));
}

bool SourceParser::isNumberInt(const std::string &line) {
    static const  std::regex INTEGER(R"([0-9]+)");
    return (std::regex_match(line, INTEGER));
}

std::ifstream& SourceParser::gotoLine(std::ifstream& file, uint32_t num) {
    file.seekg(std::ios::beg);
    for(uint32_t i=0; i < num - 1; ++i) {
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

/**************************************************************
 *  Getter / Setter
 * ***********************************************************/
void SourceParser::setVariablesToLookUp(const shared_ptr<ParseResultBase>& var) {
    if(var->getAbstractDataType().compare("struct") == 0) {
        vector<shared_ptr<ParseResultBase>> structElements = dynamic_cast<ParseResultStruct&>(*var).getElements();
        for(auto& element : structElements) {
            variablesToLookUp.push_back(element); //extract elements and add them to vector
        }
    } else {
        variablesToLookUp.push_back(var);
    }
}




