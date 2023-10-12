#ifndef CSVROW_HPP
#define CSVROW_HPP

#include <iostream>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>


class CSVRow {
private:
    int rowID;
    bool replaceNulls;
    std::string nullReplacement;
    std::unordered_map<std::string, std::string> CSVData;

public:
    CSVRow();
    CSVRow(int row, bool replace, std::string nullReplacement);
    struct Hash;
    bool operator==(const CSVRow &other) const;
    int getIndex(std::vector<CSVRow> &rows);
    void deepCopyData(const CSVRow &other);
    void addItem(std::string col, std::string val);
    void printRow(const std::vector<std::string> &headers);
    void removeNull(std::vector<std::string> colNames, std::string replace);
    std::string getData(std::string col);
    void setData(std::string data, std::string col);
    void dropCols(std::vector<std::string> cols);
    std::string sumNumericalData(std::vector<std::string> &colNames);
    std::string subtractNumericalData(std::string col1, std::string col2);
    void renameCol(std::string oldCol, std::string newCol);
    int matchCount(const std::string &eq);
    std::string getRowStr(std::vector<std::string> header, char sep);
};

#endif  // CSVROW_HPP