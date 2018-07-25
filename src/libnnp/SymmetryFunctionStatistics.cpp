// Copyright 2018 Andreas Singraber (University of Vienna)
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "SymmetryFunctionStatistics.h"
#include <algorithm> // std::min, std::max
#include <limits>    // std::numeric_limits
#include "utility.h"

using namespace std;
using namespace nnp;

SymmetryFunctionStatistics::Container::Container() :
                                       count  (0                             ),
                                       countEW(0                             ),
                                       min    ( numeric_limits<double>::max()),
                                       max    (-numeric_limits<double>::max()),
                                       Gmin   (0.0                           ),
                                       Gmax   (0.0                           ),
                                       sum    (0.0                           ),
                                       sum2   (0.0                           )
{
}

void SymmetryFunctionStatistics::Container::reset()
{
    resetStatistics();
    resetExtrapolationWarnings();

    return;
}

void SymmetryFunctionStatistics::Container::resetStatistics()
{
    count = 0;
    min   =  numeric_limits<double>::max();
    max   = -numeric_limits<double>::max();
    sum   = 0.0;
    sum2  = 0.0;

    return;
}

void SymmetryFunctionStatistics::Container::resetExtrapolationWarnings()
{
    countEW = 0;
    Gmin = 0.0;
    Gmax = 0.0;
    indexStructureEW.clear();
    indexAtomEW.clear();
    valueEW.clear();

    return;
}

SymmetryFunctionStatistics::SymmetryFunctionStatistics() :
                                           collectStatistics           (false),
                                           collectExtrapolationWarnings(false),
                                           writeExtrapolationWarnings  (false),
                                           stopOnExtrapolationWarnings (false)
{
}

void SymmetryFunctionStatistics::addValue(size_t index, double value)
{
    data[index].count++;
    data[index].sum += value;
    data[index].sum2 += value * value;
    data[index].min = min(data[index].min, value);
    data[index].max = max(data[index].max, value);

    return;
}

void SymmetryFunctionStatistics::addExtrapolationWarning(size_t index,
                                                         double value,
                                                         double Gmin,
                                                         double Gmax,
                                                         size_t indexStructure,
                                                         size_t indexAtom)
{
    data[index].countEW++;
    data[index].Gmin = Gmin;
    data[index].Gmax = Gmax;
    data[index].valueEW.push_back(value);
    data[index].indexStructureEW.push_back(indexStructure);
    data[index].indexAtomEW.push_back(indexAtom);

    return;
}

vector<string> SymmetryFunctionStatistics::getExtrapolationWarningLines() const
{
    vector<string> vs;
    for (map<size_t, Container>::const_iterator it = data.begin();
         it != data.end(); ++it)
    {
        SymmetryFunctionStatistics::Container const& d = it->second;
        for (size_t i = 0; i < d.valueEW.size(); ++i)
        {
            vs.push_back(strpr("### NNP EXTRAPOLATION WARNING ### "
                               "STRUCTURE: %6zu ATOM: %6zu SYMFUNC: %4zu "
                               "VALUE: %10.3E MIN: %10.3E MAX: %10.3E\n",
                               d.indexStructureEW[i],
                               d.indexAtomEW[i],
                               it->first,
                               d.valueEW[i],
                               d.Gmin,
                               d.Gmax));
        }
    }

    return vs;
}

size_t SymmetryFunctionStatistics::countExtrapolationWarnings() const
{
    size_t n = 0;

    for (map<size_t, Container>::const_iterator it = data.begin();
         it != data.end(); ++it)
    {
        n += it->second.countEW;
    }

    return n;
}

void SymmetryFunctionStatistics::resetStatistics()
{
    for (map<size_t, Container>::iterator it = data.begin();
         it != data.end(); ++it)
    {
        it->second.resetStatistics();
    }

    return;
}

void SymmetryFunctionStatistics::resetExtrapolationWarnings()
{
    for (map<size_t, Container>::iterator it = data.begin();
         it != data.end(); ++it)
    {
        it->second.resetExtrapolationWarnings();
    }

    return;
}

void SymmetryFunctionStatistics::clear()
{
    data.clear();
    return;
}
