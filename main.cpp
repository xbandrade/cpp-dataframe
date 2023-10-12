#include "dataframe.hpp"

int main() {
    Dataframe df("dt1.csv");
    std::vector<std::string> cols = {"Name", "Monthly Salary", "Department"};
    std::string newCol = "Monthly Salary";
    std::string col = "Salary";
    std::string op = "/";
    double val = 12.;
    df.createNewColumn(newCol, col, op, val);
    df.roundDouble(newCol, 2);
    df.printDataframe();
    std::cout << "----------------" << std::endl;
    df.loc(cols);
    df.printDataframe();
    df.saveToCSV(';', "dt2.csv");
    return 0;
}