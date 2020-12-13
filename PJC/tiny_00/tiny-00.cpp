#include "tiny-00.hpp"

#include <ostream>
#include <iomanip>

void write_stats(std::vector<double> const& data, std::ostream& out)
{
    double maxy = data[0], miny = data[0], sum = 0, mean;
    for (double number : data)
    {
        if (number > maxy)
        {
            maxy = number;
        }
        else if (number < miny)
        {
            miny = number;
        }

        sum += number;
    }

    mean = sum / data.size();

    out << std::setprecision(2) << std::fixed;
    out << "min: " << miny << "\nmax: " << maxy << "\nmean: " << mean << "\n";
}
