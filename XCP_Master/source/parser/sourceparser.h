/**
*@file sourceparser.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The parser for the OpenXCP comments in the C-source code of the slave (XCP attributes).
*/

#ifndef SOURCEPARSER_H
#define SOURCEPARSER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <fstream>
#include <stdexcept>
#include <memory>

#include "sourceParserobj.h"
#include "sourceparsemeasurement.h"
#include "sourceparsecharacteristic.h"

#include "sourceparserconstants.h"


#include "parseResultBase.h"

using std::string;
using std::vector;
using std::map;
using std::shared_ptr;

enum class SourceParserStates {ResetParser, SearchForComment, ReadHeadToken, ReadTokens, CheckIfCommentBelongsToVariable};

class SourceParser {
  public:
    SourceParser();
    SourceParser(const string &srcFilePathName);
    void parse();

    void setVariablesToLookUp(const shared_ptr<ParseResultBase>& var);

  private:
    SourceParserStates state;
    bool isValidCommentFound;
    struct CommentFoundLines {
        int begin;
        int end;
    };
    CommentFoundLines commentFoundLines;
    std::unordered_set<int> linesToIgnore;
    vector<shared_ptr<ParseResultBase>> variablesToLookUp;
    map<const TokenId, const string> tokens;
    SourceParserObj* pFoundCommentType;
    SourceParseMeasurement tmpMeasurement;
    SourceParseCharacteristic tmpCharacteristic;


    std::ifstream& gotoLine(std::ifstream& file, uint32_t num);
    void processLine(const string &line,int lineNr, ParseResultBase& variableToLookUp);

    struct TokenValue {
        bool isTokenFound;
        TokenId tokenId;
        string value;
    };
    TokenValue readTokens(const string& line);

    void saveTokensValueTemporarily(const TokenValue& tokenValue);

    void saveTokensValueToFoundVariable(ParseResultBase& foundVar);

    void resetProcessLineParser();
    void resetTemporaryComment();
    bool isTokenInLine(const TokenId& tokenId, const string& line);
    string getValueOfCommentLine(const string& line);

    bool isLineInIgnoreList(int lineNr);
    void addCommentLinesToIgnoreList();

    bool convertValueToBool(const string& value) throw (std::invalid_argument);
    bool isNumberFloat(const string& line);
    bool isNumberInt(const string& line);
};

#endif // SRCPARSER_H
