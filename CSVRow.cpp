#include "CSVRow.hpp"

CSVRow::CSVRow() {
    rowID = -1;
    replaceNulls = false;
    nullReplacement = "0";
}

CSVRow::CSVRow(int row, bool replace, std::string nullReplacement)
: rowID(row), replaceNulls(replace), nullReplacement(nullReplacement) {
    //
}

struct CSVRow::Hash {
    size_t operator()(const CSVRow &row) const {
        size_t seed = 0;
        std::hash<std::string> stringHasher;
        for (const auto &entry : row.CSVData) {
            seed ^= stringHasher(entry.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= stringHasher(entry.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

bool CSVRow::operator==(const CSVRow &other) const {
    return CSVData == other.CSVData;
}

void CSVRow::deepCopyData(const CSVRow &other) {
    CSVData.clear();
    for (const auto &entry : other.CSVData) {
        CSVData[entry.first] = entry.second;
    }
    replaceNulls = other.replaceNulls;
    nullReplacement = other.nullReplacement;
}

std::string CSVRow::sumNumericalData(std::vector<std::string> &colNames) {
    double sum = 0;
    for (const auto col : colNames) {
        if (CSVData[col] != "") {
            sum += std::stod(CSVData[col]);
        }
    }
    return std::to_string(sum);
}

std::string CSVRow::subtractNumericalData(std::string col1, std::string col2) {
    double result = 0;
    if (CSVData[col1] != "") {
        result += std::stod(CSVData[col1]);
    }
    if (CSVData[col2] != "") {
        result -= std::stod(CSVData[col2]);
    }
    return std::to_string(result);
}

int CSVRow::matchCount(const std::string &eq) {
    int counter = 0;
    for (const auto &entry : CSVData) {
        if (entry.second == eq) {
            ++counter;
        }
    }
    return counter;
}

void CSVRow::addItem(std::string col, std::string val) {
    CSVData[col] = (val == "" && replaceNulls) ? nullReplacement : val;
}

void CSVRow::printRow(const std::vector<std::string> &headers) {
    for (const auto &header : headers) {
        std::cout << CSVData[header] << "\t";
    }
    std::cout << std::endl;
}

void CSVRow::removeNull(std::vector<std::string> colNames, std::string replace) {
    for (const auto col : colNames) {
        if (CSVData[col] == "" || CSVData[col] == "NaN") {
            CSVData[col] = replace;
        }
    }
}

int CSVRow::getIndex(std::vector<CSVRow> &rows) {
    auto it = std::find(rows.begin(), rows.end(), *this);
    if (it != rows.end()) {
        return std::distance(rows.begin(), it);
    }
    else {
        return -1;
    }
}

std::string CSVRow::getData(std::string col) {
    return CSVData[col];
}

void CSVRow::setData(std::string data, std::string col) {
    CSVData[col] = data;
}

void CSVRow::dropCols(std::vector<std::string> cols) {
    for (const auto &col : cols) {
        CSVData.erase(col);
    }
}

void CSVRow::renameCol(std::string oldCol, std::string newCol) {
    std::string val = CSVData[oldCol];
    CSVData.erase(oldCol);
    CSVData[newCol] = val;
} 

std::string CSVRow::getRowStr(std::vector<std::string> header, char sep) {
    std::string rowStr;
    for (size_t i = 0; i < header.size(); ++i) {
        if (i > 0) {
            rowStr += sep;
        }
        rowStr += CSVData[header[i]];
    }
    return rowStr;
}
