#include "dataframe.hpp"

template void Dataframe::filterRows<std::string>(const std::string &colName, const std::string &op, std::string value);
template void Dataframe::filterRows<double>(const std::string &colName, const std::string &op, double value);
template void Dataframe::dropColumns<std::string, std::string>(std::string &&start, std::string &&end);
template void Dataframe::dropColumns<const char *, const char *>(const char* &&start, const char* &&end);

Dataframe::Dataframe() {
    isReplacingNulls = true;
    nullReplacement = "0";
}

Dataframe::Dataframe(const std::string path, char sep, int rowSkip) {
    doc = rapidcsv::Document(
        path,
        rapidcsv::LabelParams(rowSkip, -1),
        rapidcsv::SeparatorParams(sep)
    );
    headers = doc.GetColumnNames();
    readData();
}

Dataframe::Dataframe(const Dataframe &other) {
    headers.resize(other.headers.size());
    for (size_t i = 0; i < other.headers.size(); ++i) {
        headers[i] = other.headers[i];
    }
    this->isReplacingNulls = other.isReplacingNulls;
    this->nullReplacement = other.nullReplacement;
    for (size_t i = 0; i < other.rows.size(); ++i) {
        CSVRow row(i, isReplacingNulls, nullReplacement);
        row.deepCopyData(other.rows[i]);
        rows.push_back(row);
    }
}

Dataframe::Dataframe(std::vector<std::vector<std::string>> &dfData) {
    headers.resize(dfData[0].size());
    for (size_t i = 0; i < dfData[0].size(); ++i) {
        headers[i] = dfData[0][i];
    }
    this->isReplacingNulls = true;
    this->nullReplacement = "0";
    for (size_t i = 1; i < dfData.size(); ++i) {
        CSVRow row(i, isReplacingNulls, nullReplacement);
        for (size_t j = 0; j < dfData[i].size(); ++j) {
            row.addItem(headers[j], dfData[i][j]);
        }
        rows.push_back(row);
    }
}

int Dataframe::getColumnIndex(std::string colName) {
    auto it = std::find(headers.begin(), headers.end(), colName);
    if (it != headers.end()) {
        return std::distance(headers.begin(), it);
    }
    return -1;
}

void Dataframe::renameColumns(const std::vector<std::pair<std::string, std::string>> &columns, bool renameCSVRowMaps) {
    for (const auto &p : columns) {
        std::string currName = p.first;
        std::string newName = p.second;
        int index = getColumnIndex(currName);
        if (index >= 0) {
            headers[index] = newName;
            if (renameCSVRowMaps) {
                for (auto &row : rows) {
                    row.renameCol(currName, newName);
                }
            }
        }
        else {
            std::cout << "Column not found: " << currName << std::endl;
        }
    }
}

void Dataframe::readData() {
    for (size_t i = 0; i < doc.GetRowCount(); ++i) {
        CSVRow rowData(i, isReplacingNulls, nullReplacement);
        for (size_t j = 0; j < headers.size(); ++j) {
            rowData.addItem(headers[j], doc.GetCell<std::string>(j, i));
        }
        rows.push_back(rowData);
    }
}


std::vector<std::string> Dataframe::getHeaders() {
    return headers;
}

void Dataframe::printHeaders(const std::vector<std::string> &headers) {        
    for (const auto &header : headers) {
        std::cout << header << "\t";
    }
    std::cout << std::endl;
}

void Dataframe::printDataframe() {
    printHeaders(headers);
    for (auto &row : rows){
        row.printRow(headers);
    }
}

void Dataframe::replaceNull(std::string replace, const std::vector<std::string> &colNames) {
    if (!colNames.empty()) {
        for (auto &row : rows) {
            row.removeNull(colNames, replace);
        }
    }
    else {
        for (auto &row : rows) {
            row.removeNull(headers, replace);
        }
    }
}

void Dataframe::setReplaceNull(bool rep) {
    isReplacingNulls = rep;
}

void Dataframe::setNullReplacement(std::string rep) {
    nullReplacement = rep;
}

/* void Dataframe::dropDuplicates() {
    std::unordered_set<CSVRow, CSVRow::Hash> uniqueRows;
    auto it = rows.begin();
    while (it != rows.end()) {
        if (!uniqueRows.insert(*it).second) {
            it = rows.erase(it);
        }
        else {
            ++it;
        }
    }
} */ // FIXME

void Dataframe::dropColumns(const std::vector<std::string> &columns) {
    for (auto &row : rows) {
        row.dropCols(columns);
    }
    auto it = headers.begin();
    headers.erase(
        std::remove_if(headers.begin(), headers.end(), [&](const std::string &col) {
            return std::find(columns.begin(), columns.end(), col) != columns.end();
        }), headers.end()
    );
}

// Start and end inclusive
template<typename T1, typename T2>
void Dataframe::dropColumns(T1 &&start, T2 &&end) {
    std::string startStr(std::forward<T1>(start));
    std::string endStr(std::forward<T2>(end));
    if (startStr == "") {
        if (endStr == "") {
            return;
        }
        startStr = headers[0];
    }
    std::vector<std::string> toRemove;
    bool startFound = false;
    for (const auto &h : headers) {
        if (h == end) {
            break;
        }
        else if (h == start) {
            startFound = true;
        }
        if (startFound) {
            toRemove.push_back(h);
        }
    }
    dropColumns(toRemove);
}


void Dataframe::removeSpecialCharacters(const std::vector<std::string> &specialCharacters, const std::string &col) {
    std::string replace = "";
    for (auto &row : rows) {
        std::string input = row.getData(col);
        for (const std::string &c : specialCharacters) {
            size_t pos = 0;
            while ((pos = input.find(c, pos)) != std::string::npos) {
                input.replace(pos, c.length(), replace);
                pos += replace.length();
            }
        }
        row.setData(input, col);
    }
}

/*
void Dataframe::createNewColumnDiv(const std::string &colName, const std::string &col1, const std::string &col2) {
    headers.push_back(colName);
    for (auto i = 0; i < rows.size(); ++i) {
        double cellVal = std::stod(rows[i].getData(col1));
        double cellVal2 = std::stod(rows[i].getData(col2));
        if (cellVal2 == 0) {
            rows[i].addItem(colName, "0");
        }
        else {
            rows[i].addItem(colName, std::to_string(cellVal / cellVal2));
        }
    }
}

void Dataframe::createNewColumnMult(const std::string &colName, const std::string &col1, const std::string &col2) {
    headers.push_back(colName);
    for (auto i = 0; i < rows.size(); ++i) {
        double cellVal = std::stod(rows[i].getData(col1));
        double cellVal2 = std::stod(rows[i].getData(col2));
        rows[i].addItem(colName, std::to_string(cellVal * cellVal2));
    }
}*/


void Dataframe::createNewColumn(const std::string &colName, std::string &defaultValue) {
    headers.push_back(colName);
    for (auto &row : rows) {
        row.addItem(colName, defaultValue);
    }
}

void Dataframe::createNewColumn(const std::string &colName, std::vector<std::string> &sumColumns) {
    headers.push_back(colName);
    for (auto &row : rows) {
        row.addItem(colName, row.sumNumericalData(sumColumns));
    }
}

void Dataframe::createNewColumn(const std::string &colName, std::string &col1, std::string &col2) {
    headers.push_back(colName);
    for (auto &row : rows) {
        row.addItem(colName, row.subtractNumericalData(col1, col2));
    }
}

void Dataframe::createNewColumn(const std::string &colName, const std::string &baseColumn, const std::string &op, double value) {
    double result;
    headers.push_back(colName);
    for (auto &row : rows) {
        double cellVal = std::stod(row.getData(baseColumn));
        if (op == "/") {
            result = cellVal / value;
        }
        else if (op == "*") {
            result = cellVal * value;
        }
        else if (op == "+") {
            result = cellVal + value;
        }
        else if (op == "-") {
            result = cellVal - value;
        }
        else {
            result = cellVal;
        }
        row.addItem(colName, std::to_string(result));
    }
}

int Dataframe::getRowIndex(CSVRow &row) {
    auto it = std::find(rows.begin(), rows.end(), row);
    if (it != rows.end()) {
        return std::distance(rows.begin(), it);
    }
    else {
        return -1;
    }
}

void Dataframe::concatCol(Dataframe &df) {
    std::vector<std::string> newHeaders = df.getHeaders();
    headers.insert(headers.end(), newHeaders.begin(), newHeaders.end());
    for (auto i = 0; i < df.getRows().size(); ++i) {
        for (auto &col : newHeaders) {
            setValue(col, i, df.getValue(col, i));
        }
    }
}

void Dataframe::concatRow(Dataframe &df) {
    std::vector<std::string> newHeaders = df.getHeaders();
    for (auto &row : df.getRows()) {
        CSVRow newRow;
        newRow.deepCopyData(row);
        rows.push_back(newRow);
    }
}

std::string Dataframe::max(const std::string &col) {
    double maxVal = -999999.;
    for (auto &row : rows) {
        double cellVal = std::stod(row.getData(col));
        if (cellVal > maxVal) {
            maxVal = cellVal;
        }
    }
    return std::to_string(maxVal);
} 

std::string Dataframe::sum(const std::string &col) {
    double sum = 0;
    for (auto &row : rows) {
        double cellVal = std::stod(row.getData(col));
        sum += cellVal;
    }
    return std::to_string(sum);
} 

std::vector<std::string> Dataframe::unique(const std::string &col) {
    std::unordered_set<std::string> uniqueValues;
    std::vector<std::string> result;
    for (auto &row : rows) {
        uniqueValues.insert(row.getData(col));
    }
    std::copy(uniqueValues.begin(), uniqueValues.end(), std::back_inserter(result));
    return result;
}

void Dataframe::roundDouble(const std::string &col, int precision) {
    for (auto &row : rows) {
        double cellVal = std::stod(row.getData(col));
        double multiplier = std::pow(10, precision);
        double rounded = std::round(cellVal * multiplier) / multiplier;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << rounded;
        row.setData(ss.str(), col);
    }
}

void Dataframe::sliceValues(const std::string &col, int start, int end) {
    for (auto &row : rows) {
        std::string data = row.getData(col);
        if (start < 0) {
            start = 0;
        }
        if (end > data.size()) {
            end = data.size();
        }
        std::string slicedData = data.substr(start, end - start);
        row.setData(slicedData, col);
    }
}

bool Dataframe::operationResult(CSVRow &row, const std::string &col, const std::string &op, std::string &value) {
    std::string data = row.getData(col);
    if (op == "==") {
        return data == value;
    }
    else if (op == "!=") {
        std::vector<std::string> splitValues = splitStr(value, ',');
        for (auto &st : splitValues) {
            if (data == value) {
                return false;
            }
        }
        return true;
    }
    else if (op == "in") {
        size_t pos = data.find(value);
        return pos != std::string::npos;
    }
    else if (op == "or") {
        std::vector<std::string> splitValues = splitStr(value, ',');
        for (auto &st : splitValues) {
            size_t pos = data.find(st);
            if (pos != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    else if (op == "~") {
        size_t pos = data.find(value);
        return pos == std::string::npos;
    }
    else if (op == "startswith") {
        if (value.size() <= data.size()) {
            std::string substrData = data.substr(0, value.size());
            return substrData == value;
        }
        return false;
    }
    return true;
}


bool Dataframe::operationResult(CSVRow &row, const std::string &col, const std::string &op, double value) {
    std::string data = row.getData(col);
    if (op == "len"){
        int length = static_cast<int>(value);
        return data.size() == length;
    }
    double doubleData = std::stod(data);
    if (op == "==") {
        return doubleData == value;
    }
    else if (op == "!=") {
        return doubleData != value;
    }
    else if (op == "<") {
        return doubleData < value;
    }
    else if (op == "<=") {
        return doubleData <= value;
    }
    else if (op == ">") {
        return doubleData > value;
    }
    else if (op == ">=") {
        return doubleData >= value;
    }
    return true;
}

template<typename T> 
void Dataframe::filterRows(const std::string &colName, const std::string &op, T value) {
    auto it = rows.begin();
    while (it != rows.end()) {
        if (!operationResult(*it, colName, op, value)) {
            it = rows.erase(it);
        }
        else {
            ++it;
        }
    }
}
std::vector<CSVRow> Dataframe::getRows() {
    return rows;
}

void Dataframe::renameHeader(const std::vector<std::string> &columns) {
    headers.clear();
    for (const auto &col : columns) {
        headers.push_back(col);
    }
}

std::string Dataframe::sumDigitStr(std::string str1, std::string str2) {
    try {
        double value1 = std::stod(str1);
        double value2 = std::stod(str2);
        return std::to_string(value1 + value2);
    } catch (const std::invalid_argument& ex) {
        return str1;
    }
}

void Dataframe::createEqMatchColumn(const std::string &colName, const std::string &eq) {
    headers.push_back(colName);
    for (auto i = 0; i < rows.size(); ++i) {
        int matches = rows[i].matchCount(eq);
        setValue(colName, i, std::to_string(matches));
    }
}

void Dataframe::sortBy(const std::string &colName, bool ascending) {
    auto compareRows = [colName, ascending](CSVRow& row1, CSVRow& row2) {
        double v1 = std::stod(row1.getData(colName));
        double v2 = std::stod(row2.getData(colName));
        if (ascending) {
            return v1 < v2;
        }
        else {
            return v1 >= v2;
        }
    };
    std::sort(rows.begin(), rows.end(), compareRows);
}

void Dataframe::groupBy(const std::vector<std::string> &colNames) {
    std::map<std::string, std::vector<CSVRow>> groups;    
    std::vector<std::string> remainingHeaders;
    std::vector<int> toRemove;
    std::copy_if(
        headers.begin(), headers.end(), std::back_inserter(remainingHeaders),
        [&](const std::string &col) {
            return std::find(colNames.begin(), colNames.end(), col) == colNames.end();
        }
    );

    for (CSVRow &row : rows) {
        std::string key;
        for (const std::string &col : colNames) {
            key += row.getData(col);
        }
        groups[key].push_back(row);
    }
    if (!groups.empty()) {
        for (auto groupIt = groups.begin(); groupIt != groups.end(); ++groupIt) {
            std::vector<CSVRow> &currentGroupRows = groupIt->second;
            const int baseRowIdx = getRowIndex(currentGroupRows.front());
            for (auto rowIt = std::next(currentGroupRows.begin()); rowIt != currentGroupRows.end(); ++rowIt) {
                CSVRow currentRow = *rowIt;
                for (const auto &col : remainingHeaders) {
                    std::string newVal = sumDigitStr(rows[baseRowIdx].getData(col), currentRow.getData(col));
                    rows[baseRowIdx].setData(newVal, col);
                }
                toRemove.push_back(getRowIndex(currentRow));
            }
        }
    }
    std::unordered_set<int> indicesToRemove(toRemove.begin(), toRemove.end());
    std::vector<CSVRow> newRows;
    newRows.reserve(rows.size() - toRemove.size());
    for (int i = 0; i < rows.size(); ++i) {
        if (indicesToRemove.find(i) == indicesToRemove.end()) {
            newRows.push_back(rows[i]);
        }
    }
    rows = newRows;
}

void Dataframe::loc(std::vector<std::string> &colNames) {
    std::vector<std::string> toRemove;
    for (const std::string& header : headers) {
        if (std::find(colNames.begin(), colNames.end(), header) == colNames.end()) {
            toRemove.push_back(header);
        }
    }
    dropColumns(toRemove);
}

std::unordered_map<std::string, std::string> Dataframe::prepareMerge(const std::vector<std::string> &rightHeaders,
                                                                      const std::vector<std::string> &colNames,
                                                                      std::string suffixLeft,
                                                                      std::string suffixRight,
                                                                      std::string defaultValue) {
    std::unordered_map<std::string, std::string> rightMap;
    for (const std::string &col : rightHeaders) {
        if (std::find(colNames.begin(), colNames.end(), col) == colNames.end()) {
            if (std::find(headers.begin(), headers.end(), col) == headers.end()) {
                // createNewColumn(col, defaultValue);
                rightMap[col] = col;
                headers.push_back(col);
            }
            else {
                std::string newNameLeft = col + suffixLeft;
                std::string newNameRight = col + suffixRight;
                renameColumns({{col, newNameLeft}});
                headers.push_back(newNameRight);
                // createNewColumn(newNameRight, defaultValue);
                rightMap[col] = newNameRight;
            }
        }
    }
    return rightMap;
}

void Dataframe::setValue(const std::string &colName, int row, const std::string &value) {
    rows[row].setData(value, colName);
}

std::string Dataframe::getValue(const std::string &colName, int row) {
    return rows[row].getData(colName);
}

void Dataframe::setColumnValue(const std::string &colName, const std::string &value) {
    for (auto i = 0; i < rows.size(); ++i) {
        setValue(colName, i, value);
    }
}

void Dataframe::merge(Dataframe &df, std::vector<std::string> &colNames,
const std::string &suffixLeft, const std::string &suffixRight, const std::string &defaultValue) {
    std::unordered_map<std::string, std::vector<int>> leftMap;
    std::unordered_map<std::string, std::string> rightMap;
    std::string key;
    bool mergeOk = false;
    for (auto &row : rows) {
        key = "";
        for (const std::string &col : colNames) {
            key += row.getData(col);
        }
        leftMap[key].push_back(getRowIndex(row));
    }
    for (auto &row : df.getRows()) {
        int rightIndex = df.getRowIndex(row);
        key = "";
        for (const std::string &col : colNames) {
            key += row.getData(col);
        }        
        auto it = leftMap.find(key);
        if (it != leftMap.end()) {
            std::vector<int> &indices = it->second;
            if (!mergeOk){
                mergeOk = true;
                rightMap = prepareMerge(df.getHeaders(), colNames, suffixLeft, suffixRight, defaultValue);
            }
            for (int &index : indices) {
                for (auto &kv : rightMap) {
                    const std::string originalCol = kv.first;
                    const std::string rightCol = kv.second;
                    std::string newValue = row.getData(originalCol);
                    setValue(rightCol, index, newValue);
                }
            }
        }
    }
}

std::vector<std::string> Dataframe::getColumn(const std::string &colName) {
    std::vector<std::string> col;
    for (auto &row : rows) {
        col.push_back(row.getData(colName));
    }
    return col;
}

void Dataframe::saveToCSV(char sep, std::string filename, bool header) {    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file '" << filename << "' for writing." << std::endl;
        return;
    }
    if (header) {
        for (size_t i = 0; i < headers.size(); ++i) {
            if (i > 0) {
                file << sep;
            }
            file << headers[i];
        }
        file << std::endl;
    }
    for (auto &row : rows) {
        file << row.getRowStr(headers, sep);
        file << std::endl;
    }
    file.close();
}
