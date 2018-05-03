#ifndef TAGS_H
#define TAGS_H

enum class DemandModel
{
    Linear,
    Exponential,
    Exponentiated
};

enum class ChartingOptions
{
    None,
    Native,
    Standardized
};

enum class BehaviorK
{
    Individual,
    Range,
    Fit,
    Share,
    Custom
};

enum class Behavior
{
    Keep,
    Drop,
    Modify
};

enum class ScalingMode
{
    Enabled,
    Disabled
};

enum class WeightingMode
{
    Unweighted,
    Weighted
};

enum class SystematicCheck
{
    Always,
    Flag,
    Never
};

enum class FittingAlgorithm
{
    Function,
    FunctionGradient,
    FunctionGradientHessian,
    DifferentialEvolution
};

#endif // TAGS_H
