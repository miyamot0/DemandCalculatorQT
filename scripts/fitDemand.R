myArgs <- commandArgs(trailingOnly = TRUE)

mModel <- myArgs[1]
mIds <- as.numeric(unlist(strsplit(myArgs[2], split=",")))
mPrices <- as.numeric(unlist(strsplit(myArgs[3], split=",")))
mConsume <- as.numeric(unlist(strsplit(myArgs[4], split=",")))
mParamK <- myArgs[5]

dat <- data.frame(id=as.numeric(mIds), x=as.numeric(mPrices), y=as.numeric(mConsume))

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
