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
    mReplNum <- NULL
} else if (mRem0 == "MODIFY") {
    mRem0 <- FALSE
    mReplNum <- as.numeric(mReplNum)
}

if (mRemQ0 == "DROP") {
    mRemQ0 <- TRUE
    mReplQ0 <- NULL
} else if (mRemQ0 == "KEEP") {
    mRemQ0 <- FALSE
    mReplQ0 <- NULL
} else if (mRemQ0 == "MODIFY") {
    mRemQ0 <- TRUE
    mReplQ0 <- as.numeric(mReplQ0)
}

dat <- beezdemand::ChangeData(dat, rem0 = mRem0, replnum = mReplNum, remq0e = mRemQ0, replfree = mReplQ0)

options(warn=-1)

if(!suppressMessages(require(jsonlite))){
  suppressMessages(install.packages("jsonlite"))
  if(!require(jsonlite)){
    suppressMessages(install.packages("jsonlite"))
    suppressMessages(library(jsonlite))
  }
}

if(!suppressMessages(require(beezdemand))){
  suppressMessages(install.packages("beezdemand"))
  if(!require(beezdemand)){
    suppressMessages(install.packages("beezdemand"))
    suppressMessages(library(beezdemand))
  }
}

numberCheck <- function(x) !grepl("\\D", x)

fitDemand <- NULL

if (mParamK != "NULL") {
    ### Branch logic, if non-nulled string supplied
    ### Parse apart whether or not supplied k or specified method
    ###

    if(numberCheck(mParamK)) {
        fitDemand <- suppressMessages(FitCurves(dat, mModel, k = as.numeric(mParamK)))
    } else {
        fitDemand <- suppressMessages(FitCurves(dat, mModel, k = mParamK))
    }
}

fitDemand <- toJSON(fitDemand, pretty = FALSE)
fitDemand
