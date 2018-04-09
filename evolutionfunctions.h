#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

/**
 * \file TestFunctions.h
 * \author Milos Stojanovic Stojke (milsto)
 *
 * Collection of several popular test functions for optimization algorithms
 */

#pragma once

#include <vector>
#include <QWidget>

#include <QDebug>

#include "DifferentialEvolution.h"

namespace de
{
    class ExponentialDemand : public IOptimizable
    {
    public:
        ExponentialDemand(QList<double> prices, QList<double> consumption, double kValue, double upperQ0Limit)
        {
            m_dim = 2;

            k = kValue;

            upperQ0Bound = upperQ0Limit;

            storedData.clear();

            for (int i = 0; i < prices.length(); i++)
            {
                storedData.push_back(tuple<double, double>(prices[i], consumption[i]));
            }
        }

        double EvaluteCost(std::vector<double> inputs) const override
        {
            double val = 0.0;

            double q0 = inputs[0];
            double a = inputs[1];

            double tempPrice, tempConsumption;

            double temp;

            // hack
            if (q0 <= 0)
            {
                return 1e7;
            }

            for (int j = 0; j < storedData.size(); j++)
            {
                tempPrice = std::get<0>(storedData.at(j));
                tempConsumption = std::get<1>(storedData.at(j));

                temp = log10(tempConsumption) - (log10(q0) + k * (exp(-a * q0 * tempPrice) - 1));

                val = val + (temp * temp);
            }

            val = val / (double) storedData.size();

            return val;
        }

        unsigned int NumberOfParameters() const override
        {
            return m_dim;
        }

        std::vector<Constraints> GetConstraints() const override
        {
            std::vector<Constraints> constr(2);

            // Q0
            constr[0] = Constraints(0.01, upperQ0Bound, true);

            // A
            constr[1] = Constraints(-0.1, 1, true);

            return constr;
        }
    private:
        unsigned int m_dim = 2;

        vector<tuple<double, double>> storedData;

        double upperQ0Bound;

        double k;
    };

    class ExponentialDemandFitK : public IOptimizable
    {
    public:
        ExponentialDemandFitK(QList<double> prices, QList<double> consumption, double upperQ0Limit, double upperKLimit)
        {
            m_dim = 3;

            upperQ0Bound = upperQ0Limit;

            upperKBound = upperKLimit;

            storedData.clear();

            for (int i = 0; i < prices.length(); i++)
            {
                storedData.push_back(tuple<double, double>(prices[i], consumption[i]));
            }
        }

        double EvaluteCost(std::vector<double> inputs) const override
        {
            double val = 0.0;

            double q0 = inputs[0];
            double a = inputs[1];
            double k = inputs[2];

            double tempPrice, tempConsumption;

            double temp;

            // hack
            if (q0 <= 0)
            {
                return 1e7;
            }

            for (int j = 0; j < 11; j++)
            {
                tempPrice = std::get<0>(storedData.at(j));
                tempConsumption = std::get<1>(storedData.at(j));

                temp = log10(tempConsumption) - (log10(q0) + k * (exp(-a * q0 * tempPrice) - 1));

                val = val + (temp * temp);
            }

            return val;
        }

        unsigned int NumberOfParameters() const override
        {
            return m_dim;
        }

        std::vector<Constraints> GetConstraints() const override
        {
            std::vector<Constraints> constr(3);

            // Q0
            constr[0] = Constraints(1, upperQ0Bound, true);

            // A
            constr[1] = Constraints(-1, 1, true);

            // K
            constr[2] = Constraints(0.5, upperKBound, true);

            return constr;
        }
    private:
        unsigned int m_dim = 2;

        vector<tuple<double, double>> storedData;

        double upperQ0Bound;
        double upperKBound;
    };

    class ExponentiatedDemand : public IOptimizable
    {
    public:
        ExponentiatedDemand(QList<double> prices, QList<double> consumption, double kValue, double upperQ0Limit)
        {
            m_dim = 2;

            k = kValue;

            upperQ0Bound = upperQ0Limit;

            storedData.clear();

            for (int i = 0; i < prices.length(); i++)
            {
                storedData.push_back(tuple<double, double>(prices[i], consumption[i]));
            }
        }

        double EvaluteCost(std::vector<double> inputs) const override
        {
            double val = 0.0;

            double q0 = inputs[0];
            double a = inputs[1];

            double tempPrice, tempConsumption;

            double temp;

            // hack
            if (q0 <= 0)
            {
                return 1e7;
            }

            for (int j = 0; j < 11; j++)
            {
                tempPrice = std::get<0>(storedData.at(j));
                tempConsumption = std::get<1>(storedData.at(j));

                temp = tempConsumption - (q0 * pow(10, (k * (exp(-a * q0 * tempPrice) - 1))));

                val = val + (temp * temp);
            }

            return val;
        }

        unsigned int NumberOfParameters() const override
        {
            return m_dim;
        }

        std::vector<Constraints> GetConstraints() const override
        {
            std::vector<Constraints> constr(2);

            // Q0
            constr[0] = Constraints(1, upperQ0Bound, true);

            // A
            constr[1] = Constraints(-0.1, 0.1, true);

            return constr;
        }
    private:
        unsigned int m_dim = 2;

        vector<tuple<double, double>> storedData;

        double upperQ0Bound;

        double k;
    };

    class ExponentiatedDemandFitK : public IOptimizable
    {
    public:
        ExponentiatedDemandFitK(QList<double> prices, QList<double> consumption, double upperQ0Limit, double upperKLimit)
        {
            m_dim = 3;

            upperQ0Bound = upperQ0Limit;

            upperKBound = upperKLimit;

            storedData.clear();

            for (int i = 0; i < prices.length(); i++)
            {
                storedData.push_back(tuple<double, double>(prices[i], consumption[i]));
            }
        }

        double EvaluteCost(std::vector<double> inputs) const override
        {
            double val = 0.0;

            double q0 = inputs[0];
            double a = inputs[1];
            double k = inputs[2];

            double tempPrice, tempConsumption;

            double temp;

            // hack
            if (q0 <= 0)
            {
                return 1e7;
            }

            for (int j = 0; j < 11; j++)
            {
                tempPrice = std::get<0>(storedData.at(j));
                tempConsumption = std::get<1>(storedData.at(j));

                temp = tempConsumption - (q0 * pow(10, (k * (exp(-a * q0 * tempPrice) - 1))));

                val = val + (temp * temp);
            }

            return val;
        }

        unsigned int NumberOfParameters() const override
        {
            return m_dim;
        }

        std::vector<Constraints> GetConstraints() const override
        {
            std::vector<Constraints> constr(3);

            // Q0
            constr[0] = Constraints(1, upperQ0Bound, true);

            // A
            constr[1] = Constraints(-0.1, 0.1, true);

            // K
            constr[2] = Constraints(0.5, upperKBound, true);

            return constr;
        }
    private:
        unsigned int m_dim = 2;

        vector<tuple<double, double>> storedData;

        double upperQ0Bound;
        double upperKBound;
    };
}

#endif // TESTFUNCTIONS_H
