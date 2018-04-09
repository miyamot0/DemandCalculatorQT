#ifndef LAMBERTRESULT_H
#define LAMBERTRESULT_H


class LambertResult
{
public:
    LambertResult();

    double Val;
    double Err;
    int Iterations;
    bool Success;
};

#endif // LAMBERTRESULT_H
