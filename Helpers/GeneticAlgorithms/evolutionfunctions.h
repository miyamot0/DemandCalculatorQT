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
            return 0;
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

        // L
        constr[0] = Constraints(0.01, _upperLBound, true);

        // a
        constr[1] = Constraints(-10, 10, true);

        // b
        constr[2] = Constraints(-10, 10, true);

        return constr;
    }

    double _upperLBound = 0;
};


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

        for (int j = 0; j < (int) storedData.size(); j++)
        {
            tempPrice = std::get<0>(storedData.at(j));
            tempConsumption = std::get<1>(storedData.at(j));

            temp = log10(tempConsumption) - (log10(q0) + k * (exp(-a * q0 * tempPrice) - 1));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
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

    QList<double> weights;
    bool isWeighted;

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

        for (int j = 0; j < (int) storedData.size(); j++)
        {
            tempPrice = std::get<0>(storedData.at(j));
            tempConsumption = std::get<1>(storedData.at(j));

            temp = log10(tempConsumption) - (log10(q0) + k * (exp(-a * q0 * tempPrice) - 1));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
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
    unsigned int m_dim = 3;

    vector<tuple<double, double>> storedData;

    QList<double> weights;
    bool isWeighted;

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

    double EvaluteCost(std::vector<double> inputs) const override
    {
        // hack
        if (inputs[0] <= 0)
        {
            return 1e7;
        }

        double val = 0.0;

        double q0 = pow(10, inputs[0]);
        double a = inputs[1];

        double tempPrice, tempConsumption;

        double temp;



        for (int j = 0; j < (int) storedData.size(); j++)
        {
            tempPrice = std::get<0>(storedData.at(j));
            tempConsumption = std::get<1>(storedData.at(j));

            temp = tempConsumption - (q0 * pow(10, (k * (exp(-a * q0 * tempPrice) - 1))));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
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
        constr[0] = Constraints(-3, log10(upperQ0Bound) * 2, true);

        // A
        constr[1] = Constraints(-0.1, 0.1, true);

        return constr;
    }
private:
    unsigned int m_dim = 2;

    vector<tuple<double, double>> storedData;

    QList<double> weights;
    bool isWeighted;

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

    double EvaluteCost(std::vector<double> inputs) const override
    {
        // hack
        if (inputs[0] <= 0)
        {
            return 1e7;
        }

        double val = 0.0;

        double q0 = pow(10, inputs[0]);
        double a = inputs[1];
        double k = inputs[2];

        double tempPrice, tempConsumption;

        double temp;

        for (int j = 0; j < (int) storedData.size(); j++)
        {
            tempPrice = std::get<0>(storedData.at(j));
            tempConsumption = std::get<1>(storedData.at(j));

            temp = tempConsumption - (q0 * pow(10, (k * (exp(-a * q0 * tempPrice) - 1))));

            if (isWeighted)
            {
                val = val + (temp * temp) * weights[j];
            }
            else
            {
                val = val + (temp * temp);
            }
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
        constr[0] = Constraints(1, log10(upperQ0Bound) * 2, true);

        // A
        constr[1] = Constraints(-0.1, 0.1, true);

        // K
        constr[2] = Constraints(0.5, upperKBound, true);

        return constr;
    }
private:
    unsigned int m_dim = 3;

    vector<tuple<double, double>> storedData;

    QList<double> weights;
    bool isWeighted;

    double upperQ0Bound;
    double upperKBound;
};

#endif // EVOLUTIONFUNCTIONS_H
