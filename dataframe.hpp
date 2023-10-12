/*
 * dataframe.hpp
 *
 * URL:      https://github.com/xbandrade/cpp-dataframe
 *
 *
 */


#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP

#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <cmath>
#include "nlohmann/json.hpp"
#include "rapidcsv.h"
#include "CSVRow.hpp"
#include "utils.hpp"

using json = nlohmann::json;

class Dataframe {
private:
    rapidcsv::Document doc;
    std::vector<std::string> headers;
    std::vector<CSVRow> rows;
    bool isReplacingNulls;
    std::string nullReplacement;
    void printHeaders(const std::vector<std::string> &headers);

public:
    Dataframe();
    Dataframe(const std::string path, char sep = ',', int rowSkip = 0);
    Dataframe(const Dataframe &other);
    Dataframe(std::vector<std::vector<std::string>> &dfData);
    std::vector<std::string> getHeaders();
    std::vector<CSVRow> getRows();
    int getRowIndex(CSVRow &row);
    void readData();
    void renameColumns(const std::vector<std::pair<std::string, std::string>> &columns, bool renameCSVRowMaps = true);
    void printDataframe();
    int getColumnIndex(std::string colName);
    void setReplaceNull(bool rep);
    void setNullReplacement(std::string rep);
    void replaceNull(std::string replace, const std::vector<std::string> &colNames);
    // void dropDuplicates(); // FIXME
    void dropColumns(const std::vector<std::string> &columns);
    void renameHeader(const std::vector<std::string> &columns);
    void setValue(const std::string &colName, int row, const std::string &value);
    std::string getValue(const std::string &colName, int row);
    std::string sumDigitStr(std::string str1, std::string str2);
    std::vector<std::string> getColumn(const std::string &colName);
    void loc(std::vector<std::string> &colNames);
    template<typename T1, typename T2> void dropColumns(T1 &&start, T2 &&end);
    void removeSpecialCharacters(const std::vector<std::string> &specialCharacters, const std::string &col);
    std::unordered_map<std::string, std::string> prepareMerge(const std::vector<std::string> &rightHeaders,
        const std::vector<std::string> &colNames, std::string suffixLeft, std::string suffixRight, std::string defaultValue);
    void createNewColumn(const std::string &colName, const std::string &baseColumn, const std::string &op, double value = 1.);
    void createNewColumn(const std::string &colName, std::string &defaultValue);
    void createNewColumn(const std::string &colName, std::vector<std::string> &sumColumns);
    void createNewColumn(const std::string &colName, std::string &col1, std::string &col2);
    // void createNewColumnDiv(const std::string &colName, const std::string &col1, const std::string &col2);
    // void createNewColumnMult(const std::string &colName, const std::string &col1, const std::string &col2);
    void setColumnValue(const std::string &colName, const std::string &value);
    void createEqMatchColumn(const std::string &colName, const std::string &eq);
    bool operationResult(CSVRow &row, const std::string &col, const std::string &op, std::string &value);
    bool operationResult(CSVRow &row, const std::string &col, const std::string &op, double value);
    template<typename T> void filterRows(const std::string &colName, const std::string &op, T value);
    void merge(Dataframe &df, std::vector<std::string> &colNames,
        const std::string &suffixLeft, const std::string &suffixRight, const std::string &defaultValue);
    void groupBy(const std::vector<std::string> &colNames);
    std::vector<std::string> unique(const std::string &col);
    std::string sum(const std::string &col);
    void sliceValues(const std::string &col, int start, int end);
    void roundDouble(const std::string &col, int precision);
    void sortBy(const std::string &colName, bool ascending = true);
    void concatCol(Dataframe &df);
    void concatRow(Dataframe &df);
    std::string max(const std::string &col);
    void saveToCSV(char sep, std::string filename, bool header = true);
};

#endif  // DATAFRAME_HPP
