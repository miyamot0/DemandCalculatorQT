#ifndef EVOLUTIONFUNCTIONS_H
#define EVOLUTIONFUNCTIONS_H

#pragma once

#include <vector>
#include <QWidget>

#include "Libraries/differential-evolution/differentialevolution.h"

using namespace de;

enum FittingScale
{
    Normal,
    Log,
    Log10
};

class BaseModel : public IOptimizable
{
public:
    BaseModel(QList<double> prices, QList<double> consumption)
    {
        _prices = prices;
        _consumption = consumption;
    }

    void SetWeights(QString customWeights)
    {
        weights = SplitWeights(customWeights);
        isWeighted = true;
    }

    QList<double> SplitWeights(QString value)
    {
        QString temp = value.replace("[", "");
        QString temp2 = temp.replace("]", "");

        QStringList tempList = temp2.split(",");

        QList<double> returnList;

        double num;
        foreach (QString str, tempList) {
            num = str.toDouble();

            returnList << num;
        }

        return returnList;
    }

    /**
     * @brief CostFunction
     *
     * @param inputs Prospective parameters
     * @param delay point in time
     *
     * @return Subjective value at delay
     */
    virtual double CostFunction(std::vector<double> inputs, double price) const = 0;

    /**
     * @brief EvaluteCost
     *
     * Return cost function by way of MSE, and inherited cost function
     *
     * @param inputs parameters
     * @return
     */
    double EvaluteCost(std::vector<double> inputs) const override
    {
        double val = 0.0;

        double tempDelay, tempValue;

        double temp;

        for (int j = 0; j < _prices.length(); j++)
        {
            tempDelay = _prices[j];
            tempValue = _consumption[j];

            temp = ScaleVariable(tempValue) - CostFunction(inputs, tempDelay);

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
        }

        val = val / (double) _prices.length();

        return val;
    }

    double ScaleVariable(double value) const
    {
        switch (fittingMethod) {
        case FittingScale::Normal:
            return value;

        case FittingScale::Log:
            return log(value);

        case FittingScale::Log10:
            return log10(value);

        default:
            return value;
        }
    }

    unsigned int NumberOfParameters() const override
    {
        return m_dim;
    }

    Constraints DefaultConstraints() const
    {
        return Constraints(-100, 100, true);
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(1);

        constr[0] = DefaultConstraints();

        return constr;
    }

    unsigned int m_dim = 3;

    FittingScale fittingMethod;

private:
    QList<double> _prices, _consumption, weights;
    bool isWeighted;
};

class LinearDemand : public BaseModel
{
public:
    LinearDemand(QList<double> prices, QList<double> consumption, double upperLBound) : BaseModel(prices, consumption)
    {
        m_dim = 3;
        _upperLBound = upperLBound;

        fittingMethod = FittingScale::Log;
    }

    double CostFunction(std::vector<double> inputs, double price) const override
    {
        return (log(inputs[0]) + (inputs[2] * log(price)) - inputs[1] * price);
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(3);

        constr[0] = Constraints(0.01, _upperLBound, true);  // L
        constr[1] = Constraints(-10, 10, true);             // a
        constr[2] = Constraints(-10, 10, true);             // b

        return constr;
    }

    double _upperLBound = 0;
};

class ExponentialDemand : public BaseModel
{
public:
    ExponentialDemand(QList<double> prices, QList<double> consumption, double kValue, double upperQ0Limit) : BaseModel(prices, consumption)
    {
        m_dim = 2;
        k = kValue;
        upperQ0Bound = upperQ0Limit;

        fittingMethod = FittingScale::Log10;
    }

    double CostFunction(std::vector<double> inputs, double price) const override
    {
        return (log10(inputs[0]) + k * (exp(-inputs[1] * inputs[0] * price) - 1));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(0.01, upperQ0Bound, true);  // Q0
        constr[1] = Constraints(-0.1, 1, true);             // A

        return constr;
    }

    double k = 0;
    double upperQ0Bound = 0;
};

class ExponentialDemandFitK : public BaseModel
{
public:
    ExponentialDemandFitK(QList<double> prices, QList<double> consumption, double upperQ0Limit, double upperKLimit) : BaseModel(prices, consumption)
    {
        m_dim = 3;
        upperKBound = upperKLimit;
        upperQ0Bound = upperQ0Limit;

        fittingMethod = FittingScale::Log10;
    }

    double CostFunction(std::vector<double> inputs, double price) const override
    {
        return (log10(inputs[0]) + inputs[2] * (exp(-inputs[1] * inputs[0] * price) - 1));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(3);

        constr[0] = Constraints(1, upperQ0Bound, true);     // Q0
        constr[1] = Constraints(-1, 1, true);               // A
        constr[2] = Constraints(0.5, upperKBound, true);    // K

        return constr;
    }

    double upperKBound = 0;
    double upperQ0Bound = 0;
};

class ExponentiatedDemand : public BaseModel
{
public:
    ExponentiatedDemand(QList<double> prices, QList<double> consumption, double kValue, double upperQ0Limit) : BaseModel(prices, consumption)
    {
        m_dim = 2;
        k = kValue;
        upperQ0Bound = upperQ0Limit;

        fittingMethod = FittingScale::Normal;
    }

    double CostFunction(std::vector<double> inputs, double price) const override
    {
        return (exp(inputs[0]) * pow(10, (k * (exp(-inputs[1] * exp(inputs[0]) * price) - 1))));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(2);

        constr[0] = Constraints(-10, log(upperQ0Bound * 2) + 1, true);      // Q0
        constr[1] = Constraints(-0.1, 0.1, true);                           // A

        return constr;
    }

    double k = 0;
    double upperQ0Bound = 0;
};

class ExponentiatedDemandFitK : public BaseModel
{
public:
    ExponentiatedDemandFitK(QList<double> prices, QList<double> consumption, double upperQ0Limit, double upperKLimit) : BaseModel(prices, consumption)
    {
        m_dim = 3;
        upperKBound = upperKLimit;
        upperQ0Bound = upperQ0Limit;

        fittingMethod = FittingScale::Normal;
    }

    double CostFunction(std::vector<double> inputs, double price) const override
    {
        return (exp(inputs[0]) * pow(10, (inputs[2] * (exp(-inputs[1] * exp(inputs[0]) * price) - 1))));
    }

    std::vector<Constraints> GetConstraints() const override
    {
        std::vector<Constraints> constr(3);

        constr[0] = Constraints(-10, log(upperQ0Bound * 2) + 1, true);  // Q0
        constr[1] = Constraints(-0.1, 0.1, true);                       // A
        constr[2] = Constraints(0.5, upperKBound, true);                // K

        return constr;
    }

    double upperKBound = 0;
    double upperQ0Bound = 0;
};

#endif // EVOLUTIONFUNCTIONS_H
