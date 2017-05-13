myArgs <- commandArgs(trailingOnly = TRUE)

mModel <- myArgs[1]
mIds <- as.numeric(unlist(strsplit(myArgs[2], split=",")))
mPrices <- as.numeric(unlist(strsplit(myArgs[3], split=",")))
mConsume <- as.numeric(unlist(strsplit(myArgs[4], split=",")))
mParamK <- myArgs[5]
mRem0 <- myArgs[6]
mReplNum <- myArgs[7]
mRemQ0 <- myArgs[8]
mReplQ0 <- myArgs[9]

dat <- data.frame(id=as.numeric(mIds), x=as.numeric(mPrices), y=as.numeric(mConsume))

if (mRem0 == "DROP") {
    mRem0 <- TRUE
    mReplNum <- NULL
} else if (mRem0 == "KEEP") {
    mRem0 <- FALSE
    mReplNum <- 0
} else if (mRem0 == "MODIFY") {
    mRem0 <- FALSE
    mReplNum <- as.numeric(mReplNum)
}

if (mRemQ0 == "DROP") {
    mRemQ0 <- TRUE
    mReplQ0 <- NULL
} else if (mRemQ0 == "KEEP") {
    mRemQ0 <- FALSE
    mReplQ0 <- 0.01
} else if (mRemQ0 == "MODIFY") {
    mRemQ0 <- TRUE
    mReplQ0 <- as.numeric(mReplQ0)
}

options(warn=-1)

message(dat)

suppressMessages(require(jsonlite))
suppressMessages(require(beezdemand))



#message(paste(mRem0, mReplNum, mRemQ0, mReplQ0, sep = " "))

### Clean up data, per user req's
dat <- beezdemand::ChangeData(dat, rem0 = mRem0, replnum = mReplNum, remq0e = mRemQ0, replfree = mReplQ0)

message(dat)

### Regex, to determine nature of command
numberCheck <- function(x) !grepl("\\D", x)

### Reference to results
fitDemand <- NULL

if (mParamK != "NULL") {
    if(numberCheck(mParamK)) {
        ### K is a numeric assignment
        fitDemand <- suppressMessages(FitCurves(dat, mModel, k = as.numeric(mParamK)))
    } else {
        ### K is a string command
        fitDemand <- suppressMessages(FitCurves(dat, mModel, k = mParamK))
    }
}

fitDemand <- toJSON(fitDemand, pretty = FALSE)
fitDemand
